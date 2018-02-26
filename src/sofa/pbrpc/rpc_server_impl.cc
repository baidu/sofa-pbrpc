// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
#include <algorithm>

#include <sofa/pbrpc/rpc_server_impl.h>
#include <sofa/pbrpc/rpc_controller_impl.h>
#include <sofa/pbrpc/tran_buf_pool.h>
#include <sofa/pbrpc/flow_controller.h>
#include <sofa/pbrpc/builtin_service_impl.h>
#include <sofa/pbrpc/closure.h>
#include <sofa/pbrpc/ptime.h>
#include <sofa/pbrpc/compressed_stream.h>
#include <sofa/pbrpc/io_service_pool.h>
#include <sofa/pbrpc/web_service.h>

namespace sofa {
namespace pbrpc {

RpcServerImpl::RpcServerImpl(const RpcServerOptions& options,
        RpcServer::EventHandler* handler)
    : _options(options)
    , _event_handler(handler)
    , _is_running(false)
    , _start_time(ptime_now())
    , _ticks_per_second(time_duration_seconds(1).ticks())
    , _last_maintain_ticks(0)
    , _last_restart_listen_ticks(0)
    , _last_switch_stat_slot_ticks(0)
    , _last_print_connection_ticks(0)
{
    _service_pool.reset(new ServicePool(this));

    _web_service.reset(new WebService(_service_pool));
    _web_service->Init();

    _slice_count = std::max(1, 1000 / MAINTAIN_INTERVAL_IN_MS);
    _slice_quota_in = _options.max_throughput_in == -1 ?
        -1 : std::max(0L, _options.max_throughput_in * 1024L * 1024L) / _slice_count;
    _slice_quota_out = _options.max_throughput_out == -1 ?
        -1 : std::max(0L, _options.max_throughput_out * 1024L * 1024L) / _slice_count;
    _max_pending_buffer_size =
        std::max(0L, _options.max_pending_buffer_size * 1024L * 1024L);
    _keep_alive_ticks = _options.keep_alive_time == -1 ?
        -1 : std::max(1, _options.keep_alive_time) * _ticks_per_second;
    _restart_listen_interval_ticks = _ticks_per_second * 3;
    _switch_stat_slot_interval_ticks = _ticks_per_second * STAT_SLOT_SECONDS;

#if defined( LOG )
    LOG(INFO) << "RpcServerImpl(): quota_in="
              << (_slice_quota_in == -1 ? -1 : _slice_quota_in * _slice_count / (1024L * 1024L))
              << "MB/s, quota_out="
              << (_slice_quota_out == -1 ? -1 : _slice_quota_out * _slice_count / (1024L * 1024L))
              << "MB/s, max_pending_buffer_size="
              << (_max_pending_buffer_size / (1024L * 1024L))
              << "MB, keep_alive_time="
              << (_keep_alive_ticks == -1 ? -1 : _keep_alive_ticks / _ticks_per_second)
              << "seconds";
#else
    SLOG(INFO, "RpcServerImpl(): quota_in=%lldMB/s, quota_out=%lldMB/s"
            ", max_pending_buffer_size=%lldMB, keep_alive_time=%lldseconds",
            _slice_quota_in == -1 ? -1 : _slice_quota_in * _slice_count / (1024L * 1024L),
            _slice_quota_out == -1 ? -1 : _slice_quota_out * _slice_count / (1024L * 1024L),
            _max_pending_buffer_size / (1024L * 1024L),
            _keep_alive_ticks == -1 ? -1 : _keep_alive_ticks / _ticks_per_second);
#endif
}

RpcServerImpl::~RpcServerImpl()
{
    SOFA_PBRPC_FUNCTION_TRACE;
    Stop();
    _web_service.reset();
    _service_pool.reset();
    if (_event_handler) delete _event_handler;
}

bool RpcServerImpl::Start(const std::string& server_address)
{
    ScopedLocker<MutexLock> _(_start_stop_lock);
    if (_is_running) return true;

    _flow_controller.reset(new FlowController(
                _slice_quota_in == -1, _slice_quota_in,
                _slice_quota_out == -1, _slice_quota_out));

    _maintain_thread_group.reset(new ThreadGroupImpl(
                1, "sofa_pbrpc_server_maintain_thread_group"));
    if (!_maintain_thread_group->start())
    {
#if defined( LOG )
        LOG(ERROR) << "Start(): start maintain thread group failed";
#else
        SLOG(ERROR, "Start(): start maintain thread group failed");
#endif
        _maintain_thread_group.reset();
        _flow_controller.reset();
    }

    _io_service_pool.reset(new IOServicePool(
                _options.io_service_pool_size, _options.work_thread_num));
    _io_service_pool->set_init_func(_options.work_thread_init_func);
    _io_service_pool->set_dest_func(_options.work_thread_dest_func);
    if (!_io_service_pool->Run())
    {
#if defined( LOG )
        LOG(ERROR) << "Start(): start io service pool failed";
#else
        SLOG(ERROR, "Start(): start io service pool failed");
#endif
        _io_service_pool.reset();
        _maintain_thread_group.reset();
        _flow_controller.reset();
        return false;
    }

    _server_address = server_address;
    if (!ResolveAddress(_io_service_pool->GetIOService(), _server_address, &_listen_endpoint))
    {
#if defined( LOG )
        LOG(ERROR) << "Start(): resolve server address failed: " << _server_address;
#else
        SLOG(ERROR, "Start(): resolve server address failed: %s", _server_address.c_str());
#endif
        _io_service_pool.reset();
        _maintain_thread_group.reset();
        _flow_controller.reset();
        return false;
    }

    _listener.reset(new RpcListener(_io_service_pool, _listen_endpoint));
    _listener->set_create_callback(boost::bind(
                &RpcServerImpl::OnCreated, shared_from_this(), _1));
    _listener->set_accept_callback(boost::bind(
                &RpcServerImpl::OnAccepted, shared_from_this(), _1));
    _listener->set_accept_fail_callback(boost::bind(
                &RpcServerImpl::OnAcceptFailed, shared_from_this(), _1, _2));
    if (!_listener->start_listen())
    {
#if defined( LOG )
        LOG(ERROR) << "Start(): listen failed: " << _server_address;
#else
        SLOG(ERROR, "Start(): listen failed: %s", _server_address.c_str());
#endif
        _listener.reset();
        _io_service_pool.reset();
        _maintain_thread_group.reset();
        _flow_controller.reset();
        return false;
    }
#if defined( LOG )
    LOG(INFO) << "Start(): listen succeed: " << _server_address
              << " [" << RpcEndpointToString(_listen_endpoint) << "]";
#else
    SLOG(INFO, "Start(): listen succeed: %s [%s]",
            _server_address.c_str(), RpcEndpointToString(_listen_endpoint).c_str());
#endif

    _timer_worker.reset(new TimerWorker(_maintain_thread_group->io_service()));
    _timer_worker->set_time_duration(time_duration_milliseconds(MAINTAIN_INTERVAL_IN_MS));
    _timer_worker->set_work_routine(boost::bind(
                &RpcServerImpl::TimerMaintain, shared_from_this(), _1));
    _timer_worker->start();

    if (!_options.disable_builtin_services) {
        _service_pool->RegisterService(new sofa::pbrpc::builtin::BuiltinServiceImpl(
                    shared_from_this(), _service_pool, _options.disable_list_service));
    }

    _is_running = true;
#if defined( LOG )
    LOG(INFO) << "Start(): rpc server started";
#else
    SLOG(INFO, "Start(): rpc server started");
#endif
    return true;
}

void RpcServerImpl::Stop()
{
    ScopedLocker<MutexLock> _(_start_stop_lock);
    if (!_is_running) return;
    _is_running = false;

    _timer_worker->stop();
    _listener->close();
    StopStreams();

    _timer_worker.reset();
    _listener.reset();
    ClearStreams();
    _io_service_pool->Stop();
    _maintain_thread_group->stop();

    _io_service_pool.reset();
    _maintain_thread_group.reset();
    _flow_controller.reset();

#if defined( LOG )
    LOG(INFO) << "Stop(): rpc server stopped";
#else
    SLOG(INFO, "Stop(): rpc server stopped");
#endif
}

PTime RpcServerImpl::GetStartTime()
{
    return _start_time;
}

RpcServerOptions RpcServerImpl::GetOptions()
{
    return _options;
}

void RpcServerImpl::ResetOptions(const RpcServerOptions& options)
{
    int64 old_slice_quota_in = _slice_quota_in;
    int64 old_slice_quota_out = _slice_quota_out;
    int64 old_max_pending_buffer_size = _max_pending_buffer_size;
    int64 old_keep_alive_ticks = _keep_alive_ticks;
    int64 old_max_connection_count = _options.max_connection_count;

    _options.max_throughput_in = options.max_throughput_in;
    _options.max_throughput_out = options.max_throughput_out;
    _options.max_pending_buffer_size = options.max_pending_buffer_size;
    _options.keep_alive_time = options.keep_alive_time;
    _options.max_connection_count = options.max_connection_count;

    _slice_quota_in = _options.max_throughput_in == -1 ?
        -1 : std::max(0L, _options.max_throughput_in * 1024L * 1024L) / _slice_count;
    _slice_quota_out = _options.max_throughput_out == -1 ?
        -1 : std::max(0L, _options.max_throughput_out * 1024L * 1024L) / _slice_count;
    _max_pending_buffer_size = 
        std::max(0L, _options.max_pending_buffer_size * 1024L * 1024L);
    _keep_alive_ticks = _options.keep_alive_time == -1 ?
        -1 : std::max(1, _options.keep_alive_time) * _ticks_per_second;

    if (_max_pending_buffer_size != old_max_pending_buffer_size)
    {
        ScopedLocker<FastLock> _(_stream_set_lock);
        for (StreamSet::iterator it = _stream_set.begin(); it != _stream_set.end(); ++it)
        {
            (*it)->set_max_pending_buffer_size(_max_pending_buffer_size);
        }
    }

    if (_slice_quota_in != old_slice_quota_in)
    {
        _flow_controller->reset_read_quota(_slice_quota_in == -1, _slice_quota_in);
    }

    if (_slice_quota_out != old_slice_quota_out)
    {
        _flow_controller->reset_write_quota(_slice_quota_out == -1, _slice_quota_out);
    }

#if defined( LOG )
    LOG(INFO) << "ResetOptions(): quota_in="
              << (_slice_quota_in == -1 ? -1 : _slice_quota_in * _slice_count / (1024L * 1024L))
              << "MB/s(old "
              << (old_slice_quota_in == -1 ? -1 : old_slice_quota_in * _slice_count / (1024L * 1024L))
              << "MB/s), quota_out="
              << (_slice_quota_out == -1 ? -1 : _slice_quota_out * _slice_count / (1024L * 1024L))
              << "MB/s(old "
              << (old_slice_quota_out == -1 ? -1 : old_slice_quota_out * _slice_count / (1024L * 1024L))
              << "MB/s), max_pending_buffer_size="
              << (_max_pending_buffer_size / (1024L * 1024L))
              << "MB(old "
              << (old_max_pending_buffer_size / (1024L * 1024L))
              << "MB), keep_alive_time="
              << (_keep_alive_ticks == -1 ? -1 : _keep_alive_ticks / _ticks_per_second)
              << "seconds(old "
              << (old_keep_alive_ticks == -1 ? -1 : old_keep_alive_ticks / _ticks_per_second)
              << "seconds), max_connection_count="
              << _options.max_connection_count
              << "(old "
              << old_max_connection_count
              << ")";
#else
    SLOG(INFO, "ResetOptions(): quota_in=%lldMB/s(old %lldMB/s)"
            ", quota_out=%lldMB/s(old %lldMB/s)"
            ", max_pending_buffer_size=%lldMB(old %lldMB)"
            ", keep_alive_time=%lldseconds(old %lldseconds)"
            ", max_connection_count=%lld(old %lld)",
            _slice_quota_in == -1 ? -1 : _slice_quota_in * _slice_count / (1024L * 1024L),
            old_slice_quota_in == -1 ? -1 : old_slice_quota_in * _slice_count / (1024L * 1024L),
            _slice_quota_out == -1 ? -1 : _slice_quota_out * _slice_count / (1024L * 1024L),
            old_slice_quota_out == -1 ? -1 : old_slice_quota_out * _slice_count / (1024L * 1024L),
            _max_pending_buffer_size / (1024L * 1024L),
            old_max_pending_buffer_size / (1024L * 1024L),
            _keep_alive_ticks == -1 ? -1 : _keep_alive_ticks / _ticks_per_second,
            old_keep_alive_ticks == -1 ? -1 : old_keep_alive_ticks / _ticks_per_second,
            _options.max_connection_count,
            old_max_connection_count);
#endif
}

bool RpcServerImpl::RegisterService(google::protobuf::Service* service, bool take_ownership)
{
    return _service_pool->RegisterService(service, take_ownership);
}

int RpcServerImpl::ServiceCount()
{
    return _service_pool->ServiceCount();
}

int RpcServerImpl::ConnectionCount()
{
    ScopedLocker<FastLock> _(_stream_set_lock);
    return _stream_set.size();
}

void RpcServerImpl::GetPendingStat(int64* pending_message_count,
        int64* pending_buffer_size, int64* pending_data_size)
{
    ScopedLocker<FastLock> _(_stream_set_lock);
    int64 message_count = 0;
    int64 buffer_size = 0;
    int64 data_size = 0;
    for (StreamSet::iterator it = _stream_set.begin();
            it != _stream_set.end(); ++it)
    {
        message_count += (*it)->pending_message_count();
        buffer_size += (*it)->pending_buffer_size();
        data_size += (*it)->pending_data_size();
    }
    *pending_message_count = message_count;
    *pending_buffer_size = buffer_size;
    *pending_data_size = data_size;
}

bool RpcServerImpl::IsListening()
{
    ScopedLocker<MutexLock> _(_start_stop_lock);
    return _is_running && !_listener->is_closed();
}

bool RpcServerImpl::RestartListen()
{
    ScopedLocker<MutexLock> _(_start_stop_lock);
    if (!_is_running)
    {
#if defined( LOG )
        LOG(ERROR) << "RestartListen(): server not in running";
#else
        SLOG(ERROR, "RestartListen(): server not in running");
#endif
        return false;
    }

    // close listener first
    _listener->close();

    // reset and restart listener
    _listener.reset(new RpcListener(_io_service_pool, _listen_endpoint));
    _listener->set_create_callback(boost::bind(
                &RpcServerImpl::OnCreated, shared_from_this(), _1));
    _listener->set_accept_callback(boost::bind(
                &RpcServerImpl::OnAccepted, shared_from_this(), _1));
    _listener->set_accept_fail_callback(boost::bind(
                &RpcServerImpl::OnAcceptFailed, shared_from_this(), _1, _2));
    if (!_listener->start_listen())
    {
#if defined( LOG )
        LOG(ERROR) << "RestartListen(): listen failed: " << _server_address;
#else
        SLOG(ERROR, "RestartListen(): listen failed: %s", _server_address.c_str());
#endif
        return false;
    }

#if defined( LOG )
    LOG(INFO) << "RestartListen(): restart listener succeed";
#else
    SLOG(INFO, "RestartListen(): restart listener succeed");
#endif
    return true;
}

void RpcServerImpl::OnCreated(const RpcServerStreamPtr& stream)
{
    stream->set_flow_controller(_flow_controller);
    stream->set_received_request_callback(
            boost::bind(&RpcServerImpl::OnReceived, shared_from_this(), _1, _2));
    stream->set_closed_stream_callback(
            boost::bind(&RpcServerImpl::OnClosed, shared_from_this(), _1));
}

void RpcServerImpl::OnAccepted(const RpcServerStreamPtr& stream)
{
    if (!_is_running)
    {
        stream->close("server not running");
        return;
    }
    
    size_t read_base_block_factor = std::min(_options.read_buffer_base_block_factor, 
                                             (size_t)SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR);
    stream->set_read_buffer_base_block_factor(read_base_block_factor);

    size_t write_base_block_factor = std::min(_options.write_buffer_base_block_factor, 
                                              (size_t)SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR);
    stream->set_write_buffer_base_block_factor(write_base_block_factor);

    stream->set_no_delay(_options.no_delay);

    if (_options.max_connection_count != -1 && ConnectionCount() >= _options.max_connection_count)
    {
        stream->close("exceed max connection count "
                      + boost::lexical_cast<std::string>(_options.max_connection_count));
        return;
    }

    stream->set_max_pending_buffer_size(_max_pending_buffer_size);
    stream->reset_ticks((ptime_now() - _start_time).ticks(), true);

    ScopedLocker<FastLock> _(_stream_set_lock);
    _stream_set.insert(stream);
}

void RpcServerImpl::OnAcceptFailed(RpcErrorCode error_code, const std::string& error_text)
{
    if (!_is_running)
        return;

    // callback
    if (_event_handler)
    {
        _event_handler->NotifyAcceptFailed(error_code, error_text);
    }
}

void RpcServerImpl::OnReceived(const RpcServerStreamWPtr& stream, const RpcRequestPtr& request)
{
    if (!_is_running)
    {
#if defined( LOG )
        LOG(ERROR) << "OnReceived(): " << RpcEndpointToString(request->RemoteEndpoint())
                   << ": server not in running, ignore";
#else
        SLOG(ERROR, "OnReceived(): %s: server not in running, ignore",
                RpcEndpointToString(request->RemoteEndpoint()).c_str());
#endif
        return;
    }

    request->ProcessRequest(stream, _service_pool);
}

void RpcServerImpl::OnClosed(const RpcServerStreamPtr& stream)
{
    if (!_is_running)
        return;

    ScopedLocker<FastLock> _(_stream_set_lock);
    _stream_set.erase(stream);
}

void RpcServerImpl::StopStreams()
{
    ScopedLocker<FastLock> _(_stream_set_lock);
    for (StreamSet::iterator it = _stream_set.begin();
            it != _stream_set.end(); ++it)
    {
        (*it)->close("server stopped");
    }
}

void RpcServerImpl::ClearStreams()
{
    ScopedLocker<FastLock> _(_stream_set_lock);
    _stream_set.clear();
}

void RpcServerImpl::TimerMaintain(const PTime& now)
{
    SOFA_PBRPC_FUNCTION_TRACE;

    int64 now_ticks = (now - _start_time).ticks();

    // check listener, if closed, then try to restart it every interval.
    if (_listener->is_closed()
            && now_ticks - _last_restart_listen_ticks >= _restart_listen_interval_ticks)
    {
        _last_restart_listen_ticks = now_ticks;
        RestartListen();
    }

    // checks which need iterator streams
    if (_keep_alive_ticks != -1 || _slice_quota_in != -1 || _slice_quota_out != -1)
    {
        StreamSet streams;
        {
            ScopedLocker<FastLock> _(_stream_set_lock);
            streams = _stream_set;
        }

        // check keep alive time
        if (_keep_alive_ticks != -1)
        {
            for (StreamSet::iterator it = streams.begin(); it != streams.end(); ++it)
            {
                const RpcServerStreamPtr& stream = *it;
                if (stream->is_closed())
                {
                    continue;
                }
                if (now_ticks - stream->last_rw_ticks() >= _keep_alive_ticks
                        && stream->pending_process_count() == 0)
                {
                    stream->close("keep alive timeout: "
                                  + boost::lexical_cast<std::string>(_options.keep_alive_time)
                                  + " seconds");
                }
                else
                {
                    stream->reset_ticks(now_ticks, false);
                }
            }
        }

        // flow control in
        if (_slice_quota_in != -1)
        {
            // recharge quota pool
            _flow_controller->recharge_read_quota(_slice_quota_in);

            // collect streams need to trigger
            std::vector<FlowControlItem> trigger_list;
            trigger_list.reserve(streams.size());
            for (StreamSet::iterator it = streams.begin(); it != streams.end(); ++it)
            {
                const RpcServerStreamPtr& stream = *it;
                if (stream->is_closed())
                {
                    continue;
                }
                int token = stream->read_quota_token();
                if (token <= 0)
                {
                    // only need trigger streams whose token <= 0
                    trigger_list.push_back(FlowControlItem(token, stream.get()));
                }
            }

            // sort by token: token closer to zero, earlier to trigger
            std::sort(trigger_list.begin(), trigger_list.end());

            // trigger in order
            for (std::vector<FlowControlItem>::iterator t_it = trigger_list.begin();
                    t_it != trigger_list.end(); ++t_it)
            {
                t_it->stream->trigger_receive();
            }
        }

        // flow control out
        if (_slice_quota_out != -1)
        {
            // recharge quota pool
            _flow_controller->recharge_write_quota(_slice_quota_out);

            // collect streams need to trigger
            std::vector<FlowControlItem> trigger_list;
            trigger_list.reserve(streams.size());
            for (StreamSet::iterator it = streams.begin(); it != streams.end(); ++it)
            {
                const RpcServerStreamPtr& stream = *it;
                if (stream->is_closed())
                {
                    continue;
                }
                int token = stream->write_quota_token();
                if (token <= 0)
                {
                    // only need trigger streams whose token <= 0
                    trigger_list.push_back(FlowControlItem(token, stream.get()));
                }
            }

            // sort by token: token closer to zero, earlier to trigger
            std::sort(trigger_list.begin(), trigger_list.end());

            // trigger in order
            for (std::vector<FlowControlItem>::iterator t_it = trigger_list.begin();
                    t_it != trigger_list.end(); ++t_it)
            {
                t_it->stream->trigger_send();
            }
        }
    }

    if (now_ticks - _last_switch_stat_slot_ticks >= _switch_stat_slot_interval_ticks)
    {
        _last_switch_stat_slot_ticks = now_ticks;
        _service_pool->NextSlot();
    }

    _last_maintain_ticks = now_ticks;
}

WebServicePtr RpcServerImpl::GetWebService()
{
    return _web_service;
}

bool RpcServerImpl::RegisterWebServlet(const std::string& path, Servlet servlet, bool take_ownership)
{
    if (!_web_service) 
    {
        return false;
    }
    return _web_service->RegisterServlet(path, servlet, take_ownership);
}

Servlet RpcServerImpl::UnregisterWebServlet(const std::string& path)
{
    if (!_web_service)
    {
        return NULL;
    }
    return _web_service->UnregisterServlet(path);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
