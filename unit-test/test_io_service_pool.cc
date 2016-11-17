// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <gtest/gtest.h>
#include <sofa/pbrpc/io_service_pool.h>

using namespace ::sofa::pbrpc;

class IOServicePoolTest: public ::testing::Test
{
protected:
    IOServicePoolTest() { }

    virtual ~IOServicePoolTest() { }

    virtual void SetUp() { }

    virtual void TearDown() { }
};

TEST(IOServicePool, Run)
{
    IOServicePool* pool = new IOServicePool(2, 8);
    bool ret = pool->Run();
    delete pool;
    ASSERT_TRUE(ret == true);
}

TEST(IOServicePool, GetIOService)
{
    IOServicePool* pool = new IOServicePool(2, 8);
    IOService& io_service1 = pool->GetIOService();
    IOService& io_service2 = pool->GetIOService();
    ASSERT_TRUE(&io_service1 != &io_service2);

    IOService& io_service3 = pool->GetIOService();
    ASSERT_TRUE(&io_service3 == &io_service1);
    delete pool;
}

TEST(IOServicePool, Stop)
{
    IOServicePool* pool = new IOServicePool(2, 8);
    pool->Stop();
    delete pool;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
