// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/builtin_service_impl.h>
#include <sofa/pbrpc/service_pool.h>
#include <sofa/pbrpc/rpc_server_impl.h>

namespace sofa {
namespace pbrpc {
namespace builtin {

// Copy from RpcServerOptions to protobuf ServerOptions.
static void CopyOptions(const RpcServerOptions& options,
        ::sofa::pbrpc::builtin::ServerOptions* options_pb)
{
    options_pb->set_work_thread_num(options.work_thread_num);
    options_pb->set_max_connection_count(options.max_connection_count);
    options_pb->set_keep_alive_time(options.keep_alive_time);
    options_pb->set_max_pending_buffer_size(options.max_pending_buffer_size);
    options_pb->set_max_throughput_in(options.max_throughput_in);
    options_pb->set_max_throughput_out(options.max_throughput_out);
    options_pb->set_disable_builtin_services(options.disable_builtin_services);
    options_pb->set_disable_list_service(options.disable_list_service);
}

BuiltinServiceImpl::BuiltinServiceImpl(
        const RpcServerImplWPtr& rpc_server,
        const ServicePoolWPtr& service_pool,
        bool disable_list_service)
    : _rpc_server(rpc_server)
    , _service_pool(service_pool)
    , _disable_list_service(disable_list_service)
    , _list_service_last_count(0)
{
}

BuiltinServiceImpl::~BuiltinServiceImpl()
{
}

void BuiltinServiceImpl::Health(::google::protobuf::RpcController* /* controller */,
        const ::sofa::pbrpc::builtin::HealthRequest* /* request */,
        ::sofa::pbrpc::builtin::HealthResponse* response,
        ::google::protobuf::Closure* done)
{
    RpcServerImplPtr server = _rpc_server.lock();
    if (server && server->IsListening()) {
        response->set_health("OK");
    } else {
        response->set_health("NotOK");
    }
    response->set_version(SOFA_PBRPC_VERSION);
    response->set_start_time(ptime_to_string(server->GetStartTime()));
    done->Run();
}

void BuiltinServiceImpl::ServerOptions(::google::protobuf::RpcController* controller,
        const ::sofa::pbrpc::builtin::ServerOptionsRequest* /* request */,
        ::sofa::pbrpc::builtin::ServerOptionsResponse* response,
        ::google::protobuf::Closure* done)
{
    RpcServerImplPtr server = _rpc_server.lock();
    if (!server) {
        controller->SetFailed("server not exist");
        done->Run();
        return;
    }
    CopyOptions(server->GetOptions(), response->mutable_options());
    done->Run();
}

void BuiltinServiceImpl::UpdateOptions(::google::protobuf::RpcController* controller,
        const ::sofa::pbrpc::builtin::UpdateOptionsRequest* request,
        ::sofa::pbrpc::builtin::UpdateOptionsResponse* response,
        ::google::protobuf::Closure* done)
{
    if (!request->has_options()) {
        controller->SetFailed("options not set");
        done->Run();
        return;
    }
    const ::sofa::pbrpc::builtin::ServerOptions& request_options = request->options();
    if (!request_options.has_max_connection_count()
            && !request_options.has_keep_alive_time()
            && !request_options.has_max_pending_buffer_size()
            && !request_options.has_max_throughput_in()
            && !request_options.has_max_throughput_out()) {
        controller->SetFailed("no option need to update");
        done->Run();
        return;
    }
    RpcServerImplPtr server = _rpc_server.lock();
    if (!server) {
        controller->SetFailed("server not exist");
        done->Run();
        return;
    }
    RpcServerOptions options = server->GetOptions();
    if (request_options.has_max_connection_count()) {
        options.max_connection_count = request_options.max_connection_count();
    }
    if (request_options.has_keep_alive_time()) {
        options.keep_alive_time = request_options.keep_alive_time();
    }
    if (request_options.has_max_pending_buffer_size()) {
        options.max_pending_buffer_size = request_options.max_pending_buffer_size();
    }
    if (request_options.has_max_throughput_in()) {
        options.max_throughput_in = request_options.max_throughput_in();
    }
    if (request_options.has_max_throughput_out()) {
        options.max_throughput_out = request_options.max_throughput_out();
    }
#if defined( LOG )
    LOG(INFO) << "UpdateOptions(): update by builtin service";
#else
    SLOG(INFO, "UpdateOptions(): update by builtin service");
#endif
    server->ResetOptions(options);
    CopyOptions(server->GetOptions(), response->mutable_options());
    done->Run();
}

void BuiltinServiceImpl::ServerStatus(::google::protobuf::RpcController* controller,
        const ::sofa::pbrpc::builtin::ServerStatusRequest* /* request */,
        ::sofa::pbrpc::builtin::ServerStatusResponse* response,
        ::google::protobuf::Closure* done)
{
    RpcServerImplPtr server = _rpc_server.lock();
    if (!server) {
        controller->SetFailed("server not exist");
        done->Run();
        return;
    }
    int64 pending_message_count;
    int64 pending_buffer_size;
    int64 pending_data_size;
    server->GetPendingStat(&pending_message_count, &pending_buffer_size, &pending_data_size);
    response->set_is_listening(server->IsListening());
    response->set_connection_count(server->ConnectionCount());
    response->set_service_count(server->ServiceCount());
    response->set_pending_message_count(pending_message_count);
    response->set_pending_buffer_size(pending_buffer_size);
    response->set_pending_data_size(pending_data_size);
    done->Run();
}

void BuiltinServiceImpl::ListService(::google::protobuf::RpcController* controller,
        const ::sofa::pbrpc::builtin::ListServiceRequest* /* request */,
        ::sofa::pbrpc::builtin::ListServiceResponse* response,
        ::google::protobuf::Closure* done)
{
    if (_disable_list_service) {
        controller->SetFailed("service disabled");
        done->Run();
        return;
    }
    ScopedLocker<MutexLock> _(_list_service_lock);
    ServicePoolPtr pool = _service_pool.lock();
    if (!pool) {
        controller->SetFailed("service pool not exist");
        done->Run();
        return;
    }
    if (_list_service_last_count == pool->ServiceCount()) {
        response->CopyFrom(_list_service_last_response);
        done->Run();
        return;
    }
    _list_service_last_response.Clear();
    pool->ListService(&_list_service_last_response);
    response->CopyFrom(_list_service_last_response);
    done->Run();
}

void BuiltinServiceImpl::Stat(::google::protobuf::RpcController* controller,
        const ::sofa::pbrpc::builtin::StatRequest* request,
        ::sofa::pbrpc::builtin::StatResponse* response,
        ::google::protobuf::Closure* done)
{
    ServicePoolPtr pool = _service_pool.lock();
    if (!pool) {
        controller->SetFailed("service pool not exist");
        done->Run();
        return;
    }
    response->set_server_start_time(ptime_to_string(pool->RpcServer()->GetStartTime()));
    if (request->service_name() == "all") {
        std::list<ServiceBoard*> svc_list;
        pool->ListService(&svc_list);
        for (std::list<ServiceBoard*>::iterator it = svc_list.begin();
                it != svc_list.end(); ++it) {
            ServiceBoard* board = *it;
            board->LatestStats(request->period_seconds(), response->add_service_stats());
        }
        done->Run();
    }
    else {
        ServiceBoard* board = pool->FindService(request->service_name());
        if (board == NULL) {
            controller->SetFailed("service not exist");
            done->Run();
            return;
        }
        board->LatestStats(request->period_seconds(), response->add_service_stats());
        done->Run();
    }
}

} // namespace builtin
} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
