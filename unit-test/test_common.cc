// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/common.h>
#include <gtest/gtest.h>

using namespace sofa;

class CommonTest : public ::testing::Test
{
protected:
    CommonTest(){};
    virtual ~CommonTest(){};
    virtual void SetUp() {
        //Called befor every TEST_F(CommonTest, *)
    };
    virtual void TearDown() {
        //Called after every TEST_F(CommonTest, *)
    };
};

TEST_F(CommonTest, test_log)
{
    sofa::pbrpc::internal::set_log_level(sofa::pbrpc::LOG_LEVEL_NOTICE);
    int flag_notice = 0;
    SLOG(NOTICE, "notice message: %d: %s", ++flag_notice, "should be logged");
    ASSERT_EQ(1, flag_notice);
    int flag_trace = 0;
    SLOG(TRACE, "trace message: %d: %s", ++flag_trace, "should not be logged");
    ASSERT_EQ(0, flag_trace);
}

TEST_F(CommonTest, test_check)
{
    int value = 1;
    SCHECK_EQ(1, value);
    SCHECK_LE(1, value);
    SCHECK_LE(0, value);
    SCHECK_LT(0, value);
    SCHECK_GE(1, value);
    SCHECK_GE(2, value);
    SCHECK_GT(2, value);
}

static std::string s_test_log_buf;
void test_log_handler(
    sofa::pbrpc::LogLevel level, const char* filename, int,
    const char *fmt, va_list ap)
{
    char buf[1024];
    vsnprintf(buf, 1024, fmt, ap);
    char result[1500];
    snprintf(result, 1024, "level=%d filename=%s %s", level, filename, buf);
    s_test_log_buf.assign(result);
}

TEST_F(CommonTest, test_set_log_handler)
{
    sofa::pbrpc::LogHandler *old = sofa::pbrpc::set_log_handler(test_log_handler);
    ASSERT_NE(old, (void*)NULL);

    SLOG(ERROR, "found error %s", "boom!");
#ifdef BAZEL_BUILD
    ASSERT_STREQ(s_test_log_buf.c_str(), "level=1 filename=unit-test/test_common.cc found error boom!");
#else
    ASSERT_STREQ(s_test_log_buf.c_str(), "level=1 filename=test_common.cc found error boom!");
#endif
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
