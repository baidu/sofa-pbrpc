// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_CHANNEL_IMPL_H_
#define _SOFA_PBRPC_RPC_CHANNEL_IMPL_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/rpc_channel.h>

namespace sofa {
namespace pbrpc {

class RpcChannelImpl : public ::google::protobuf::RpcChannel
{
public:
    virtual ~RpcChannelImpl() {}

    // Init the channel.
    virtual bool Init() = 0;

    // Stop the channel.
    virtual void Stop() = 0;

    // Call method.
    virtual void CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done) = 0;

    // Get the count of calls which are not done yet.
    virtual uint32 WaitCount() = 0;
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_CHANNEL_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
