// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/binary_rpc_request.h>
#include <sofa/pbrpc/compressed_stream.h>
#include <sofa/pbrpc/rpc_error_code.h>
#include <sofa/pbrpc/rpc_server_stream.h>

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

std::string BinaryRpcRequest::Method()
{
    return _req_meta.method();
}

uint64 BinaryRpcRequest::SequenceId()
{
    return _req_meta.sequence_id();
}

void BinaryRpcRequest::ProcessRequest(
        const RpcServerStreamWPtr& stream,
        const ServicePoolPtr& service_pool)
{
    std::string service_name;
    std::string method_name;
    if (!ParseMethodFullName(_req_meta.method(), &service_name, &method_name))
    {
#if defined( LOG )
        LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << _req_meta.sequence_id() << "}"
                   << ": invalid method full name: " << _req_meta.method();
#else
        SLOG(ERROR, "ProcessRequest(): %s: {%lu}: invalid method full name: %s",
                RpcEndpointToString(_remote_endpoint).c_str(),
                _req_meta.sequence_id(), _req_meta.method().c_str());
#endif
        SendFailedResponse(stream,
                RPC_ERROR_PARSE_METHOD_NAME, "method full name: " + _req_meta.method());
        return;
    }

    MethodBoard* method_board = FindMethodBoard(service_pool, service_name, method_name);
    if (method_board == NULL)
    {
#if defined( LOG )
        LOG(ERROR) << "ProcessRequest(): " << RpcEndpointToString(_remote_endpoint)
                   << ": {" << _req_meta.sequence_id() << "}"
                   << ": method not found: " << _req_meta.method();
#else
        SLOG(ERROR, "ProcessRequest(): %s: {%lu}: method not found: %s",
                RpcEndpointToString(_remote_endpoint).c_str(),
                _req_meta.sequence_id(), _req_meta.method().c_str());
#endif
        SendFailedResponse(stream,
                RPC_ERROR_FOUND_METHOD, "method full name: " + _req_meta.method());
        return;
    }

    google::protobuf::Service* service = method_board->GetServiceBoard()->Service();
    const google::protobuf::MethodDescriptor* method_desc = method_board->Descriptor();

    google::protobuf::Message* request = service->GetRequestPrototype(method_desc).New();
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

    google::protobuf::Message* response = service->GetResponsePrototype(method_desc).New();

    RpcController* controller = new RpcController();
    const RpcControllerImplPtr& cntl = controller->impl();
    cntl->SetSequenceId(_req_meta.sequence_id());
    cntl->SetMethodId(_req_meta.method());
    cntl->SetLocalEndpoint(_local_endpoint);
    cntl->SetRemoteEndpoint(_remote_endpoint);
    cntl->SetRpcServerStream(stream);
    if (_req_meta.has_server_timeout() && _req_meta.server_timeout() > 0)
    {
        cntl->SetServerTimeout(_req_meta.server_timeout());
    }
    cntl->SetRequestReceivedTime(_received_time);
    cntl->SetResponseCompressType(_req_meta.has_expected_response_compress_type() ?
            _req_meta.expected_response_compress_type() : CompressTypeNone);

    CallMethod(method_board, controller, request, response);
}

ReadBufferPtr BinaryRpcRequest::AssembleSucceedResponse(
        const RpcControllerImplPtr& cntl,
        const google::protobuf::Message* response,
        std::string& err)
{
    RpcMeta meta;
    meta.set_type(RpcMeta::RESPONSE);
    meta.set_sequence_id(cntl->SequenceId());
    meta.set_failed(false);
    meta.set_compress_type(cntl->ResponseCompressType());

    RpcMessageHeader header;
    int header_size = static_cast<int>(sizeof(header));
    WriteBuffer write_buffer;
    write_buffer.set_base_block_factor(_write_buffer_base_block_factor);

    int64 header_pos = write_buffer.Reserve(header_size);
    if (header_pos < 0)
    {
        err = "reserve rpc message header failed";
        return ReadBufferPtr();
    }
    if (!meta.SerializeToZeroCopyStream(&write_buffer))
    {
        err = "serialize rpc meta failed";
        return ReadBufferPtr();
    }
    header.meta_size = static_cast<int>(write_buffer.ByteCount() - header_pos - header_size);
    bool ser_ret = false;
    if (meta.compress_type() == CompressTypeNone)
    {
        ser_ret = response->SerializeToZeroCopyStream(&write_buffer);
    }
    else
    {
        sofa::pbrpc::scoped_ptr<AbstractCompressedOutputStream> os(
                get_compressed_output_stream(&write_buffer, meta.compress_type()));
        ser_ret = response->SerializeToZeroCopyStream(os.get());
        os->Flush();
    }
    if (!ser_ret)
    {
        err = "serialize response message failed";
        return ReadBufferPtr();
    }
    header.data_size = write_buffer.ByteCount() - header_pos - header_size - header.meta_size;
    header.message_size = header.meta_size + header.data_size;
    write_buffer.SetData(header_pos, reinterpret_cast<const char*>(&header), header_size);

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());

    return read_buffer;
}

ReadBufferPtr BinaryRpcRequest::AssembleFailedResponse(
        int32 error_code,
        const std::string& reason,
        std::string& err)
{
    RpcMeta meta;
    meta.set_type(RpcMeta::RESPONSE);
    meta.set_sequence_id(_req_meta.sequence_id());
    meta.set_failed(true);
    meta.set_error_code(error_code);
    meta.set_reason(reason);

    RpcMessageHeader header;
    int header_size = static_cast<int>(sizeof(header));
    WriteBuffer write_buffer;

    int64 header_pos = write_buffer.Reserve(header_size);
    if (header_pos < 0)
    {
        err = "reserve rpc message header failed";
        return ReadBufferPtr();
    }
    if (!meta.SerializeToZeroCopyStream(&write_buffer))
    {
        err = "serialize rpc meta failed";
        return ReadBufferPtr();
    }
    header.meta_size = static_cast<int>(write_buffer.ByteCount() - header_pos - header_size);
    header.data_size = 0;
    header.message_size = header.meta_size + header.data_size;
    write_buffer.SetData(header_pos, reinterpret_cast<const char*>(&header), header_size);

    ReadBufferPtr read_buffer(new ReadBuffer());
    write_buffer.SwapOut(read_buffer.get());

    return read_buffer;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
