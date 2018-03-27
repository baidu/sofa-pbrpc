// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_TRAN_BUF_POOL_H_
#define _SOFA_PBRPC_TRAN_BUF_POOL_H_

#include <sofa/pbrpc/common_internal.h>

#ifndef SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE
// base_block_size = 64B
#define SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE (64u)
#endif

#ifndef SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR
// max_block_size = 64<<9 = 32K
#define SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR (9u)
#endif

namespace sofa {
namespace pbrpc {

// Reference counted tran buf pool.
//
//   Block = BlockSize(int) + RefCount(int) + Data(char[capacity])
//
// Thread safe.
class TranBufPool
{
public:
    // Allocate a block.  Return NULL if failed.
    //
    //   block_size = SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE << factor
    //
    // Postconditions:
    // * If succeed, the reference count of the block is equal to 1.
    inline static void * malloc(int factor = 0)
    {
        void * p = ::malloc(SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE << factor);
        if (p != NULL)
        {
            *(reinterpret_cast<int*>(p)) = SOFA_PBRPC_TRAN_BUF_BLOCK_BASE_SIZE << factor;
            *(reinterpret_cast<int*>(p) + 1) = 1;
            p = reinterpret_cast<int*>(p) + 2;
        }
        return p;
    }

    // Return block size pointed by "p", including the hidden header.
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    inline static int block_size(void * p)
    {
        return *(reinterpret_cast<int*>(p) - 2);
    }

    // Return capacity size pointed by "p", not including the hidden header.
    //
    //   capacity = block_size - sizeof(int) * 2
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    inline static int capacity(void * p)
    {
        return *(reinterpret_cast<int*>(p) - 2) - sizeof(int) * 2;
    }

    // Increase the reference count of the block.
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    inline static void add_ref(void * p)
    {
        sofa::pbrpc::atomic_inc(reinterpret_cast<int*>(p) - 1);
    }

    // Decrease the reference count of the block.  If the reference count equals
    // to 0 afterward, then put the block back to the free list
    //
    // Preconditions:
    // * The block pointed by "p" was allocated by this pool and is in use currently.
    inline static void free(void * p)
    {
        if (sofa::pbrpc::atomic_dec_ret_old(reinterpret_cast<int*>(p) - 1) == 1)
        {
            ::free(reinterpret_cast<int*>(p) - 2);
        }
    }
}; // class TranBufPool

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_TRAN_BUF_POOL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
