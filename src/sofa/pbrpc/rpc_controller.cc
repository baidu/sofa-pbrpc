// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/rpc_controller.h>
#include <sofa/pbrpc/rpc_controller_impl.h>

namespace sofa {
namespace pbrpc {

RpcController::RpcController()
    : _impl(new RpcControllerImpl())
{
}

RpcController::~RpcController()
{
}

std::string RpcController::LocalAddress() const
{
    return RpcEndpointToString(_impl->LocalEndpoint());
}

std::string RpcController::RemoteAddress() const
{
    return RpcEndpointToString(_impl->RemoteEndpoint());
}

void RpcController::Reset()
{
    _impl.reset(new RpcControllerImpl());
}

void RpcController::SetTimeout(int64 timeout_in_ms)
{
    _impl->SetTimeout(timeout_in_ms);
}

int64 RpcController::Timeout() const
{
    return _impl->Timeout();
}

void RpcController::SetRequestCompressType(CompressType compress_type)
{
    _impl->SetRequestCompressType(compress_type);
}

void RpcController::SetResponseCompressType(CompressType compress_type)
{
    _impl->SetResponseCompressType(compress_type);
}

bool RpcController::Failed() const
{
    return _impl->Failed();
}

int RpcController::ErrorCode() const
{
    return _impl->ErrorCode();
}

std::string RpcController::ErrorText() const
{
    return _impl->ErrorText();
}

bool RpcController::IsRequestSent() const
{
    return _impl->IsRequestSent();
}

int64 RpcController::SentBytes() const
{
    return _impl->SentBytes();
}

void RpcController::StartCancel()
{
    _impl->StartCancel();
}

bool RpcController::IsHttp() const
{
    return _impl->IsHttp();
}

const std::string& RpcController::HttpPath() const
{
    return _impl->HttpPath();
}

const std::map<std::string, std::string>& RpcController::HttpQueryParams() const
{
    return _impl->HttpQueryParams();
}

const std::map<std::string, std::string>& RpcController::HttpHeaders() const
{
    return _impl->HttpHeaders();
}

void RpcController::SetFailed(const std::string& reason)
{
    _impl->SetFailed(reason);
}

bool RpcController::IsCanceled() const
{
    return _impl->IsCanceled();
}

void RpcController::NotifyOnCancel(google::protobuf::Closure* callback)
{
    _impl->NotifyOnCancel(callback);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
