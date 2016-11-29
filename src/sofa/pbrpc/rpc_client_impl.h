// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_CLIENT_IMPL_H_
#define _SOFA_PBRPC_RPC_CLIENT_IMPL_H_

#include <list>
#include <map>

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_client.h>
#include <sofa/pbrpc/rpc_client_stream.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/thread_group_impl.h>
#include <sofa/pbrpc/timer_worker.h>
#include <sofa/pbrpc/rpc_timeout_manager.h>

namespace sofa {
namespace pbrpc {

class RpcClientImpl: public sofa::pbrpc::enable_shared_from_this<RpcClientImpl>
{
public:
    static const int MAINTAIN_INTERVAL_IN_MS = 100;

public:
    explicit RpcClientImpl(const RpcClientOptions& options);

    virtual ~RpcClientImpl();

    void Start();

    void Stop();

    RpcClientOptions GetOptions();

    void ResetOptions(const RpcClientOptions& options);

    int ConnectionCount();

    // Rpc call method to remote endpoint.
    //
    // The call can be done in following cases:
    // * send failed
    // * timeouted
    // * response received
    void CallMethod(const google::protobuf::Message* request,
            google::protobuf::Message* response,
            const RpcControllerImplPtr& cntl);

    const ThreadGroupImplPtr& GetCallbackThreadGroup() const;

    bool ResolveAddress(const std::string& address,
            RpcEndpoint* endpoint);

private:
    // Get stream for "remote_endpoint".  Return null ptr if failed.
    RpcClientStreamPtr FindOrCreateStream(const RpcEndpoint& remote_endpoint);

    void OnClosed(const RpcClientStreamPtr& stream);

    void StopStreams();

    void ClearStreams();

    void DoneCallback(google::protobuf::Message* response,
            const RpcControllerImplPtr& cntl);

    bool ShouldStreamRemoved(const RpcClientStreamPtr& stream);

    void TimerMaintain(const PTime& now);

    uint64 GenerateSequenceId();

private:
    struct FlowControlItem
    {
        int token; // always <= 0
        RpcClientStream* stream;

        FlowControlItem(int t, RpcClientStream* s) : token(t), stream(s) {}
        // comparator: nearer to zero, higher priority
        bool operator< (const FlowControlItem& o) const
        {
            return token > o.token;
        }
    };

private:
    RpcClientOptions _options;
    volatile bool _is_running;
    MutexLock _start_stop_lock;

    AtomicCounter64 _next_request_id;

    PTime _epoch_time;
    int64 _ticks_per_second;
    int64 _last_maintain_ticks;
    int64 _last_print_connection_ticks;

    int64 _slice_count;
    int64 _slice_quota_in;
    int64 _slice_quota_out;
    int64 _max_pending_buffer_size;
    int64 _keep_alive_ticks;

    FlowControllerPtr _flow_controller;

    ThreadGroupImplPtr _maintain_thread_group;
    ThreadGroupImplPtr _callback_thread_group;
    ThreadGroupImplPtr _work_thread_group;

    TimerWorkerPtr _timer_worker;
    RpcTimeoutManagerPtr _timeout_manager;

    typedef std::map<RpcEndpoint, RpcClientStreamPtr> StreamMap;
    StreamMap _stream_map;
    FastLock _stream_map_lock;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcClientImpl);
}; // class RpcClientImpl

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_CLIENT_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
