// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_RPC_MESSAGE_HEADER_H_
#define _SOFA_PBRPC_RPC_MESSAGE_HEADER_H_

#include <sofa/pbrpc/common_internal.h>

namespace sofa {
namespace pbrpc {

// Magic string "SOFA" in little endian.
#define SOFA_RPC_MAGIC 1095126867u

// total 24 bytes
struct RpcMessageHeader {
    union {
        char    magic_str[4];
        uint32  magic_str_value;
    };                    // 4 bytes
    int32   meta_size;    // 4 bytes
    int64   data_size;    // 8 bytes
    int64   message_size; // 8 bytes: message_size = meta_size + data_size, for check

    RpcMessageHeader()
        : magic_str_value(SOFA_RPC_MAGIC)
        , meta_size(0), data_size(0), message_size(0) {}

    bool CheckMagicString() const
    {
        return magic_str_value == SOFA_RPC_MAGIC;
    }
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_MESSAGE_HEADER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
