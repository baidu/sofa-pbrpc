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

static std::string GetHostName(const std::string& ip)
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

template <class T>
std::string format_number(T num)
{
    std::ostringstream oss;
    oss << num;
    std::string str = oss.str();
    size_t len = str.size();
    std::string ret;
    for (size_t i = 0; i < str.size(); ++i)
    {
        if (i != 0 && len % 3 == 0)
        {
            ret.push_back(',');
        }
        ret.push_back(str.at(i));
        --len;
    }
    return ret;
}

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
        if (RoutePage(server_stream, service_pool))
        {
            return;
        }
        else
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

bool HTTPRpcRequest::RoutePage(
        const RpcServerStreamWPtr& server_stream,
        const ServicePoolPtr& service_pool)
{
    if (_method == "" || _method == "home")
    {
        std::ostringstream oss;
        PageHeader(oss);
        ServerBrief(oss, service_pool);
        ServerStatus(oss, service_pool);
        ServiceList(oss, service_pool);
        ServerOptions(oss, service_pool);
        PageFooter(oss);
        SendPage(server_stream, oss.str());
        return true;
    }
    else if (_method == "options")
    {
        std::ostringstream oss;
        PageHeader(oss);
        oss << "<a href=\"/\">&lt;&lt;&lt;&lt;back to Home</a><br>";
        ServerOptions(oss, service_pool);
        PageFooter(oss);
        SendPage(server_stream, oss.str());
        return true;
    }
    else if (_method == "status")
    {
        std::ostringstream oss;
        PageHeader(oss);
        oss << "<a href=\"/\">&lt;&lt;&lt;&lt;back to Home</a><br>";
        ServerStatus(oss, service_pool);
        PageFooter(oss);
        SendPage(server_stream, oss.str());
        return true;
    }
    else if (_method == "services")
    {
        std::ostringstream oss;
        PageHeader(oss);
        oss << "<a href=\"/\">&lt;&lt;&lt;&lt;back to Home</a><br>";
        ServiceList(oss, service_pool);
        PageFooter(oss);
        SendPage(server_stream, oss.str());
        return true;
    }
    else if (_method == "service")
    {
        if (_query_params.find("name") == _query_params.end())
        {
            SendError(server_stream, "Lack of name param");
            return true;
        }
        ServiceBoard* svc_board = service_pool->FindService(_query_params["name"]);
        if (svc_board == NULL)
        {
            SendError(server_stream, "Service not found");
            return true;
        }
        std::ostringstream oss;
        PageHeader(oss);
        oss << "<a href=\"/\">&lt;&lt;&lt;&lt;back to Home</a><br>"
            << "<a href=\"/services\">&lt;&lt;&lt;&lt;back to Services</a>";
        MethodList(oss, svc_board);
        PageFooter(oss);
        SendPage(server_stream, oss.str());
        return true;
    }
    return false;
}

void HTTPRpcRequest::SendPage(
        const RpcServerStreamWPtr& server_stream,
        const std::string& page)
{
    WriteBuffer write_buffer;
    if (!RenderHtmlResponse(&write_buffer, page))
    {
#if defined( LOG )
        LOG(ERROR) << "SendPage(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << SequenceId() << "}"
                   << ": render response failed";
#else
        SLOG(ERROR, "SendPage(): %s: {%lu}: render response failed",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId());
#endif
        return;
    }

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());

    SendSucceedResponse(server_stream, read_buffer);
}

void HTTPRpcRequest::SendError(
        const RpcServerStreamWPtr& server_stream,
        const std::string& error)
{
    std::ostringstream oss;
    PageHeader(oss);
    oss << "<h1>ERROR: " << error << "</h1>";
    PageFooter(oss);
    SendPage(server_stream, oss.str());
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

void HTTPRpcRequest::PageHeader(std::ostream& out)
{
    out << "<html>"
        << "<head><title>SOFA-PBRPC</title></head>"
        << "<body>";
}

void HTTPRpcRequest::PageFooter(std::ostream& out)
{
    out << "<hr>"
        << "Provided by SOFA-PBRPC."
        << "</body>"
        << "</html>";
}

void HTTPRpcRequest::ServerBrief(
        std::ostream& out,
        const ServicePoolPtr& service_pool)
{
    std::string ip = HostOfRpcEndpoint(_local_endpoint);
    uint32 port = PortOfRpcEndpoint(_local_endpoint);
    std::string host = GetHostName(ip);

    out << "<h1>" << host << "</h1>"
        << "<b>IP:</b> " << ip << "<br>"
        << "<b>Port:</b> " << port << "<br>"
        << "<b>Started:</b> " << ptime_to_string(service_pool->RpcServer()->GetStartTime()) << "<br>"
        << "<b>Version:</b> " << SOFA_PBRPC_VERSION << "<br>"
        << "<b>Compiled:</b> " << compile_info() << "<br>";
}

void HTTPRpcRequest::ServerOptions(
        std::ostream& out,
        const ServicePoolPtr& service_pool)
{
    RpcServerImpl* server = service_pool->RpcServer();
    RpcServerOptions options = server->GetOptions();
    out << "<h3>ServerOptions</h3><hr>"
        << "<table border=\"2\">"
        << "<tr><th align=\"left\">Name</th><th align=\"right\">Value</th></tr>"
        << "<tr><td>work_thread_num</td>"
        << "<td align=\"right\">" << options.work_thread_num << "</td></tr>"
        << "<tr><td>keep_alive_time (seconds)</td>"
        << "<td align=\"right\">" << options.keep_alive_time << "</td></tr>"
        << "<tr><td>max_pending_buffer_size (MB)</td>"
        << "<td align=\"right\">" << options.max_pending_buffer_size << "</td></tr>"
        << "<tr><td>max_throughput_in (MB)</td>"
        << "<td align=\"right\">" << options.max_throughput_in << "</td></tr>"
        << "<tr><td>max_throughput_out (MB)</td>"
        << "<td align=\"right\">" << options.max_throughput_out << "</td></tr>"
        << "<tr><td>disable_builtin_services</td>"
        << "<td align=\"right\">" << (options.disable_builtin_services ? "true" : "false") << "</td></tr>"
        << "<tr><td>disable_list_service</td>"
        << "<td align=\"right\">" << (options.disable_list_service ? "true" : "false") << "</td></tr>"
        << "</table>";
}

void HTTPRpcRequest::ServerStatus(
        std::ostream& out,
        const ServicePoolPtr& service_pool)
{
    RpcServerImpl* server = service_pool->RpcServer();
    int64 pending_message_count;
    int64 pending_buffer_size;
    int64 pending_data_size;
    server->GetPendingStat(&pending_message_count, &pending_buffer_size, &pending_data_size);
    out << "<h3>ServerStatus</h3><hr>"
        << "<table border=\"2\">"
        << "<tr><th align=\"left\">Name</td><th align=\"right\">Value</th></tr>"
        << "<tr><td>connection_count</td>"
        << "<td align=\"right\">" << server->ConnectionCount() << "</td></tr>"
        << "<tr><td>service_count</td>"
        << "<td align=\"right\">" << server->ServiceCount() << "</td></tr>"
        << "<tr><td>pending_message_count</td>"
        << "<td align=\"right\">" << pending_message_count << "</td></tr>"
        << "<tr><td>pending_buffer_size (bytes)</td>"
        << "<td align=\"right\">" << format_number(pending_buffer_size) << "</td></tr>"
        << "<tr><td>pending_data_size (bytes)</td>"
        << "<td align=\"right\">" << format_number(pending_data_size) << "</td></tr>"
        << "</table>";
}

void HTTPRpcRequest::ServiceList(
        std::ostream& out,
        const ServicePoolPtr& service_pool)
{
    out << "<h3>Services</h3><hr>"
        << "<table border=\"2\">"
        << "<tr>"
        << "<th rowspan=\"2\" align=\"left\">Name</th>"
        << "<th colspan=\"3\" align=\"center\">Stat in last second</th>"
        << "<th colspan=\"3\" align=\"center\">Stat in last minute</th>"
        << "</tr>"
        << "<tr>"
        << "<th align=\"right\">Requested</th><th align=\"right\">Succeed</th><th align=\"right\">Failed</th>"
        << "<th align=\"right\">Requested</th><th align=\"right\">Succeed</th><th align=\"right\">Failed</th>"
        << "</tr>";
    std::list<ServiceBoard*> svc_list;
    service_pool->ListService(&svc_list);
    for (std::list<ServiceBoard*>::iterator it = svc_list.begin();
            it != svc_list.end(); ++it)
    {
        ServiceBoard* svc_board = *it;
        std::string name = svc_board->ServiceName();
        sofa::pbrpc::builtin::ServiceStat stat1;
        sofa::pbrpc::builtin::ServiceStat stat60;
        svc_board->LatestStats(1, &stat1);
        svc_board->LatestStats(60, &stat60);
        out << "<tr>"
            << "<td><a href=\"/service?name=" << name << "\">" << name << "</a></td>"
            << "<td align=\"right\">" << (stat1.succeed_count() + stat1.failed_count()) << "</td>"
            << "<td align=\"right\">" << stat1.succeed_count() << "</td>" 
            << "<td align=\"right\">" << stat1.failed_count() << "</td>"
            << "<td align=\"right\">" << (stat60.succeed_count() + stat60.failed_count()) << "</td>"
            << "<td align=\"right\">" << stat60.succeed_count() << "</td>"
            << "<td align=\"right\">" << stat60.failed_count() << "</td>"
            << "</tr>";
    }
    out << "</table>";
}

void HTTPRpcRequest::MethodList(
        std::ostream& out,
        ServiceBoard* svc_board)
{
    out << "<h3>Methods of [" << svc_board->ServiceName() << "]</h3><hr>"
        << "<table border=\"2\">"
        << "<tr>"
        << "<th rowspan=\"3\" align=\"left\">Name</th>"
        << "<th colspan=\"6\" align=\"center\">Stat in last second</th>"
        << "<th colspan=\"6\" align=\"center\">Stat in last minute</th>"
        << "</tr>"
        << "<tr>"
        << "<th colspan=\"3\" align=\"center\">Succeed</th>"
        << "<th colspan=\"3\" align=\"center\">Failed</th>"
        << "<th colspan=\"3\" align=\"center\">Succeed</th>"
        << "<th colspan=\"3\" align=\"center\">Failed</th>"
        << "</tr>"
        << "<tr>"
        << "<th align=\"right\">Count</th>"
        << "<th align=\"right\">AvgTime</th>"
        << "<th align=\"right\">MaxTime</th>"
        << "<th align=\"right\">Count</th>"
        << "<th align=\"right\">AvgTime</th>"
        << "<th align=\"right\">MaxTime</th>"
        << "<th align=\"right\">Count</th>"
        << "<th align=\"right\">AvgTime</th>"
        << "<th align=\"right\">MaxTime</th>"
        << "<th align=\"right\">Count</th>"
        << "<th align=\"right\">AvgTime</th>"
        << "<th align=\"right\">MaxTime</th>"
        << "</tr>";
    int method_count = svc_board->Descriptor()->method_count();
    for (int i = 0; i < method_count; ++i)
    {
        MethodBoard* method_board = svc_board->Method(i);
        std::string name = method_board->Descriptor()->name();
        sofa::pbrpc::builtin::MethodStat stat1;
        sofa::pbrpc::builtin::MethodStat stat60;
        method_board->LatestStats(1, &stat1);
        method_board->LatestStats(60, &stat60);
        out << "<tr>"
            << "<td>" << name << "</td>"
            << std::setprecision(6) << std::fixed
            << "<td align=\"right\">" << stat1.succeed_count() << "</td>"
            << "<td align=\"right\">" << stat1.succeed_avg_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << (float)stat1.succeed_max_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << stat1.failed_count() << "</td>"
            << "<td align=\"right\">" << stat1.failed_avg_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << (float)stat1.failed_max_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << stat60.succeed_count() << "</td>"
            << "<td align=\"right\">" << stat60.succeed_avg_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << (float)stat60.succeed_max_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << stat60.failed_count() << "</td>"
            << "<td align=\"right\">" << stat60.failed_avg_time_us() / 1000 << "</td>"
            << "<td align=\"right\">" << (float)stat60.failed_max_time_us() / 1000 << "</td>"
            << "</tr>";
    }
    out << "</table>";
    out << "Notes: all the time in the table is in milliseconds.";
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
