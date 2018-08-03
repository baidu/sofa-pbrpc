// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <unistd.h>
#include <netdb.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <arpa/inet.h>

#include <sofa/pbrpc/http_rpc_request.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/rpc_server_stream.h>
#include <sofa/pbrpc/rpc_server_impl.h>
#include <sofa/pbrpc/pbjson.h>
#include <sofa/pbrpc/string_utils.h>
#include <sofa/pbrpc/http.h>
#include <sofa/pbrpc/web_service.h>

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

uint64 HTTPRpcRequest::SequenceId()
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
        InnerProcess(server_stream, service_pool);
        return;
    }

    MethodBoard* method_board = FindMethodBoard(service_pool, service_name, method_name);
    if (method_board == NULL)
    {
        InnerProcess(server_stream, service_pool);
        return;
    }

    google::protobuf::Service* service = method_board->GetServiceBoard()->Service();
    const google::protobuf::MethodDescriptor* method_desc = method_board->Descriptor();

    google::protobuf::Message* request = service->GetRequestPrototype(method_desc).New();
    if (_type == POST_PB)
    {
        bool parse_request_return = request->ParseFromZeroCopyStream(_req_body.get());
        if (!parse_request_return)
        {
#if defined( LOG )
            LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                       << ": {" << SequenceId() << "}: parse pb body failed";
#else
            SLOG(ERROR, "ProcessRequest(): %s: {%lu}: parse pb body failed",
                    RpcEndpointToString(_remote_endpoint).c_str(), SequenceId());
#endif
            SendFailedResponse(server_stream,
                    RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse pb body failed");
            delete request;
            return;
        }
    }
    else 
    {
        std::string json_str;
        if (_type == POST)
        {
            json_str = _req_body->ToString();
        }
        else
        {
            json_str = _query_params["request"];
        }
        if (json_str.empty())
        {
            // if null json str, set as null object
            json_str = "{}";
        }

        std::string err;
        _req_json = ParseJson(json_str.c_str(), err);
        if (_req_json == NULL || jsonobject2pb(_req_json, request, err) < 0)
        {
#if defined( LOG )
            LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                       << ": {" << SequenceId() << "}: parse json failed: " << err;
#else
            SLOG(ERROR, "ProcessRequest(): %s: {%lu}: parse json failed: %s",
                    RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(), err.c_str());
#endif
            SendFailedResponse(server_stream,
                    RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse json failed: " + err);
            delete request;
            return;
        }
    }

    google::protobuf::Message* response = service->GetResponsePrototype(method_desc).New();

    RpcController* controller = new RpcController();
    const RpcControllerImplPtr& cntl = controller->impl();
    cntl->SetSequenceId(0);
    cntl->SetMethodId(_method);
    cntl->SetLocalEndpoint(_local_endpoint);
    cntl->SetRemoteEndpoint(_remote_endpoint);
    cntl->SetRpcServerStream(server_stream);
    cntl->SetRequestReceivedTime(_received_time);
    cntl->SetResponseCompressType(CompressTypeNone);
    // ATTENTION: because the lifetime of HttpRpcRequest covers CallMethod() of service,
    // so we can just store pointers to avoid unnecessary data copy.
    cntl->SetHttp();
    cntl->SetHttpPath(&_path);
    cntl->SetHttpQueryParams(&_query_params);
    cntl->SetHttpHeaders(&_headers);

    CallMethod(method_board, controller, request, response);
}

ReadBufferPtr HTTPRpcRequest::AssembleSucceedResponse(
        const RpcControllerImplPtr& /*cntl*/,
        const google::protobuf::Message* response,
        std::string& err)
{
    WriteBuffer write_buffer;
    if (_type == POST_PB)
    {
        if (!RenderResponse(&write_buffer, PROTOBUF, response->SerializeAsString()))
        {
            err = "render protobuf response failed";
            return ReadBufferPtr();
        }
    }
    else
    {
        std::string json_str;
        pb2json(response, json_str);
        if (!RenderResponse(&write_buffer, JSON, json_str))
        {
            err = "render json response failed";
            return ReadBufferPtr();
        }
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
    if (!RenderResponse(&write_buffer, JSON, oss.str()))
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
    if (_original_path.empty() || _original_path[0] != '/')
    {
        return false;
    }
    // parse method
    size_t start = 1; // skip first '/'
    size_t end = _original_path.size();
    for (size_t i = start; i != end; ++i)
    {
        if (_original_path[i] == '?' || _original_path[i] == '#')
        {
            end = i;
            break;
        }
    }
    _path = _original_path.substr(0, end);
    _method = _original_path.substr(start, end - start);
#if defined( LOG )
        LOG(DEBUG) << "ParsePath(): original_path=" <<
                   << "[" << _original_path << "]"
                   << ", _method=[" << _method << "]";
#else
        SLOG(DEBUG, "ParsePath(): original_path=[%s], _method=[%s]",
                _original_path.c_str(), _method.c_str());
#endif
    // parse query
    if (end < _original_path.size() && _original_path[end] == '?')
    {
        start = end + 1;
        end = _original_path.size();
        for (size_t i = start; i != end; ++i)
        {
            if (_original_path[i] == '#')
            {
                end = i;
                break;
            }
        }
        _query_string = _original_path.substr(start, end - start);
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
                    const std::string& value = param.substr(pos + 1);
                    const std::string& decoded_value = StringUtils::decode_url(value,
                        (StringUtils::E_DECODE_RESERVED_CHAR | StringUtils::E_DECODE_PERCENT_SIGN_CHAR));
                    _query_params[key] = decoded_value;
                }
            }
        }
    }
    // parse fragment
    if (end < _original_path.size() && _original_path[end] == '#')
    {
        _fragment_id = _original_path.substr(end + 1);
    }
    return true;
}

void HTTPRpcRequest::SendResponse(
        const RpcServerStreamWPtr& server_stream,
        const HTTPResponse& response)
{
    WriteBuffer write_buffer;
    if (!RenderResponse(&write_buffer, response))
    {
#if defined( LOG )
        LOG(ERROR) << "SendResponse(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << SequenceId() << "}"
                   << ": render response failed";
#else
        SLOG(ERROR, "SendResponse(): %s: {%lu}: render response failed",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId());
#endif
        return;
    }

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());
    response.content->SwapOut(read_buffer.get());

    SendSucceedResponse(server_stream, read_buffer);
}

bool HTTPRpcRequest::RenderResponse(
        google::protobuf::io::ZeroCopyOutputStream* output,
        const RenderType type,
        const std::string& body)
{
    std::ostringstream oss;
    oss << body.size();
    google::protobuf::io::Printer printer(output, '$');
    printer.Print("HTTP/1.1 200 OK\r\n");
    switch (type)
    {
        case JSON:
            printer.Print("Content-Type: application/json\r\n");
            break;
        case PROTOBUF:
            printer.Print("Content-Type: application/protobuf\r\n");
            break;
        case HTML:
            printer.Print("Content-Type: text/html; charset=UTF-8\r\n");
            break;
        default:
            break;
    }
    printer.Print("Access-Control-Allow-Origin: *\r\n");
    printer.Print("Content-Length: $LENGTH$\r\n", "LENGTH", oss.str());
    printer.Print("\r\n");
    printer.PrintRaw(body);
    return !printer.failed();
}

bool HTTPRpcRequest::RenderResponse(
        google::protobuf::io::ZeroCopyOutputStream* output,
        const HTTPResponse& response)
{
    std::ostringstream oss;
    oss << response.content->ByteCount();
    google::protobuf::io::Printer printer(output, '$');
    printer.Print("$STATUS_LINE$\r\n", "STATUS_LINE", response.status_line);
    printer.Print("Content-Type: $TYPE$\r\n", "TYPE", response.content_type);
    printer.Print("Access-Control-Allow-Origin: *\r\n");
    printer.Print("Content-Length: $LENGTH$\r\n", "LENGTH", oss.str());
    printer.Print("\r\n");
    return !printer.failed();
}

rapidjson::Document* HTTPRpcRequest::ParseJson(
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

void HTTPRpcRequest::InnerProcess(const RpcServerStreamWPtr& server_stream,
         const ServicePoolPtr& service_pool)
{
    RpcServerImpl* server = service_pool->RpcServer();
    WebServicePtr web_service = server->GetWebService();
    if (web_service && web_service->RoutePage(
                shared_from_this(), server_stream))
    {
        return;
    }
    else
    {
#if defined( LOG )
        LOG(ERROR) << "InnerProcess(): " << RpcEndpointToString(_remote_endpoint)
            << ": {" << SequenceId() << "}: method not found: " << _method;
#else
        SLOG(ERROR, "InnerProcess(): %s: {%lu}: method not found: %s",
             RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(), _method.c_str());
#endif
        SendFailedResponse(server_stream,
                           RPC_ERROR_FOUND_METHOD, "method not found: " + _method);
        return;
    }
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
