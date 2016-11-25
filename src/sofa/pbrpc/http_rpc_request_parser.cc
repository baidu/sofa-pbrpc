// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/http_rpc_request_parser.h>

namespace sofa {
namespace pbrpc {

const std::string HTTPRpcRequestParser::CONTENT_LENGTH = "Content-Length";
const std::string HTTPRpcRequestParser::ACCEPT = "Accept";
const std::string HTTPRpcRequestParser::ACCEPT_PROTOBUF = "application/protobuf";

HTTPRpcRequestParser::HTTPRpcRequestParser() :
    _state(PS_METHOD),
    _content_length(0),
    _req(new HTTPRpcRequest())
{
}

HTTPRpcRequestParser::~HTTPRpcRequestParser()
{
}

const char* HTTPRpcRequestParser::Name()
{
    return "HTTPRpcRequestParser";
}

void HTTPRpcRequestParser::Reset()
{
    _state = PS_METHOD;
    _req.reset(new HTTPRpcRequest());
    _content_length = 0;
}

bool HTTPRpcRequestParser::CheckMagicString(const char* magic_string)
{
    SCHECK(_state == PS_METHOD);
    if (memcmp(magic_string, "GET ", 4) == 0)
    {
        _req->_type = HTTPRpcRequest::GET;
        _state = PS_PATH;
        return true;
    }
    if (memcmp(magic_string, "POST", 4) == 0)
    {
        _req->_type = HTTPRpcRequest::POST;
        _state = PS_METHOD; // need skip white space
        return true;
    }
    return false;
}

int HTTPRpcRequestParser::Parse(const char* buf,
        int data_size, int offset, int* bytes_consumed)
{
    if (data_size == 0)
    {
        return 0;
    }
    *bytes_consumed = 0;
    if (_state != PS_BODY)
    {
        const char* ptr = buf + offset;
        const char* end = buf + offset + data_size;
        std::string err;
        for (; ptr != end; ++ptr)
        {
            int ret = ParseInternal(*ptr, err);
            if (ret < 0)
            {
#if defined( LOG )
                LOG(ERROR) << "Parse(): " << RpcEndpointToString(_req->_remote_endpoint)
                           << ": parse error: " << err;
#else
                SLOG(ERROR, "Parse(): %s: parse error: %s",
                        RpcEndpointToString(_req->_remote_endpoint).c_str(), err.c_str());
#endif
                return ret;
            }
            if (ret > 0)  // header ready
            {
                ++ptr;
                break;
            }
        }
        *bytes_consumed = ptr - (buf + offset);
        data_size -= *bytes_consumed;
        offset += *bytes_consumed;
    }
    if (_state == PS_BODY)
    {
        int bytes_remain = _content_length - _req->_req_body->TotalCount();
        if (bytes_remain == 0)
        {
            return 1;
        }
        if (data_size == 0)
        {
            return 0;
        }
        int consume = std::min(data_size, bytes_remain);
        _req->_req_body->Append(BufHandle(const_cast<char*>(buf), consume, offset));
        *bytes_consumed += consume;
        if (_content_length == _req->_req_body->TotalCount())
        {
            return 1;
        }
        return 0;
    }
    return 0;
}

RpcRequestPtr HTTPRpcRequestParser::GetRequest()
{
    return _req;
}

int HTTPRpcRequestParser::ParseInternal(char c, std::string& err)
{
    switch (_state)
    {
    case PS_METHOD:
        if (c == ' ')
        {
            _state = PS_PATH;
            return 0;
        }
        err = "expect space after POST";
        return -1;
    case PS_PATH:
        if (c == ' ')
        {
            if (!_req->ParsePath())
            {
                err = "invalid path: " + _req->_original_path;
                return -1;
            }
            _state = PS_HTTP_VERSION;
            return 0;
        }
        _req->_original_path.push_back(c);
        return 0;
    case PS_HTTP_VERSION:
        if (c == '\r')
        {
            _state = PS_EXPECT_NEW_LINE1;
            return 0;
        }
        _req->_http_version.push_back(c);
        return 0;
    case PS_EXPECT_NEW_LINE1:
        if (c == '\n')
        {
            _state = PS_HEAD_LINE_START;
            return 0;
        }
        err = "expect \'\\n\' after \'\\r\'";
        return -1;
    case PS_HEAD_LINE_START:
        if (c == '\r')
        {
            _state = PS_EXPECT_NEW_LINE2;
            return 0;
        }
        _state = PS_HEAD_NAME;
        _header_name.clear();
        _header_value.clear();
        _header_name.push_back(c);
        return 0;
    case PS_HEAD_NAME:
        if (c == ':')
        {
            _state = PS_HEAD_COLON;
            return 0;
        }
        _header_name.push_back(c);
        return 0;
    case PS_HEAD_COLON:
        if (c == ' ')
        {
            _state = PS_HEAD_VALUE;
            return 0;
        }
        err = "expect space after \':\' in header key-value pair";
        return -1;
    case PS_HEAD_VALUE:
        if (c == '\r')
        {
            if (!_header_name.empty())
            {
                _req->_headers[_header_name] = _header_value;
            }
            _state = PS_EXPECT_NEW_LINE1;
            return 0;
        }
        _header_value.push_back(c);
        return 0;
    case PS_EXPECT_NEW_LINE2:
        if (c == '\n')
        {
            std::map<std::string, std::string>::const_iterator it =
                _req->_headers.find(CONTENT_LENGTH);
            if (it != _req->_headers.end())
            {
                char* endptr = NULL;
                _content_length = strtoll(it->second.c_str(), &endptr, 10);
                if (endptr == NULL || *endptr != '\0' || _content_length < 0)
                {
                    err = "invalid Content-Length: " + it->second;
                    return -1;
                }
            }

            it = _req->_headers.find(ACCEPT);
            if (it != _req->_headers.end())
            {
                if (it->second == ACCEPT_PROTOBUF)
                {
                    _req->_type = HTTPRpcRequest::POST_PB;
                }
            }

            _state = PS_BODY;
            return 1;
        }
        err = "expect \'\\n\' after \'\\r\'";
        return -1;
    default:
        SCHECK(false);
        return -1;
    }
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
