// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_BINARY_RPC_REQUEST_H_
#define _SOFA_PBRPC_BINARY_RPC_REQUEST_H_

#include <sofa/pbrpc/rpc_request.h>
#include <sofa/pbrpc/rpc_message_header.h>
#include <sofa/pbrpc/rpc_meta.pb.h>

namespace sofa {
namespace pbrpc {

class BinaryRpcRequestParser;

class BinaryRpcRequest;
typedef sofa::pbrpc::shared_ptr<BinaryRpcRequest> BinaryRpcRequestPtr;

class BinaryRpcRequest : public RpcRequest
{
public:
    BinaryRpcRequest();
    virtual ~BinaryRpcRequest();

    virtual RpcRequestType RequestType();

    virtual std::string Method();

    virtual uint64 SequenceId();

    virtual void ProcessRequest(
            const RpcServerStreamWPtr& server_stream,
            const ServicePoolPtr& service_pool);

    virtual ReadBufferPtr AssembleSucceedResponse(
            const RpcControllerImplPtr& cntl,
            const google::protobuf::Message* response,
            std::string& err);

    virtual ReadBufferPtr AssembleFailedResponse(
            int32 error_code,
            const std::string& reason,
            std::string& err);

private:
    friend class BinaryRpcRequestParser;

    RpcMessageHeader _req_header;
    RpcMeta          _req_meta;
    ReadBufferPtr    _req_body;
}; // class BinaryRpcRequest

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BINARY_RPC_REQUEST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
