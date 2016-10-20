// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

volatile bool g_quit = false;

static void SignalIntHandler(int /* sig */)
{
    g_quit = true;
}

int main(int argc, char** argv)
{
    // check command line arguments.
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <address1> <address2> ...\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::vector<std::string> address_list;
    for (int i = 1; i < argc; ++i) {
        address_list.push_back(argv[i]);
    }

    std::string message_str;
    message_str.resize(100, 'z');

    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, address_list, channel_options);

    // Prepare objects.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
    request->set_message(message_str);
    sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    int count = 0;
    long elapsed_time_us;
    struct timeval tv1, tv2;
    struct timezone tz1, tz2;
    while (!g_quit) {
        ++count;
        usleep(200000);
        cntl->Reset();
        cntl->SetTimeout(3000);
        gettimeofday(&tv1, &tz1);
        stub->Echo(cntl, request, response, NULL);
        gettimeofday(&tv2, &tz2);
        if (cntl->Failed()) {
            SLOG(ERROR, "* %d * request failed: %s", count, cntl->ErrorText().c_str());
            continue;
        }
        if (response->message().size() != request->message().size()
                || response->message().at(0) != request->message().at(0)
                || response->message().at(response->message().size() - 1)
                != request->message().at(request->message().size() - 1)) {
            SLOG(ERROR, "* %d * response not matched", count);
            continue;
        }
        elapsed_time_us = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
        SLOG(NOTICE, " * %d * request succeed, remote_address=%s, elapsed_time_in_us=%ld",
                count, cntl->RemoteAddress().c_str(), elapsed_time_us);
    }
    
    delete request;
    delete response;
    delete cntl;
    delete stub;

    rpc_client.Shutdown();
    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
