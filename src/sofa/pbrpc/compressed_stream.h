// This file is modified from `protobuf-zerocopy-compression':
//   https://github.com/JohannesEbke/protobuf-zerocopy-compression

// Copyright (c) 2013, Johannes Ebke and Peter Waller. All rights reserved.
// Author: peter.waller@gmail.com (Peter Waller)
// Author: johannes@ebke.org (Johannes Ebke)

#ifndef _SOFA_PBRPC_COMPRESSION_COMPRESSED_STREAM_H_
#define _SOFA_PBRPC_COMPRESSION_COMPRESSED_STREAM_H_

#include <google/protobuf/io/zero_copy_stream.h>

#include <sofa/pbrpc/rpc_option.pb.h>

#define HAVE_SNAPPY 1

namespace sofa {
namespace pbrpc {

/// Base class for all compressed output streams with additional methods
class AbstractCompressedOutputStream : public google::protobuf::io::ZeroCopyOutputStream {
public:
    /// Make sure that all data is compressed and written
    virtual bool Flush() = 0;
    virtual bool Close() = 0;
};

/// Base class for all compressed input streams with an ExpectAtEnd method.
class AbstractCompressedInputStream : public google::protobuf::io::ZeroCopyInputStream {
public:
    /// ExpectAtEnd returns true if there is no more compressed data to process
    virtual bool ExpectAtEnd() = 0;
};

/////////////////////////////////////////////////
// Compress type defined in `rpc_option.proto':
//   enum CompressType {
//       CompressTypeNone = 0;
//       CompressTypeGzip = 1;
//       CompressTypeZlib = 2;
//       CompressTypeSnappy = 3;
//       CompressTypeLZ4 = 4;
//   }
/////////////////////////////////////////////////

/// Get a pointer to a compressed output stream given an underlying ZeroCopyOutputStream.
/// Specify any of the above compression types, and a compression level (for use in ZLIB).
AbstractCompressedOutputStream * get_compressed_output_stream(
        google::protobuf::io::ZeroCopyOutputStream * ostream, CompressType type, int level=1);

/// Get a pointer to a compressed input stream given an underlying ZeroCopyInputStream.
/// Specify any of the above compression types.
AbstractCompressedInputStream * get_compressed_input_stream(
        google::protobuf::io::ZeroCopyInputStream * istream, CompressType type);

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_COMPRESSION_COMPRESSED_STREAM_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
