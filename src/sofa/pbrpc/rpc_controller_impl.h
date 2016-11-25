// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_CONTROLLER_IMPL_H_
#define _SOFA_PBRPC_RPC_CONTROLLER_IMPL_H_

#include <deque>
#include <list>

#include <google/protobuf/descriptor.h>

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/rpc_option.pb.h>
#include <sofa/pbrpc/wait_event.h>

namespace sofa {
namespace pbrpc {

#define CompressTypeAuto ((CompressType)-1)

class RpcControllerImpl : public sofa::pbrpc::enable_shared_from_this<RpcControllerImpl>
{
public:
    typedef boost::function<void(const RpcControllerImplPtr&)> InternalDoneCallback;

public:
    RpcControllerImpl()
        : _sequence_id(0)
        , _error_code(RPC_SUCCESS)
        , _is_done(NOT_DONE)
        , _is_request_sent(false)
        , _sent_bytes(0)
        , _is_start_cancel(false)
        , _is_sync(false)
        , _timeout_id(0)
        , _server_timeout(0)
        , _is_http(false)
        , _http_path(NULL)
        , _http_query_params(NULL)
        , _http_headers(NULL)
    {}

    virtual ~RpcControllerImpl() {}

    void SetRequestCompressType(CompressType compress_type)
    {
        _user_options.request_compress_type = compress_type;
    }

    CompressType RequestCompressType()
    {
        return _user_options.request_compress_type != CompressTypeAuto ?
            _user_options.request_compress_type : _auto_options.request_compress_type;
    }

    void SetResponseCompressType(CompressType compress_type)
    {
        _user_options.response_compress_type = compress_type;
    }

    CompressType ResponseCompressType()
    {
        return _user_options.response_compress_type != CompressTypeAuto ?
            _user_options.response_compress_type : _auto_options.response_compress_type;
    }

    void SetTimeout(int64 timeout)
    {
        _user_options.timeout = timeout;
    }

    int64 Timeout() const
    {
        return _user_options.timeout > 0 ?
            _user_options.timeout : _auto_options.timeout;
    }

    bool Failed() const
    {
        return _error_code != RPC_SUCCESS;
    }

    int ErrorCode() const
    {
        return _error_code;
    }

    const std::string& Reason() const
    {
        return _reason;
    }

    std::string ErrorText() const
    {
        if (_reason.empty()) {
            return RpcErrorCodeToString(_error_code);
        } else {
            return RpcErrorCodeToString(_error_code)
                + std::string(": ") + _reason;
        }
    }

    void StartCancel()
    {
        // TODO to support
    }

    void SetFailed(const std::string& reason)
    {
        SetFailed(RPC_ERROR_FROM_USER, reason);
    }

    bool IsCanceled() const
    {
        // TODO to support
        return false;
    }

    void NotifyOnCancel(google::protobuf::Closure* /* callback */)
    {
        // TODO to support
    }


    // -----------------------------------------------------------------
    // Used both in client and server.
    // -----------------------------------------------------------------
    void SetSequenceId(uint64 sequence_id)
    {
        _sequence_id = sequence_id;
    }

    uint64 SequenceId() const
    {
        return _sequence_id;
    }

    void SetMethodId(const std::string& method_id)
    {
        _method_id = method_id;
    }

    const std::string& MethodId() const
    {
        return _method_id;
    }

    void SetLocalEndpoint(const RpcEndpoint& local_endpoint)
    {
        _local_endpoint = local_endpoint;
    }

    const RpcEndpoint& LocalEndpoint() const
    {
        return _local_endpoint;
    }

    void SetRemoteEndpoint(const RpcEndpoint& remote_endpoint)
    {
        _remote_endpoint = remote_endpoint;
    }

    const RpcEndpoint& RemoteEndpoint() const
    {
        return _remote_endpoint;
    }

    void SetFailed(int error_code, const std::string& reason)
    {
        _error_code = error_code;
        _reason = reason;
        if (_error_code == RPC_ERROR_REQUEST_TIMEOUT)
        {
            _reason += _is_request_sent ?
                ": request already sent to remote" :
                ": request not sent to remote";
        }
    }


    // -----------------------------------------------------------------
    // Used only in client.
    // -----------------------------------------------------------------
    bool IsDone() const
    {
        return _is_done == DONE;
    }

    bool IsRequestSent() const
    {
        return _is_request_sent;
    }

    int64 SentBytes() const
    {
        return _sent_bytes;
    }

    bool IsStartCancel() const
    {
        return _is_start_cancel;
    }

    void PushDoneCallback(const InternalDoneCallback& callback)
    {
        SCHECK(callback);
        _done_callbacks.push_back(callback);
    }

    void Done(int error_code, const std::string& reason)
    {
        // make sure that the callback is only called once.
        if (atomic_comp_swap(&_is_done, DONE, NOT_DONE) == NOT_DONE)
        {
            SetFailed(error_code, reason);

            while (!_done_callbacks.empty())
            {
                const InternalDoneCallback& callback = _done_callbacks.back();
                callback(shared_from_this());
                _done_callbacks.pop_back();
            }
        }
    }

    void FillFromMethodDescriptor(const google::protobuf::MethodDescriptor* method)
    {
        _method_id = method->full_name();
        if (_user_options.timeout <= 0)
        {
            int64 timeout_in_ms = method->options().HasExtension(method_timeout) ?
                method->options().GetExtension(method_timeout) :
                method->service()->options().GetExtension(service_timeout);
            if (timeout_in_ms <= 0) {
                // Just a protection, it shouldn't happen.
                timeout_in_ms = 1;
            }
            _auto_options.timeout = timeout_in_ms;
        }
        if (_user_options.request_compress_type == CompressTypeAuto) {
            _auto_options.request_compress_type =
                method->options().HasExtension(request_compress_type) ?
                method->options().GetExtension(request_compress_type) :
                CompressTypeNone;
        }
        if (_user_options.response_compress_type == CompressTypeAuto) {
            _auto_options.response_compress_type =
                method->options().HasExtension(response_compress_type) ?
                method->options().GetExtension(response_compress_type) :
                CompressTypeNone;
        }
    }

    void SetSync()
    {
        _is_sync = true;
    }

    bool IsSync() const
    {
        return _is_sync;
    }

    void SetWaitEvent(const WaitEventPtr& wait_event)
    {
        _wait_event = wait_event;
    }

    const WaitEventPtr& WaitEvent() const
    {
        return _wait_event;
    }

    void SetRpcClientStream(const RpcClientStreamWPtr& stream)
    {
        _client_stream = stream;
    }

    const RpcClientStreamWPtr& RpcClientStream() const
    {
        return _client_stream;
    }

    void StartTimer()
    {
        int64 timeout = Timeout();
        _expiration = timeout <= 0 ? ptime_infin()
            : ptime_now() + time_duration_milliseconds(timeout);
    }

    const PTime& Expiration() const
    {
        return _expiration;
    }

    void SetTimeoutId(uint64 timeout_id)
    {
        _timeout_id = timeout_id;
    }

    uint64 TimeoutId() const
    {
        return _timeout_id;
    }

    void SetRequestBuffer(const ReadBufferPtr& request_buffer)
    {
        _request_buffer = request_buffer;
    }

    const ReadBufferPtr& RequestBuffer() const
    {
        return _request_buffer;
    }

    void SetResponseBuffer(const ReadBufferPtr& response_buffer)
    {
        _response_buffer = response_buffer;
    }

    const ReadBufferPtr& ResponseBuffer() const
    {
        return _response_buffer;
    }

    void NotifyRequestSent(const RpcEndpoint& local_endpoint, int64 sent_bytes)
    {
        _is_request_sent = true;
        _local_endpoint = local_endpoint;
        _sent_bytes = sent_bytes;
        _request_sent_time = ptime_now();
    }


    // -----------------------------------------------------------------
    // Used only in server.
    // -----------------------------------------------------------------
    void SetRpcServerStream(const RpcServerStreamWPtr& stream)
    {
        _server_stream = stream;
    }

    const RpcServerStreamWPtr& RpcServerStream() const
    {
        return _server_stream;
    }

    void SetServerTimeout(int64 timeout)
    {
        _server_timeout = timeout;
    }

    int64 ServerTimeout() const
    {
        return _server_timeout;
    }

    void SetRequestReceivedTime(const PTime& time)
    {
        _request_received_time = time;
    }

    const PTime& RequestReceivedTime()
    {
        return _request_received_time;
    }

    void SetStartProcessTime(const PTime& time)
    {
        _start_process_time = time;
    }

    const PTime& StartProcessTime()
    {
        return _start_process_time;
    }

    void SetFinishProcessTime(const PTime& time)
    {
        _finish_process_time = time;
    }

    const PTime& FinishProcessTime()
    {
        return _finish_process_time;
    }

    void SetHttp()
    {
        _is_http = true;
    }

    bool IsHttp() const
    {
        return _is_http;
    }

    void SetHttpPath(const std::string* path)
    {
        _http_path = path;
    }

    const std::string& HttpPath() const
    {
        SCHECK(_is_http);
        return *_http_path;
    }

    void SetHttpQueryParams(const std::map<std::string, std::string>* params)
    {
        _http_query_params = params;
    }

    const std::map<std::string, std::string>& HttpQueryParams() const
    {
        SCHECK(_is_http);
        return *_http_query_params;
    }

    void SetHttpHeaders(const std::map<std::string, std::string>* headers)
    {
        _http_headers = headers;
    }

    const std::map<std::string, std::string>& HttpHeaders() const
    {
        SCHECK(_is_http);
        return *_http_headers;
    }

private:
    uint64 _sequence_id;
    std::string _method_id;
    RpcEndpoint _local_endpoint;
    RpcEndpoint _remote_endpoint;
    int _error_code;
    std::string _reason;

    // used only in client side
    static const int NOT_DONE = 0;
    static const int DONE = 1;
    volatile int _is_done; // 0 means not done, 1 means aleady done
    bool _is_request_sent; // if the request has been sent
    int64 _sent_bytes; // sent bytes including the header
    bool _is_start_cancel; // if has started canceling the call
    std::deque<InternalDoneCallback> _done_callbacks; // internal done callbacks
    bool _is_sync;
    WaitEventPtr _wait_event; // used only for sync call
    RpcClientStreamWPtr _client_stream;
    PTime _expiration;
    uint64 _timeout_id;
    ReadBufferPtr _request_buffer;
    ReadBufferPtr _response_buffer;
    PTime _request_sent_time;

    struct RequestOptions {
        int64 timeout;
        CompressType request_compress_type;
        CompressType response_compress_type;
        RequestOptions() :
            timeout(0),
            request_compress_type(CompressTypeAuto),
            response_compress_type(CompressTypeAuto) {}
    };
    RequestOptions _user_options; // options set by user
    RequestOptions _auto_options; // options from proto

    // used only in server side
    RpcServerStreamWPtr _server_stream;
    int64 _server_timeout;
    PTime _request_received_time;
    PTime _start_process_time;
    PTime _finish_process_time;

    bool _is_http;
    const std::string* _http_path;
    const std::map<std::string, std::string>* _http_query_params;
    const std::map<std::string, std::string>* _http_headers;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(RpcControllerImpl);
}; // class RpcControllerImpl

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_CONTROLLER_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
