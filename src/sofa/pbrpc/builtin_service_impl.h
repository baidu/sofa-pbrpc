// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_BUILTIN_SERVICE_IMPL_H_
#define _SOFA_PBRPC_BUILTIN_SERVICE_IMPL_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/builtin_service.pb.h>

namespace sofa {
namespace pbrpc {
namespace builtin {

class BuiltinServiceImpl : public BuiltinService {
public:
    BuiltinServiceImpl(const RpcServerImplWPtr& rpc_server,
                       const ServicePoolWPtr& service_pool,
                       bool disable_list_service = false);
    virtual ~BuiltinServiceImpl();

    virtual void Health(::google::protobuf::RpcController* controller,
            const ::sofa::pbrpc::builtin::HealthRequest* request,
            ::sofa::pbrpc::builtin::HealthResponse* response,
            ::google::protobuf::Closure* done);

    virtual void ServerOptions(::google::protobuf::RpcController* controller,
            const ::sofa::pbrpc::builtin::ServerOptionsRequest* request,
            ::sofa::pbrpc::builtin::ServerOptionsResponse* response,
            ::google::protobuf::Closure* done);

    virtual void UpdateOptions(::google::protobuf::RpcController* controller,
            const ::sofa::pbrpc::builtin::UpdateOptionsRequest* request,
            ::sofa::pbrpc::builtin::UpdateOptionsResponse* response,
            ::google::protobuf::Closure* done);

    virtual void ServerStatus(::google::protobuf::RpcController* controller,
            const ::sofa::pbrpc::builtin::ServerStatusRequest* request,
            ::sofa::pbrpc::builtin::ServerStatusResponse* response,
            ::google::protobuf::Closure* done);

    virtual void ListService(::google::protobuf::RpcController* controller,
            const ::sofa::pbrpc::builtin::ListServiceRequest* request,
            ::sofa::pbrpc::builtin::ListServiceResponse* response,
            ::google::protobuf::Closure* done);

    virtual void Stat(::google::protobuf::RpcController* controller,
            const ::sofa::pbrpc::builtin::StatRequest* request,
            ::sofa::pbrpc::builtin::StatResponse* response,
            ::google::protobuf::Closure* done);

private:
    RpcServerImplWPtr _rpc_server;
    ServicePoolWPtr _service_pool;
    bool _disable_list_service;

    MutexLock _list_service_lock;
    ListServiceResponse _list_service_last_response;
    int _list_service_last_count;
};

} // namespace builtin
} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BUILTIN_SERVICE_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
