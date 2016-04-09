// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <unistd.h>
#include <sofa/pbrpc/pbrpc.h>
#include <sofa/pbrpc/plugin/cookie/rpc_cookie.h>
#include "echo_service.pb.h"

typedef sofa::pbrpc::shared_ptr<sofa::pbrpc::RpcCookie> RpcCookiePtr;
sofa::pbrpc::RpcCookieManager g_cookie_manager;

void EchoCallback(sofa::pbrpc::RpcController* cntl,
        sofa::pbrpc::test::EchoRequest* request,
        sofa::pbrpc::test::EchoResponse* response,
        bool* callbacked)
{
    SLOG(NOTICE, "RemoteAddress=%s", cntl->RemoteAddress().c_str());
    SLOG(NOTICE, "IsRequestSent=%s", cntl->IsRequestSent() ? "true" : "false");
    if (cntl->IsRequestSent())
    {
        SLOG(NOTICE, "LocalAddress=%s", cntl->LocalAddress().c_str());
        SLOG(NOTICE, "SentBytes=%ld", cntl->SentBytes());
    }

    if (cntl->Failed()) {
        SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
    }
    else 
    {
        SLOG(NOTICE, "request succeed: %s", response->message().c_str());
        RpcCookiePtr cookie(new sofa::pbrpc::RpcCookie(&g_cookie_manager));
        if (cntl->GetResponseAttachment(cookie.get()))
        {
            std::string version;
            cookie->Get("version", version);
            SLOG(NOTICE, "cookie version=%s", version.c_str());
            cookie->Store();
        }
    }

    delete cntl;
    delete request;
    delete response;
    *callbacked = true;
}

int main()
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc server.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:12321", channel_options);

    RpcCookiePtr cookie(new sofa::pbrpc::RpcCookie(&g_cookie_manager));
    cookie->Load();
    cookie->Set("type", "async");
    cookie->Set("logid", "123456");

    // Prepare parameters.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    cntl->SetTimeout(3000);
    cntl->SetRequestAttachment(cookie.get());
    sofa::pbrpc::test::EchoRequest* request = new sofa::pbrpc::test::EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    sofa::pbrpc::test::EchoResponse* response = new sofa::pbrpc::test::EchoResponse();
    bool callbacked = false;
    google::protobuf::Closure* done = sofa::pbrpc::NewClosure(
            &EchoCallback, cntl, request, response, &callbacked);

    // Async call.
    sofa::pbrpc::test::EchoServer_Stub stub(&rpc_channel);
    stub.Echo(cntl, request, response, done);

    // Wait call done.
    while (!callbacked) {
        usleep(100000);
    }

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
