#include <sofa/pbrpc/binary_rpc_request.h>
#include <sofa/pbrpc/rpc_server_stream.h>
#include <sofa/pbrpc/compressed_stream.h>
#include <sofa/pbrpc/rpc_controller_impl.h>
#include <sofa/pbrpc/closure.h>

namespace sofa {
namespace pbrpc {

BinaryRpcRequest::BinaryRpcRequest() : _req_body(new ReadBuffer())
{
}

BinaryRpcRequest::~BinaryRpcRequest()
{
}

RpcRequest::RpcRequestType BinaryRpcRequest::RequestType()
{
    return RpcRequest::BINARY;
}

std::string BinaryRpcRequest::MethodFullName()
{
    return _req_meta.method();
}

std::string BinaryRpcRequest::ServiceName()
{
    return _service_name;
}

std::string BinaryRpcRequest::MethodName()
{
    return _method_name;
}

uint64 BinaryRpcRequest::SequenceId()
{
    return _req_meta.sequence_id();
}

void BinaryRpcRequest::ProcessRequest(
        const RpcServerStreamWPtr& stream,
        ServiceBoard* service_board)
{
    google::protobuf::Service* service = service_board->Service();
    const google::protobuf::MethodDescriptor* method =
        service->GetDescriptor()->FindMethodByName(_method_name);
    SCHECK(method != NULL);

    google::protobuf::Message* request = service->GetRequestPrototype(method).New();
    CompressType compress_type =
        _req_meta.has_compress_type() ? _req_meta.compress_type(): CompressTypeNone;
    bool parse_request_return = false;
    if (compress_type == CompressTypeNone)
    {
        parse_request_return = request->ParseFromZeroCopyStream(_req_body.get());
    }
    else
    {
        sofa::pbrpc::scoped_ptr<AbstractCompressedInputStream> is(
                get_compressed_input_stream(_req_body.get(), compress_type));
        parse_request_return = request->ParseFromZeroCopyStream(is.get());
    }
    if (!parse_request_return)
    {
#if defined( LOG )
        LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << _req_meta.sequence_id() << "}: parse request message failed";
#else
        SLOG(ERROR, "ProcessRequest(): %s: {%lu}: parse request message failed",
                RpcEndpointToString(_remote_endpoint).c_str(), _req_meta.sequence_id());
#endif
        SendFailedResponse(stream,
                RPC_ERROR_PARSE_REQUEST_MESSAGE, "method full name: " + _req_meta.method());
        delete request;
        return;
    }

    google::protobuf::Message* response = service->GetResponsePrototype(method).New();
    RpcController* controller = new RpcController();
    const RpcControllerImplPtr& cntl = controller->impl();
    cntl->SetSequenceId(_req_meta.sequence_id());
    cntl->SetMethodId(_req_meta.method());
    cntl->SetLocalEndpoint(_local_endpoint);
    cntl->SetRemoteEndpoint(_remote_endpoint);
    cntl->SetRpcServerStream(stream);
    cntl->SetRequestReceivedTime(ptime_now());
    cntl->SetResponseCompressType(_req_meta.has_expected_response_compress_type() ?
            _req_meta.expected_response_compress_type() : CompressTypeNone);

    MethodBoard* method_board = service_board->Method(method->index());
    method_board->ReportProcessBegin();

    google::protobuf::Closure* done = NewClosure(
            shared_from_this(), &BinaryRpcRequest::OnCallMethodDone,
            controller, request, response, method_board, ptime_now());
    service->CallMethod(method, controller, request, response, done);
}

void BinaryRpcRequest::OnCallMethodDone(
        RpcController* controller,
        google::protobuf::Message* request,
        google::protobuf::Message* response,
        MethodBoard* method_board,
        PTime start_time)
{
    int64 process_time_us = (ptime_now() - start_time).total_microseconds();
    const RpcControllerImplPtr& cntl = controller->impl();
    if (cntl->Failed())
    {
#if defined( LOG )
        LOG(ERROR) << "OnCallMethodDone(): call method \"" << cntl->MethodId()
                   << "\" failed: " << RpcErrorCodeToString(cntl->ErrorCode())
                   << ": " << cntl->Reason();
#else
        SLOG(ERROR, "OnCallMethodDone(): call method \"%s\" failed: %s: %s",
                cntl->MethodId().c_str(),
                RpcErrorCodeToString(cntl->ErrorCode()),
                cntl->Reason().c_str());
#endif
        method_board->ReportProcessEnd(false, process_time_us);
        SendFailedResponse(cntl->RpcServerStream(), cntl->ErrorCode(), cntl->Reason());
    }
    else
    {
#if defined( LOG )
#else
        SLOG(DEBUG, "OnCallMethodDone(): call method \"%s\" succeed",
                cntl->MethodId().c_str());
#endif
        method_board->ReportProcessEnd(true, process_time_us);
        SendSucceedResponse(cntl->RpcServerStream(),
                cntl->ResponseCompressType(), response);
    }

    delete request;
    delete response;
    delete controller;
}

void BinaryRpcRequest::SendFailedResponse(
        const RpcServerStreamWPtr& stream,
        int32 error_code,
        const std::string& reason)
{
    uint64 sequence_id = _req_meta.sequence_id();

    RpcServerStreamPtr real_stream = stream.lock();
    if (!real_stream)
    {
#if defined( LOG )
        LOG(ERROR) << "SendFailedResponse(): {" << sequence_id << "}: stream already closed";
#else
        SLOG(ERROR, "SendFailedResponse(): {%lu}: stream already closed", sequence_id);
#endif
        return;
    }

    RpcMeta meta;
    meta.set_type(RpcMeta::RESPONSE);
    meta.set_sequence_id(sequence_id);
    meta.set_failed(true);
    meta.set_error_code(error_code);
    meta.set_reason(reason);

    RpcMessageHeader header;
    int header_size = static_cast<int>(sizeof(header));
    WriteBuffer write_buffer;
    int64 header_pos = write_buffer.Reserve(header_size);
    if (header_pos < 0)
    {
#if defined( LOG )
        LOG(ERROR) << "SendFailedResponse(): {" << sequence_id << "}"
                   << ": reserve rpc message header failed";
#else
        SLOG(ERROR, "SendFailedResponse(): {%lu}"
                ": reserve rpc message header failed", sequence_id);
#endif
        return;
    }
    if (!meta.SerializeToZeroCopyStream(&write_buffer))
    {
#if defined( LOG )
        LOG(ERROR) << "SendFailedResponse(): {" << sequence_id << "}"
                   << ": serialize rpc meta failed";
#else
        SLOG(ERROR, "SendFailedResponse(): {%lu}"
                ": serialize rpc meta failed", sequence_id);
#endif
        return;
    }
    header.meta_size = static_cast<int>(write_buffer.ByteCount() - header_pos - header_size);
    header.data_size = 0;
    header.message_size = header.meta_size + header.data_size;
    write_buffer.SetData(header_pos, reinterpret_cast<const char*>(&header), header_size);

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());

    real_stream->send_response(read_buffer,
            boost::bind(&BinaryRpcRequest::OnSendResponseDone,
                real_stream->remote_endpoint(), sequence_id, _1));
}

void BinaryRpcRequest::SendSucceedResponse(
        const RpcServerStreamWPtr& stream,
        CompressType compress_type,
        google::protobuf::Message* response)
{
    uint64 sequence_id = _req_meta.sequence_id();

    RpcServerStreamPtr real_stream = stream.lock();
    if (!real_stream)
    {
#if defined( LOG )
        LOG(ERROR) << "SendSucceedResponse(): {" << sequence_id << "}"
                   << ": stream already closed";
#else
        SLOG(ERROR, "SendSucceedResponse(): {%lu}: stream already closed", sequence_id);
#endif
        return;
    }

    RpcMeta meta;
    meta.set_type(RpcMeta::RESPONSE);
    meta.set_sequence_id(sequence_id);
    meta.set_failed(false);
    meta.set_compress_type(compress_type);

    RpcMessageHeader header;
    int header_size = static_cast<int>(sizeof(header));
    WriteBuffer write_buffer;
    int64 header_pos = write_buffer.Reserve(header_size);
    if (header_pos < 0)
    {
#if defined( LOG )
        LOG(ERROR) << "SendSucceedResponse(): {" << sequence_id << "}"
                   << ": reserve rpc message header failed";
#else
        SLOG(ERROR, "SendSucceedResponse(): {%lu}"
                ": reserve rpc message header failed", sequence_id);
#endif
        return;
    }
    if (!meta.SerializeToZeroCopyStream(&write_buffer))
    {
#if defined( LOG )
        LOG(ERROR) << "SendSucceedResponse(): {" << sequence_id << "}"
                   << ": serialize rpc meta failed";
#else
        SLOG(ERROR, "SendSucceedResponse(): {%lu}"
                ": serialize rpc meta failed", sequence_id);
#endif
        SendFailedResponse(stream,
                RPC_ERROR_SERIALIZE_RESPONSE, "serialize rpc meta failed");
        return;
    }
    header.meta_size = static_cast<int>(write_buffer.ByteCount() - header_pos - header_size);
    bool ser_ret = false;
    if (compress_type == CompressTypeNone)
    {
        ser_ret = response->SerializeToZeroCopyStream(&write_buffer);
    }
    else
    {
        sofa::pbrpc::scoped_ptr<AbstractCompressedOutputStream> os(
                get_compressed_output_stream(&write_buffer, compress_type));
        ser_ret = response->SerializeToZeroCopyStream(os.get());
        os->Flush();
    }
    if (!ser_ret)
    {
#if defined( LOG )
        LOG(ERROR) << "SendSucceedResponse(): {" << sequence_id << "}"
                   << ": serialize response message failed";
#else
        SLOG(ERROR, "SendSucceedResponse(): {%lu}"
                ": serialize response message failed", sequence_id);
#endif
        SendFailedResponse(stream,
                RPC_ERROR_SERIALIZE_RESPONSE, "serialize response message failed");
        return;
    }
    header.data_size = write_buffer.ByteCount() - header_pos - header_size - header.meta_size;
    header.message_size = header.meta_size + header.data_size;
    write_buffer.SetData(header_pos, reinterpret_cast<const char*>(&header), header_size);

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());

    real_stream->send_response(read_buffer,
            boost::bind(&BinaryRpcRequest::OnSendResponseDone,
                real_stream->remote_endpoint(), sequence_id, _1));
}

void BinaryRpcRequest::OnSendResponseDone(
        const RpcEndpoint& remote_endpoint,
        uint64 sequence_id,
        RpcErrorCode error_code)
{
    if (error_code == RPC_SUCCESS)
    {
#if defined( LOG )
        LOG(DEBUG) << "OnSendResponseDone(): " << RpcEndpointToString(remote_endpoint)
                   << " {" << sequence_id << "}: send succeed";
#else
        SLOG(DEBUG, "OnSendResponseDone(): %s {%lu}: send succeed",
                RpcEndpointToString(remote_endpoint).c_str(), sequence_id);
#endif
    }
    else
    {
#if defined( LOG )
        LOG(ERROR) << "OnSendResponseDone(): " << RpcEndpointToString(remote_endpoint)
                   << " {" << sequence_id << "}"
                   << ": send failed: " << RpcErrorCodeToString(error_code);
#else
        SLOG(ERROR, "OnSendResponseDone(): %s {%lu}: send failed: %s",
                RpcEndpointToString(remote_endpoint).c_str(), sequence_id,
                RpcErrorCodeToString(error_code));
#endif
    }
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
