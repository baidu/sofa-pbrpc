// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/closure.h>
#include <gtest/gtest.h>

using namespace std;
using namespace sofa::pbrpc;
	
class ClosureTest : public testing::Test {
public:
    void SetA123Method()   { a_ = 123; }
    static void SetA123Function() { current_instance_->a_ = 123; }

    void SetAMethod(int a)         { a_ = a; }
    void SetCMethod(string c)      { c_ = c; }

    static void SetAFunction(int a)         { current_instance_->a_ = a; }
    static void SetCFunction(string c)      { current_instance_->c_ = c; }

    void SetABMethod(int a, const char* b)  { a_ = a; b_ = b; }
    static void SetABFunction(int a, const char* b) {
        current_instance_->a_ = a;
        current_instance_->b_ = b;
    }

    void SetABCMethod(int a, const char* b, std::string c) {
        a_ = a; b_ = b; c_ = c;
    }
    static void SetABCFunction(int a, const char* b, std::string c) {
        current_instance_->a_ = a;
        current_instance_->b_ = b;
        current_instance_->c_ = c;
    }

    void SetABCDMethod(int a, const char* b, const std::string& c, double d) {
        a_ = a; b_ = b; c_ = c; d_ = d;
    }
    static void SetABCDFunction(int a, const char* b, const std::string& c, double d) {
        current_instance_->a_ = a;
        current_instance_->b_ = b;
        current_instance_->c_ = c;
        current_instance_->d_ = d;
    }

    virtual void SetUp() {
        current_instance_ = this;
        a_ = 0;
        b_ = NULL;
        c_.clear();
        d_ = 0.0;
        permanent_closure_ = NULL;
    }

    void DeleteClosureInCallback() {
        delete permanent_closure_;
    }

    int a_;
    const char* b_;
    string c_;
    double d_;
    google::protobuf::Closure* permanent_closure_;

    static ClosureTest* current_instance_;
};

ClosureTest* ClosureTest::current_instance_ = NULL;

TEST_F(ClosureTest, TestClosureFunction0) {
    google::protobuf::Closure* closure = NewClosure(&SetA123Function);
    EXPECT_NE(123, a_);
    closure->Run();
    EXPECT_EQ(123, a_);
}

TEST_F(ClosureTest, TestClosureMethod0) {
    google::protobuf::Closure* closure = NewClosure(current_instance_,
            &ClosureTest::SetA123Method);
    EXPECT_NE(123, a_);
    closure->Run();
    EXPECT_EQ(123, a_);
}

TEST_F(ClosureTest, TestClosureFunction1) {
    google::protobuf::Closure* closure = NewClosure(&SetAFunction, 456);
    EXPECT_NE(456, a_);
    closure->Run();
    EXPECT_EQ(456, a_);
}

TEST_F(ClosureTest, TestClosureMethod1) {
    google::protobuf::Closure* closure = NewClosure(current_instance_,
            &ClosureTest::SetAMethod, 456);
    EXPECT_NE(456, a_);
    closure->Run();
    EXPECT_EQ(456, a_);
}

TEST_F(ClosureTest, TestClosureFunction1String) {
    google::protobuf::Closure* closure = NewClosure(&SetCFunction, string("test"));
    EXPECT_NE("test", c_);
    closure->Run();
    EXPECT_EQ("test", c_);
}

TEST_F(ClosureTest, TestClosureMethod1String) {
    google::protobuf::Closure* closure = NewClosure(current_instance_,
            &ClosureTest::SetCMethod, string("test"));
    EXPECT_NE("test", c_);
    closure->Run();
    EXPECT_EQ("test", c_);
}

TEST_F(ClosureTest, TestClosureFunction2) {
    const char* cstr = "hello";
    google::protobuf::Closure* closure = NewClosure(&SetABFunction, 789, cstr);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
}

TEST_F(ClosureTest, TestClosureMethod2) {
    const char* cstr = "hello";
    google::protobuf::Closure* closure = NewClosure(current_instance_,
            &ClosureTest::SetABMethod, 789, cstr);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
}

TEST_F(ClosureTest, TestClosureFunction3) {
    const char* cstr = "hello";
    std::string str = cstr;
    google::protobuf::Closure* closure = NewClosure(&SetABCFunction, 789, cstr, str);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    EXPECT_NE(str, c_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    EXPECT_EQ(str, c_);
}

TEST_F(ClosureTest, TestClosureMethod3) {
    const char* cstr = "hello";
    std::string str = cstr;
    google::protobuf::Closure* closure = NewClosure(current_instance_,
            &ClosureTest::SetABCMethod, 789, cstr, str);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    EXPECT_NE(str, c_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    EXPECT_EQ(str, c_);
}

TEST_F(ClosureTest, TestClosureFunction4) {
    const char* cstr = "hello";
    std::string str = cstr;
    google::protobuf::Closure* closure = NewClosure(&SetABCDFunction, 789, cstr, str, 1.0);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    EXPECT_NE(str, c_);
    EXPECT_NE(1.0, d_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    EXPECT_EQ(str, c_);
    EXPECT_EQ(1.0, d_);
}

TEST_F(ClosureTest, TestClosureMethod4) {
    const char* cstr = "hello";
    std::string str = cstr;
    google::protobuf::Closure* closure = NewClosure(current_instance_,
            &ClosureTest::SetABCDMethod, 789, cstr, str, 1.0);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    EXPECT_NE(str, c_);
    EXPECT_NE(1.0, d_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    EXPECT_EQ(str, c_);
    EXPECT_EQ(1.0, d_);
}

// Repeat all of the above with NewPermanentClosure()

TEST_F(ClosureTest, TestPermanentClosureFunction0) {
    google::protobuf::Closure* closure = NewPermanentClosure(&SetA123Function);
    EXPECT_NE(123, a_);
    closure->Run();
    EXPECT_EQ(123, a_);
    a_ = 0;
    closure->Run();
    EXPECT_EQ(123, a_);
    delete closure;
}

TEST_F(ClosureTest, TestPermanentClosureMethod0) {
    google::protobuf::Closure* closure = NewPermanentClosure(current_instance_,
            &ClosureTest::SetA123Method);
    EXPECT_NE(123, a_);
    closure->Run();
    EXPECT_EQ(123, a_);
    a_ = 0;
    closure->Run();
    EXPECT_EQ(123, a_);
    delete closure;
}

TEST_F(ClosureTest, TestPermanentClosureFunction1) {
    google::protobuf::Closure* closure = NewPermanentClosure(&SetAFunction, 456);
    EXPECT_NE(456, a_);
    closure->Run();
    EXPECT_EQ(456, a_);
    a_ = 0;
    closure->Run();
    EXPECT_EQ(456, a_);
    delete closure;
}

TEST_F(ClosureTest, TestPermanentClosureMethod1) {
    google::protobuf::Closure* closure = NewPermanentClosure(current_instance_,
            &ClosureTest::SetAMethod, 456);
    EXPECT_NE(456, a_);
    closure->Run();
    EXPECT_EQ(456, a_);
    a_ = 0;
    closure->Run();
    EXPECT_EQ(456, a_);
    delete closure;
}

TEST_F(ClosureTest, TestPermanentClosureFunction2) {
    const char* cstr = "hello";
    google::protobuf::Closure* closure = NewPermanentClosure(&SetABFunction, 789, cstr);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    a_ = 0;
    b_ = NULL;
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    delete closure;
}

TEST_F(ClosureTest, TestPermanentClosureMethod2) {
    const char* cstr = "hello";
    google::protobuf::Closure* closure = NewPermanentClosure(current_instance_,
            &ClosureTest::SetABMethod, 789, cstr);
    EXPECT_NE(789, a_);
    EXPECT_NE(cstr, b_);
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    a_ = 0;
    b_ = NULL;
    closure->Run();
    EXPECT_EQ(789, a_);
    EXPECT_EQ(cstr, b_);
    delete closure;
}

TEST_F(ClosureTest, TestPermanentClosureDeleteInCallback) {
    permanent_closure_ = NewPermanentClosure((ClosureTest*) this,
            &ClosureTest::DeleteClosureInCallback);
    permanent_closure_->Run();
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
