// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_BINARY_RPC_REQUEST_PARSER_H_
#define _SOFA_PBRPC_BINARY_RPC_REQUEST_PARSER_H_

#include <sofa/pbrpc/rpc_request_parser.h>
#include <sofa/pbrpc/binary_rpc_request.h>

namespace sofa {
namespace pbrpc {

class BinaryRpcRequestParser : public RpcRequestParser
{
public:
    BinaryRpcRequestParser();
    virtual ~BinaryRpcRequestParser();

    virtual const char* Name();

    virtual void Reset();

    virtual bool CheckMagicString(const char* magic_string);

    virtual int Parse(const char* buf, int data_size, int offset, int* bytes_consumed);

    virtual RpcRequestPtr GetRequest();

private:
    enum ParseState
    {
        PS_MAGIC_STRING,
        PS_MSG_HEADER,
        PS_MSG_BODY
    };
    ParseState _state; // current parsing state
    int _bytes_recved; // bytes received in current state
    BinaryRpcRequestPtr _req;

    static const int64 MAX_MESSAGE_SIZE;
}; // class BinaryRpcRequestParser

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BINARY_RPC_REQUEST_PARSER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
