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

class AutoAdjustServiceImpl : public sofa::pbrpc::vui::AutoAdjustService
{
public:
    AutoAdjustServiceImpl() {}
    virtual ~AutoAdjustServiceImpl() {}

private:
    virtual void auto_adjust(google::protobuf::RpcController* /*controller*/,
                      const ::sofa::pbrpc::vui::Info* request,
                      sofa::pbrpc::vui::OutInfo* response,
                      google::protobuf::Closure* done)
    {
        SLOG(NOTICE, "request byte size : %d , uapinfo1 value is %s, uapinfo2 value %s",
            request->ByteSize(), request->uapinfo(0).value().c_str(), request->uapinfo(1).value().c_str());
        response->set_debuginfo("Debug Info from server");
        response->set_query_search(9999);
        sofa::pbrpc::vui::OutNodeInfo* out_node_info_a = response->add_resultinfo();
        out_node_info_a->set_gsample_log("gsample_log in out_node_info_a");
        out_node_info_a->set_index(1);
        out_node_info_a->set_click_weight(3);
        out_node_info_a->set_click_time_ratio_a(0.003333444);
        out_node_info_a->set_click_time_ratio_b(0.001111222);
        sofa::pbrpc::vui::OutNodeInfo* out_node_info_b = response->add_resultinfo();
        out_node_info_b->set_gsample_log("gsample_log in out_node_info_b");
        out_node_info_b->set_index(2);
        out_node_info_b->set_click_weight(30);
        out_node_info_b->set_click_time_ratio_a(0.007777999);
        out_node_info_b->set_click_time_ratio_b(0.006666555);
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
    sofa::pbrpc::vui::AutoAdjustService* echo_service = new AutoAdjustServiceImpl();
    if (!rpc_server.RegisterService(echo_service)) {
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
