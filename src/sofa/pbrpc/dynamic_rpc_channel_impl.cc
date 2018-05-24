// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/dynamic_rpc_channel_impl.h>
#include <sofa/pbrpc/closure.h>

namespace sofa {
namespace pbrpc {

class EventHandlerImp : public RpcChannel::EventHandler
{
public:
    EventHandlerImp(::sofa::pbrpc::weak_ptr<DynamicRpcChannelImpl> channel) : _channel(channel) {}
    virtual ~EventHandlerImp() {}
    virtual void OnAddressAdded(const std::vector<std::string>& address_list)
    {
        ::sofa::pbrpc::shared_ptr<DynamicRpcChannelImpl> channel = _channel.lock();
        if (channel) {
            channel->OnAddressAdded(address_list);
        }
    }
    virtual void OnAddressRemoved(const std::vector<std::string>& address_list)
    {
        ::sofa::pbrpc::shared_ptr<DynamicRpcChannelImpl> channel = _channel.lock();
        if (channel) {
            channel->OnAddressRemoved(address_list);
        }
    }
private:
    ::sofa::pbrpc::weak_ptr<DynamicRpcChannelImpl> _channel;
};

DynamicRpcChannelImpl::DynamicRpcChannelImpl(
        const RpcClientImplPtr& client_impl,
        ::sofa::pbrpc::RpcChannel::AddressProvider* address_provider,
        const RpcChannelOptions& options)
    : _client_impl(client_impl)
    , _address_provider(address_provider)
    , _options(options)
    , _timer_id(0)
    , _is_detect_disabled(false)
{
    _address_provider->GetInitAddress(&_init_address_list);
}

DynamicRpcChannelImpl::DynamicRpcChannelImpl(
        const RpcClientImplPtr& client_impl,
        const std::vector<std::string>& address_list,
        const RpcChannelOptions& options)
    : _client_impl(client_impl)
    , _address_provider(NULL)
    , _init_address_list(address_list)
    , _options(options)
    , _timer_id(0)
    , _is_detect_disabled(false)
{
}

DynamicRpcChannelImpl::~DynamicRpcChannelImpl()
{
}

bool DynamicRpcChannelImpl::Init()
{
    // add init addresses
    OnAddressAdded(_init_address_list);

    // register event handler
    if (_address_provider) {
        _address_provider->RegisterEventHandler(new EventHandlerImp(shared_from_this()));
    }

    // register TimerDetect
    _timeout_manager.reset(new TimeoutManager());
    _timer_id = _timeout_manager->add_repeating(kDetectInterval,
            ::sofa::pbrpc::NewPermanentExtClosure(shared_from_this(),
                &DynamicRpcChannelImpl::TimerDetect));

#if defined( LOG )
    LOG(INFO) << "Init(): init address count: " << _added_set.size();
#else
    SLOG(INFO, "Init(): init address count: %u", _added_set.size());
#endif
    return true;
}

void DynamicRpcChannelImpl::Stop()
{
    // remove TimerDetect
    _timeout_manager->erase(_timer_id);
    {
        ScopedLocker<LockType> _(_detect_lock);
        _is_detect_disabled = true;
    }
    _timeout_manager.reset();

    // clear SimpleRpcChannelImpl
    {
        ScopedLocker<LockType> _1(_add_remove_lock);
        ScopedLocker<LockType> _2(_map_lock);
        for (ServerContextMap::const_iterator it = _live_map.begin();
                it != _live_map.end(); ++it) {
            const ServerContextPtr& server = it->second;
            if (server->is_channel_inited) {
                server->channel->Stop();
            }
        }
        for (ServerContextMap::const_iterator it = _unlive_map.begin();
                it != _unlive_map.end(); ++it) {
            const ServerContextPtr& server = it->second;
            if (server->is_channel_inited) {
                server->channel->Stop();
            }
        }
        _live_map.clear();
        _unlive_map.clear();
    }

#if defined( LOG )
    LOG(INFO) << "Stop(): dynamic rpc channel stopped";
#else
    SLOG(INFO, "Stop(): dynamic rpc channel stopped");
#endif
}

void DynamicRpcChannelImpl::CallMethod(
        const ::google::protobuf::MethodDescriptor* method,
        ::google::protobuf::RpcController* controller,
        const ::google::protobuf::Message* request,
        ::google::protobuf::Message* response,
        ::google::protobuf::Closure* done)
{
    ++_wait_count;

    // prepare controller
    RpcController* sofa_controller = dynamic_cast<RpcController*>(controller);
    SCHECK(sofa_controller != NULL); // should be sofa::pbrpc::RpcController
    RpcControllerImplPtr cntl = sofa_controller->impl();

    // Choose an server.
    // If the server's channel is not created, then create it first.
    ServerContextPtr server;
    RpcErrorCode choose_ret;
    int retry_count = 0;
    while (true) {
        choose_ret = ChooseServer(server, retry_count + 1);
        if (choose_ret == RPC_SUCCESS){
            if (server->InitChannel(_client_impl, _options)) {
                // init ok
                break;
            }
            else  {
#if defined( LOG )
                LOG(ERROR) << "CallMethod(): init channel failed: ["
                           << server->server_address << "]";
#else
                SLOG(ERROR, "CallMethod(): init channel failed: [%s]",
                        server->server_address.c_str());
#endif
                MoveToUnlive(server->server_address, "init channel failed");
                choose_ret = RPC_ERROR_SERVER_UNAVAILABLE;
                server.reset();
            }
        }
        // check retry count
        if (retry_count >= kRetryCount) {
            // retry out
            server.reset();
            break;
        }
        // sleep to retry
        usleep(kRetryInterval * 1000);
        ++retry_count;
    }

    // If no server available, callback with error.
    if (!server) {
#if defined( LOG )
        LOG(ERROR) << "CallMethod(): choose server failed with retry_count="
                   << retry_count << ": " << RpcErrorCodeToString(choose_ret);
#else
        SLOG(ERROR, "CallMethod(): choose server failed with retry_count=%d: %s",
                retry_count, RpcErrorCodeToString(choose_ret));
#endif
        cntl->SetFailed(choose_ret, "choose server failed in dynamic channel");
        CallDone(server, cntl);
        if (done != NULL) {
            _client_impl->GetCallbackThreadGroup()->post(done);
        }
        return;
    }
    SCHECK(server->channel);

    // Call method.
    if (done != NULL) {
        // async call, replace callback with AsyncCallback
        done = ::sofa::pbrpc::NewClosure(shared_from_this(),
                &DynamicRpcChannelImpl::AsyncCallback, server, cntl, done);
    }
    server->last_request_seq = ++_request_count;
    server->channel->CallMethod(method, controller, request, response, done);
    if (done == NULL) {
        // sync call
        CallDone(server, cntl);
    }
}

uint32 DynamicRpcChannelImpl::WaitCount()
{
    return _wait_count;
}

void DynamicRpcChannelImpl::OnAddressAdded(const std::vector<std::string>& address_list)
{
    ScopedLocker<LockType> _(_add_remove_lock);
    // prepare
    AddResultMap add_results;
    for (std::vector<std::string>::const_iterator it = address_list.begin();
            it != address_list.end(); ++it) {
        std::string server_address = *it;
        if (server_address.empty()) {
            // ignore empty address
            continue;
        }
        if (add_results.find(server_address) != add_results.end()) {
            // duplicate address
#if defined( LOG )
            LOG(WARNING) << "OnAddressAdded(): duplicate address: [" << server_address << "]";
#else
            SLOG(WARNING, "OnAddressAdded(): duplicate address: [%s]", server_address.c_str());
#endif
            continue;
        }
        AddResult& result = add_results[server_address];
        result.add_failed = false;
        if (_added_set.find(server_address) != _added_set.end()) {
            result.add_failed = true;
            result.fail_reason = "already added";
            continue;
        }
        result.server_context.reset(new ServerContext(server_address));
    }
    // add into _live_map
    {
        ScopedLocker<LockType> _(_map_lock);
        for (AddResultMap::iterator it = add_results.begin();
                it != add_results.end(); ++it) {
            const std::string& server_address = it->first;
            const AddResult& result = it->second;
            if (!result.add_failed) {
                _live_map[server_address] = result.server_context;
                _added_set.insert(server_address);
            }
        }
    }
    // print log
    for (AddResultMap::iterator it = add_results.begin();
            it != add_results.end(); ++it) {
        const std::string& server_address = it->first;
        const AddResult& result = it->second;
        if (!result.add_failed) {
#if defined( LOG )
            LOG(INFO) << "OnAddressAdded(): add address succeed: [" << server_address << "]";
#else
            SLOG(INFO, "OnAddressAdded(): add address succeed: [%s]", server_address.c_str());
#endif
        }
        else {
#if defined( LOG )
            LOG(ERROR) << "OnAddressAdded(): add address failed: [" << server_address << "]"
                       << result.fail_reason;
#else
            SLOG(ERROR, "OnAddressAdded(): add address failed: [%s]: %s",
                    server_address.c_str(), result.fail_reason.c_str());
#endif
        }
    }
}

void DynamicRpcChannelImpl::OnAddressRemoved(const std::vector<std::string>& address_list)
{
    ScopedLocker<LockType> _(_add_remove_lock);
    // prepare
    RemoveResultMap remove_results;
    for (std::vector<std::string>::const_iterator it = address_list.begin();
            it != address_list.end(); ++it) {
        std::string server_address = *it;
        if (server_address.empty()) {
            // ignore empty address
            continue;
        }
        if (remove_results.find(server_address) != remove_results.end()) {
#if defined( LOG )
            LOG(WARNING) << "OnAddressRemoved(): duplicate address: [" << server_address << "]";
#else
            SLOG(WARNING, "OnAddressRemoved(): duplicate address: [%s]", server_address.c_str());
#endif
            continue;
        }
        RemoveResult& result = remove_results[server_address];
        result.remove_failed = false;
        if (_added_set.find(server_address) == _added_set.end()) {
            result.remove_failed = true;
            result.fail_reason = "not exist";
            continue;
        }
    }
    // remove from _live_map or _unlive_map
    {
        ScopedLocker<LockType> _(_map_lock);
        for (RemoveResultMap::iterator it = remove_results.begin();
                it != remove_results.end(); ++it) {
            const std::string& server_address = it->first;
            const RemoveResult& result = it->second;
            if (!result.remove_failed) {
                ServerContextMap::iterator find = _live_map.find(server_address);
                if (find != _live_map.end()) {
                    // remove from _live_map
                    find->second->is_removed = true;
                    _live_map.erase(find);
                }
                else {
                    // remove from _unlive_map
                    find = _unlive_map.find(server_address);
                    SCHECK(find != _unlive_map.end());
                    find->second->is_removed = true;
                    _unlive_map.erase(find);
                }
                SCHECK_EQ(1u, _added_set.erase(server_address));
            }
        }
    }
    // print log
    for (RemoveResultMap::iterator it = remove_results.begin();
            it != remove_results.end(); ++it) {
        const std::string& server_address = it->first;
        const RemoveResult& result = it->second;
        if (!result.remove_failed) {
#if defined( LOG )
            LOG(INFO) << "OnAddressRemoved(): remove address succeed: [" << server_address << "]";
#else
            SLOG(INFO, "OnAddressRemoved(): remove address succeed: [%s]", server_address.c_str());
#endif
        }
        else {
#if defined( LOG )
            LOG(ERROR) << "OnAddressRemoved(): remove address failed: [" << server_address << "]"
                       << result.fail_reason;
#else
            SLOG(ERROR, "OnAddressRemoved(): remove address failed: [%s]: %s",
                    server_address.c_str(), result.fail_reason.c_str());
#endif
        }
    }
}

void DynamicRpcChannelImpl::CallDone(
        const ServerContextPtr& server,
        const RpcControllerImplPtr& cntl)
{
    --_wait_count;

    if (server) {
        if (cntl->Failed()) {
            server->is_last_request_succeed = false;
            if (server->is_in_live_map) {
                MoveToUnlive(server->server_address, cntl->Reason());
            }
        }
        else {
            server->is_last_request_succeed = true;
        }
    }
}

void DynamicRpcChannelImpl::AsyncCallback(
        const ServerContextPtr& server,
        const RpcControllerImplPtr& cntl,
        ::google::protobuf::Closure* done)
{
    CallDone(server, cntl);
    done->Run();
}

RpcErrorCode DynamicRpcChannelImpl::ChooseServer(ServerContextPtr& choosed_server, int try_num)
{
    choosed_server.reset();
    ScopedLocker<LockType> _(_map_lock);

    // first choose the channel in _live_map with minimal <wait_count,last_request_seq>.
    if (!_live_map.empty()) {
        uint32 min_wait_count = kuint32max;
        uint64 min_last_request_seq = kuint64max;
        for (ServerContextMap::const_iterator it = _live_map.begin();
                it != _live_map.end(); ++it) {
            const ServerContextPtr& server = it->second;
            // if channel is not inited, set wait_count as 0.
            uint32 wait_count = server->is_channel_inited ? server->channel->WaitCount() : 0;
            // compare by <wait_count,last_request_seq>
            if (wait_count < min_wait_count
                    || (wait_count == min_wait_count
                        && server->last_request_seq < min_last_request_seq)) {
                min_wait_count = wait_count;
                min_last_request_seq = server->last_request_seq;
                choosed_server = server;
            }
        }
        if (_options.server_load_capacity > 0 && choosed_server
                && min_wait_count >= _options.server_load_capacity) {
            // all the live servers exceed the load capacity
#if defined( LOG )
            LOG(ERROR) << "ChooseServer(): reached server capacity: try_num=" << try_num
                       << ", live_map_size=" << _live_map.size()
                       << ", min_wait_count=" << min_wait_count
                       << ", load_capacity=" << _options.server_load_capacity;
#else
            SLOG(ERROR, "ChooseServer(): reached server capacity: try_num=%d"
                    ", live_map_size=%u, min_wait_count=%u, load_capacity=%u",
                    try_num, _live_map.size(), min_wait_count,
                    _options.server_load_capacity);
#endif
            choosed_server.reset();
            return RPC_ERROR_SERVER_BUSY;
        }
    }

    // found in _live_map, return it.
    if (choosed_server) {
        return RPC_SUCCESS;
    }

    // then choose the channel in _unlive_map with minimal last_request_seq,
    // and prefer channel with last_request_succeed=true.
    if (!_unlive_map.empty()) {
        bool last_request_succeed = false;
        uint64 min_last_request_seq = kuint64max;
        for (ServerContextMap::const_iterator it = _unlive_map.begin();
                it != _unlive_map.end(); ++it) {
            const ServerContextPtr& server = it->second;
            if (last_request_succeed) {
                if (server->is_last_request_succeed) {
                    if (server->last_request_seq < min_last_request_seq) {
                        min_last_request_seq = server->last_request_seq;
                        choosed_server = server;
                    }
                }
            }
            else { // !last_request_succeed
                if (server->is_last_request_succeed) {
                    last_request_succeed = true;
                    min_last_request_seq = server->last_request_seq;
                    choosed_server = server;
                }
                else { // !server->is_last_request_succeed
                    if (server->last_request_seq < min_last_request_seq) {
                        min_last_request_seq = server->last_request_seq;
                        choosed_server = server;
                    }
                }
            }
        }
    }
    
    if (choosed_server) {
        return RPC_SUCCESS;
    }
    else {
#if defined( LOG )
        LOG(ERROR) << "ChooseServer(): no server choosed: try_num=" << try_num
                   << ", live_map_size=" << _live_map.size()
                   << ", unlive_map_size=" << _unlive_map.size();
#else
        SLOG(ERROR, "ChooseServer(): no server choosed: try_num=%d"
                ", live_map_size=%u, unlive_map_size=%u",
                try_num, _live_map.size(), _unlive_map.size());
#endif
        return RPC_ERROR_SERVER_UNAVAILABLE;
    }
}

void DynamicRpcChannelImpl::MoveToUnlive(
        const std::string& server_address,
        const std::string& reason)
{
    bool succeed = false;
    {
        ScopedLocker<LockType> _(_map_lock);
        ServerContextMap::iterator find = _live_map.find(server_address);
        if (find != _live_map.end()) {
            ServerContextPtr server = find->second;
            _live_map.erase(find);
            server->is_in_live_map = false;
            _unlive_map[server_address] = server;
            succeed = true;
        }
    }
    if (succeed) {
#if defined( LOG )
        LOG(INFO) << "MoveToUnlive(): [" << server_address << "]: " << reason;
#else
        SLOG(INFO, "MoveToUnlive(): [%s]: %s", server_address.c_str(), reason.c_str());
#endif
    }
}

void DynamicRpcChannelImpl::MoveToLive(const std::string& server_address)
{
    bool succeed = false;
    {
        ScopedLocker<LockType> _(_map_lock);
        ServerContextMap::iterator find = _unlive_map.find(server_address);
        if (find != _unlive_map.end()) {
            ServerContextPtr server = find->second;
            _unlive_map.erase(find);
            server->is_in_live_map = true;
            _live_map[server_address] = server;
            succeed = true;
        }
    }
    if (succeed) {
#if defined( LOG )
        LOG(INFO) << "MoveToLive(): [" << server_address << "]";
#else
        SLOG(INFO, "MoveToLive(): [%s]", server_address.c_str());
#endif
    }
}

void DynamicRpcChannelImpl::TimerDetect(
        TimeoutManager::Id /*id*/,
        TimeoutManager::Type type)
{
    if (type == TimeoutManager::ERASED || type == TimeoutManager::CLEARED) {
        // ignore ERASED and CLEARED event
        return;
    }

    ScopedLocker<LockType> _(_detect_lock);
    if (_is_detect_disabled) {
        // disabled
        return;
    }

    ServerContextMap detect_map;
    {
        ScopedLocker<LockType> _(_map_lock);
        detect_map = _unlive_map;
    }
    for (ServerContextMap::iterator it = detect_map.begin();
            it != detect_map.end(); ++it) {
        ServerContextPtr& server = it->second;
        if (server->is_removed || server->is_in_live_map) {
            // ignore
            continue;
        }
        // init builtin service.
        if (!server->InitBuiltinService(_options)) {
            // init failed
#if defined( LOG )
            LOG(WARNING) << "TimerDetect(): init builtin service failed: ["
                         << server->server_address << "]";
#else
            SLOG(WARNING, "TimerDetect(): init builtin service failed: [%s]",
                    server->server_address.c_str());
#endif
            continue;
        }
        SCHECK(server->builtin_service);
        // async detect
        RpcController* cntl = new RpcController();
        cntl->SetTimeout(kDetectTimeout);
        builtin::HealthRequest* request = new builtin::HealthRequest();
        builtin::HealthResponse* response = new builtin::HealthResponse();
        ::google::protobuf::Closure* done = ::sofa::pbrpc::NewClosure(shared_from_this(),
                    &DynamicRpcChannelImpl::DetectCallback, server, cntl, request, response);
        server->builtin_service->Health(cntl, request, response, done);
    }
}

void DynamicRpcChannelImpl::DetectCallback(
        const ServerContextPtr& server,
        RpcController* cntl,
        builtin::HealthRequest* request,
        builtin::HealthResponse* response)
{
    if (!server->is_removed && !server->is_in_live_map
            && !cntl->Failed() && response->health() == "OK") {
        // detect succeed
        MoveToLive(server->server_address);
    }
    delete request;
    delete response;
    delete cntl;
}

DynamicRpcChannelImpl::ServerContext::ServerContext(const std::string& server_address_)
{
    server_address = server_address_;
    is_last_request_succeed = true;
    is_in_live_map = true;
    is_removed = false;
    is_channel_inited = false;
    is_builtin_service_inited = false;
    last_request_seq = 0;
}

DynamicRpcChannelImpl::ServerContext::~ServerContext()
{
}

bool DynamicRpcChannelImpl::ServerContext::InitChannel(
        const RpcClientImplPtr& client_impl,
        const RpcChannelOptions& options)
{
    if (is_channel_inited) {
        return true;
    }
    ScopedLocker<LockType> _(channel_init_lock);
    if (is_channel_inited) {
        // double check
        return true;
    }
    SCHECK(!channel);
    channel.reset(new SimpleRpcChannelImpl(client_impl, server_address, options));
    if (!channel->Init()) {
        // init failed
#if defined( LOG )
        LOG(ERROR) << "InitChannel(): init simple channel failed: [" << server_address << "]";
#else
        SLOG(ERROR, "InitChannel(): init simple channel failed: [%s]", server_address.c_str());
#endif
        channel.reset();
        return false;
    }
#if defined( LOG )
    LOG(INFO) << "InitChannel(): init simple channel succeed: [" << server_address << "]";
#else
    SLOG(INFO, "InitChannel(): init simple channel succeed: [%s]", server_address.c_str());
#endif
    is_channel_inited = true;
    return true;
}

bool DynamicRpcChannelImpl::ServerContext::InitBuiltinService(const RpcChannelOptions& /*options*/)
{
    if (is_builtin_service_inited) {
        return true;
    }
    ScopedLocker<LockType> _(builtin_service_init_lock);
    if (is_builtin_service_inited) {
        // double check
        return true;
    }
    ScopedLocker<LockType> _auto_ref(channel_init_lock);
    if (!channel) {
        // no channel available
        return false;
    }
    builtin_service.reset(new builtin::BuiltinService_Stub(channel.get()));
#if defined( LOG )
    LOG(INFO) << "InitBuiltinService(): init succeed: [" << server_address << "]";
#else
    SLOG(INFO, "InitBuiltinService(): init succeed: [%s]", server_address.c_str());
#endif
    is_builtin_service_inited = true;
    return true;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
