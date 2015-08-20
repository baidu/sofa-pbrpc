// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_HTTP_RPC_REQUEST_H_
#define _SOFA_PBRPC_HTTP_RPC_REQUEST_H_

#include <map>

#include <sofa/pbrpc/rpc_request.h>

#include <rapidjson/document.h>

namespace sofa {
namespace pbrpc {

class HTTPRpcRequestParser;

class HTTPRpcRequest;
typedef sofa::pbrpc::shared_ptr<HTTPRpcRequest> HTTPRpcRequestPtr;

class HTTPRpcRequest : public RpcRequest
{
public:
    HTTPRpcRequest();
    virtual ~HTTPRpcRequest();

    virtual RpcRequestType RequestType();

    virtual std::string Method();

    virtual uint64 SequenceId();

    virtual void ProcessRequest(
            const RpcServerStreamWPtr& server_stream,
            const ServicePoolPtr& service_pool);

    virtual ReadBufferPtr AssembleSucceedResponse(
            const RpcControllerImplPtr& cntl,
            const google::protobuf::Message* response,
            std::string& err);

    virtual ReadBufferPtr AssembleFailedResponse(
            int32 error_code,
            const std::string& reason,
            std::string& err);

private:
    // Parse http path.
    // @return false if parse failed.
    bool ParsePath();

    bool RoutePage(
            const RpcServerStreamWPtr& server_stream,
            const ServicePoolPtr& service_pool);

    void SendPage(
            const RpcServerStreamWPtr& server_stream,
            const std::string& page);

    void SendError(
            const RpcServerStreamWPtr& server_stream,
            const std::string& error);

    static bool RenderJsonResponse(
            google::protobuf::io::ZeroCopyOutputStream* output,
            const std::string& json);

    static bool RenderHtmlResponse(
            google::protobuf::io::ZeroCopyOutputStream* output,
            const std::string& html);

    static rapidjson::Document* ParseJson(
            const char* str,
            std::string& err);

    static void PageHeader(std::ostream& out);

    static void PageFooter(std::ostream& out);

    void ServerBrief(
            std::ostream& out,
            const ServicePoolPtr& service_pool);

    static void ServerOptions(
            std::ostream& out,
            const ServicePoolPtr& service_pool);

    static void ServerStatus(
            std::ostream& out,
            const ServicePoolPtr& service_pool);

    static void ServiceList(
            std::ostream& out,
            const ServicePoolPtr& service_pool);

    static void MethodList(
            std::ostream& out,
            ServiceBoard* svc_board);

private:
    friend class HTTPRpcRequestParser;

    enum Type
    {
        GET = 0,
        POST = 1
    };
    Type                               _type;
    std::string                        _path;
    std::string                        _decoded_path;
    std::string                        _query_string;
    std::string                        _fragment_id;
    std::string                        _method;
    std::map<std::string, std::string> _query_params;
    std::string                        _http_version;
    std::map<std::string, std::string> _headers;
    ReadBufferPtr                      _req_body;
    rapidjson::Document*               _req_json;
}; // class HTTPRpcRequest

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_HTTP_RPC_REQUEST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
