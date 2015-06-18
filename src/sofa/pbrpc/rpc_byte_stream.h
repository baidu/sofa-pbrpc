// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_BYTE_STREAM_H_
#define _SOFA_PBRPC_RPC_BYTE_STREAM_H_

#include <cstdio> // for snprintf()
#include <cstring> // for memset()

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_endpoint.h>

namespace sofa {
namespace pbrpc {

using boost::asio::ip::tcp;

class RpcByteStream : public sofa::pbrpc::enable_shared_from_this<RpcByteStream>
{
public:
    RpcByteStream(IOService& io_service, const RpcEndpoint& endpoint)
        : _io_service(io_service)
        , _remote_endpoint(endpoint)
        , _ticks(0)
        , _last_rw_ticks(0)
        , _socket(io_service)
        , _status(STATUS_INIT)
    {
        SOFA_PBRPC_INC_RESOURCE_COUNTER(RpcByteStream);
        memset(_error_message, 0, sizeof(_error_message));
    }

    virtual ~RpcByteStream()
    {
        SOFA_PBRPC_FUNCTION_TRACE;
        close("stream destructed");
        boost::system::error_code ec;
        _socket.close(ec);
        SOFA_PBRPC_DEC_RESOURCE_COUNTER(RpcByteStream);
    }

    // Close the channel.
    void close(const std::string& reason)
    {
        // should run for only once
        if (atomic_swap(&_status, (int)STATUS_CLOSED) != STATUS_CLOSED)
        {
            snprintf(_error_message, sizeof(_error_message), "%s", reason.c_str());
            boost::system::error_code ec;
            _socket.shutdown(tcp::socket::shutdown_both, ec);
            on_closed();
            if (_remote_endpoint != RpcEndpoint())
            {
#if defined( LOG )
                LOG(INFO) << "close(): connection closed: "
                          << RpcEndpointToString(_remote_endpoint)
                          << ": " << _error_message;
#else
                SLOG(INFO, "close(): connection closed: %s: %s",
                        RpcEndpointToString(_remote_endpoint).c_str(), _error_message);
#endif
            }
        }
    }

    // Connect the channel.  Used by client.
    void async_connect()
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        _last_rw_ticks = _ticks;

        _status = STATUS_CONNECTING;
        _socket.async_connect(_remote_endpoint,
                boost::bind(&RpcByteStream::on_connect, shared_from_this(), _1));
    }

    // Set socket connected.  Used by server.
    //
    // Precondition:
    // * the "socket" is opened.
    void set_socket_connected()
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        _last_rw_ticks = _ticks;

        boost::system::error_code ec;
        _local_endpoint = _socket.local_endpoint(ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "set_socket_connected(): get local endpoint failed: "
                       << ec.message();
#else
            SLOG(ERROR, "set_socket_connected(): get local endpoint failed: %s",
                    ec.message().c_str());
#endif
            close("init stream failed: " + ec.message());
            return;
        }
        _remote_endpoint = _socket.remote_endpoint(ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "set_socket_connected(): get remote endpoint failed: "
                       << ec.message();
#else
            SLOG(ERROR, "set_socket_connected(): get remote endpoint failed: %s",
                    ec.message().c_str());
#endif
            close("init stream failed: " + ec.message());
            return;
        }

        if (!on_connected())
        {
#if defined( LOG )
            LOG(ERROR) << "set_socket_connected(): call on_connected() failed";
#else
            SLOG(ERROR, "set_socket_connected(): call on_connected() failed");
#endif
            close("init stream failed: call on_connected() failed");
            return;
        }

        _status = STATUS_CONNECTED;
        trigger_receive();
        trigger_send();
    }

    // Get the socket.
    tcp::socket& socket()
    {
        return _socket;
    }

    // Get the local endpoint.
    const RpcEndpoint& local_endpoint() const
    {
        return _local_endpoint;
    }

    // Get the remote endpoint.
    const RpcEndpoint& remote_endpoint() const
    {
        return _remote_endpoint;
    }

    // Check if the channel is connecting.
    bool is_connecting() const
    {
        return _status == STATUS_CONNECTING;
    }

    // Check if the channel is connected.
    bool is_connected() const
    {
        return _status == STATUS_CONNECTED;
    }

    // Check if the channel is closed.
    bool is_closed() const
    {
        return _status == STATUS_CLOSED;
    }

    // Reset current time ticks.
    void reset_ticks(int64 ticks, bool update_last_rw_ticks)
    {
        _ticks = ticks;
        if (update_last_rw_ticks)
        {
            _last_rw_ticks = ticks;
        }
    }

    // Get the last time ticks for read or write.
    int64 last_rw_ticks() const
    {
        return _last_rw_ticks;
    }

    // Trigger receiving operator.
    // @return true if suceessfully triggered
    virtual bool trigger_receive() = 0;

    // Trigger sending operator.
    // @return true if suceessfully triggered
    virtual bool trigger_send() = 0;

protected:
    // Async read some data from the stream.
    void async_read_some(char* data, size_t size)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        _socket.async_read_some(boost::asio::buffer(data, size),
                boost::bind(&RpcByteStream::on_read_some, 
                    shared_from_this(), _1, _2));
    }

    // Async write some data to the stream.
    void async_write_some(const char* data, size_t size)
    {
        SOFA_PBRPC_FUNCTION_TRACE;
        
        _socket.async_write_some(boost::asio::buffer(data, size),
                boost::bind(&RpcByteStream::on_write_some, 
                    shared_from_this(), _1, _2));
    }

    // Hook function when connected.
    // @return false if some error occured.
    virtual bool on_connected() = 0;

    // Hook function when closed.
    virtual void on_closed() = 0;

    // Callback of "async_read_some()".
    virtual void on_read_some(
            const boost::system::error_code& error,
            std::size_t bytes_transferred) = 0;

    // Callback of "async_write_some()".
    virtual void on_write_some(
            const boost::system::error_code& error,
            std::size_t bytes_transferred) = 0;

private:
    // Callback of "async_connect()".
    void on_connect(const boost::system::error_code& error)
    {
        SOFA_PBRPC_FUNCTION_TRACE;

        if (error)
        {
            // TODO retry connect?
#if defined( LOG )
            LOG(ERROR) << "on_connect(): connect error: "
                       << RpcEndpointToString(_remote_endpoint) << ": " << error.message();
#else
            SLOG(ERROR, "on_connect(): connect error: %s: %s",
                    RpcEndpointToString(_remote_endpoint).c_str(),
                    error.message().c_str());
#endif
            
            close("init stream failed: " + error.message());
            return;
        }

        boost::system::error_code ec;
        _local_endpoint = _socket.local_endpoint(ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "on_connect(): get local endpoint failed: "
                       << ec.message();
#else
            SLOG(ERROR, "on_connect(): get local endpoint failed: %s",
                    ec.message().c_str());
#endif
            close("init stream failed: " + ec.message());
            return;
        }

        if (!on_connected())
        {
#if defined( LOG )
            LOG(ERROR) << "on_connect(): call on_connected() failed";
#else
            SLOG(ERROR, "on_connect(): call on_connected() failed");
#endif
            close("init stream failed: call on_connected() failed");
            return;
        }

#if defined( LOG )
        LOG(INFO) << "on_connect(): connection established: "
                  << RpcEndpointToString(_remote_endpoint);
#else
        SLOG(INFO, "on_connect(): connection established: %s",
                RpcEndpointToString(_remote_endpoint).c_str());
#endif

        _status = STATUS_CONNECTED;
        trigger_receive();
        trigger_send();
    }

protected:
    IOService& _io_service;
    RpcEndpoint _local_endpoint;
    RpcEndpoint _remote_endpoint;
    char _error_message[128];
    volatile int64 _ticks;
    volatile int64 _last_rw_ticks;

private:
    tcp::socket _socket;

    enum {
        STATUS_INIT       = 0,
        STATUS_CONNECTING = 1,
        STATUS_CONNECTED  = 2,
        STATUS_CLOSED     = 3,
    };
    volatile int _status;
}; // class RpcByteStream

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_BYTE_STREAM_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
