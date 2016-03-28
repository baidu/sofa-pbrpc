// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_RPC_COOKIE_H_
#define _SOFA_PBRPC_RPC_COOKIE_H_

#include <stdint.h>
#include <string.h>
#include <sofa/pbrpc/buffer.h>

// String max serialize size: 64MB.
#define SOFA_STRING_MAX_SERIALIZE_SIZE 67108864u

// Container max serialize size: 64MB.
#define SOFA_CONTAINER_MAX_SERIALIZE_SIZE 67108864u

namespace sofa {
namespace pbrpc {

// helper for fast copy small data block
template<int N>
struct byte_helper_t
{
    uint8_t data[N];
};

// Serializer on output stream
class Serializer 
{
public:
    explicit Serializer(WriteBufferPtr& stream) : _stream(stream)
    {
        _buf = NULL;
        _buf_size = 0;
    }
    void set_stream(WriteBufferPtr& stream)
    {
        close();
        _stream = stream;
    }
    ~Serializer()
    { }
    // close serializer, backup un-used buffer
    bool close();

    template <typename T>
    bool serialize_raw_data(const T& d)
    {
        if (static_cast<size_t>(_buf_size) >= sizeof(T))
        {
            *(T*)_buf = d;
            _buf = ((char*)_buf) + sizeof(T);
            _buf_size -= sizeof(T);
            return true;
        }
        return serialize_buffer(&d, sizeof(T));
    }
    template <typename T>
    static size_t raw_data_len(const T&)
    {
        return sizeof(T);
    }
    template <typename T>
    static size_t max_raw_data_len(const T&)
    {
        return sizeof(T);
    }
    bool serialize_1_byte(const void __attribute__((may_alias)) * d)
    {
        return serialize_raw_data(*(const uint8_t __attribute__((may_alias))*)d);
    }
    bool serialize_2_byte(const void* d)
    {
        return serialize_raw_data(*(const uint16_t*)d);
    }
    bool serialize_3_byte(const void* d)
    {
        return serialize_raw_data(*(const byte_helper_t<3>*)d);
    }
    bool serialize_4_byte(const void* d)
    {
        return serialize_raw_data(*(const uint32_t*)d);
    }
    bool serialize_5_byte(const void* d)
    {
        return serialize_raw_data(*(const byte_helper_t<5>*)d);
    }
    bool serialize_6_byte(const void* d)
    {
        return serialize_raw_data(*(const byte_helper_t<6>*)d);
    }
    bool serialize_7_byte(const void* d)
    {
        return serialize_raw_data(*(const byte_helper_t<7>*)d);
    }
    bool serialize_8_byte(const void* d)
    {
        return serialize_raw_data(*(const uint64_t*)d);
    }
    bool serialize_buffer(const void* data, int data_len);
    bool serialize_string(const std::string& d);
    static size_t string_len(const std::string& d)
    {
        return varint_len(d.size()) + d.size();
    }
    bool serialize_varint(uint8_t d)
    {
        return serialize_varint((uint64_t)d);
    }
    static size_t varint_len(uint8_t d)
    {
        return varint_len((uint64_t)d);
    }
    static size_t max_varint_len(uint8_t)
    {
        return max_varint_len((uint64_t)0);
    }
    bool serialize_varint(uint16_t d)
    {
        return serialize_varint((uint64_t)d);
    }
    static size_t varint_len(uint16_t d)
    {
        return varint_len((uint64_t)d);
    }
    static size_t max_varint_len(uint16_t)
    {
        return max_varint_len((uint64_t)0);
    }
    bool serialize_varint(uint32_t d)
    {
        return serialize_varint((uint64_t)d);
    }
    static size_t varint_len(uint32_t d)
    {
        return varint_len((uint64_t)d);
    }
    static size_t max_varint_len(uint32_t)
    {
        return max_varint_len((uint64_t)0);
    }
    bool serialize_varint(int8_t d)
    {
        return serialize_varint((int64_t)d);
    }
    static size_t varint_len(int8_t d)
    {
        return varint_len((int64_t)d);
    }
    static size_t max_varint_len(int8_t)
    {
        return max_varint_len((int64_t)0);
    }
    bool serialize_varint(int16_t d)
    {
        return serialize_varint((int64_t)d);
    }
    static size_t varint_len(int16_t d)
    {
        return varint_len((int64_t)d);
    }
    static size_t max_varint_len(int16_t)
    {
        return max_varint_len((int64_t)0);
    }
    bool serialize_varint(int32_t d)
    {
        return serialize_varint((int64_t)d);
    }
    static size_t varint_len(int32_t d)
    {
        return varint_len((int64_t)d);
    }
    static size_t max_varint_len(int32_t)
    {
        return max_varint_len((int64_t)0);
    }
    bool serialize_varint(int64_t d)
    {
        return serialize_varint((uint64_t)((d << 1) ^ (d >> 63)));
    }
    static size_t varint_len(int64_t d)
    {
        return varint_len((uint64_t)((d << 1) ^ (d >> 63)));
    }
    static size_t max_varint_len(int64_t)
    {
        return max_varint_len((uint64_t)0);
    }
    bool serialize_varint(uint64_t d);
    static size_t varint_len(uint64_t d);
    static size_t max_varint_len(uint64_t)
    {
        return 9;
    }
private:
    void* _buf;              // current buffer to write
    int _buf_size;           // size of current buffer to write
    WriteBufferPtr _stream;  // output stream
};

// Deserializer on input stream
class Deserializer
{
public:
    explicit Deserializer(ReadBufferPtr& stream) : _stream(stream)
    {
        _buf = NULL;
        _buf_size = 0;
    }
    ~Deserializer()
    { }
    // close deserializer, backup un-used buffer
    void close();

    template <typename T>
    bool deserialize_raw_data(T& d)
    {
        if (static_cast<size_t>(_buf_size) >= sizeof(T))
        {
            d = *(const T*)_buf;
            _buf = ((const char*)_buf) + sizeof(T);
            _buf_size -= sizeof(T);
            return true;
        }
        return deserialize_buffer(&d, sizeof(T));
    }
    bool deserialize_1_byte(void* d)
    {
        return deserialize_raw_data(*(uint8_t*)d);
    }
    bool deserialize_2_byte(void* d)
    {
        return deserialize_raw_data(*(uint16_t*)d);
    }
    bool deserialize_3_byte(void* d)
    {
        return deserialize_raw_data(*(byte_helper_t<3>*)d);
    }
    bool deserialize_4_byte(void* d)
    {
        return deserialize_raw_data(*(uint32_t*)d);
    }
    bool deserialize_5_byte(void* d)
    {
        return deserialize_raw_data(*(byte_helper_t<5>*)d);
    }
    bool deserialize_6_byte(void* d)
    {
        return deserialize_raw_data(*(byte_helper_t<6>*)d);
    }
    bool deserialize_7_byte(void* d)
    {
        return deserialize_raw_data(*(byte_helper_t<7>*)d);
    }
    bool deserialize_8_byte(void* d)
    {
        return deserialize_raw_data(*(uint64_t*)d);
    }
    bool deserialize_buffer(void* data, size_t data_len);
    bool deserialize_string(std::string& d);
    bool deserialize_varint(uint8_t& d)
    {
        uint64_t tmp;
        if (!deserialize_varint(tmp))
        {
            return false;
        }
        d = tmp;
        return true;
    }
    bool deserialize_varint(uint16_t& d)
    {
        uint64_t tmp;
        if (!deserialize_varint(tmp))
        {
            return false;
        }
        d = tmp;
        return true;
    }
    bool deserialize_varint(uint32_t& d)
    {
        uint64_t tmp;
        if (!deserialize_varint(tmp))
        {
            return false;
        }
        d = tmp;
        return true;
    }
    bool deserialize_varint(int8_t& d)
    {
        int64_t tmp;
        if (!deserialize_varint(tmp))
        {
            return false;
        }
        d = tmp;
        return true;
    }
    bool deserialize_varint(int16_t& d)
    {
        int64_t tmp;
        if (!deserialize_varint(tmp))
        {
            return false;
        }
        d = tmp;
        return true;
    }
    bool deserialize_varint(int32_t& d)
    {
        int64_t tmp;
        if (!deserialize_varint(tmp))
        {
            return false;
        }
        d = tmp;
        return true;
    }
    bool deserialize_varint(int64_t& d)
    {
        if (!deserialize_varint((uint64_t&)d))
        {
            return false;
        }
        d = (((d & 1) << 63 >> 63) ^ (((uint64_t)d) >> 1));
        return true;
    }
    bool deserialize_varint(uint64_t& d);
private:
    const void* _buf;         // current buffer to read
    int _buf_size;            // size of current buffer to read
    ReadBufferPtr _stream;    // input stream
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_RPC_COOKIE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
