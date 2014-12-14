// This file is modified from `protobuf-zerocopy-compression':
//   https://github.com/JohannesEbke/protobuf-zerocopy-compression

// Copyright (c) 2013, Johannes Ebke and Peter Waller.  All rights reserved.
// Author: peter.waller@gmail.com (Peter Waller)
// Author: johannes@ebke.org (Johannes Ebke)

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/compressed_stream.h>
#include <sofa/pbrpc/gzip_stream.h>
#include <sofa/pbrpc/block_wrappers.h>

namespace sofa {
namespace pbrpc {

AbstractCompressedInputStream * get_compressed_input_stream(
        ZeroCopyInputStream * istream, CompressType type) {
    switch(type) {
        case CompressTypeGzip:
            return new GzipInputStream(istream, GzipInputStream::GZIP);
        case CompressTypeZlib:
            return new GzipInputStream(istream, GzipInputStream::ZLIB);
        case CompressTypeSnappy:
#ifdef HAVE_SNAPPY
            return new SnappyInputStream(istream);
#else 
            SCHECK(false);
#endif
        case CompressTypeLZ4:
            return new LZ4InputStream(istream);
        default:
            SCHECK(false);
    }
    return NULL;
}

AbstractCompressedOutputStream * get_compressed_output_stream(
        ZeroCopyOutputStream * ostream, CompressType type, int level) {
    switch (type) {
        case CompressTypeGzip:
            {
                GzipOutputStream::Options o;
                o.format = GzipOutputStream::GZIP;
                o.compression_level = level;
                return new GzipOutputStream(ostream, o);
            }
        case CompressTypeZlib:
            {
                GzipOutputStream::Options o;
                o.format = GzipOutputStream::ZLIB;
                o.compression_level = level;
                return new GzipOutputStream(ostream, o);
            }
        case CompressTypeSnappy:
#ifdef HAVE_SNAPPY
            return new SnappyOutputStream(ostream);
#else
            SCHECK(false);
#endif
        case CompressTypeLZ4:
            return new LZ4OutputStream(ostream);
        default:
            SCHECK(false);
    }
    return NULL;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
