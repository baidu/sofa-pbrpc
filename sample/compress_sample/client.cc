// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/pbrpc.h>
#include "echo_service.pb.h"

int main(int /*argc*/, char** /*argv*/)
{
    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);

    // Define an rpc client.
    sofa::pbrpc::RpcClientOptions client_options;
    sofa::pbrpc::RpcClient rpc_client(client_options);

    // Define an rpc channel.
    sofa::pbrpc::RpcChannelOptions channel_options;
    sofa::pbrpc::RpcChannel rpc_channel(&rpc_client, "127.0.0.1:12321", channel_options);

    // Prepare params.
    sofa::pbrpc::RpcController* cntl = new sofa::pbrpc::RpcController();
    //cntl->SetTimeout(3000);
    sofa::pbrpc::test::EchoRequest* request =
        new sofa::pbrpc::test::EchoRequest();
    request->set_message("Hello from qinzuoyan01");
    sofa::pbrpc::test::EchoResponse* response =
        new sofa::pbrpc::test::EchoResponse();

    // Sync call.
    sofa::pbrpc::test::EchoServer_Stub* stub =
        new sofa::pbrpc::test::EchoServer_Stub(&rpc_channel);
    stub->Echo(cntl, request, response, NULL);
    int ret = EXIT_SUCCESS;
    if (cntl->Failed())
    {
        SLOG(ERROR, "request failed: %s", cntl->ErrorText().c_str());
        ret = EXIT_FAILURE;
    }
    else
    {
        SLOG(NOTICE, "request succeed: %s", response->message().c_str());
    }

    delete request;
    delete response;
    delete cntl;
    delete stub;

    return ret;
}
/* vim: set ts=4 sw=4 sts=4 tw=100 */
