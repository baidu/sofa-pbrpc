// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <sofa/pbrpc/rpc_request.h>
#include <sofa/pbrpc/rpc_server_stream.h>
#include <sofa/pbrpc/closure.h>

namespace sofa {
namespace pbrpc{

void RpcRequest::CallMethod(
        MethodBoard* method_board,
        RpcController* controller,
        google::protobuf::Message* request,
        google::protobuf::Message* response)
{
    method_board->ReportProcessBegin();
    google::protobuf::Closure* done = NewClosure(
            shared_from_this(), &RpcRequest::OnCallMethodDone,
            method_board, controller, request, response);
    method_board->GetServiceBoard()->Service()->CallMethod(
            method_board->Descriptor(), controller, request, response, done);
}

void RpcRequest::OnCallMethodDone(
        MethodBoard* method_board,
        RpcController* controller,
        google::protobuf::Message* request,
        google::protobuf::Message* response)
{
    const RpcControllerImplPtr& cntl = controller->impl();
    cntl->SetFinishProcessTime(ptime_now());
    int64 process_time_us =
        (cntl->FinishProcessTime() - cntl->StartProcessTime()).total_microseconds();
    if (cntl->Failed())
    {
#if defined( LOG )
        LOG(ERROR) << "OnCallMethodDone(): " << RpcEndpointToString(_remote_endpoint)
                   << " {" << cntl->SequenceId() << "}"
                   << ": call method \"" << cntl->MethodId() << "\" failed"
                   ": " << RpcErrorCodeToString(cntl->ErrorCode()) << ": " << cntl->Reason();
#else
        SLOG(ERROR, "OnCallMethodDone(): %s {%lu}: call method \"%s\" failed: %s: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), cntl->SequenceId(),
                cntl->MethodId().c_str(),
                RpcErrorCodeToString(cntl->ErrorCode()), cntl->Reason().c_str());
#endif
        method_board->ReportProcessEnd(false, process_time_us);
        SendFailedResponse(cntl->RpcServerStream(), cntl->ErrorCode(), cntl->Reason());
    }
    else
    {
#if defined( LOG )
#else
        SLOG(DEBUG, "OnCallMethodDone(): %s {%lu}: call method \"%s\" succeed",
                RpcEndpointToString(_remote_endpoint).c_str(), cntl->SequenceId(),
                cntl->MethodId().c_str());
#endif
        method_board->ReportProcessEnd(true, process_time_us);
        SendSucceedResponse(cntl, response);
    }

    delete request;
    delete response;
    delete controller;
}

void RpcRequest::SendSucceedResponse(
        const RpcControllerImplPtr& cntl,
        const google::protobuf::Message* response)
{
    std::string err;
    ReadBufferPtr read_buffer = AssembleSucceedResponse(cntl, response, err);
    if (!read_buffer)
    {
#if defined( LOG )
        LOG(ERROR) << "SendSucceedResponse(): " << RpcEndpointToString(_remote_endpoint)
                   << " {" << SequenceId() << "}"
                   << ": assemble response buffer failed: " << err;
#else
        SLOG(ERROR, "SendSucceedResponse(): %s {%lu}: assemble response buffer failed: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(), err.c_str());
#endif
        return;
    }

    SendSucceedResponse(cntl->RpcServerStream(), read_buffer);
}

void RpcRequest::SendSucceedResponse(
        const RpcServerStreamWPtr& stream,
        const ReadBufferPtr& buffer)
{
    RpcServerStreamPtr real_stream = stream.lock();
    if (!real_stream)
    {
#if defined( LOG )
        LOG(ERROR) << "SendSucceedResponse(): " << RpcEndpointToString(_remote_endpoint)
                   << " {" << SequenceId() << "}"
                   << ": stream already closed, ignore";
#else
        SLOG(ERROR, "SendSucceedResponse(): %s {%lu}: stream already closed, ignore",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId());
#endif
        return;
    }

    real_stream->send_response(buffer,
            boost::bind(&RpcRequest::OnSendResponseDone, shared_from_this(), _1));
}

void RpcRequest::SendFailedResponse(
        const RpcServerStreamWPtr& stream,
        int32 error_code,
        const std::string& reason)
{
    uint64 sequence_id = SequenceId();
    RpcServerStreamPtr real_stream = stream.lock();
    if (!real_stream)
    {
#if defined( LOG )
        LOG(ERROR) << "SendFailedResponse(): " << RpcEndpointToString(_remote_endpoint)
                   << " {" << sequence_id << "}"
                   << ": stream already closed, ignore";
#else
        SLOG(ERROR, "SendFailedResponse(): %s {%lu}: stream already closed, ignore",
                RpcEndpointToString(_remote_endpoint).c_str(), sequence_id);
#endif
        return;
    }

    std::string err;
    ReadBufferPtr read_buffer = AssembleFailedResponse(error_code, reason, err);
    if (!read_buffer)
    {
#if defined( LOG )
        LOG(ERROR) << "SendFailedResponse(): " << RpcEndpointToString(_remote_endpoint)
                   << " {" << sequence_id << "}"
                   << ": assemble response buffer failed: " << err;
#else
        SLOG(ERROR, "SendFailedResponse(): %s {%lu}: assemble response buffer failed: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), sequence_id, err.c_str());
#endif
        return;
    }

    real_stream->send_response(read_buffer,
            boost::bind(&RpcRequest::OnSendResponseDone, shared_from_this(), _1));
}

void RpcRequest::OnSendResponseDone(
        RpcErrorCode error_code)
{
    if (error_code == RPC_SUCCESS)
    {
#if defined( LOG )
#else
        SLOG(DEBUG, "OnSendResponseDone(): %s {%lu}: send succeed",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId());
#endif
    }
    else
    {
#if defined( LOG )
        LOG(ERROR) << "OnSendResponseDone(): " << RpcEndpointToString(_remote_endpoint)
                   << " {" << SequenceId() << "}"
                   << ": send failed: " << RpcErrorCodeToString(error_code);
#else
        SLOG(ERROR, "OnSendResponseDone(): %s {%lu}: send failed: %s",
                RpcEndpointToString(_remote_endpoint).c_str(), SequenceId(),
                RpcErrorCodeToString(error_code));
#endif
    }
}

bool RpcRequest::ParseMethodFullName(
        const std::string& method_full_name,
        std::string* service_name,
        std::string* method_name)
{
    std::string::size_type pos = method_full_name.rfind('.');
    if (pos == std::string::npos)
    {
        return false;
    }
    *service_name = method_full_name.substr(0, pos);
    *method_name = method_full_name.substr(pos + 1);
    return true;
}

MethodBoard* RpcRequest::FindMethodBoard(
        const ServicePoolPtr& service_pool,
        const std::string& service_name,
        const std::string& method_name)
{
    ServiceBoard* service_board = service_pool->FindService(service_name);
    if (service_board == NULL)
    {
        return NULL;
    }
    const google::protobuf::MethodDescriptor* method_desc =
        service_board->Descriptor()->FindMethodByName(method_name);
    if (method_desc == NULL)
    {
        return NULL;
    }
    return service_board->Method(method_desc->index());
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
