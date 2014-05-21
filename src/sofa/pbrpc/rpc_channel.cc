// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <sstream>

#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/rpc_channel_impl.h>
#include <sofa/pbrpc/rpc_client.h>

namespace sofa {
namespace pbrpc {

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::string& server_address,
        const RpcChannelOptions& options)
    : _impl(new RpcChannelImpl(rpc_client->impl(), server_address, options))
{
}

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::string& server_ip,
        uint32 server_port,
        const RpcChannelOptions& options)
{
    std::ostringstream os;
    os << server_ip << ":" << server_port;
    _impl.reset(new RpcChannelImpl(rpc_client->impl(), os.str(), options));
}

RpcChannel::~RpcChannel()
{
}

bool RpcChannel::IsAddressValid()
{
    return _impl->IsAddressValid();
}

void RpcChannel::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                            ::google::protobuf::RpcController* controller,
                            const ::google::protobuf::Message* request,
                            ::google::protobuf::Message* response,
                            ::google::protobuf::Closure* done)
{
    _impl->CallMethod(method, controller, request, response, done);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
