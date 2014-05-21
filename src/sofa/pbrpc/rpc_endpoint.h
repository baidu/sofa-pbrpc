// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_ENDPOINT_H_
#define _SOFA_PBRPC_RPC_ENDPOINT_H_

#include <sofa/pbrpc/common_internal.h>

namespace sofa {
namespace pbrpc {

typedef boost::asio::ip::tcp::endpoint RpcEndpoint;

std::string RpcEndpointToString(const RpcEndpoint& endpoint);

bool ResolveAddress(IOService& io_service,
        const std::string& host, const std::string& svc,
        RpcEndpoint* endpoint);

bool ResolveAddress(IOService& io_service,
        const std::string& address,
        RpcEndpoint* endpoint);

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_ENDPOINT_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
