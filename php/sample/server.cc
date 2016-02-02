// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com (Zhangdi Di)

#include <signal.h>
#include <unistd.h>
#include <iostream>
#include <sofa/pbrpc/pbrpc.h>
#include "test.pb.h"

class TestServiceImpl : public sofa::pbrpc::test::TestService
{
public:
    TestServiceImpl() {}
    virtual ~TestServiceImpl() {}

private:
    virtual void test_func(google::protobuf::RpcController* /*controller*/,
                      const ::sofa::pbrpc::test::Request* request,
                      sofa::pbrpc::test::Response* response,
                      google::protobuf::Closure* done)
    {
        SLOG(NOTICE, "request byte size : %d , uapinfo1 value is %s, uapinfo2 value %s",
            request->ByteSize(), request->pair(0).value().c_str(), request->pair(1).value().c_str());
        response->set_debug_message("Debug Info from server");
        sofa::pbrpc::test::Result* result_a = response->add_result();
        result_a->set_index(1);
        result_a->set_data("Debug data in result a");
        sofa::pbrpc::test::Result* result_b = response->add_result();
        result_b->set_index(2);
        result_b->set_data("Debug data in result b");
        done->Run();
    }
};

bool thread_init_func()
{
    sleep(1);
    SLOG(INFO, "Init work thread succeed");
    return true;
}

void thread_dest_func()
{
    SLOG(INFO, "Destroy work thread succeed");
}

int main()
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    options.work_thread_init_func = sofa::pbrpc::NewPermanentExtClosure(&thread_init_func);
    options.work_thread_dest_func = sofa::pbrpc::NewPermanentExtClosure(&thread_dest_func);
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start("0.0.0.0:9008")) {
        SLOG(ERROR, "start server failed");
        return EXIT_FAILURE;
    }
    
    // Register service.
    sofa::pbrpc::test::TestService* test_service = new TestServiceImpl();
    if (!rpc_server.RegisterService(test_service)) {
        SLOG(ERROR, "export service failed");
        return EXIT_FAILURE;
    }

    // Wait signal.
    rpc_server.Run();

    // Stop rpc server.
    rpc_server.Stop();

    // Delete closures.
    // Attention: should delete the closures after server stopped, or may be crash.
    delete options.work_thread_init_func;
    delete options.work_thread_dest_func;

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
