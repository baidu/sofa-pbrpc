// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/rpc_error_code.h>

namespace sofa {
namespace pbrpc {

#define MAKE_CASE(name) case name: return (#name)

const char* RpcErrorCodeToString(int error_code)
{
    switch(error_code)
    {
        MAKE_CASE(RPC_SUCCESS);

        MAKE_CASE(RPC_ERROR_PARSE_REQUEST_MESSAGE);
        MAKE_CASE(RPC_ERROR_PARSE_RESPONSE_MESSAGE);
        MAKE_CASE(RPC_ERROR_UNCOMPRESS_MESSAGE);
        MAKE_CASE(RPC_ERROR_COMPRESS_TYPE);
        MAKE_CASE(RPC_ERROR_NOT_SPECIFY_METHOD_NAME);
        MAKE_CASE(RPC_ERROR_PARSE_METHOD_NAME);
        MAKE_CASE(RPC_ERROR_FOUND_SERVICE);
        MAKE_CASE(RPC_ERROR_FOUND_METHOD);
        MAKE_CASE(RPC_ERROR_CHANNEL_BROKEN);
        MAKE_CASE(RPC_ERROR_CONNECTION_CLOSED);
        MAKE_CASE(RPC_ERROR_REQUEST_TIMEOUT);
        MAKE_CASE(RPC_ERROR_REQUEST_CANCELED);
        MAKE_CASE(RPC_ERROR_SERVER_UNAVAILABLE);
        MAKE_CASE(RPC_ERROR_SERVER_UNREACHABLE);
        MAKE_CASE(RPC_ERROR_SERVER_SHUTDOWN);
        MAKE_CASE(RPC_ERROR_SEND_BUFFER_FULL);
        MAKE_CASE(RPC_ERROR_SERIALIZE_REQUEST);
        MAKE_CASE(RPC_ERROR_SERIALIZE_RESPONSE);
        MAKE_CASE(RPC_ERROR_RESOLVE_ADDRESS);
        MAKE_CASE(RPC_ERROR_CREATE_STREAM);
        MAKE_CASE(RPC_ERROR_NOT_IN_RUNNING);
        MAKE_CASE(RPC_ERROR_SERVER_BUSY);

        MAKE_CASE(RPC_ERROR_TOO_MANY_OPEN_FILES);

        MAKE_CASE(RPC_ERROR_UNKNOWN);
        MAKE_CASE(RPC_ERROR_FROM_USER);
    }
    return "RPC_ERROR_UNDEFINED";
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
