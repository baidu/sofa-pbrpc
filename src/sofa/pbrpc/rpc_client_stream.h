// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_CLIENT_STREAM_H_
#define _SOFA_PBRPC_RPC_CLIENT_STREAM_H_

#include <sofa/pbrpc/rpc_message_stream.h>
#include <sofa/pbrpc/rpc_controller_impl.h>
#include <sofa/pbrpc/rpc_meta.pb.h>

namespace sofa {
namespace pbrpc {

// Callback function when closed client stream.
typedef boost::function<void(
        const RpcClientStreamPtr& /* stream */)> ClosedClientStreamCallback;

class RpcClientStream : public RpcMessageStream<RpcControllerImplPtr>
{
public:
    RpcClientStream(IOService& io_service, const RpcEndpoint& endpoint)
        : RpcMessageStream<RpcControllerImplPtr>(ROLE_TYPE_CLIENT, io_service, endpoint)
    {}

    virtual ~RpcClientStream() 
    {
        SOFA_PBRPC_FUNCTION_TRACE;
        for (ControllerMap::iterator it = _controller_map.begin();
                it != _controller_map.end(); ++it)
        {
            it->second->Done(RPC_ERROR_CONNECTION_CLOSED, _error_message);
        }
    }

    // Set the callback function when closed stream.
    void set_closed_stream_callback(
            const ClosedClientStreamCallback& callback)
    {
        _closed_stream_callback = callback;
    }

    // Get the callback function when closed stream.
    const ClosedClientStreamCallback& closed_stream_callback() const
    {
        return _closed_stream_callback;
    }

    // Get pending process count.
    uint32 pending_process_count()
    {
        ScopedLocker<FastLock> _(_controller_map_lock);
        return _controller_map.size();
    }

    void call_method(const RpcControllerImplPtr& cntl) 
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        if (is_closed())
        {
#if defined( LOG )
            LOG(ERROR) << "call_method(): " << RpcEndpointToString(_remote_endpoint)
                       << ": stream already closed: " << _error_message;
#else
            SLOG(ERROR, "call_method(): %s: stream already closed: %s",
                    RpcEndpointToString(_remote_endpoint).c_str(), _error_message);
#endif
            cntl->Done(RPC_ERROR_CONNECTION_CLOSED, _error_message);
            return;
        }

        // add to map
        uint64 sequence_id = cntl->SequenceId();
        {
            ScopedLocker<FastLock> _(_controller_map_lock);
            _controller_map[sequence_id] = cntl;
        }

        /// !!!Attention: the call may be already done at this point.
        async_send_message(cntl->RequestBuffer(), cntl);
    }

    // erase request from controller map by sequence id.
    void erase_request(uint64 sequence_id)
    {
        ScopedLocker<FastLock> _(_controller_map_lock);
        _controller_map.erase(sequence_id);
    }

private:
    virtual void on_closed()
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        ControllerMap tmp_map;
        {
            ScopedLocker<FastLock> _(_controller_map_lock);
            tmp_map.swap(_controller_map);
        }

        for (ControllerMap::iterator it = tmp_map.begin();
                it != tmp_map.end(); ++it)
        {
            it->second->Done(RPC_ERROR_CONNECTION_CLOSED, _error_message);
        }

        if (_closed_stream_callback)
        {
            _closed_stream_callback(
                    sofa::pbrpc::dynamic_pointer_cast<RpcClientStream>(shared_from_this()));
        }
    }

    virtual bool on_sending(
            const ReadBufferPtr& /* request_message */,
            const RpcControllerImplPtr& cntl)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        // TODO: set rpc_meta.server_timeout here to make it more accurate.

        // if already done (may be done by timeout manager),
        // should cancel sending.
        return !cntl->IsDone() && !cntl->IsStartCancel();
    }

    virtual void on_sent(
            const ReadBufferPtr& request_message,
            const RpcControllerImplPtr& cntl)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        // the request has been sent.
        cntl->NotifyRequestSent(_local_endpoint, request_message->TotalCount());
    }

    virtual void on_send_failed(
            RpcErrorCode error_code,
            const ReadBufferPtr& /* request_message */,
            const RpcControllerImplPtr& cntl)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        // TODO more efficient sync map
        // remove the call handle from map
        uint64 sequence_id = cntl->SequenceId();
        {
            ScopedLocker<FastLock> _(_controller_map_lock);
            _controller_map.erase(sequence_id);
        }

        cntl->Done(error_code, _error_message);
    }

    virtual void on_received(
            const ReadBufferPtr& message,
            int meta_size,
            int64 data_size)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        RpcMeta meta;
        if (!meta.ParseFromBoundedZeroCopyStream(message.get(), meta_size))
        {
#if defined( LOG )
            LOG(ERROR) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                       << ": parse rpc meta failed, ignore";
#else
            SLOG(ERROR, "on_received(): %s: parse rpc meta failed, ignore",
                    RpcEndpointToString(_remote_endpoint).c_str());
#endif
            return;
        }

        if (meta.type() != RpcMeta::RESPONSE)
        {
            // TODO handle un-expected message type
            //
            // just ignore it
#if defined( LOG )
            LOG(ERROR) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                       << " {" << meta.sequence_id() << "}"
                       << ": un-expected message type " << meta.type() << ", ignore";
#else
            SLOG(ERROR, "on_received(): %s {%lu}: un-expected message type %d, ignore",
                    RpcEndpointToString(_remote_endpoint).c_str(),
                    meta.sequence_id(), meta.type());
#endif
            return;
        }

        // find corresponding call handle and erase from map
        // TODO more efficient sync map
        RpcControllerImplPtr cntl;
        {
            ScopedLocker<FastLock> _(_controller_map_lock);
            ControllerMap::iterator it = _controller_map.find(meta.sequence_id());
            if (it != _controller_map.end())
            {
                cntl = it->second;
                _controller_map.erase(it);
            }
        }

        if (!cntl)
        {
            // TODO handle un-expected sequence id
            //
            // just ignore it
#if defined( LOG )
            LOG(WARNING) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                         << " {" << meta.sequence_id() << "}"
                         << ": sequence id not found (maybe already timeout), ignore";
#else
            SLOG(WARNING, "on_received(): %s {%lu}: sequence id not found (maybe already timeout), ignore",
                    RpcEndpointToString(_remote_endpoint).c_str(), meta.sequence_id());
#endif
            return;
        }

        if (cntl->IsDone())
        {
            // just ignore it
#if defined( LOG )
            LOG(WARNING) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                         << " {" << meta.sequence_id() << "}"
                         << ": request already done (maybe already timeout), ignore";
#else
            SLOG(WARNING, "on_received(): %s {%lu}: request already done (maybe already timeout), ignore",
                    RpcEndpointToString(_remote_endpoint).c_str(), meta.sequence_id());
#endif
            return;
        }

        if (!meta.has_failed())
        {
#if defined( LOG )
            LOG(ERROR) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                       << " {" << meta.sequence_id() << "}"
                       << ": bad rpc meta: \"failed\" field not set";
#else
            SLOG(ERROR,  "on_received(): %s {%lu}: bad rpc meta: \"failed\" field not set",
                    RpcEndpointToString(_remote_endpoint).c_str(), meta.sequence_id());
#endif
            cntl->Done(RPC_ERROR_PARSE_RESPONSE_MESSAGE, "rpc meta: \"failed\" field not set");
        }
        else if (meta.failed())
        {
            if (!meta.has_error_code())
            {
#if defined( LOG )
                LOG(ERROR) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                           << " {" << meta.sequence_id() << "}"
                           << ": bad rpc meta: \"error_code\" field not set";
#else
                SLOG(ERROR,  "on_received(): %s {%lu}: bad rpc meta: \"error_code\" field not set",
                        RpcEndpointToString(_remote_endpoint).c_str(), meta.sequence_id());
#endif
                cntl->Done(RPC_ERROR_PARSE_RESPONSE_MESSAGE, "rpc meta: \"error_code\" field not set");
            }
            else if (!meta.has_reason())
            {
#if defined( LOG )
                LOG(ERROR) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                           << " {" << meta.sequence_id() << "}"
                           << ": bad rpc meta: \"reason\" field not set";
#else
                SLOG(ERROR,  "on_received(): %s {%lu}: bad rpc meta: \"reason\" field not set",
                        RpcEndpointToString(_remote_endpoint).c_str(), meta.sequence_id());
#endif
                cntl->Done(RPC_ERROR_PARSE_RESPONSE_MESSAGE, "rpc meta: \"reason\" field not set");
            }
            else
            {
                cntl->Done(meta.error_code(), meta.reason());
            }
        }
        else // !meta.failed()
        {
            SCHECK_EQ(data_size, message->TotalCount() - message->ByteCount());
            cntl->SetResponseBuffer(message);
            cntl->SetResponseCompressType(meta.has_compress_type() ?
                    meta.compress_type() : CompressTypeNone);
            cntl->Done(RPC_SUCCESS, "succeed");
        }
    }

private:
    ClosedClientStreamCallback _closed_stream_callback;

    // sequence_id ==> RpcControllerImplPtr
    // TODO more efficient sync map
    typedef std::map<uint64, RpcControllerImplPtr> ControllerMap;
    ControllerMap _controller_map;
    FastLock _controller_map_lock;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcClientStream);
}; // class RpcClientStream

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_CLIENT_STREAM_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
