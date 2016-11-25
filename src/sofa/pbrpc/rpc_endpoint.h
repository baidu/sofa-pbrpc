// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_ENDPOINT_H_
#define _SOFA_PBRPC_RPC_ENDPOINT_H_

#include <sofa/pbrpc/common_internal.h>

namespace sofa {
namespace pbrpc {

// RpcEndpoint is a tuple <host:port>.
typedef boost::asio::ip::tcp::endpoint RpcEndpoint;

// Convert RpcEndpoint to string.
std::string RpcEndpointToString(const RpcEndpoint& endpoint);

// Get host of RpcEndpoint.
std::string HostOfRpcEndpoint(const RpcEndpoint& endpoint);

// Get port of RpcEndpoint.
uint32 PortOfRpcEndpoint(const RpcEndpoint& endpoint);

// DNS resolve server address to RpcEndpoint, only choose the first one.
// @param io_service is the io service used for resolving.
// @param host can be an ip or host name, eg. "127.0.0.1" or "baidu.com". 
// @param svc can be a port or service name, eg. "21" or "ftp".
// @param endpoints is out param, stores resolved RpcEndpoint if succeed,
// but may be empty.
// @return true if resolve succeed.
// @return false if resolve failed.
bool ResolveAddress(IOService& io_service,
        const std::string& host, const std::string& svc,
        RpcEndpoint* endpoint);

// DNS resolve server address to RpcEndpoint, only choose the first one.
// @param io_service is the io service used for resolving.
// @param server address should be in format of "host:port".
// @param endpoint is out param, stores resolved RpcEndpoint if succeed.
// @return true if resolve succeed.
// @return false if resolve failed or not address found.
bool ResolveAddress(IOService& io_service,
        const std::string& address,
        RpcEndpoint* endpoint);

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_ENDPOINT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
