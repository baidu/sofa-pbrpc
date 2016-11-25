// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <signal.h>
#include <unistd.h>

#include <sofa/pbrpc/rpc_server.h>
#include <sofa/pbrpc/rpc_server_impl.h>

namespace sofa {
namespace pbrpc {

RpcServer::RpcServer(const RpcServerOptions& options, 
                     EventHandler* handler, 
                     const ProfilingLinker& /*linker*/)
{
    touch_boost_error_category();
    _impl.reset(new RpcServerImpl(options, handler));
}

RpcServer::~RpcServer()
{
    Stop();
}

bool RpcServer::Start(const std::string& server_address)
{
    return _impl->Start(server_address);
}

void RpcServer::Stop()
{
    _impl->Stop();
}

static volatile bool s_quit = false;
static void SignalIntHandler(int /*sig*/)
{
    s_quit = true;
}
int RpcServer::Run()
{
    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);
    while (!s_quit) {
        sleep(1);
    }
    return 0;
}

RpcServerOptions RpcServer::GetOptions()
{
    return _impl->GetOptions();
}

void RpcServer::ResetOptions(const RpcServerOptions& options)
{
    _impl->ResetOptions(options);
}

bool RpcServer::RegisterService(google::protobuf::Service* service, bool take_ownership)
{
    return _impl->RegisterService(service, take_ownership);
}

int RpcServer::ServiceCount()
{
    return _impl->ServiceCount();
}

int RpcServer::ConnectionCount()
{
    return _impl->ConnectionCount();
}

bool RpcServer::IsListening()
{
    return _impl->IsListening();
}

bool RpcServer::RegisterWebServlet(const std::string& path, Servlet servlet, bool take_ownership)
{
    return _impl->RegisterWebServlet(path, servlet, take_ownership);
}

Servlet RpcServer::UnregisterWebServlet(const std::string& path)
{
    return _impl->UnregisterWebServlet(path);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
