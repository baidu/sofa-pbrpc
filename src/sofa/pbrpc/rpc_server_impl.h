// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_SERVER_IMPL_H_
#define _SOFA_PBRPC_RPC_SERVER_IMPL_H_

#include <set>

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sofa/pbrpc/rpc_server.h>
#include <sofa/pbrpc/rpc_server_stream.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/rpc_listener.h>
#include <sofa/pbrpc/service_pool.h>
#include <sofa/pbrpc/thread_group_impl.h>
#include <sofa/pbrpc/timer_worker.h>

namespace sofa {
namespace pbrpc {

class RpcServerImpl : public sofa::pbrpc::enable_shared_from_this<RpcServerImpl>
{
public:
    static const int MAINTAIN_INTERVAL_IN_MS = 100;

public:
    RpcServerImpl(const RpcServerOptions& options,
                  RpcServer::EventHandler* handler);

    virtual ~RpcServerImpl();

    bool Start(const std::string& server_address);

    void Stop();

    PTime GetStartTime();

    RpcServerOptions GetOptions();

    void ResetOptions(const RpcServerOptions& options);

    bool RegisterService(google::protobuf::Service* service, bool take_ownership);

    int ServiceCount();

    int ConnectionCount();

    void GetPendingStat(int64* pending_message_count,
            int64* pending_buffer_size, int64* pending_data_size);

    bool IsListening();

    // Restart the listener.  It will restart listening anyway, even if it is already in
    // listening.  Return false if the server is not started, or fail to restart listening.
    bool RestartListen();

    WebServicePtr GetWebService();

    bool RegisterWebServlet(const std::string& path, Servlet servlet, bool take_ownership);

    Servlet UnregisterWebServlet(const std::string& path);

private:
    void OnCreated(const RpcServerStreamPtr& stream);

    void OnAccepted(const RpcServerStreamPtr& stream);

    void OnAcceptFailed(RpcErrorCode error_code, const std::string& error_text);

    void OnReceived(const RpcServerStreamWPtr& stream, const RpcRequestPtr& request);

    void OnClosed(const RpcServerStreamPtr& stream);

    void StopStreams();

    void ClearStreams();

    void TimerMaintain(const PTime& now);

private:
    struct FlowControlItem
    {
        int token; // always <= 0
        RpcServerStream* stream;

        FlowControlItem(int t, RpcServerStream* s) : token(t), stream(s) {}
        // comparator: nearer to zero, higher priority
        bool operator< (const FlowControlItem& o) const
        {
            return token > o.token;
        }
    };

private:
    RpcServerOptions _options;
    RpcServer::EventHandler* _event_handler;
    volatile bool _is_running;
    MutexLock _start_stop_lock;

    PTime _start_time;
    int64 _ticks_per_second;
    int64 _last_maintain_ticks;
    int64 _last_restart_listen_ticks;
    int64 _last_switch_stat_slot_ticks;
    int64 _last_print_connection_ticks;

    int64 _slice_count;
    int64 _slice_quota_in;
    int64 _slice_quota_out;
    int64 _max_pending_buffer_size;
    int64 _keep_alive_ticks;
    int64 _restart_listen_interval_ticks;
    int64 _switch_stat_slot_interval_ticks;

    ServicePoolPtr _service_pool;

    FlowControllerPtr _flow_controller;

    ThreadGroupImplPtr _maintain_thread_group;

    std::string _server_address;
    RpcEndpoint _listen_endpoint;
    RpcListenerPtr _listener;

    TimerWorkerPtr _timer_worker;

    typedef std::set<RpcServerStreamPtr> StreamSet;
    StreamSet _stream_set;
    FastLock _stream_set_lock;

    IOServicePoolPtr _io_service_pool;

    WebServicePtr _web_service;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcServerImpl);
}; // class RpcServerImpl

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_SERVER_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
