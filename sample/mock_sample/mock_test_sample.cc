// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/pbrpc.h>
#include <sofa/pbrpc/mock_test_helper.h>
#include <gtest/gtest.h>
#include <unistd.h>
#include "echo_service.pb.h"

using namespace ::sofa::pbrpc;
using namespace ::sofa::pbrpc::test;
	
void MockEchoSuccess(::google::protobuf::RpcController* /*controller*/,
              const ::google::protobuf::Message* request,
              ::google::protobuf::Message* response,
              ::google::protobuf::Closure* done)
{
    SLOG(NOTICE, "MockEchoSuccess() called");
    ((EchoResponse*)response)->set_message(((EchoRequest*)request)->message());
    done->Run();
}

void MockEchoFail(::google::protobuf::RpcController* controller,
              const ::google::protobuf::Message* /*request*/,
              ::google::protobuf::Message* /*response*/,
              ::google::protobuf::Closure* done)
{
    SLOG(NOTICE, "MockEchoFail() called");
    controller->SetFailed("mock failed");
    done->Run();
}

class MockTest : public ::testing::Test
{
protected:
    MockTest() {}
    virtual ~MockTest() {}
    virtual void SetUp() {
        SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

        // enable mock and register mock method
        SOFA_PBRPC_ENABLE_MOCK();

        _rpc_client = new RpcClient();
    }
    virtual void TearDown() {
        delete _rpc_client;
    }

    RpcClient* _rpc_client;
};
 
TEST_F(MockTest, test_registered_mock_sync_success)
{
    SOFA_PBRPC_CLEAR_MOCK_METHOD();
    SOFA_PBRPC_REGISTER_MOCK_METHOD("sofa.pbrpc.test.EchoServer.Echo",
            NewPermanentExtClosure(&MockEchoSuccess));

    RpcChannel mock_channel(_rpc_client, SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX + std::string("a:0"));
    EchoServer_Stub* stub = new EchoServer_Stub(&mock_channel);
    RpcController* cntl = new sofa::pbrpc::RpcController();
    EchoRequest* request = new EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    EchoResponse* response = new EchoResponse();

    stub->Echo(cntl, request, response, NULL);

    SLOG(NOTICE, "ErrorText: %s", cntl->ErrorText().c_str());
    ASSERT_FALSE(cntl->Failed());
    ASSERT_TRUE(cntl->IsRequestSent());
    ASSERT_GT(cntl->SentBytes(), 0);
    ASSERT_EQ("Hello from qinzuoyan01", response->message());

    delete request;
    delete response;
    delete cntl;
    delete stub;
}

TEST_F(MockTest, test_registered_mock_sync_fail)
{
    SOFA_PBRPC_CLEAR_MOCK_METHOD();
    SOFA_PBRPC_REGISTER_MOCK_METHOD("sofa.pbrpc.test.EchoServer.Echo",
            NewPermanentExtClosure(&MockEchoFail));

    RpcChannel mock_channel(_rpc_client, SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX + std::string("a:0"));
    EchoServer_Stub* stub = new EchoServer_Stub(&mock_channel);
    RpcController* cntl = new sofa::pbrpc::RpcController();
    EchoRequest* request = new EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    EchoResponse* response = new EchoResponse();

    stub->Echo(cntl, request, response, NULL);
    ASSERT_TRUE(cntl->Failed());
    ASSERT_FALSE(cntl->IsRequestSent());
    ASSERT_EQ(RPC_ERROR_FROM_USER, cntl->ErrorCode());
    ASSERT_EQ("RPC_ERROR_FROM_USER: mock failed", cntl->ErrorText());

    delete request;
    delete response;
    delete cntl;
    delete stub;
}

void EchoCallback(sofa::pbrpc::RpcController* cntl, bool* callbacked)
{
    SLOG(NOTICE, "EchoCallback(): %s", cntl->ErrorText().c_str());
    ASSERT_TRUE(cntl->Failed());
    ASSERT_EQ(RPC_ERROR_FROM_USER, cntl->ErrorCode());
    ASSERT_EQ("RPC_ERROR_FROM_USER: mock failed", cntl->ErrorText());
    *callbacked = true;
}
TEST_F(MockTest, test_registered_mock_async_fail)
{
    SOFA_PBRPC_CLEAR_MOCK_METHOD();
    SOFA_PBRPC_REGISTER_MOCK_METHOD("sofa.pbrpc.test.EchoServer.Echo",
            NewPermanentExtClosure(&MockEchoFail));

    RpcChannel mock_channel(_rpc_client, SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX + std::string("a:0"));
    EchoServer_Stub* stub = new EchoServer_Stub(&mock_channel);
    RpcController* cntl = new sofa::pbrpc::RpcController();
    EchoRequest* request = new EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    EchoResponse* response = new EchoResponse();
    bool callbacked = false;
    google::protobuf::Closure* done = NewClosure(&EchoCallback, cntl, &callbacked);

    stub->Echo(cntl, request, response, done);
    while (!callbacked) {
        usleep(10);
    }
    ASSERT_TRUE(cntl->Failed());
    ASSERT_EQ(RPC_ERROR_FROM_USER, cntl->ErrorCode());
    ASSERT_EQ("RPC_ERROR_FROM_USER: mock failed", cntl->ErrorText());

    delete request;
    delete response;
    delete cntl;
    delete stub;
}

TEST_F(MockTest, test_unregistered_method)
{
    SOFA_PBRPC_CLEAR_MOCK_METHOD();

    RpcChannel mock_channel(_rpc_client, SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX + std::string("a:0"));
    EchoServer_Stub* stub = new EchoServer_Stub(&mock_channel);
    RpcController* cntl = new sofa::pbrpc::RpcController();
    EchoRequest* request = new EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    EchoResponse* response = new EchoResponse();

    stub->Echo(cntl, request, response, NULL);
    ASSERT_TRUE(cntl->Failed());
    ASSERT_EQ(RPC_ERROR_FOUND_METHOD, cntl->ErrorCode());
    ASSERT_EQ("RPC_ERROR_FOUND_METHOD: mock method not registered: sofa.pbrpc.test.EchoServer.Echo", cntl->ErrorText());

    delete request;
    delete response;
    delete cntl;
    delete stub;
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
