// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_SERVER_STREAM_H_
#define _SOFA_PBRPC_RPC_SERVER_STREAM_H_

#include <sofa/pbrpc/rpc_message_stream.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/rpc_meta.pb.h>

namespace sofa {
namespace pbrpc {

// Callback function when received request message.
typedef boost::function<void(
        const RpcEndpoint& /* local_endpoint */,
        const RpcEndpoint& /* remote_endpoint */,
        const RpcMeta& /* meta */,
        const RpcServerStreamWPtr& /* stream */,
        const ReadBufferPtr& /* buffer */,
        int64 /*data_size*/)> ReceivedRequestCallback;

/// Callback function when send response message done.
//  * if "status" == RPC_SUCCESS, means send response succeed;
//  * else, means send failed.
typedef boost::function<void(RpcErrorCode /* status */)> SendResponseCallback;

class RpcServerStream : public RpcMessageStream<SendResponseCallback>
{
public:
    RpcServerStream(IOService& io_service)
        : RpcMessageStream<SendResponseCallback>(ROLE_TYPE_SERVER, io_service, RpcEndpoint())
    {}

    virtual ~RpcServerStream() 
    {
        SOFA_PBRPC_FUNCTION_TRACE;
        close("stream destructed");
    }

    // Set the callback function when received request.
    void set_received_request_callback(
            const ReceivedRequestCallback& callback)
    {
        _received_request_callback = callback;
    }

    // Get the callback function when received request.
    const ReceivedRequestCallback& received_request_callback() const
    {
        return _received_request_callback;
    }

    // Send response message.  No timeout is set for sending response.
    // If send done, on mater succeed or failed, SendResponseCallback will be called.
    // @param message  the response message to send, including the header.  not null.
    // @param callback  the callback function when send succeed or failed.  NULL means no callback.
    void send_response(const ReadBufferPtr& message, 
            const SendResponseCallback& callback)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        async_send_message(message, callback);
    }

private:
    virtual void on_closed()
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        // do nothing
    }

    virtual bool on_sending(
            const ReadBufferPtr& /* response_message */,
            const SendResponseCallback& /* callback */)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        return true;
    }

    virtual void on_sent(
            const ReadBufferPtr& /* response_message */,
            const SendResponseCallback& callback)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        if (callback) callback(RPC_SUCCESS);
    }

    virtual void on_send_failed(
            RpcErrorCode error_code,
            const ReadBufferPtr& /* response_message */,
            const SendResponseCallback& callback)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        if (callback) callback(error_code);
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
                       << ": parse rpc meta failed";
#else
            SLOG(ERROR, "on_received(): %s: parse rpc meta failed",
                    RpcEndpointToString(_remote_endpoint).c_str());
#endif
            return;
        }

        if (meta.type() == RpcMeta::REQUEST)
        {
            if (_received_request_callback)
            {
                _received_request_callback(
                        _local_endpoint,
                        _remote_endpoint,
                        meta,
                        RpcServerStreamWPtr(
                            sofa::pbrpc::dynamic_pointer_cast<RpcServerStream>(shared_from_this())),
                        message,
                        data_size);
            }
        }
        else
        {
            // TODO handle un-expected message type
            //
            // just ignore it
#if defined( LOG )
            LOG(ERROR) << "on_received(): " << RpcEndpointToString(_remote_endpoint)
                       << " {" << meta.sequence_id() << "}:"
                       << " un-expected message type: " << meta.type();
#else
            SLOG(ERROR, "on_received(): %s {%lu}: un-expected message type: %d",
                    RpcEndpointToString(_remote_endpoint).c_str(),
                    meta.sequence_id(), meta.type());
#endif
            return;
        }
    }

private:
    ReceivedRequestCallback _received_request_callback;
}; // class RpcServerStream

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_SERVER_STREAM_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
