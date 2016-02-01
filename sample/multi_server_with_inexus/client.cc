// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: zhangdi05@baidu.com 

#include <stdio.h>
#include <unistd.h>
#include <vector>
#include <set>
#include <list>
#include <sstream>
#include <iostream>

#include <sofa/pbrpc/pbrpc.h>
#include <sofa/pbrpc/closure.h>
#include "echo_service.pb.h"
#include "ins_address_provider.h"

int main(int argc, char** argv)
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    //if (argc < 2) {
    //    fprintf(stderr, "USAGE: %s <address-list-file>\n", argv[0]);
    //    return EXIT_FAILURE;
    //}

    //std::string service_name = argv[1];

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient* rpc_client = new sofa::pbrpc::RpcClient(client_options);

    std::string address_list = "db-apptest-sf00.db01.baidu.com:8868,db-apptest-sf00.db01.baidu.com:8869,db-apptest-sf00.db01.baidu.com:8870,db-apptest-sf00.db01.baidu.com:8871,db-apptest-sf00.db01.baidu.com:8872";

    sofa::pbrpc::InsAddressProvider* addr_provider = new  sofa::pbrpc::InsAddressProvider(address_list, "/test/rpc");
    if (!addr_provider)
    {
        SLOG(ERROR, "create inexus sdk failed");
        return EXIT_FAILURE;
    }
    addr_provider->Init();
    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel* rpc_channel = 
        new sofa::pbrpc::RpcChannel(rpc_client, addr_provider, channel_options);

    // Define an rpc stub.
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(rpc_channel);

    while (true) {
        // Prepare parameters.
        sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
        cntl->SetTimeout(3000);
        sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
        request->set_message("Hello from Ins test");
        sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();

        // Sync call.
        stub->Echo(cntl, request, response, NULL);

        // Check if the request has been sent.
        // If has been sent, then can get the sent bytes.
        SLOG(NOTICE, "RemoteAddress=%s", cntl->RemoteAddress().c_str());
        SLOG(NOTICE, "IsRequestSent=%s", cntl->IsRequestSent() ? "true" : "false");
        if (cntl->IsRequestSent()) {
            SLOG(NOTICE, "LocalAddress=%s", cntl->LocalAddress().c_str());
            SLOG(NOTICE, "SentBytes=%ld", cntl->SentBytes());
        }

        // Check if failed.
        if (cntl->Failed()) {
            SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
        }
        else {
            SLOG(NOTICE, "request succeed: %s", response->message().c_str());
        }

        // Destroy objects.
        delete cntl;
        delete request;
        delete response;

        sleep(1);
    }

    delete stub;
    delete rpc_channel;
    delete rpc_client;
    
    delete addr_provider; 

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
