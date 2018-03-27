// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_REQUEST_H_
#define _SOFA_PBRPC_RPC_REQUEST_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/rpc_controller_impl.h>
#include <sofa/pbrpc/service_pool.h>
#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/ptime.h>
#include <sofa/pbrpc/smart_ptr/shared_ptr.hpp>
#include <sofa/pbrpc/smart_ptr/enable_shared_from_this.hpp>

namespace sofa {
namespace pbrpc {

class RpcRequest;
typedef sofa::pbrpc::shared_ptr<RpcRequest> RpcRequestPtr;

class RpcRequest : public sofa::pbrpc::enable_shared_from_this<RpcRequest>
{
public:
    enum RpcRequestType
    {
        BINARY,
        HTTP
    };

public:
    RpcRequest():_write_buffer_base_block_factor(4) {}
    virtual ~RpcRequest() {}

    // The request type: BINARY or HTTP.
    virtual RpcRequestType RequestType() = 0;

    // The method full name to request.
    virtual std::string Method() = 0;

    // The sequence id if supported.
    virtual uint64 SequenceId() = 0;

    // Process the request.
    virtual void ProcessRequest(
            const RpcServerStreamWPtr& server_stream,
            const ServicePoolPtr& service_pool) = 0;

    // Assemble succeed response to buffer.
    virtual ReadBufferPtr AssembleSucceedResponse(
            const RpcControllerImplPtr& cntl,
            const google::protobuf::Message* response,
            std::string& err) = 0;

    // Assemble failed response to buffer.
    virtual ReadBufferPtr AssembleFailedResponse(
            int32 error_code,
            const std::string& reason,
            std::string& err) = 0;

public:
    void CallMethod(
            MethodBoard* method_board,
            RpcController* controller,
            google::protobuf::Message* request,
            google::protobuf::Message* response);

    void OnCallMethodDone(
            MethodBoard* method_board,
            RpcController* controller,
            google::protobuf::Message* request,
            google::protobuf::Message* response);

    void SendSucceedResponse(
            const RpcControllerImplPtr& cntl,
            const google::protobuf::Message* response);

    void SendSucceedResponse(
            const RpcServerStreamWPtr& stream,
            const ReadBufferPtr& buffer);

    void SendFailedResponse(
            const RpcServerStreamWPtr& stream,
            int32 error_code,
            const std::string& reason);

    void OnSendResponseDone(
            RpcErrorCode error_code);

    // Parse service & method name from method full name.
    // @return false if parse failed.
    static bool ParseMethodFullName(
            const std::string& method_full_name,
            std::string* service_name,
            std::string* method_name);

    // Find method board from service pool.
    // @return NULL if not found.
    static MethodBoard* FindMethodBoard(
            const ServicePoolPtr& service_pool,
            const std::string& service_name,
            const std::string& method_name);

    void set_write_buffer_base_block_factor(size_t factor)
    {
        _write_buffer_base_block_factor = factor;
    }

    size_t write_buffer_base_block_factor()
    {
        return _write_buffer_base_block_factor;
    }

    void SetLocalEndpoint(const RpcEndpoint& local_endpoint)
    {
        _local_endpoint = local_endpoint;
    }

    const RpcEndpoint& LocalEndpoint()
    {
        return _local_endpoint;
    }

    void SetRemoteEndpoint(const RpcEndpoint& remote_endpoint)
    {
        _remote_endpoint = remote_endpoint;
    }

    const RpcEndpoint& RemoteEndpoint()
    {
        return _remote_endpoint;
    }

    void SetReceivedTime(const PTime& received_time)
    {
        _received_time = received_time;
    }

    const PTime& ReceivedTime()
    {
        return _received_time;
    }

protected:
    RpcEndpoint _local_endpoint;
    RpcEndpoint _remote_endpoint;
    PTime _received_time;
    size_t _write_buffer_base_block_factor;
}; // class RpcRequest

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_REQUEST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
