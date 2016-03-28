// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <sofa/pbrpc/string_utils.h>
#include "serialize.h"

namespace sofa {
namespace pbrpc {

// NOTICE: When compiling by gcc with '-O2' option, the data may be out of order.
// It seems a bug of gcc, fuck it.  So we use 'sofa_io_barrier()' to prevent it. 
#define sofa_io_barrier() __asm__ __volatile__("": : :"memory")

// fast copy small data block
static inline void fast_memcpy(void* dest, const void* src, size_t n)
{
    switch (n)
    {
    case 0:
        return;
    case 1:
        *(uint8_t*)dest = *(const uint8_t*)src;
        return;
    case 2:
        *(uint16_t*)dest = *(const uint16_t*)src;
        return;
    case 3:
        *(byte_helper_t<3>*)dest = *(const byte_helper_t<3>*)src;
        return;
    case 4:
        *(uint32_t*)dest = *(const uint32_t*)src;
        return;
    case 5:
        *(byte_helper_t<5>*)dest = *(const byte_helper_t<5>*)src;
        return;
    case 6:
        *(byte_helper_t<6>*)dest = *(const byte_helper_t<6>*)src;
        return;
    case 7:
        *(byte_helper_t<7>*)dest = *(const byte_helper_t<7>*)src;
        return;
    case 8:
        *(uint64_t*)dest = *(const uint64_t*)src;
        return;
    case 9:
        *(byte_helper_t<9>*)dest = *(const byte_helper_t<9>*)src;
        return;
    case 10:
        *(byte_helper_t<10>*)dest = *(const byte_helper_t<10>*)src;
        return;
    case 11:
        *(byte_helper_t<11>*)dest = *(const byte_helper_t<11>*)src;
        return;
    case 12:
        *(byte_helper_t<12>*)dest = *(const byte_helper_t<12>*)src;
        return;
    case 13:
        *(byte_helper_t<13>*)dest = *(const byte_helper_t<13>*)src;
        return;
    case 14:
        *(byte_helper_t<14>*)dest = *(const byte_helper_t<14>*)src;
        return;
    case 15:
        *(byte_helper_t<15>*)dest = *(const byte_helper_t<15>*)src;
        return;
    case 16:
        *(byte_helper_t<16>*)dest = *(const byte_helper_t<16>*)src;
        return;
    default:
        memcpy(dest, src, n);
    }
}

bool Serializer::close()
{
        bool ret = true;
        if (_buf_size > 0)
        {
            _stream->BackUp(_buf_size);
        }
        _buf = NULL;
        _buf_size = 0;
        return ret;
}

bool Serializer::serialize_buffer(const void* data, int data_len)
{
    while (_buf_size < data_len)
    {
        fast_memcpy(_buf, data, _buf_size);
        data = ((const char*)data) + _buf_size;
        data_len -= _buf_size;
        if (!_stream->Next(&_buf, &_buf_size))
        {
            _buf_size = 0;
            return false;
        }
    }
    fast_memcpy(_buf, data, data_len);
    _buf = ((char*)_buf) + data_len;
    _buf_size -= data_len;
    return true;
}

bool Serializer::serialize_string(const std::string& d)
{
    if (d.size() > SOFA_STRING_MAX_SERIALIZE_SIZE)
    {
        SLOG(ERROR, "too big string size: %u", d.size());
        return false;
    }
    if (!serialize_varint(d.size()))
    {
        return false;
    }
    return serialize_buffer(d.c_str(), d.size());
}

// NOTICE: When compiling by gcc with '-O2' option, the data may be out of order.
// It occurs between modify 'd' and 'serialize_X_byte', so we use 'sofa_io_barrier()' to prevent it. 
// Fuck gcc again!
bool Serializer::serialize_varint(uint64_t d)
{
    if (d < (1UL << 7))
    {
        d <<= 1;
        sofa_io_barrier();
        return serialize_1_byte(&d);
    }
    if (d < (1UL << 14))
    {
        d <<= 2;
        d |= 0x1;
        sofa_io_barrier();
        return serialize_2_byte(&d);
    }
    if (d < (1UL << 21))
    {
        d <<= 3;
        d |= 0x3;
        sofa_io_barrier();
        return serialize_3_byte(&d);
    }
    if (d < (1UL << 28))
    {
        d <<= 4;
        d |= 0x7;
        sofa_io_barrier();
        return serialize_4_byte(&d);
    }
    if (d < (1UL << 35))
    {
        d <<= 5;
        d |= 0xF;
        sofa_io_barrier();
        return serialize_5_byte(&d);
    }
    if (d < (1UL << 42))
    {
        d <<= 6;
        d |= 0x1F;
        sofa_io_barrier();
        return serialize_6_byte(&d);
    }
    if (d < (1UL << 49))
    {
        d <<= 7;
        d |= 0x3F;
        sofa_io_barrier();
        return serialize_7_byte(&d);
    }
    if (d < (1UL << 56))
    {
        d <<= 8;
        d |= 0x7F;
        sofa_io_barrier();
        return serialize_8_byte(&d);
    }
    return serialize_raw_data((uint8_t)0xFF) && serialize_raw_data(d);
}

size_t Serializer::varint_len(uint64_t d)
{
    if (d < (1UL << 7))
    {
        return 1;
    }
    if (d < (1UL << 14))
    {
        return 2;
    }
    if (d < (1UL << 21))
    {
        return 3;
    }
    if (d < (1UL << 28))
    {
        return 4;
    }
    if (d < (1UL << 35))
    {
        return 5;
    }
    if (d < (1UL << 42))
    {
        return 6;
    }
    if (d < (1UL << 49))
    {
        return 7;
    }
    if (d < (1UL << 56))
    {
        return 8;
    }
    return 9;
}

void Deserializer::close()
{
    if (_buf_size > 0)
    {
        _stream->BackUp(_buf_size);
    }
    _buf = NULL;
    _buf_size = 0;
}

bool Deserializer::deserialize_buffer(void* data, size_t data_len)
{
    while (static_cast<size_t>(_buf_size) < data_len)
    {
        fast_memcpy(data, _buf, _buf_size);
        data = ((char*)data) + _buf_size;
        data_len -= _buf_size;
        if (!_stream->Next(&_buf, &_buf_size))
        {
            _buf_size = 0;
            return false;
        }
    }
    fast_memcpy(data, _buf, data_len);
    _buf = ((const char*)_buf) + data_len;
    _buf_size -= data_len;
    return true;
}

bool Deserializer::deserialize_string(std::string& d)
{
    size_t str_len = 0;
    if (!deserialize_varint(str_len))
    {
        return false;
    }
    if (str_len == 0)
    {
        d.clear();
        return true;
    }
    if (str_len > SOFA_STRING_MAX_SERIALIZE_SIZE)
    {
        SLOG(ERROR, "too big string size: %u", str_len);
        return false;
    }
    StringUtils::resize_uninitialized(&d, str_len);
    return deserialize_buffer(StringUtils::string_as_array(&d), str_len);
}

bool Deserializer::deserialize_varint(uint64_t& d)
{
    d = 0;
    if (!deserialize_raw_data(*(uint8_t*)&d))
    {
        return false;
    }
    if ((d & 0x1) == 0)
    {
        d >>= 1;
        return true;
    }
    if ((d & 0x2) == 0)
    {
        if (!deserialize_1_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 2;
        return true;
    }
    if ((d & 0x4) == 0)
    {
        if (!deserialize_2_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 3;
        return true;
    }
    if ((d & 0x8) == 0)
    {
        if (!deserialize_3_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 4;
        return true;
    }
    if ((d & 0x10) == 0)
    {
        if (!deserialize_4_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 5;
        return true;
    }
    if ((d & 0x20) == 0)
    {
        if (!deserialize_5_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 6;
        return true;
    }
    if ((d & 0x40) == 0)
    {
        if (!deserialize_6_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 7;
        return true;
    }
    if ((d & 0x80) == 0)
    {
        if (!deserialize_7_byte((uint8_t*)&d + 1))
        {
            return false;
        }
        sofa_io_barrier();
        d >>= 8;
        return true;
    }
    return deserialize_raw_data(d);
}

} // namespace io
} // namespace sofa

void sofa_touch_sofa_io_serializer()
{
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
