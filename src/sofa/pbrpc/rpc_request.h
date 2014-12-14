// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_REQUEST_H_
#define _SOFA_PBRPC_RPC_REQUEST_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/service_pool.h>
#include <sofa/pbrpc/smart_ptr/shared_ptr.hpp>

namespace sofa {
namespace pbrpc {

class RpcRequest;
typedef sofa::pbrpc::shared_ptr<RpcRequest> RpcRequestPtr;

class RpcRequest
{
public:
    enum RpcRequestType
    {
        BINARY,
        HTTP
    };

public:
    RpcRequest() {}
    virtual ~RpcRequest() {}

    virtual RpcRequestType RequestType() = 0;

    virtual std::string MethodFullName() = 0;

    virtual std::string ServiceName() = 0;

    virtual std::string MethodName() = 0;

    virtual uint64 SequenceId() = 0;

    virtual void ProcessRequest(
            const RpcServerStreamWPtr& server_stream,
            ServiceBoard* service_board) = 0;

    virtual void SendFailedResponse(
            const RpcServerStreamWPtr& server_stream,
            int32 error_code,
            const std::string& reason) = 0;

public:
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

protected:
    RpcEndpoint _local_endpoint;
    RpcEndpoint _remote_endpoint;
}; // class RpcRequest

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_REQUEST_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
