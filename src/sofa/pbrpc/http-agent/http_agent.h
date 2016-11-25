// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBPRC_HTTP_AGENT_HTTP_AGENT_H_
#define _SOFA_PBPRC_HTTP_AGENT_HTTP_AGENT_H_

#include <list>
#include <map>

#include <google/protobuf/descriptor.h>
#include <google/protobuf/descriptor.pb.h>
#include <google/protobuf/message.h>

#include <sofa/pbrpc/pbrpc.h>

namespace sofa {
namespace pbrpc {

class RpcControllerImpl;

namespace builtin {
class BuiltinService_Stub;
} // namespace builtin

namespace http_agent {

class HttpAgent
{
public:
    HttpAgent(RpcClient* rpc_client);
    ~HttpAgent();

    bool Init(const std::string& server_address);

    bool UpdateServiceInfo();

    bool ListService(std::map<std::string, ::google::protobuf::ServiceDescriptorProto>* service_map);

    // Returns map: name --> desc
    bool ListService(std::map<std::string, std::string>* desc_map);

    enum ProtobufType {
        PT_SERVICE,
        PT_MESSAGE,
        PT_ENUM,
    };
    bool GetDescriptor(const std::string& name, ProtobufType* type, std::string* desc);

    void CallMethod(const std::string& method_full_name,
                    ::google::protobuf::RpcController* controller,
                    const std::string* request,
                    std::string* response,
                    ::google::protobuf::Closure* done);

private:
    static void CallMethodDone(const sofa::pbrpc::shared_ptr<RpcControllerImpl>& cntl,
                               std::string* response,
                               ::google::protobuf::Message* request_message,
                               ::google::protobuf::Message* response_message,
                               ::google::protobuf::Closure* done);

    bool ParseMethodFullName(const std::string& method_full_name,
            std::string* service_full_name, std::string* method_name);

private:
    RpcClient* _rpc_client;
    RpcChannel* _rpc_channel;
    sofa::pbrpc::builtin::BuiltinService_Stub* _stub;
    std::string _server_address;

    RWLock _desc_lock;
    ::google::protobuf::DescriptorPool* _desc_pool;
    ::google::protobuf::MessageFactory* _msg_factory;
    typedef std::map<std::string, const ::google::protobuf::ServiceDescriptor*> ServiceDescriptorMap;
    ServiceDescriptorMap _svc_desc_map;
};

} // namespace http_agent
} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBPRC_HTTP_AGENT_HTTP_AGENT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
