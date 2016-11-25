// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <sys/time.h>
#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

static sofa::pbrpc::AtomicCounter s_succeed_count(0);

class EchoServerImpl : public sofa::pbrpc::test::EchoServer
{
public:
    EchoServerImpl() {}
    virtual ~EchoServerImpl() {}

private:
    virtual void Echo(google::protobuf::RpcController* /* controller */,
                      const sofa::pbrpc::test::EchoRequest* request,
                      sofa::pbrpc::test::EchoResponse* response,
                      google::protobuf::Closure* done)
    {
        response->set_message(request->message());
        ++s_succeed_count;
        done->Run();
    }
};

volatile bool g_quit = false;

static void SignalIntHandler(int /* sig */)
{
    g_quit = true;
}

int main(int argc, char** argv)
{
    if (argc < 4)
    {
        fprintf(stderr, "Usage: %s <host> <port> <thread_num>\n", argv[0]);
        return EXIT_FAILURE;
    }
    std::string address = argv[1] + std::string(":") + argv[2];
    int thread_num = atoi(argv[3]);

    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc server.
    sofa::pbrpc::RpcServerOptions options;
    options.work_thread_num = thread_num;
    options.max_pending_buffer_size = 100;
    sofa::pbrpc::RpcServer rpc_server(options);

    // Start rpc server.
    if (!rpc_server.Start(address)) {
        SLOG(ERROR, "start server failed");
        return EXIT_FAILURE;
    }
    
    sofa::pbrpc::test::EchoServer* echo_service = new EchoServerImpl();
    if (!rpc_server.RegisterService(echo_service)) {
        SLOG(ERROR, "export service failed");
        return EXIT_FAILURE;
    }

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    long elapsed_time_us;
    long print_interval_us = 1000000;
    struct timeval tv1, tv2;
    struct timezone tz1, tz2;
    gettimeofday(&tv1, &tz1);
    long last_succeed_count = 0;
    while (!g_quit) {
        usleep(100000);

        gettimeofday(&tv2, &tz2);
        elapsed_time_us = (tv2.tv_sec - tv1.tv_sec) * 1000000 + (tv2.tv_usec - tv1.tv_usec);
        if (elapsed_time_us >= print_interval_us) {
            long curr_succeed_count = static_cast<long>(s_succeed_count);
            SLOG(NOTICE, "QPS=%ld", curr_succeed_count - last_succeed_count);
            last_succeed_count = curr_succeed_count;
            gettimeofday(&tv1, &tz1);
        }
    }

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
