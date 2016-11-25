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
    if (argc < 4) {
        fprintf(stderr, "Usage: %s <host> <port> <message_size>\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::string address = argv[1] + std::string(":") + argv[2];
    int message_size = atoi(argv[3]);
    std::string message_str;
    message_str.resize(message_size, 'z');

    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, address, channel_options);

    // Prepare objects.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
    request->set_message(message_str);
    sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
    sofa::pbrpc::test::EchoServer_Stub* stub = new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    int count = 0;
    long elapsed_time_us;
    struct timeval tv1, tv2;
    struct timezone tz1, tz2;
    while (!g_quit) {
        ++count;
        cntl->Reset();
        cntl->SetTimeout(3000);
        gettimeofday(&tv1, &tz1);
        stub->Echo(cntl, request, response, NULL);
        gettimeofday(&tv2, &tz2);
        if (cntl->Failed()) {
            SLOG(ERROR, "* %d * request failed: %s", count, cntl->ErrorText().c_str());
            break;
        }
        if (response->message().size() != request->message().size()
                || response->message().at(0) != request->message().at(0)
                || response->message().at(response->message().size() / 2)
                   != request->message().at(request->message().size() / 2)
                || response->message().at(response->message().size() - 1)
                   != request->message().at(request->message().size() - 1)) {
            SLOG(ERROR, "response not matched");
            break;
        }
        elapsed_time_us = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
        SLOG(NOTICE, " * %d * request succeed, elapsed time in us: %ld", count, elapsed_time_us);
    }
    
    delete request;
    delete response;
    delete cntl;
    delete stub;
    rpc_client.Shutdown();
    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
