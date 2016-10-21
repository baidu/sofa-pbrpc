// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_DYNAMIC_RPC_CHANNEL_IMPL_H_
#define _SOFA_PBRPC_DYNAMIC_RPC_CHANNEL_IMPL_H_

#include <map>
#include <set>

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_channel_impl.h>
#include <sofa/pbrpc/simple_rpc_channel_impl.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/timeout_manager.h>
#include <sofa/pbrpc/counter.h>
#include <sofa/pbrpc/builtin_service.pb.h>
#include <sofa/pbrpc/smart_ptr/enable_shared_from_this.hpp>

namespace sofa {
namespace pbrpc {

class DynamicRpcChannelImpl : public RpcChannelImpl,
    public ::sofa::pbrpc::enable_shared_from_this<DynamicRpcChannelImpl>
{
public:
    // Detect timeout, 3 second.
    const static int64 kDetectTimeout = 3000; // in ms

    // Detect interval, 5 second.
    const static int64 kDetectInterval = 5000; // in ms

    // Retry interval and count when choose server.
    const static int kRetryInterval = 800; // in ms
    const static int kRetryCount = 3;

private:
    typedef MutexLock LockType;

    struct ServerContext {
        std::string server_address;
        RpcEndpoint remote_endpoint;
        volatile bool is_last_request_succeed; // if the last request is succeed.
        volatile bool is_in_live_map; // if this channel is in live map.
        volatile bool is_removed; // if this address is already removed.
        volatile bool is_channel_inited; // if the channel is already inited.
        LockType channel_init_lock;
        ::sofa::pbrpc::shared_ptr<SimpleRpcChannelImpl> channel;
        volatile bool is_builtin_service_inited; // if the builtin service is already inited.
        LockType builtin_service_init_lock;
        ::sofa::pbrpc::scoped_ptr<builtin::BuiltinService_Stub> builtin_service;
        uint64 last_request_seq; // the last request sequence number sent by this channel.

        ServerContext(const std::string& server_address_);
        ~ServerContext();
        bool InitChannel(const RpcClientImplPtr& client_impl, const RpcChannelOptions& options);
        bool InitBuiltinService(const RpcChannelOptions& options);
    };
    typedef ::sofa::pbrpc::shared_ptr<ServerContext> ServerContextPtr;
    typedef std::map<std::string, ServerContextPtr> ServerContextMap;

    struct AddResult {
        bool add_failed;
        std::string fail_reason;
        ServerContextPtr server_context;
    };
    typedef std::map<std::string, AddResult> AddResultMap;

    struct RemoveResult {
        bool remove_failed;
        std::string fail_reason;
    };
    typedef std::map<std::string, RemoveResult> RemoveResultMap;

public:
    DynamicRpcChannelImpl(const RpcClientImplPtr& rpc_client_impl,
                          ::sofa::pbrpc::RpcChannel::AddressProvider* address_provider,
                          const RpcChannelOptions& options);

    DynamicRpcChannelImpl(const RpcClientImplPtr& client_impl,
                          const std::vector<std::string>& address_list,
                          const RpcChannelOptions& options);

    virtual ~DynamicRpcChannelImpl();

    virtual bool Init();

    virtual void Stop();

    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done);

    virtual uint32 WaitCount();

    void OnAddressAdded(const std::vector<std::string>& address_list);

    void OnAddressRemoved(const std::vector<std::string>& address_list);

private:
    void CallDone(const ServerContextPtr& server,
                  const RpcControllerImplPtr& cntl);

    void AsyncCallback(const ServerContextPtr& server,
                       const RpcControllerImplPtr& cntl,
                       ::google::protobuf::Closure* done);

    RpcErrorCode ChooseServer(ServerContextPtr& choosed_server, int try_num);

    void MoveToUnlive(const std::string& server_address, const std::string& reason);

    void MoveToLive(const std::string& server_address);

    void TimerDetect(TimeoutManager::Id id, TimeoutManager::Type type);

    void DetectCallback(const ServerContextPtr& server,
                        RpcController* cntl,
                        builtin::HealthRequest* request,
                        builtin::HealthResponse* response);

private:
    RpcClientImplPtr _client_impl;
    ::sofa::pbrpc::RpcChannel::AddressProvider* _address_provider;
    std::vector<std::string> _init_address_list;
    RpcChannelOptions _options;

    LockType _map_lock;
    ServerContextMap _live_map;
    ServerContextMap _unlive_map;

    LockType _add_remove_lock;
    std::set<std::string> _added_set;

    AtomicCounter64 _request_count;
    AtomicCounter _wait_count;

    TimeoutManagerPtr _timeout_manager;
    TimeoutManager::Id _timer_id;
    LockType _detect_lock;
    bool _is_detect_disabled;
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_DYNAMIC_RPC_CHANNEL_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
