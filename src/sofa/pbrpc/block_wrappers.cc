// This file is modified from `protobuf-zerocopy-compression':
//   https://github.com/JohannesEbke/protobuf-zerocopy-compression

// Copyright (c) 2013, Johannes Ebke and Peter Waller. All rights reserved.
// Author: peter.waller@gmail.com (Peter Waller)
// Author: johannes@ebke.org (Johannes Ebke)

#include <google/protobuf/stubs/common.h>

#include <google/protobuf/io/coded_stream.h>
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::CodedOutputStream;

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/block_wrappers.h>
#include <sofa/pbrpc/lz4.h>

#if HAVE_SNAPPY
#include <snappy.h>
#endif

namespace sofa {
namespace pbrpc {

const size_t BLOCKSIZE = 64 * 1024;

BlockCompressionInputStream::BlockCompressionInputStream(
    ZeroCopyInputStream* sub_stream)
    : _output_buffer(NULL), _output_buffer_size(0), _output_buffer_capacity(0),
      _sub_stream(NULL), _backed_up_bytes(0), _byte_count(0) {
    _raw_stream = sub_stream;
    _sub_stream = new CodedInputStream(_raw_stream);
    _sub_stream->SetTotalBytesLimit(1 << 30, 1 << 30);
}

BlockCompressionInputStream::~BlockCompressionInputStream() {
    delete _sub_stream;
    delete[] _output_buffer;
}

bool BlockCompressionInputStream::Skip(int /*count*/) {
    SCHECK(false);
    return false;
};

void BlockCompressionInputStream::BackUp(int count) {
    if (count <= 0) return;
    SCHECK(_output_buffer);
    SCHECK_LE(static_cast<size_t>(count), _output_buffer_size - _backed_up_bytes);
    _backed_up_bytes += count;
    _byte_count -= count;
};

void BlockCompressionInputStream::reset_input_stream() {
    delete _sub_stream;
    _sub_stream = new CodedInputStream(_raw_stream);
    _sub_stream->SetTotalBytesLimit(1 << 30, 1 << 30);
}

bool BlockCompressionInputStream::Next(const void** data, int* size) {
    if (_backed_up_bytes) {
        size_t skip = _output_buffer_size - _backed_up_bytes;
        (*data) = _output_buffer + skip;
        (*size) = _backed_up_bytes;
        _backed_up_bytes = 0;
        _byte_count += *size;
        return true;
    }
    
    uint32_t compressed_size = 0;
    if (!_sub_stream->ReadVarint32(&compressed_size) || compressed_size > 2*BLOCKSIZE) {
        return false;
    }

    char * tempbuffer = new char[compressed_size];
    if (!_sub_stream->ReadRaw(tempbuffer, compressed_size)) {
        delete[] tempbuffer;
        return false;
    }

    RawUncompress(tempbuffer, compressed_size);
    delete[] tempbuffer;

    // TODO: probably call this only every Limit/BLOCKSIZE
    if (_sub_stream->BytesUntilLimit() < 1024*1024) reset_input_stream();
    
    (*data) = _output_buffer;
    (*size) = _output_buffer_size;
    _byte_count += *size;
    return true;
}

// =========================================================================

BlockCompressionOutputStream::BlockCompressionOutputStream(
    ZeroCopyOutputStream* sub_stream)
    : _input_buffer(NULL), _sub_stream(new CodedOutputStream(sub_stream)), _backed_up_bytes(0),
      _byte_count(0) {}

BlockCompressionOutputStream::~BlockCompressionOutputStream() {
    if (_input_buffer) {
        // The buffer is not empty, there is stuff yet to be written.
        // This is necessary because we can't call virtual functions from any
        // destructor. Often this results in a pure virtual function call.
        // Call BlockCompressionOutputStream::Flush() before destroying 
        // this object.
        SCHECK(false);
        // Flush();
    }
    delete _sub_stream;
}

void BlockCompressionOutputStream::BackUp(int count) {
    if (count <= 0) return;
    SCHECK(_input_buffer);
    SCHECK_LE(static_cast<size_t>(count), BLOCKSIZE - _backed_up_bytes);
    _backed_up_bytes += count;
    _byte_count -= count;
}

bool BlockCompressionOutputStream::Flush()
{
    size_t size = BLOCKSIZE - _backed_up_bytes;
    if (!_input_buffer || size == 0) return true;
    
    size_t compressed_size = 0;
    char * compressed_data = new char[MaxCompressedLength(size)];
    compressed_size = RawCompress(_input_buffer, size, compressed_data);

    SCHECK_LE(compressed_size, 2*BLOCKSIZE);
    
    uint32_t compressed_size_32 = static_cast<uint32_t>(compressed_size);
    _sub_stream->WriteVarint32(compressed_size_32);
    _sub_stream->WriteRaw(compressed_data, compressed_size_32);
    delete[] compressed_data;

    _backed_up_bytes = 0;
    delete[] _input_buffer;
    _input_buffer = 0;
    return true;
}

bool BlockCompressionOutputStream::Next(void** data, int* size) {
    if (_backed_up_bytes) {
        size_t skip = BLOCKSIZE - _backed_up_bytes;
        (*data) = _input_buffer + skip;
        (*size) = _backed_up_bytes;
        _backed_up_bytes = 0;
        _byte_count += *size;
        return true;
    }
    if(_input_buffer) Flush();
    _input_buffer = new char[BLOCKSIZE];
    (*data) = _input_buffer;
    (*size) = BLOCKSIZE;
    _byte_count += *size;
    return true;
}


#if HAVE_SNAPPY

/// Snappy implementation

void SnappyInputStream::RawUncompress(char* input_buffer, uint32_t compressed_size) {
    size_t uncompressed_size;
    bool success = ::snappy::GetUncompressedLength(
        input_buffer, compressed_size, &uncompressed_size);
    SCHECK(success);
    
    if (uncompressed_size > _output_buffer_capacity) {
        delete[] _output_buffer;
        _output_buffer = new char[uncompressed_size];
        _output_buffer_capacity = uncompressed_size;
    }
    _output_buffer_size = uncompressed_size;
    success = ::snappy::RawUncompress(input_buffer, compressed_size,
        _output_buffer);
    SCHECK(success);    
}


uint32_t SnappyOutputStream::MaxCompressedLength(size_t input_size)
{
    return ::snappy::MaxCompressedLength(input_size);
}

uint32_t SnappyOutputStream::RawCompress(char* input_buffer, size_t input_size,
    char* output_buffer)
{
    size_t compressed_size = 0;
    ::snappy::RawCompress(input_buffer, input_size, output_buffer,
        &compressed_size);
    return compressed_size;
}

#endif  // HAVE_SNAPPY


/// LZ4 implementation

void LZ4InputStream::RawUncompress(char* input_buffer, uint32_t compressed_size) {
    if (!_output_buffer) {
        _output_buffer = new char[BLOCKSIZE];
        _output_buffer_capacity = BLOCKSIZE;
    }
    _output_buffer_size = LZ4_uncompress_unknownOutputSize(input_buffer,
        _output_buffer, compressed_size, BLOCKSIZE);
}

uint32_t LZ4OutputStream::MaxCompressedLength(size_t input_size)
{
    return LZ4_compressBound(input_size);
}

uint32_t LZ4OutputStream::RawCompress(char* input_buffer, size_t input_size,
    char* output_buffer)
{
    return LZ4_compress(input_buffer, output_buffer, input_size);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
