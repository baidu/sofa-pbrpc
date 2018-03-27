// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <algorithm>

#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/tran_buf_pool.h>

namespace sofa {
namespace pbrpc {

ReadBuffer::ReadBuffer()
    : _total_block_size(0)
    , _total_bytes(0)
    , _cur_it(_buf_list.begin())
    , _cur_pos(0)
    , _last_bytes(0)
    , _read_bytes(0)
{}

ReadBuffer::~ReadBuffer()
{
    for (BufHandleListIterator it = _buf_list.begin();
            it != _buf_list.end(); ++it)
    {
        TranBufPool::free(it->data);
    }
    _buf_list.clear();
}

void ReadBuffer::Append(const BufHandle& buf_handle)
{
    SCHECK_GT(buf_handle.size, 0);
    _buf_list.push_back(buf_handle);
    TranBufPool::add_ref(buf_handle.data);
    _total_block_size += TranBufPool::block_size(buf_handle.data);
    _total_bytes += buf_handle.size;
    _cur_it = _buf_list.begin();
}

void ReadBuffer::Append(const ReadBuffer* read_buffer)
{
    SCHECK(read_buffer != NULL);
    BufHandleList::const_iterator it = read_buffer->_buf_list.begin();
    BufHandleList::const_iterator end = read_buffer->_buf_list.end();
    for (; it != end; ++it) 
    {
        _buf_list.push_back(*it);
        TranBufPool::add_ref(it->data);
    }
    _total_block_size += read_buffer->_total_block_size;
    _total_bytes += read_buffer->_total_bytes;
    _cur_it = _buf_list.begin();
}

int64 ReadBuffer::TotalCount() const
{
    return _total_bytes;
}

int ReadBuffer::BlockCount() const
{
    return _buf_list.size();
}

int64 ReadBuffer::TotalBlockSize() const
{
    return _total_block_size;
}

std::string ReadBuffer::ToString() const
{
    std::string str;
    str.reserve(_total_bytes);
    for (BufHandleList::const_iterator it = _buf_list.begin();
            it != _buf_list.end(); ++it)
    {
        str.append(it->data + it->offset, it->size);
    }
    return str;
}

bool ReadBuffer::Next(const void** data, int* size)
{
    if (_cur_it != _buf_list.end())
    {
        SCHECK_LT(_cur_pos, _cur_it->size);
        *data = _cur_it->data + _cur_it->offset + _cur_pos;
        *size = _cur_it->size - _cur_pos;
        ++_cur_it;
        _cur_pos = 0;
        _last_bytes = *size;
        _read_bytes += _last_bytes;
        return true;
    }
    else
    {
        _last_bytes = 0;
        return false;
    }
}

// BackUp() can only be called after a successful Next().
// "count" should be greater than or equal to 0.
void ReadBuffer::BackUp(int count)
{
    SCHECK_GT(_last_bytes, 0);
    SCHECK_GE(count, 0);
    SCHECK_LE(count, _last_bytes);
    if (count > 0)
    {
        --_cur_it;
        _cur_pos = _cur_it->size - count;
    }
    _last_bytes = 0;
    _read_bytes -= count;
}

// "count" should be greater than or equal to 0.
bool ReadBuffer::Skip(int count)
{
    SCHECK_GE(count, 0);
    const void* data;
    int size;
    while (count > 0 && Next(&data, &size))
    {
        if (size > count)
        {
            BackUp(size - count);
            size = count;
        }
        count -= size;
    }
    _last_bytes = 0;
    return count == 0;
}

int64 ReadBuffer::ByteCount() const
{
    return _read_bytes;
}

WriteBuffer::WriteBuffer()
    : _total_block_size(0)
    , _total_capacity(0)
    , _last_bytes(0)
    , _write_bytes(0)
    , _base_block_factor(4)
{}

WriteBuffer::~WriteBuffer()
{
    for (BufHandleListIterator it = _buf_list.begin();
            it != _buf_list.end(); ++it)
    {
        TranBufPool::free(it->data);
    }
    _buf_list.clear();
}

int64 WriteBuffer::TotalCapacity() const
{
    return _total_capacity;
}

int WriteBuffer::BlockCount() const
{
    return _buf_list.size();
}

int64 WriteBuffer::TotalBlockSize() const
{
    return _total_block_size;
}

void WriteBuffer::SwapOut(ReadBuffer* is)
{
    while (!_buf_list.empty())
    {
        BufHandle& buf_handle = _buf_list.front();
        if (buf_handle.size > 0)
        {
            buf_handle.offset = 0; // capacity -> offset
            is->Append(buf_handle);
        }
        TranBufPool::free(buf_handle.data);
        _buf_list.pop_front();
    }
    _total_block_size = 0;
    _total_capacity = 0;
    _last_bytes = 0;
    _write_bytes = 0;
}

int64 WriteBuffer::Reserve(int count)
{
    SCHECK_GT(count, 0);
    int64 head = ByteCount();
    void* data;
    int size;
    while (count > 0)
    {
        if (!Next(&data, &size))
            return -1;
        if (size > count)
        {
            BackUp(size - count);
            count = 0;
        }
        else
        {
            count -= size;
        }
    }
    return head;
}

void WriteBuffer::SetData(int64 pos, const char* data, int size)
{
    SCHECK_GE(pos, 0);
    SCHECK_GT(size, 0);
    SCHECK_LE(pos + size, ByteCount());
    BufHandleListIterator cur_it = _buf_list.begin();
    int cur_offset = 0;
    while (pos > 0)
    {
        SCHECK(cur_it != _buf_list.end());
        int cur_size = cur_it->size - cur_offset;
        if (cur_size > pos)
        {
            cur_offset += pos;
            pos = 0;
        }
        else
        {
            pos -= cur_size;
            ++cur_it;
            cur_offset = 0;
        }
    }
    while (size > 0)
    {
        SCHECK(cur_it != _buf_list.end());
        int cur_size = cur_it->size - cur_offset;
        if (cur_size > size)
        {
            memcpy(cur_it->data + cur_offset, data, size);
            size = 0;
        }
        else
        {
            memcpy(cur_it->data + cur_offset, data, cur_size);
            size -= cur_size;
            data += cur_size;
            ++cur_it;
            cur_offset = 0;
        }
    }
}

bool WriteBuffer::Next(void** data, int* size)
{
    BufHandleListReverseIterator last = _buf_list.rbegin();
    if (last == _buf_list.rend() || last->size == last->capacity)
    {
        if (!Extend())
        {
            _last_bytes = 0;
            return false;
        }
        last = _buf_list.rbegin();
    }
    *data = last->data + last->size;
    *size = last->capacity - last->size;
    last->size = last->capacity;
    _last_bytes = *size;
    _write_bytes += _last_bytes;
    return true;
}

// BackUp() can only be called after a successful Next().
// "count" should be greater than or equal to 0.
void WriteBuffer::BackUp(int count)
{
    SCHECK_GT(_last_bytes, 0);
    SCHECK_GE(count, 0);
    SCHECK_LE(count, _last_bytes);
    _buf_list.back().size -= count;
    _last_bytes = 0;
    _write_bytes -= count;
}

int64 WriteBuffer::ByteCount() const
{
    return _write_bytes;
}

bool WriteBuffer::Extend()
{
    // incrementally extend block
    unsigned int current_factor = _buf_list.size() + _base_block_factor;
    char* block = static_cast<char*>(TranBufPool::malloc(std::min(
                    SOFA_PBRPC_TRAN_BUF_BLOCK_MAX_FACTOR,
                    current_factor)));
    if (block == NULL) return false;
    _buf_list.push_back(BufHandle(block, TranBufPool::capacity(block)));
    _total_block_size += TranBufPool::block_size(block);
    _total_capacity += TranBufPool::capacity(block);
    return true;
}

bool WriteBuffer::Append(const std::string& data)
{
    return Append(data.c_str(), data.size());
}

bool WriteBuffer::Append(const char* data, int size)
{
    SCHECK_GE(size, 0);
    if (size == 0) return true;
    int64 head = Reserve(size);
    if (head < 0)
    {
        return false;
    }
    SetData(head, data, size);
    return true;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
