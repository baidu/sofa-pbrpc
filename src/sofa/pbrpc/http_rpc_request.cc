// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <sofa/pbrpc/http_rpc_request.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/rpc_server_stream.h>

#include <google/protobuf/io/printer.h>

namespace sofa {
namespace pbrpc {

HTTPRpcRequest::HTTPRpcRequest() :
    _type(GET),
    _req_body(new ReadBuffer()),
    _req_json(NULL)
{
}

HTTPRpcRequest::~HTTPRpcRequest()
{
    if (_req_json != NULL)
    {
        delete _req_json;
    }
}

RpcRequest::RpcRequestType HTTPRpcRequest::RequestType()
{
    return RpcRequest::HTTP;
}

std::string HTTPRpcRequest::Method()
{
    return _method;
}

uint64_t HTTPRpcRequest::SequenceId()
{
    // id is not supported in HTTP
    return 0;
}

void HTTPRpcRequest::ProcessRequest(
        const RpcServerStreamWPtr& server_stream,
        const ServicePoolPtr& service_pool)
{
    std::string service_name;
    std::string method_name;
    if (!ParseMethodFullName(_method, &service_name, &method_name))
    {
        SendFailedResponse(server_stream,
                RPC_ERROR_PARSE_METHOD_NAME, "invalid method name: " + _method);
        return;
    }

    std::string err;
    if (_type == POST)
    {
        std::string json_str = _req_body->ToString();
        _req_json = ParseJson(json_str.c_str(), err);
        if (_req_json == NULL)
        {
            SendFailedResponse(server_stream,
                    RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse json failed: " + err);
            return;
        }
    }
    else
    {
        std::map<std::string, std::string>::iterator find = _query_params.find("request");
        if (find == _query_params.end())
        {
            SendFailedResponse(server_stream,
                    RPC_ERROR_PARSE_REQUEST_MESSAGE, "no request param specified");
            return;
        }
        _req_json = ParseJson(find->second.c_str(), err);
        if (_req_json == NULL)
        {
            SendFailedResponse(server_stream,
                    RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse json failed: " + err);
            return;
        }
    }

    MethodBoard* method_board = FindMethodBoard(service_pool, service_name, method_name);
    if (method_board == NULL)
    {
        SendFailedResponse(server_stream,
                RPC_ERROR_FOUND_METHOD, "method not found: " + _method);
        return;
    }

    google::protobuf::Service* service = method_board->ServiceBoard()->Service();
    const google::protobuf::MethodDescriptor* method_desc = method_board->Descriptor();

    google::protobuf::Message* request = service->GetRequestPrototype(method_desc).New();
    if (jsonobject2pb(_req_json, request, err) < 0)
    {
        SendFailedResponse(server_stream,
                RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse json to pb failed: " + err);
        return;
    }

    google::protobuf::Message* response = service->GetResponsePrototype(method).New();

    RpcController* controller = new RpcController();
    const RpcControllerImplPtr& cntl = controller->impl();
    cntl->SetSequenceId(_sequence_id);
    cntl->SetMethodId(_method);
    cntl->SetLocalEndpoint(_local_endpoint);
    cntl->SetRemoteEndpoint(_remote_endpoint);
    cntl->SetRpcServerStream(stream);
    cntl->SetRequestReceivedTime(_received_time);
    cntl->SetStartProcessTime(ptime_now());
    cntl->SetResponseCompressType(CompressTypeNone);

    CallMethod(method_board, controller, request, response);
}

ReadBufferPtr HTTPRpcRequest::AssembleSucceedResponse(
        const RpcControllerImplPtr& cntl,
        const google::protobuf::Message* response,
        std::string& err)
{
    std::string json_str;
    pb2json(response, json_str);

    WriteBuffer write_buffer;
    if (!RenderJsonResponse(&write_buffer, json_str))
    {
        err = "render json response failed";
        return ReadBufferPtr();
    }

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());
    return read_buffer;
}

ReadBufferPtr HTTPRpcRequest::AssembleFailedResponse(
        int32 error_code,
        const std::string& reason,
        std::string& err)
{
    std::ostringstream oss;
    oss << "\"ERROR: " << error_code << ": "
        << StringUtils::replace_all(reason, "\"", "\\\"") << "\"";

    WriteBuffer write_buffer;
    if (!RenderJsonResponse(&write_buffer, oss.str()))
    {
        err = "render json response failed";
        return ReadBufferPtr();
    }

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());
    return read_buffer;
}

bool HTTPRpcRequest::ParsePath()
{
    if (_path.empty() || _path[0] != '/')
    {
        return false;
    }
    // decode
    _decoded_path = StringUtils::decode_url(_path,
            (StringUtils::E_DECODE_RESERVED_CHAR | StringUtils::E_DECODE_PERCENT_SIGN_CHAR));
#if defined( LOG )
        LOG(DEBUG) << "ParsePath(): path=[" << _path << "]"
                   << ", decoded_path=[" << _decoded_path << "]";
#else
        SLOG(DEBUG, "ParsePath(): path=[%s], decoded_path=[%s]",
                _path.c_str(), _decoded_path.c_str());
#endif
    // parse method
    size_t start = 1; // skip first '/'
    size_t end = _decoded_path.size();
    for (size_t i = start; i != end; ++i)
    {
        if (_decoded_path[i] == '?' || _decoded_path[i] == '#')
        {
            end = i;
            break;
        }
    }
    _method = _decoded_path.substr(start, end - start);
    // parse query
    if (end < _decoded_path.size() && _decoded_path[end] == '?')
    {
        start = end + 1;
        end = _decoded_path.size();
        for (size_t i = start; i != end; ++i)
        {
            if (_decoded_path[i] == '#')
            {
                end = i;
                break;
            }
        }
        _query_string = _decoded_path.substr(start, end - start);
        if (!_query_string.empty())
        {
            std::vector<std::string> param_list;
            StringUtils::split(_query_string, "&", &param_list);
            for (size_t i = 0; i < param_list.size(); ++i)
            {
                const std::string& param = param_list[i];
                std::string::size_type pos = param.find('=');
                if (pos != std::string::npos)
                {
                    std::string key = param.substr(0, pos);
                    std::string value = param.substr(pos + 1);
                    _query_params[key] = value;
                }
            }
        }
    }
    // parse fragment
    if (end < _decoded_path.size() && _decoded_path[end] == '#')
    {
        _fragment_id = _decoded_path.substr(end + 1);
    }
    return true;
}

bool HTTPRpcRequest::RenderJsonResponse(
        google::protobuf::io::ZeroCopyOutputStream* output,
        const std::string& json)
{
    google::protobuf::io::Printer printer(output);
    std::ostringstream oss;
    oss << json.size();
    printer->Print("HTTP/1.1 200 OK\r\n");
    printer->Print("Content-Type: application/json\r\n");
    printer->Print("Access-Control-Allow-Origin: *\r\n");
    printer->Print("Content-Length: $LENGTH$\r\n", "LENGTH", oss.str());
    printer->Print("\r\n");
    printer->PrintRaw(json);
    return !priner->failed();
}

rapidjson::Value* HTTPRpcRequest::ParseJson(
        const char* str,
        std::string& err)
{
    rapidjson::Document* d = new rapidjson::Document();
    d->Parse<0>(str);
    if (d->HasParseError())
    {
        err = d->GetParseError();
        delete d;
        return NULL;
    }
    return d;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
