// This file is modified from `protobuf-zerocopy-compression':
//   https://github.com/JohannesEbke/protobuf-zerocopy-compression

// Copyright (c) 2013, Johannes Ebke and Peter Waller. All rights reserved.
// Author: peter.waller@gmail.com (Peter Waller)
// Author: johannes@ebke.org (Johannes Ebke)

#ifndef _SOFA_PBRPC_BLOCK_WRAPPER_H_
#define _SOFA_PBRPC_BLOCK_WRAPPER_H_

#include <google/protobuf/io/zero_copy_stream.h>
using google::protobuf::io::ZeroCopyInputStream;
using google::protobuf::io::ZeroCopyOutputStream;

#include <google/protobuf/io/coded_stream.h>
using google::protobuf::io::CodedInputStream;
using google::protobuf::io::CodedOutputStream;

#include <sofa/pbrpc/compressed_stream.h>

namespace sofa {
namespace pbrpc {

// This class provides scaffolding for implementing compression streams based
// on compression algorithms that do not support streaming operations but must
// be operated blockwise.
class BlockCompressionInputStream : public AbstractCompressedInputStream {
 public:
  explicit BlockCompressionInputStream(ZeroCopyInputStream* sub_stream);
  virtual ~BlockCompressionInputStream();
  
  bool Next(const void** data, int* size);
  void BackUp(int count);
  bool Skip(int count);
  int64_t ByteCount() const { return _byte_count; }
  bool ExpectAtEnd() { return true; }

 protected:
  virtual void RawUncompress(char* input_buffer, uint32_t compressed_size) = 0;
  
  char* _output_buffer;
  size_t _output_buffer_size;
  size_t _output_buffer_capacity;
  
 private:

  CodedInputStream* _sub_stream;
  ZeroCopyInputStream* _raw_stream;

  int _backed_up_bytes;
  size_t _byte_count;

  void reset_input_stream();
  
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(BlockCompressionInputStream);
};

class BlockCompressionOutputStream : public AbstractCompressedOutputStream {
 public:
  // Create a BlockCompressionOutputStream with default options.
  explicit BlockCompressionOutputStream(ZeroCopyOutputStream* sub_stream);
  virtual ~BlockCompressionOutputStream();
  
  bool Next(void** data, int* size);
  void BackUp(int count);
  int64_t ByteCount() const { return _byte_count; };

  bool Flush();
  bool Close() { return Flush(); }

 protected:
  virtual uint32_t MaxCompressedLength(size_t input_size) = 0;
  virtual uint32_t RawCompress(char* input_buffer, size_t input_size, char* output_buffer) = 0;
  char* _input_buffer;
   
 private:
  CodedOutputStream* _sub_stream;
  
  int _backed_up_bytes;
  size_t _byte_count;

  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(BlockCompressionOutputStream);
};

#ifdef HAVE_SNAPPY

class SnappyInputStream : public BlockCompressionInputStream {
 public:
  explicit SnappyInputStream(ZeroCopyInputStream* sub_stream) : BlockCompressionInputStream(sub_stream) {};

  virtual void RawUncompress(char* input_buffer, uint32_t compressed_size);
  
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(SnappyInputStream);
};

class SnappyOutputStream : public BlockCompressionOutputStream {
 public:
  explicit SnappyOutputStream(ZeroCopyOutputStream* sub_stream) : BlockCompressionOutputStream(sub_stream) {};
  
  virtual uint32_t MaxCompressedLength(size_t input_size);
  virtual uint32_t RawCompress(char* input_buffer, size_t input_size, char* output_buffer);
  
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(SnappyOutputStream);
};

#endif

class LZ4InputStream : public BlockCompressionInputStream {
 public:

  explicit LZ4InputStream(ZeroCopyInputStream* sub_stream) : BlockCompressionInputStream(sub_stream) {};
  
  virtual void RawUncompress(char* input_buffer, uint32_t compressed_size);
  
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(LZ4InputStream);
};

class LZ4OutputStream : public BlockCompressionOutputStream {
 public:
  explicit LZ4OutputStream(ZeroCopyOutputStream* sub_stream) : BlockCompressionOutputStream(sub_stream) {};
  
  virtual uint32_t MaxCompressedLength(size_t input_size);
  virtual uint32_t RawCompress(char* input_buffer, size_t input_size, char* output_buffer);
  
  GOOGLE_DISALLOW_EVIL_CONSTRUCTORS(LZ4OutputStream);
};

} // namespace pbrpc
} // namepsace sofa

#endif // _SOFA_PBRPC_BLOCK_WRAPPER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
