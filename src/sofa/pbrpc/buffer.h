// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_BUFFER_H_
#define _SOFA_PBRPC_BUFFER_H_

#include <deque>
#include <string>

#include <google/protobuf/io/zero_copy_stream.h>

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/buf_handle.h>

namespace sofa {
namespace pbrpc {

// Defined in this file.
class ReadBuffer;
typedef sofa::pbrpc::shared_ptr<ReadBuffer> ReadBufferPtr;
class WriteBuffer;
typedef sofa::pbrpc::shared_ptr<WriteBuffer> WriteBufferPtr;

typedef std::deque<BufHandle> BufHandleList;
typedef std::deque<BufHandle>::iterator BufHandleListIterator;
typedef std::deque<BufHandle>::reverse_iterator BufHandleListReverseIterator;

class ReadBuffer : public google::protobuf::io::ZeroCopyInputStream
{
public:
    ReadBuffer();
    virtual ~ReadBuffer();

    // Append a buf handle to the buffer.
    //
    // Preconditions:
    // * No method Next(), Backup() or Skip() have been called before.
    // * The size of "buf_handle" should be greater than 0.
    // * For the first one, size of "buf_handle" should be greater than 0.
    // * For the second one, "read_buffer" should not be NULL.
    void Append(const BufHandle& buf_handle);
    void Append(const ReadBuffer* read_buffer);

    // Get the total byte count of the buffer.
    int64 TotalCount() const;

    // Get the block count occupied by the buffer.
    int BlockCount() const;

    // Get the total block size occupied by the buffer.
    int64 TotalBlockSize() const;

    // Trans buffer to string.
    std::string ToString() const;

    // implements ZeroCopyInputStream ----------------------------------
    bool Next(const void** data, int* size);
    void BackUp(int count);
    bool Skip(int count);
    int64 ByteCount() const;

private:
    BufHandleList _buf_list;
    int64 _total_block_size; // total block size in the buffer
    int64 _total_bytes; // total bytes in the buffer
    BufHandleListIterator _cur_it;
    int _cur_pos;
    int _last_bytes; // last read bytes
    int64 _read_bytes; // total read bytes

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(ReadBuffer);
}; // class ReadBuffer

class WriteBuffer : public google::protobuf::io::ZeroCopyOutputStream
{
public:
    WriteBuffer();
    virtual ~WriteBuffer();

    // Get the total capacity of the buffer.
    int64 TotalCapacity() const;

    // Get the block count occupied by the buffer.
    int BlockCount() const;

    // Get the total block size occupied by the buffer.
    int64 TotalBlockSize() const;

    // Swap out data from this output stream and append to the input stream "is".
    // The "is" should not be null.
    //
    // Postconditions:
    // * This buffer is cleared afterward, just as a new output buffer.
    void SwapOut(ReadBuffer* is);

    // Reserve some space in the buffer.
    // If succeed, return the head position of reserved space.
    // If failed, return -1.
    //
    // Preconditions:
    // * "count" > 0
    int64 Reserve(int count);

    // Set data in the buffer, start from "pos".
    //
    // Preconditions:
    // * "pos" >= 0
    // * "data" != NULL && "size" > 0
    // * "pos" + "size" <= ByteCount()
    void SetData(int64 pos, const char* data, int size);

    // implements ZeroCopyOutputStream ---------------------------------
    bool Next(void** data, int* size);
    void BackUp(int count);
    int64 ByteCount() const;

    // Append string to the buffer
    // If succeed, return true
    // If failed, return false
    bool Append(const std::string& data);
    bool Append(const char* data, int size);

    void set_base_block_factor(size_t factor)
    {
        _base_block_factor = factor;
    }

    size_t base_block_factor()
    {
        return _base_block_factor;
    }

private:
    // Add a new block to the end of the buffer.
    bool Extend();

private:
    BufHandleList _buf_list;
    int64 _total_block_size; // total block size in the buffer
    int64 _total_capacity; // total capacity in the buffer
    int _last_bytes; // last write bytes
    int64 _write_bytes; // total write bytes
    size_t _base_block_factor; //base block malloc factor

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(WriteBuffer);
}; // class WriteBuffer

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BUFFER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
