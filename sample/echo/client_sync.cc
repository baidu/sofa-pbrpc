// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

// Using global RpcClient object can help share resources such as threads and buffers.
sofa::pbrpc::RpcClient g_rpc_client;

int main()
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&g_rpc_client, "127.0.0.1:12321", channel_options);

    // Prepare parameters.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    cntl->SetTimeout(3000);
    sofa::pbrpc::test::EchoRequest* request =
        new sofa::pbrpc::test::EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    sofa::pbrpc::test::EchoResponse* response =
        new sofa::pbrpc::test::EchoResponse();

    // Sync call.
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);
    stub->Echo(cntl, request, response, NULL);

    // Check if the request has been sent.
    // If has been sent, then can get the sent bytes.
    SLOG(NOTICE, "RemoteAddress=%s", cntl->RemoteAddress().c_str());
    SLOG(NOTICE, "IsRequestSent=%s", cntl->IsRequestSent() ? "true" : "false");
    if (cntl->IsRequestSent())
    {
        SLOG(NOTICE, "LocalAddress=%s", cntl->LocalAddress().c_str());
        SLOG(NOTICE, "SentBytes=%ld", cntl->SentBytes());
    }

    // Check if failed.
    if (cntl->Failed())
    {
        SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
    }
    else
    {
        SLOG(NOTICE, "request succeed: %s", response->message().c_str());
    }

    // Destroy objects.
    delete cntl;
    delete request;
    delete response;
    delete stub;

    return EXIT_SUCCESS;
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
