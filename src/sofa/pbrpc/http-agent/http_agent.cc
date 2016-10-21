// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <google/protobuf/dynamic_message.h>
#include <google/protobuf/text_format.h>

#include <sofa/pbrpc/http-agent/http_agent.h>
#include <sofa/pbrpc/builtin_service.pb.h>
#include <sofa/pbrpc/rpc_controller_impl.h>
#include <sofa/pbrpc/pbjson.h>
#include <sofa/pbrpc/string_utils.h>

namespace sofa {
namespace pbrpc {
namespace http_agent {

HttpAgent::HttpAgent(RpcClient* rpc_client)
    : _rpc_client(rpc_client)
    , _rpc_channel(NULL)
    , _stub(NULL)
    , _desc_pool(NULL)
    , _msg_factory(NULL)
{
}

HttpAgent::~HttpAgent()
{
    delete _msg_factory;
    delete _desc_pool;
    delete _stub;
    delete _rpc_channel;
}

bool HttpAgent::Init(const std::string& server_address)
{
    _server_address = server_address;
    _rpc_channel = new RpcChannel(_rpc_client, _server_address);
    _stub = new sofa::pbrpc::builtin::BuiltinService_Stub(_rpc_channel);
    return UpdateServiceInfo();
}

bool HttpAgent::UpdateServiceInfo()
{
    if (_rpc_channel == NULL) return false;
    RpcController cntl;
    cntl.SetTimeout(3000);
    sofa::pbrpc::builtin::ListServiceRequest request;
    sofa::pbrpc::builtin::ListServiceResponse response;
    _stub->ListService(&cntl, &request, &response, NULL);
    if (cntl.Failed()) {
        SLOG(ERROR, "UpdateServiceInfo(): call ListService failed: %s", cntl.ErrorText().c_str());
        return false;
    }

    WriteLocker _(&_desc_lock);
    if (_desc_pool != NULL) {
        delete _desc_pool;
        _desc_pool = NULL;
        delete _msg_factory;
        _msg_factory = NULL;
        _svc_desc_map.clear();
    }
    _desc_pool = new ::google::protobuf::DescriptorPool();
    for (int i = 0; i < response.files_size(); ++i) {
        const ::google::protobuf::FileDescriptorProto& d = response.files(i);
        const ::google::protobuf::FileDescriptor* fd = _desc_pool->BuildFile(d);
        if (fd == NULL) {
            SLOG(ERROR, "UpdateServiceInfo(): build file descriptor failed: %s", d.name().c_str());
            delete _desc_pool;
            _desc_pool = NULL;
            return false;
        }
        SLOG(INFO, "UpdateServiceInfo(): add file [%s]", d.name().c_str());
    }
    for (int j = 0; j < response.services_size(); ++j) {
        const std::string& s = response.services(j);
        SLOG(INFO, "UpdateServiceInfo(): add service [%s]", s.c_str());
        const ::google::protobuf::ServiceDescriptor* sd = _desc_pool->FindServiceByName(s);
        if (sd == NULL) {
            SLOG(ERROR, "UpdateServiceInfo(): find service descriptor failed: %s", s.c_str());
            delete _desc_pool;
            _desc_pool = NULL;
            return false;
        }
        _svc_desc_map[s] = sd;
    }
    _msg_factory = new ::google::protobuf::DynamicMessageFactory(_desc_pool);
    return true;
}

bool HttpAgent::ListService(std::map<std::string, ::google::protobuf::ServiceDescriptorProto>* service_map)
{
    ReadLocker _(&_desc_lock);
    if (_desc_pool == NULL) {
        SLOG(ERROR, "ListService(): descriptor pool not exist");
        return false;
    }

    for (ServiceDescriptorMap::iterator it = _svc_desc_map.begin();
            it != _svc_desc_map.end(); ++it) {
        const ::google::protobuf::ServiceDescriptor* sd = it->second;
        ::google::protobuf::ServiceDescriptorProto& sdp = (*service_map)[sd->full_name()];
        sd->CopyTo(&sdp);
    }
    return true;
}

bool HttpAgent::ListService(std::map<std::string, std::string>* desc_map)
{
    ReadLocker _(&_desc_lock);
    if (_desc_pool == NULL) {
        SLOG(ERROR, "ListService(): descriptor pool not exist");
        return false;
    }

    for (ServiceDescriptorMap::iterator it = _svc_desc_map.begin();
            it != _svc_desc_map.end(); ++it) {
        const ::google::protobuf::ServiceDescriptor* sd = it->second;
        (*desc_map)[sd->full_name()] = sd->DebugString();
    }
    return true;
}

bool HttpAgent::GetDescriptor(const std::string& name, ProtobufType* type, std::string* desc)
{
    ReadLocker _(&_desc_lock);
    if (_desc_pool == NULL) {
        SLOG(ERROR, "GetDescriptor(): descriptor pool not exist");
        return false;
    }

    const ::google::protobuf::ServiceDescriptor* sd = _desc_pool->FindServiceByName(name);
    if (sd != NULL) {
        *type = PT_SERVICE;
        *desc = sd->DebugString();
        return true;
    }

    const ::google::protobuf::Descriptor* d = _desc_pool->FindMessageTypeByName(name);
    if (d != NULL) {
        *type = PT_MESSAGE;
        *desc = d->DebugString();
        return true;
    }

    const ::google::protobuf::EnumDescriptor* ed = _desc_pool->FindEnumTypeByName(name);
    if (ed != NULL) {
        *type = PT_ENUM;
        *desc = ed->DebugString();
        return true;
    }

    SLOG(ERROR, "GetDescriptor(): type not found: %s", name.c_str());
    return false;
}

void HttpAgent::CallMethod(const std::string& method_full_name,
                           ::google::protobuf::RpcController* controller,
                           const std::string* request,
                           std::string* response,
                           ::google::protobuf::Closure* done)
{
    SCHECK(controller != NULL);
    SCHECK(request != NULL);
    SCHECK(response != NULL);

    RpcController* sofa_controller = dynamic_cast<RpcController*>(controller);
    SCHECK(sofa_controller != NULL); // should pass sofa::pbrpc::RpcController to CallMethod
    RpcControllerImplPtr cntl = sofa_controller->impl();

    if (_rpc_channel == NULL) {
        SLOG(ERROR, "CallMethod(): server not avaliable");
        cntl->SetFailed(RPC_ERROR_CREATE_STREAM, "server not avaliable");
        if (done != NULL) done->Run();
        return;
    }

    if (_desc_pool == NULL && !UpdateServiceInfo()) {
        SLOG(ERROR, "CallMethod(): fetch service descriptor from server failed");
        cntl->SetFailed(RPC_ERROR_CREATE_STREAM, "fetch service descriptor from server failed");
        if (done != NULL) done->Run();
        return;
    }

    std::string service_full_name;
    std::string method_name;
    if (!ParseMethodFullName(method_full_name, &service_full_name, &method_name)) {
        SLOG(ERROR, "CallMethod(): invalid method name: %s", method_full_name.c_str());
        cntl->SetFailed(RPC_ERROR_PARSE_METHOD_NAME, "invalid method name");
        if (done != NULL) done->Run();
        return;
    }

    ReadLocker _(&_desc_lock);
    ServiceDescriptorMap::iterator find = _svc_desc_map.find(service_full_name);
    if (find ==  _svc_desc_map.end()) {
        SLOG(ERROR, "CallMethod(): service not found: %s", service_full_name.c_str());
        cntl->SetFailed(RPC_ERROR_FOUND_SERVICE, "service not found");
        if (done != NULL) done->Run();
        return;
    }
    const ::google::protobuf::ServiceDescriptor* sd = find->second;
    const ::google::protobuf::MethodDescriptor* md = sd->FindMethodByName(method_name);
    if (md == NULL) {
        SLOG(ERROR, "CallMethod(): method not found: %s", method_full_name.c_str());
        cntl->SetFailed(RPC_ERROR_FOUND_METHOD, "method not found");
        if (done != NULL) done->Run();
        return;
    }
    const ::google::protobuf::Descriptor* id = md->input_type();
    const ::google::protobuf::Descriptor* od = md->output_type();
    ::google::protobuf::Message* request_message = _msg_factory->GetPrototype(id)->New();
    std::string request_str = StringUtils::trim(*request);
    bool parse_failed = false;
    if (request_str.size() > 0 && request_str[0] == '{') {
        // try parsing as json string, as json message must begin with '{'
        rapidjson::Document* doc = new rapidjson::Document();
        doc->Parse<0>(request_str.c_str());
        std::string err;
        if (doc->HasParseError()) {
            err = doc->GetParseError();
            parse_failed = true;
        }
        else if (jsonobject2pb(doc, request_message, err) < 0) {
            parse_failed = true;
        }
        delete doc;
        if (parse_failed) {
            SLOG(ERROR, "Error parsing json-format %s: %s", id->full_name().c_str(), err.c_str());
        }
    }
    else {
        // try parsing as text string
        if (!::google::protobuf::TextFormat::ParseFromString(request_str, request_message)) {
            parse_failed = true;
        }
    }
    if (parse_failed) {
        delete request_message;
        SLOG(ERROR, "CallMethod(): parse request message failed");
        cntl->SetFailed(RPC_ERROR_PARSE_REQUEST_MESSAGE, "parse request message failed");
        if (done != NULL) done->Run();
        return;
    }
    ::google::protobuf::Message* response_message = _msg_factory->GetPrototype(od)->New();
    ::google::protobuf::Closure* real_done = NULL;
    if (done == NULL) {
        // sync call
        real_done = done;
    }
    else {
        // async call
        real_done = NewClosure(&HttpAgent::CallMethodDone,
                cntl, response, request_message, response_message, done);
    }
    _rpc_channel->CallMethod(md, controller, request_message, response_message, real_done);
    if (done == NULL) {
        // sync call
        if (!cntl->Failed()) {
            if (!::google::protobuf::TextFormat::PrintToString(*response_message, response)) {
                SLOG(ERROR, "CallMethod(): print response message failed");
                cntl->SetFailed(RPC_ERROR_SERIALIZE_RESPONSE, "print response message failed");
            }
        }
        delete request_message;
        delete response_message;
    }
}

void HttpAgent::CallMethodDone(const RpcControllerImplPtr& cntl,
                               std::string* response,
                               ::google::protobuf::Message* request_message,
                               ::google::protobuf::Message* response_message,
                               ::google::protobuf::Closure* done)
{
    if (!cntl->Failed()) {
        if (!::google::protobuf::TextFormat::PrintToString(*response_message, response)) {
            SLOG(ERROR, "CallMethod(): print response message failed");
            cntl->SetFailed(RPC_ERROR_SERIALIZE_RESPONSE, "print response message failed");
        }
    }
    delete request_message;
    delete response_message;
    done->Run();
}

bool HttpAgent::ParseMethodFullName(const std::string& method_full_name,
        std::string* service_full_name, std::string* method_name)
{
    std::string::size_type pos = method_full_name.rfind('.');
    if (pos == std::string::npos) return false;
    *service_full_name = method_full_name.substr(0, pos);
    *method_name = method_full_name.substr(pos + 1);
    return true;
}

} // namespace http_agent
} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
