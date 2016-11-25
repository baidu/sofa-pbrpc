// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/rpc_request.h>
#include <sofa/pbrpc/binary_rpc_request_parser.h>
#include <sofa/pbrpc/http_rpc_request_parser.h>

namespace sofa {
namespace pbrpc {

void RpcRequestParser::RegisteredParsers(std::vector<RpcRequestParserPtr>* parsers)
{
    parsers->push_back(RpcRequestParserPtr(new BinaryRpcRequestParser()));
    parsers->push_back(RpcRequestParserPtr(new HTTPRpcRequestParser()));
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
