// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/mock_test_helper.h>
#include <sofa/pbrpc/mock_test_helper_impl.h>

namespace sofa {
namespace pbrpc {

MockTestHelper::MockTestHelper() {}
MockTestHelper::~MockTestHelper() {}

MockTestHelper* MockTestHelper::GlobalInstance()
{
    static MockTestHelperImpl s_mock_channel;
    return &s_mock_channel;
}

bool g_enable_mock = false;

void enable_mock()
{
    MockTestHelper::GlobalInstance();
    g_enable_mock = true;
}

void disable_mock()
{
    g_enable_mock = false;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
