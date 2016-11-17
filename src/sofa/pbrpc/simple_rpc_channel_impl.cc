// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/simple_rpc_channel_impl.h>
#include <sofa/pbrpc/mock_test_helper.h>
#include <sofa/pbrpc/closure.h>

namespace sofa {
namespace pbrpc {

SimpleRpcChannelImpl::SimpleRpcChannelImpl(const RpcClientImplPtr& rpc_client_impl,
                                           const std::string& server_address,
                                           const RpcChannelOptions& options)
    : _client_impl(rpc_client_impl)
    , _server_address(server_address)
    , _options(options)
    , _is_mock(false)
    , _resolve_address_succeed(false)
{
}

SimpleRpcChannelImpl::~SimpleRpcChannelImpl()
{
}

bool SimpleRpcChannelImpl::Init()
{
    if (g_enable_mock && _server_address.find(SOFA_PBRPC_MOCK_CHANNEL_ADDRESS_PREFIX) == 0)
    {
#if defined( LOG )
        LOG(INFO) << "Init(): use mock channel";
#else
        SLOG(INFO, "Init(): use mock channel");
#endif
        _is_mock = true;
        return true;
    }

    if (_client_impl->ResolveAddress(_server_address, &_remote_endpoint))
    {
#if defined( LOG )
        LOG(INFO) << "Init(): resolve address succeed: " << _server_address
                  << " [" << RpcEndpointToString(_remote_endpoint) << "]";
#else
        SLOG(INFO, "Init(): resolve address succeed: %s [%s]",
                _server_address.c_str(), RpcEndpointToString(_remote_endpoint).c_str());
#endif
        _resolve_address_succeed = true;
        return true;
    }
    else
    {
#if defined( LOG )
        LOG(ERROR) << "Init(): resolve address failed: " << _server_address;
#else
        SLOG(ERROR, "Init(): resolve address failed: %s",
                _server_address.c_str());
#endif
        _resolve_address_succeed = false;
        return false;
    }
}

void SimpleRpcChannelImpl::Stop()
{
#if defined( LOG )
    LOG(INFO) << "Stop(): simple rpc channel stopped: " << _server_address;
#else
    SLOG(INFO, "Stop(): simple rpc channel stopped: %s", _server_address.c_str());
#endif
}

void SimpleRpcChannelImpl::CallMethod(const ::google::protobuf::MethodDescriptor* method,
                                      ::google::protobuf::RpcController* controller,
                                      const ::google::protobuf::Message* request,
                                      ::google::protobuf::Message* response,
                                      ::google::protobuf::Closure* done)
{
    ++_wait_count;

    // prepare controller
    RpcController* sofa_controller = dynamic_cast<RpcController*>(controller);
    SCHECK(sofa_controller != NULL); // should be sofa::pbrpc::RpcController
    RpcControllerImplPtr cntl = sofa_controller->impl();
    cntl->PushDoneCallback(boost::bind(&SimpleRpcChannelImpl::DoneCallback,
                shared_from_this(), done, _1));
    cntl->FillFromMethodDescriptor(method);
    if (done == NULL)
    {
        cntl->SetSync(); // null done means sync call
        cntl->SetWaitEvent(WaitEventPtr(new WaitEvent()));
    }

    // check if mocked
    if (g_enable_mock && _is_mock)
    {
        MockMethodHookFunction* mock_closure =
            MockTestHelper::GlobalInstance()->GetMockMethod(method->full_name());
        if (mock_closure)
        {
            // mock method registered
#if defined( LOG )
            LOG(INFO) << "CallMethod(): mock method [" << method->full_name() << "] called";
#else
            SLOG(INFO, "CallMethod(): mock method [%s] called", method->full_name().c_str());
#endif
            ::google::protobuf::Closure* mock_done =
                NewClosure(&SimpleRpcChannelImpl::MockDoneCallback, cntl, request, response);
            mock_closure->Run(controller, request, response, mock_done);
        }
        else
        {
            // mock method not registered, but it is in mock channel
#if defined( LOG )
            LOG(ERROR) << "CallMethod(): mock method [" << method->full_name()
                       << "] not registered, but used in mock channel";
#else
            SLOG(ERROR, "CallMethod(): mock method [%s] not registered"
                    ", but used in mock channel", method->full_name().c_str());
#endif
            cntl->Done(RPC_ERROR_FOUND_METHOD, "mock method not registered: "
                    + method->full_name());
        }
        WaitDone(cntl);
        return;
    }

    if (!_resolve_address_succeed)
    {
        // TODO resolve address failed, retry resolve?
#if defined( LOG )
        LOG(ERROR) << "CallMethod(): resolve address failed: " << _server_address;
#else
        SLOG(ERROR, "CallMethod(): resolve address failed: %s", _server_address.c_str());
#endif
        cntl->Done(RPC_ERROR_RESOLVE_ADDRESS, _server_address);
        WaitDone(cntl);
        return;
    }

    // ready, go ahead to do real call
    cntl->SetRemoteEndpoint(_remote_endpoint);
    cntl->StartTimer();
    _client_impl->CallMethod(request, response, cntl);
    WaitDone(cntl);
}

uint32 SimpleRpcChannelImpl::WaitCount()
{
    return _wait_count;
}

void SimpleRpcChannelImpl::WaitDone(const RpcControllerImplPtr& cntl)
{
    // if sync, wait for callback done
    if (cntl->IsSync())
    {
        cntl->WaitEvent()->Wait();
        SCHECK(cntl->IsDone());
    }
}

void SimpleRpcChannelImpl::DoneCallback(google::protobuf::Closure* done,
                                        const RpcControllerImplPtr& cntl)
{
    --_wait_count;

    if (cntl->IsSync())
    {
        SCHECK(done == NULL);
        SCHECK(cntl->WaitEvent());
        cntl->WaitEvent()->Signal();
    }
    else
    {
        SCHECK(done != NULL);
        _client_impl->GetCallbackThreadGroup()->post(done);
    }
}

void SimpleRpcChannelImpl::MockDoneCallback(RpcControllerImplPtr cntl,
                                            const ::google::protobuf::Message* request,
                                            ::google::protobuf::Message* /*response*/)
{
    if (!cntl->Failed())
    {
        cntl->NotifyRequestSent(RpcEndpoint(), request->ByteSize());
    }
    cntl->Done(cntl->ErrorCode(), cntl->Reason());
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
