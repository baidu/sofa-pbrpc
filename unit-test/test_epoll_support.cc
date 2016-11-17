// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/io_service.h>
#include <gtest/gtest.h>

class EpollSupportTest: public ::testing::Test {};

#ifndef __APPLE__

TEST_F(EpollSupportTest, test)
{
    std::string output;
#if defined(BOOST_ASIO_HAS_IOCP)
    output = "iocp" ;
#elif defined(BOOST_ASIO_HAS_EPOLL)
    output = "epoll" ;
#elif defined(BOOST_ASIO_HAS_KQUEUE)
    output = "kqueue" ;
#elif defined(BOOST_ASIO_HAS_DEV_POLL)
    output = "/dev/poll" ;
#else
    output = "select" ;
#endif
    ASSERT_EQ("epoll", output);
}

#endif

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
