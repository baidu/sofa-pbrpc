// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>
#include <sofa/pbrpc/rpc_channel.h>
#include <sofa/pbrpc/simple_rpc_channel_impl.h>
#include <sofa/pbrpc/dynamic_rpc_channel_impl.h>
#include <sofa/pbrpc/rpc_client.h>

namespace sofa {
namespace pbrpc {

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::string& server_address,
        const RpcChannelOptions& options)
    : _impl(new SimpleRpcChannelImpl(rpc_client->impl(), server_address, options))
{
    if (options.create_with_init)
    {
        _impl->Init();
    }
}

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::string& server_ip,
        uint32 server_port,
        const RpcChannelOptions& options)
{
    std::ostringstream os;
    os << server_ip << ":" << server_port;
    _impl.reset(new SimpleRpcChannelImpl(rpc_client->impl(), os.str(), options));
    if (options.create_with_init)
    {
        _impl->Init();
    }
}

RpcChannel::RpcChannel(RpcClient* rpc_client,
        const std::vector<std::string>& address_list,
        const RpcChannelOptions& options)
    : _impl(new DynamicRpcChannelImpl(rpc_client->impl(), address_list, options))
{
    if (options.create_with_init)
    {
        _impl->Init();
    }
}

RpcChannel::RpcChannel(RpcClient* rpc_client,
        AddressProvider* address_provider,
        const RpcChannelOptions& options)
    : _impl(new DynamicRpcChannelImpl(rpc_client->impl(), address_provider, options))
{
    if (options.create_with_init)
    {
        _impl->Init();
    }
}

bool RpcChannel::Init()
{
    return _impl->Init();
}

RpcChannel::~RpcChannel()
{
    _impl->Stop();
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
