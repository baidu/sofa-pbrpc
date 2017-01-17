// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_SIMPLE_RPC_CHANNEL_IMPL_H_
#define _SOFA_PBRPC_SIMPLE_RPC_CHANNEL_IMPL_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_channel_impl.h>
#include <sofa/pbrpc/rpc_client_impl.h>
#include <sofa/pbrpc/rpc_controller_impl.h>
#include <sofa/pbrpc/rpc_endpoint.h>
#include <sofa/pbrpc/counter.h>
#include <sofa/pbrpc/smart_ptr/enable_shared_from_this.hpp>

namespace sofa {
namespace pbrpc {

class SimpleRpcChannelImpl : public RpcChannelImpl,
    public ::sofa::pbrpc::enable_shared_from_this<SimpleRpcChannelImpl>
{
public:
    SimpleRpcChannelImpl(const RpcClientImplPtr& rpc_client_impl,
                         const std::string& server_address,
                         const RpcChannelOptions& options);

    virtual ~SimpleRpcChannelImpl();

    virtual bool Init();

    virtual void Stop();

    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done);

    virtual uint32 WaitCount();

private:
    static void WaitDone(const RpcControllerImplPtr& cntl);

    void DoneCallback(google::protobuf::Closure* done,
                      const RpcControllerImplPtr& cntl);

    static void MockDoneCallback(RpcControllerImplPtr cntl,
                                 const ::google::protobuf::Message* request,
                                 ::google::protobuf::Message* response);

private:
    RpcClientImplPtr _client_impl;
    std::string _server_address;
    RpcChannelOptions _options;

    bool _is_mock;
    bool _resolve_address_succeed;
    RpcEndpoint _remote_endpoint;

    AtomicCounter _wait_count;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(SimpleRpcChannelImpl);
}; // class SimpleRpcChannelImpl

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_SIMPLE_RPC_CHANNEL_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
