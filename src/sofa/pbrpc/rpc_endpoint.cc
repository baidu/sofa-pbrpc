// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sstream>

#include <sofa/pbrpc/rpc_endpoint.h>

namespace sofa {
namespace pbrpc {

using boost::asio::ip::tcp;

std::string RpcEndpointToString(const RpcEndpoint& endpoint)
{
    std::stringstream ss;
    ss << endpoint;
    return ss.str();
}

std::string HostOfRpcEndpoint(const RpcEndpoint& endpoint)
{
    return endpoint.address().to_string();
}

uint32 PortOfRpcEndpoint(const RpcEndpoint& endpoint)
{
    return endpoint.port();
}

bool ResolveAddress(IOService& io_service,
        const std::string& host, const std::string& svc,
        RpcEndpoint* endpoint)
{
    tcp::resolver resolver(io_service);
    boost::system::error_code ec;
    tcp::resolver::iterator it = resolver.resolve(tcp::resolver::query(host, svc), ec), end;
    if (it != end)
    {
        *endpoint = it->endpoint();
        return true;
    }
    else
    {
#if defined( LOG )
        LOG(ERROR) << "ResolveAddress(): resolve address ["
                   << host << ":" << svc << "] failed: " << ec.message();
#else
        SLOG(ERROR, "ResolveAddress(): resolve address [%s:%s] failed: %s",
                host.c_str(), svc.c_str(), ec.message().c_str());
#endif
        return false;
    }
}

bool ResolveAddress(IOService& io_service,
        const std::string& address,
        RpcEndpoint* endpoint)
{
    std::string::size_type pos = address.find(':');
    if (pos == std::string::npos)
    {
#if defined( LOG )
        LOG(ERROR) << "ResolveAddress(): invalid address: " << address;
#else
        SLOG(ERROR, "ResolveAddress(): invalid address: %s", address.c_str());
#endif
        return false;
    }
    std::string host = address.substr(0, pos);
    std::string svc = address.substr(pos + 1);
    return ResolveAddress(io_service, host, svc, endpoint);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
