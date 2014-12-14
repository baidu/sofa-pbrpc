// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

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

#include <google/protobuf/io/printer.h>

namespace sofa {
namespace pbrpc {

static std::string GetHostName(const std::string& ip);

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
    if (_method == "")
    {
        OnHome(server_stream, service_pool);
        return;
    }

    std::string service_name;
    std::string method_name;
    if (!ParseMethodFullName(_method, &service_name, &method_name))
    {
#if defined( LOG )
        LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << SequenceId() << "}: method not found: " << _method;
#else
        SLOG(ERROR, "ProcessRequest(): %s: {%lu}: method not found: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(), _method.c_str());
#endif
        SendFailedResponse(server_stream,
                RPC_ERROR_FOUND_METHOD, "method not found: " + _method);
        return;
    }

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
    if (_req_json == NULL)
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
        return;
    }

    MethodBoard* method_board = FindMethodBoard(service_pool, service_name, method_name);
    if (method_board == NULL)
    {
#if defined( LOG )
        LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << SequenceId() << "}: method not found: " << _method;
#else
        SLOG(ERROR, "ProcessRequest(): %s: {%lu}: method not found: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(), _method.c_str());
#endif
        SendFailedResponse(server_stream,
                RPC_ERROR_FOUND_METHOD, "method not found: " + _method);
        return;
    }

    google::protobuf::Service* service = method_board->GetServiceBoard()->Service();
    const google::protobuf::MethodDescriptor* method_desc = method_board->Descriptor();

    google::protobuf::Message* request = service->GetRequestPrototype(method_desc).New();
    if (jsonobject2pb(_req_json, request, err) < 0)
    {
#if defined( LOG )
        LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << SequenceId() << "}: parse json to pb failed: " << err;
#else
        SLOG(ERROR, "ProcessRequest(): %s: {%lu}: parse json to pb failed: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(), err.c_str());
#endif
        SendFailedResponse(server_stream,
                RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse json to pb failed: " + err);
        return;
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
    cntl->SetStartProcessTime(ptime_now());
    cntl->SetResponseCompressType(CompressTypeNone);

    CallMethod(method_board, controller, request, response);
}

ReadBufferPtr HTTPRpcRequest::AssembleSucceedResponse(
        const RpcControllerImplPtr& /*cntl*/,
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

void HTTPRpcRequest::OnHome(
        const RpcServerStreamWPtr& server_stream,
        const ServicePoolPtr& service_pool)
{
    std::ostringstream oss;
    oss << "<html>"
        << "<head><title>SOFA-PBRPC</title></head>"
        << "<body>";

    std::string ip = HostOfRpcEndpoint(_local_endpoint);
    uint32 port = PortOfRpcEndpoint(_local_endpoint);
    std::string host = GetHostName(ip);

    oss << "<h1>" << host << "</h1>";
    oss << "<b>IP:</b> " << ip << "<br>";
    oss << "<b>Port:</b> " << port << "<br>";
    oss << "<b>Started:</b> " << ptime_to_string(service_pool->StartTime()) << "<br>";
    oss << "<b>Version:</b> " << SOFA_PBRPC_VERSION << "<br>";

    RpcServerOptions options = service_pool->RpcServer()->GetOptions();
    oss << "<h3>ServerOptions</h3>"
        << "<table border=\"2\">"
        << "<tr><td>Name</td><td>Value</td></tr>"
        << "<tr><td>work_thread_num</td><td>"
        << options.work_thread_num << "</td></tr>"
        << "<tr><td>keep_alive_time (seconds)</td><td>"
        << options.keep_alive_time << "</td></tr>"
        << "<tr><td>max_pending_buffer_size (MB)</td><td>"
        << options.max_pending_buffer_size << "</td></tr>"
        << "<tr><td>max_throughput_in (MB)</td><td>"
        << options.max_throughput_in << "</td></tr>"
        << "<tr><td>max_throughput_out (MB)</td><td>"
        << options.max_throughput_out << "</td></tr>"
        << "<tr><td>disable_builtin_services</td><td>"
        << (options.disable_builtin_services ? "true" : "false") << "</td></tr>"
        << "<tr><td>disable_list_service</td><td>"
        << (options.disable_list_service ? "true" : "false") << "</td></tr>"
        << "</table>";

    oss << "<h3>Services</h3>"
        << "<table border=\"2\">"
        << "<tr>"
        << "<td>ServiceName</td>"
        << "<td>SucceedCount (last minite)</td>"
        << "<td>FailedCount (last minite)</td>"
        << "</tr>";
    std::list<ServiceBoard*> svc_list;
    service_pool->ListService(&svc_list);
    for (std::list<ServiceBoard*>::iterator it = svc_list.begin();
            it != svc_list.end(); ++it)
    {
        ServiceBoard* svc_board = *it;
        sofa::pbrpc::builtin::ServiceStat stat;
        svc_board->LatestStats(60, &stat);
        oss << "<tr>"
            << "<td>" << stat.service_name() << "</td>"
            << "<td>" << stat.succeed_count() << "</td>"
            << "<td>" << stat.failed_count() << "</td>"
            << "</tr>";
    }
    oss << "</table>";

    oss << "</body>"
        << "</html>";

    WriteBuffer write_buffer;
    if (!RenderHtmlResponse(&write_buffer, oss.str()))
    {
#if defined( LOG )
        LOG(ERROR) << "OnHome(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << SequenceId() << "}"
                   << ": render response failed";
#else
        SLOG(ERROR, "OnHome(): %s: {%lu}: render response failed",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId());
#endif
        return;
    }

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());

    SendSucceedResponse(server_stream, read_buffer);
}

bool HTTPRpcRequest::RenderJsonResponse(
        google::protobuf::io::ZeroCopyOutputStream* output,
        const std::string& json)
{
    std::ostringstream oss;
    oss << json.size();
    google::protobuf::io::Printer printer(output, '$');
    printer.Print("HTTP/1.1 200 OK\r\n");
    printer.Print("Content-Type: application/json\r\n");
    printer.Print("Access-Control-Allow-Origin: *\r\n");
    printer.Print("Content-Length: $LENGTH$\r\n", "LENGTH", oss.str());
    printer.Print("\r\n");
    printer.PrintRaw(json);
    return !printer.failed();
}

bool HTTPRpcRequest::RenderHtmlResponse(
        google::protobuf::io::ZeroCopyOutputStream* output,
        const std::string& html)
{
    std::ostringstream oss;
    oss << html.size();
    google::protobuf::io::Printer printer(output, '$');
    printer.Print("HTTP/1.1 200 OK\r\n");
    printer.Print("Content-Type: text/html; charset=UTF-8\r\n");
    printer.Print("Access-Control-Allow-Origin: *\r\n");
    printer.Print("Content-Length: $LENGTH$\r\n", "LENGTH", oss.str());
    printer.Print("\r\n");
    printer.PrintRaw(html);
    return !printer.failed();
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

std::string GetHostName(const std::string& ip)
{
    struct sockaddr_in addr;
    struct hostent *host;
    if (inet_aton(ip.c_str(), &addr.sin_addr) != 0)
    {
        host = gethostbyaddr((char*)&addr.sin_addr, 4, AF_INET); 
    }
    else
    {
        host = gethostbyname(ip.c_str());
    }
    if (host != NULL)
    {
        return host->h_name;
    }
    else
    {
        return ip;
    }
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
