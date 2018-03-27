// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#define SOFA_PBRPC_TRAN_BUF_BLOCK_SIZE (64u)

#include <gtest/gtest.h>
#include <sofa/pbrpc/tran_buf_pool.h>

using namespace sofa::pbrpc;

class TranBufPoolTest : public ::testing::Test {};

TEST_F(TranBufPoolTest, malloc_free_test)
{
    void* data = TranBufPool::malloc(4);
    ASSERT_TRUE(NULL != data);
    ASSERT_EQ(1024, TranBufPool::block_size(data));
    ASSERT_EQ(1024 - sizeof(int) * 2, TranBufPool::capacity(data));
    TranBufPool::free(data);
}

TEST_F(TranBufPoolTest, add_ref_test)
{
    void* data = TranBufPool::malloc(4);
    ASSERT_TRUE(NULL != data);
    TranBufPool::add_ref(data);
    TranBufPool::free(data);
    TranBufPool::free(data);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
