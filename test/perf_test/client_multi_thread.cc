// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <pthread.h>
#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

#define MAX_PENDING_COUNT 1000

static volatile bool s_is_running = true;
sofa::pbrpc::AtomicCounter* pending = NULL;
std::string server_addr = "127.0.0.1:12321";
std::string message_str = "z";

void sigcatcher(int sig)
{
    SLOG(NOTICE, "signal catched: %d", sig);
    s_is_running = false;
}

void EchoCallback(int id, sofa::pbrpc::RpcController* cntl,
        sofa::pbrpc::test::EchoResponse* response) 
{
    if (cntl->Failed()) 
    {
        SLOG(WARNING, "request failed: %s", cntl->ErrorText().c_str());
    } 
    delete response;
    delete cntl;
    --pending[id];
}

void* StartCall(void* arg)
{
    using namespace sofa::pbrpc::test;
    int id = *(static_cast<int*>(arg));
    sofa::pbrpc::RpcClientOptions client_options;
    client_options.work_thread_num = 6;
    sofa::pbrpc::RpcClient rpc_client(client_options);
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, server_addr);
    EchoServer_Stub* stub = new EchoServer_Stub(&rpc_channel);
    EchoRequest* request = new EchoRequest();
    request->set_message(message_str);

    while (s_is_running) 
    {
        while(static_cast<int>(pending[id]) > MAX_PENDING_COUNT) 
        {
            usleep(10);
        }
        sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
        EchoResponse* response = new EchoResponse();
        google::protobuf::Closure* done =
            sofa::pbrpc::NewClosure(&EchoCallback, id, cntl, response);
        cntl->SetTimeout(3000);
        ++pending[id];
        stub->Echo(cntl, request, response, done);
    }
    delete request;
    delete stub;
    return NULL;
}

int main(int argc, char **argv)
{
    if (argc < 5) 
    {
        fprintf(stderr, "Usage: %s <host> <port> <message_size> <thread_number> \n", argv[0]);
        return EXIT_FAILURE;
    }

    server_addr = argv[1] + std::string(":") + argv[2];
    const int message_size = atoi(argv[3]);
    message_str.resize(message_size, 'z');
    const int tn = atoi(argv[4]);
    
    signal(SIGINT,  &sigcatcher);
    signal(SIGTERM, &sigcatcher);
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);
    
    pthread_t threads[tn];
    pending = new sofa::pbrpc::AtomicCounter[tn];
    int ids[tn];
    for (int i = 0; i < tn; ++i) 
    {
        ids[i] = i;
        if (pthread_create(&threads[i], NULL, StartCall, &ids[i]))
        {
            SLOG(ERROR, "start thread failed");
            return EXIT_FAILURE;
        }
    }
    for (int i = 0; i < tn; ++i) 
    {
        pthread_join(threads[i], NULL);
    }
    delete []pending;
    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
