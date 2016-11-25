// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_LISTENER_H_
#define _SOFA_PBRPC_RPC_LISTENER_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/locks.h>
#include <sofa/pbrpc/io_service_pool.h>

namespace sofa {
namespace pbrpc {

using boost::asio::ip::tcp;

class RpcListener : public sofa::pbrpc::enable_shared_from_this<RpcListener>
{
    // Callback function when created or accepted a new connection.
    typedef boost::function<void(const RpcServerStreamPtr& /* stream */)> Callback;
    typedef boost::function<void(RpcErrorCode /* error_code */,
            const std::string& /* error_text */)> FailCallback;

public:
    static const int LISTEN_MAX_CONNECTIONS = 4096;
    
public:
    RpcListener(IOServicePoolPtr& io_service_pool, const RpcEndpoint& endpoint)
        : _io_service_pool(io_service_pool)
        , _endpoint(endpoint)
        , _endpoint_str(RpcEndpointToString(endpoint))
        , _acceptor(io_service_pool->GetIOService())
        , _is_closed(true)
    {
        SOFA_PBRPC_INC_RESOURCE_COUNTER(RpcListener);
    }

    virtual ~RpcListener()
    {
        SOFA_PBRPC_FUNCTION_TRACE;
        close();
        SOFA_PBRPC_DEC_RESOURCE_COUNTER(RpcListener);
    }

    void close()
    {
        ScopedLocker<MutexLock> _(_close_lock);
        if (_is_closed) return;
        _is_closed = true;

        boost::system::error_code ec;
        _acceptor.cancel(ec);
        _acceptor.close(ec);

#if defined( LOG )
        LOG(INFO) << "close(): listener closed: " << _endpoint_str;
#else
        SLOG(INFO, "close(): listener closed: %s", _endpoint_str.c_str());
#endif
    }

    bool is_closed()
    {
        ScopedLocker<MutexLock> _(_close_lock);
        return _is_closed;
    }

    // Set the callback funtion when created a new connection.
    template <typename T>
    void set_create_callback(const T& create_callback)
    {
        _create_callback = create_callback;
    }

    // Set the callback funtion when accepted a new connection.
    template <typename T>
    void set_accept_callback(const T& accept_callback)
    {
        _accept_callback = accept_callback;
    }

    // Set the callback funtion when failed to accept connection.
    template <typename T>
    void set_accept_fail_callback(const T& accept_fail_callback)
    {
        _accept_fail_callback = accept_fail_callback;
    }

    // Start listen.  Return false if failed.
    bool start_listen()
    {
        boost::system::error_code ec;

        _acceptor.open(_endpoint.protocol(), ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "start_listen(): open acceptor failed: "
                       << _endpoint_str << ": " << ec.message();
#else
            SLOG(ERROR, "start_listen(): open acceptor failed: %s: %s",
                    _endpoint_str.c_str(), ec.message().c_str());
#endif
            return false;
        }

        int ret = fcntl(_acceptor.native(), F_SETFD, 
                        fcntl(_acceptor.native(), F_GETFD) | FD_CLOEXEC);
        if (ret < 0)
        {
#if defined( LOG )
            LOG(ERROR) << "start_listen(): make fd close_on_exec failed: "
                       << _endpoint_str << ": " << strerror(errno);
#else
            SLOG(ERROR, "start_listen(): make fd close_on_exec failed: %s: %s",
                    _endpoint_str.c_str(), strerror(errno));
#endif
            return false;
        }

        _acceptor.set_option(tcp::acceptor::reuse_address(true), ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "start_listen(): set acceptor option failed: "
                       << _endpoint_str << ": " << ec.message();
#else
            SLOG(ERROR, "start_listen(): set acceptor option failed: %s: %s",
                    _endpoint_str.c_str(), ec.message().c_str());
#endif
            return false;
        }

        _acceptor.bind(_endpoint, ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "start_listen(): bind acceptor failed: "
                       << _endpoint_str << ": " << ec.message();
#else
            SLOG(ERROR, "start_listen(): bind acceptor failed: %s: %s",
                    _endpoint_str.c_str(), ec.message().c_str());
#endif
            return false;
        }

        //_acceptor.listen(boost::asio::socket_base::max_connections, ec);
        _acceptor.listen(LISTEN_MAX_CONNECTIONS, ec);
        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "start_listen(): listen acceptor failed: "
                       << _endpoint_str << ": " << ec.message();
#else
            SLOG(ERROR, "start_listen(): listen acceptor failed: %s: %s",
                    _endpoint_str.c_str(), ec.message().c_str());
#endif
            return false;
        }

#if defined( LOG )
        LOG(INFO) << "start_listen(): listen succeed: " << _endpoint_str;
#else
        SLOG(INFO, "start_listen(): listen succeed: %s", _endpoint_str.c_str());
#endif
        _is_closed = false;

        async_accept();

        return true;
    }

private:
    void async_accept()
    {
        RpcServerStreamPtr stream(new RpcServerStream(_io_service_pool->GetIOService()));

        if (_create_callback)
            _create_callback(stream);

        _acceptor.async_accept(stream->socket(), boost::bind(
                    &RpcListener::on_accept, shared_from_this(), stream, _1));
    }

    void on_accept(const RpcServerStreamPtr& stream,
            const boost::system::error_code& ec)
    {
        if (_is_closed)
            return;

        if (ec)
        {
#if defined( LOG )
            LOG(ERROR) << "on_accept(): accept error at "
                       << _endpoint_str << ": " << ec.message();
#else
            SLOG(ERROR, "on_accept(): accept error at %s: %s",
                    _endpoint_str.c_str(), ec.message().c_str());
#endif

            close();

            if (_accept_fail_callback)
            {
                RpcErrorCode error_code = ec == boost::asio::error::no_descriptors ?
                    RPC_ERROR_TOO_MANY_OPEN_FILES : RPC_ERROR_UNKNOWN;
                _accept_fail_callback(error_code, ec.message());
            }
        }
        else
        {
            stream->update_remote_endpoint();

            if (!stream->is_closed() && _accept_callback)
            {
                _accept_callback(stream);
            }

            if (!stream->is_closed())
            {
                stream->set_socket_connected();
            }

            if (!stream->is_closed())
            {
#if defined( LOG )
                LOG(INFO) << "on_accept(): accept connection at "
                          << _endpoint_str << ": " << RpcEndpointToString(stream->remote_endpoint());
#else
                SLOG(INFO, "on_accept(): accept connection at %s: %s",
                        _endpoint_str.c_str(), RpcEndpointToString(stream->remote_endpoint()).c_str());
#endif
            }

            async_accept();
        }
    }

private:
    IOServicePoolPtr& _io_service_pool;
    RpcEndpoint _endpoint;
    std::string _endpoint_str;
    Callback _create_callback;
    Callback _accept_callback;
    FailCallback _accept_fail_callback;
    tcp::acceptor _acceptor;
    volatile bool _is_closed;
    MutexLock _close_lock;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcListener);
}; // class RpcListener

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_LISTENER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
