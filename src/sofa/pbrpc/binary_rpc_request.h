// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_BINARY_RPC_REQUEST_H_
#define _SOFA_PBRPC_BINARY_RPC_REQUEST_H_

#include <sofa/pbrpc/rpc_request.h>
#include <sofa/pbrpc/rpc_message_header.h>
#include <sofa/pbrpc/rpc_meta.pb.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/rpc_controller.h>
#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/service_pool.h>
#include <sofa/pbrpc/smart_ptr/enable_shared_from_this.hpp>

namespace sofa {
namespace pbrpc {

class BinaryRpcRequestParser;

class BinaryRpcRequest;
typedef sofa::pbrpc::shared_ptr<BinaryRpcRequest> BinaryRpcRequestPtr;

class BinaryRpcRequest : public RpcRequest,
    public sofa::pbrpc::enable_shared_from_this<BinaryRpcRequest>
{
public:
    BinaryRpcRequest();
    virtual ~BinaryRpcRequest();

    virtual RpcRequestType RequestType();

    virtual std::string MethodFullName();

    virtual std::string ServiceName();

    virtual std::string MethodName();

    virtual uint64 SequenceId();

    virtual void ProcessRequest(
            const RpcServerStreamWPtr& server_stream,
            ServiceBoard* service_board);

    virtual void SendFailedResponse(
            const RpcServerStreamWPtr& stream,
            int32 error_code,
            const std::string& reason);

private:
    void OnCallMethodDone(
            RpcController* controller,
            google::protobuf::Message* request,
            google::protobuf::Message* response,
            MethodBoard* method_board,
            PTime start_time);

    void SendSucceedResponse(
            const RpcServerStreamWPtr& stream,
            CompressType compress_type,
            google::protobuf::Message* response);

    static void OnSendResponseDone(
            const RpcEndpoint& remote_endpoint,
            uint64 sequence_id,
            RpcErrorCode error_code);

private:
    friend class BinaryRpcRequestParser;

    std::string _service_name;
    std::string _method_name;

    RpcMessageHeader _req_header;
    RpcMeta _req_meta;
    ReadBufferPtr _req_body;
}; // class BinaryRpcRequest

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BINARY_RPC_REQUEST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
