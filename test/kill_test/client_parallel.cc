// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

static volatile bool s_is_running = true;
static volatile bool s_should_wait = false;
static sofa::pbrpc::AtomicCounter s_print_token(1);
static sofa::pbrpc::AtomicCounter s_succeed_count(0);
static sofa::pbrpc::AtomicCounter s_pending_count(0);

void sigcatcher(int sig)
{
    SLOG(NOTICE, "signal catched: %d", sig);
    s_is_running = false;
}

void EchoCallback(
        sofa::pbrpc::RpcController* cntl,
        const sofa::pbrpc::test::EchoRequest* request,
        sofa::pbrpc::test::EchoResponse* response)
{
    --s_pending_count;

    if (cntl->Failed()) {
        if (cntl->ErrorCode() == sofa::pbrpc::RPC_ERROR_SEND_BUFFER_FULL) {
            s_should_wait = true;
        }
        else {
            s_is_running = false;
            if (--s_print_token == 0) 
            {
                SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
            }
        }
    }
    else if (request->message() != response->message()) {
        s_is_running = false;
        if (--s_print_token == 0) 
        {
            SLOG(ERROR, "request failed: response not matched");
        }
    }
    else {
        ++s_succeed_count;
    }

    delete response;
    delete cntl;
}

int main(int argc, char **argv)
{
    // check command line arguments.
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <host> <port> <message_size> [is_grace_exit]\n", argv[0]);
        return EXIT_FAILURE;
    }

    std::string address = argv[1] + std::string(":") + argv[2];
    int message_size = atoi(argv[3]);
    std::string message_str;
    message_str.resize(message_size, 'z');
    bool is_grace_exit = true;
    if (argc > 4)
    {
        if (strcmp(argv[4], "true") == 0)
            is_grace_exit = true;
        else if (strcmp(argv[4], "false") == 0)
            is_grace_exit = false;
        else
        {
            fprintf(stderr, "Invalid param 'is_grace_exit': should be 'true' or 'false'");
            return EXIT_FAILURE;
        }
    }

    signal(SIGINT,  &sigcatcher);
    signal(SIGTERM, &sigcatcher);

    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    client_options.max_throughput_in = 30;
    client_options.max_throughput_out = 30;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel and stub.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, address, channel_options);
    sofa::pbrpc::test::EchoServer_Stub stub(&rpc_channel);
    sofa::pbrpc::test::EchoRequest echo_request;
    echo_request.set_message(message_str);

    long elapsed_time_us;
    long print_interval_us = 1000000;
    struct timeval tv1, tv2;
    struct timezone tz1, tz2;
    gettimeofday(&tv1, &tz1);
    while (s_is_running) {
        sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
        const sofa::pbrpc::test::EchoRequest* request = &echo_request;
        sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
        google::protobuf::Closure* done = sofa::pbrpc::NewClosure(&EchoCallback, cntl, request, response);
        ++s_pending_count;
        stub.Echo(cntl, request, response, done);

        if (s_should_wait) {
            s_should_wait = false;
            usleep(10000);
        }

        gettimeofday(&tv2, &tz2);
        elapsed_time_us = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
        if (elapsed_time_us >= print_interval_us) {
            SLOG(NOTICE, "succeed_count=%lld, pending_count=%lld",
                    static_cast<long>(s_succeed_count),
                    static_cast<long>(s_pending_count));
            gettimeofday(&tv1, &tz1);
        }
    }

    if (is_grace_exit) {
        SLOG(NOTICE, "gracely exiting ...");
        while (s_pending_count > 0) {
            SLOG(NOTICE, "pending count: %lld", static_cast<long>(s_pending_count));
            usleep(print_interval_us);
        }
    }

    rpc_client.Shutdown(); // should call Shutdown here!
    fprintf(stdout, "Succeed %ld", static_cast<long>(s_succeed_count));
    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
