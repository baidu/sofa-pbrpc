// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdlib>
#include <cstring>
#include <vector>
#include <iostream>
#include <gtest/gtest.h>

#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/buffer.cc> // ATTENTION: include source file for different block size
#include <sofa/pbrpc/tran_buf_pool.h>
#include <sofa/pbrpc/rpc_message_header.h>
#include <sofa/pbrpc/rpc_meta.pb.h>
#include "test_data.pb.h"

using namespace sofa::pbrpc;

static char* rand_str(char* str, size_t len)
{
    for (size_t i = 0; i < len; ++i)
        str[i] = 'A'+ std::rand() % 26;
    return str;
}

class ReadBufferTest : public ::testing::Test
{
public:
    ReadBufferTest() {}
    virtual ~ReadBufferTest() {}
    virtual void SetUp() {
        _block = static_cast<char*>(TranBufPool::malloc(4));
        _capacity = TranBufPool::capacity(_block);
        ASSERT_TRUE(_block != NULL);
        rand_str(_block, _capacity);
    }
    virtual void TearDown() {
        TranBufPool::free(_block);
    }

    int _capacity;
    char* _block;
};

TEST_F(ReadBufferTest, Next)
{
    const void* data;
    int size;

    // no block
    {
        ReadBufferPtr is(new ReadBuffer());
        ASSERT_EQ(0, is->TotalCount());
        ASSERT_EQ(0, is->ByteCount());
        ASSERT_FALSE(is->Next(&data, &size));
        ASSERT_EQ(0, is->_last_bytes);
    }

    // one block
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(size, is->_last_bytes);
        ASSERT_STREQ(_block, static_cast<const char*>(data));
        ASSERT_EQ(size, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
        ASSERT_EQ(0, is->_last_bytes);
    }

    // many blocks
    {
        std::vector<char*> blocks;
        int gap = 2; // gap at head and tail
        int buf_handle_size = _capacity - gap * 2;

        ReadBufferPtr is(new ReadBuffer());
        int rb_size = 0;
        int block_count = 100;
        for (int i = 0; i < block_count; ++i)
        {
            char* block = static_cast<char*>(TranBufPool::malloc(4));
            ASSERT_TRUE(block != NULL);
            rand_str(block, _capacity);
            blocks.push_back(block);
            is->Append(BufHandle(block, buf_handle_size, gap));
            rb_size += buf_handle_size;
        }
        ASSERT_EQ(rb_size, is->TotalCount());

        const void* data;
        int size;
        int count = is->TotalCount();
        int j = 0;
        while (count > 0)
        {
            ASSERT_TRUE(is->Next(&data, &size));
            ASSERT_EQ(buf_handle_size, size);
            ASSERT_EQ(size, is->_last_bytes);
            ASSERT_EQ(0, strncmp(static_cast<const char*>(data), blocks[j]+gap, buf_handle_size));
            count -= buf_handle_size;
            ++j;
        }
        ASSERT_FALSE(is->Next(&data, &size));
        ASSERT_EQ(0, is->_last_bytes);

        for (int i = 0; i < block_count; ++i)
        {
            TranBufPool::free(blocks[i]);
        }
    }
}

TEST_F(ReadBufferTest, BackUp)
{
    const void* data;
    int size;

    // backup 0
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity * 2, is->TotalCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(_capacity, is->ByteCount());

        is->BackUp(0);
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(_capacity, is->ByteCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(_capacity * 2, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }

    // backup part
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(_capacity, is->ByteCount());

        int count = _capacity / 2 + 1;
        is->BackUp(count);
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(_capacity - count, is->ByteCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(count, size);
        ASSERT_EQ(_capacity, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }

    // backup all
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(_capacity, is->ByteCount());

        is->BackUp(_capacity);
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(0, is->ByteCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(_capacity, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }
}

TEST_F(ReadBufferTest, Skip)
{
    const void* data;
    int size;

    // skip 0
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        ASSERT_TRUE(is->Skip(0));
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(0, is->ByteCount());
        ASSERT_EQ(0, is->_last_bytes);
    }

    // skip inside block
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        int count = _capacity / 2 + 1;
        ASSERT_TRUE(is->Skip(count));
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(count, is->ByteCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity - count, size);
        ASSERT_EQ(_capacity, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }

    // skip cross block
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        is->Append(BufHandle(_block, _capacity, 0));
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity * 3, is->TotalCount());

        int count = _capacity / 2 + 1;
        ASSERT_TRUE(is->Skip(count));
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(count, is->ByteCount());

        ASSERT_TRUE(is->Skip(_capacity * 2));
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(count + _capacity * 2, is->ByteCount());

        ASSERT_TRUE(is->Next(&data, &size));
        ASSERT_EQ(_capacity - count, size);
        ASSERT_EQ(_capacity * 3, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }

    // skip to the end
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        ASSERT_TRUE(is->Skip(_capacity));
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(_capacity, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }

    // skip over end
    {
        ReadBufferPtr is(new ReadBuffer());
        is->Append(BufHandle(_block, _capacity, 0));
        ASSERT_EQ(_capacity, is->TotalCount());

        ASSERT_FALSE(is->Skip(_capacity + 1));
        ASSERT_EQ(0, is->_last_bytes);
        ASSERT_EQ(_capacity, is->ByteCount());

        ASSERT_FALSE(is->Next(&data, &size));
    }
}

class WriteBufferTest : public ::testing::Test
{
public:
    WriteBufferTest() {}
    virtual ~WriteBufferTest() {}
    virtual void SetUp() {
        _block = static_cast<char*>(TranBufPool::malloc(4));
        _capacity = TranBufPool::capacity(_block);
        ASSERT_TRUE(_block != NULL);
        rand_str(_block, _capacity);
    }
    virtual void TearDown() {
        TranBufPool::free(_block);
    }

    int _capacity;
    char* _block;
};

TEST_F(WriteBufferTest, Next)
{
    void* data;
    int size;

    // no block
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->TotalCapacity());
        ASSERT_EQ(0, os->ByteCount());
        ASSERT_TRUE(os->_buf_list.empty());
        ASSERT_EQ(0, os->_last_bytes);
    }

    // new block
    {
        WriteBufferPtr os(new WriteBuffer());

        ASSERT_TRUE(os->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(size, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(_capacity, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());
        ASSERT_EQ(_capacity, os->_last_bytes);
    }
}

TEST_F(WriteBufferTest, BackUp)
{
    void* data;
    int size;

    // backup 0
    {
        WriteBufferPtr os(new WriteBuffer());

        ASSERT_TRUE(os->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(size, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(_capacity, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());

        os->BackUp(0);
        ASSERT_EQ(0, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(_capacity, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());
    }

    // backup part
    {
        WriteBufferPtr os(new WriteBuffer());

        ASSERT_TRUE(os->Next(&data, &size));

        int count = _capacity / 2 + 1;
        os->BackUp(count);
        ASSERT_EQ(0, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(_capacity - count, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());

        ASSERT_TRUE(os->Next(&data, &size));
        ASSERT_EQ(count, size);
        ASSERT_EQ(size, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(_capacity, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());
    }

    // backup all
    {
        WriteBufferPtr os(new WriteBuffer());

        ASSERT_TRUE(os->Next(&data, &size));

        int count = _capacity;
        os->BackUp(count);
        ASSERT_EQ(0, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(0, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());

        ASSERT_TRUE(os->Next(&data, &size));
        ASSERT_EQ(_capacity, size);
        ASSERT_EQ(size, os->_last_bytes);
        ASSERT_EQ(_capacity, os->TotalCapacity());
        ASSERT_EQ(_capacity, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());
    }
}

TEST_F(WriteBufferTest, Reserve)
{
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->Reserve(10));
        ASSERT_EQ(10, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());
    }
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->Reserve(_capacity + 10));
        ASSERT_EQ(_capacity + 10, os->ByteCount());
        ASSERT_EQ(2u, os->_buf_list.size());
    }
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->Reserve(10));
        ASSERT_EQ(10, os->Reserve(_capacity));
        ASSERT_EQ(_capacity + 10, os->ByteCount());
        ASSERT_EQ(2u, os->_buf_list.size());
    }
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->Reserve(_capacity));
        ASSERT_EQ(_capacity, os->ByteCount());
        ASSERT_EQ(1u, os->_buf_list.size());

        ASSERT_EQ(_capacity, os->Reserve(_capacity));
        ASSERT_EQ(_capacity * 2, os->ByteCount());
        ASSERT_EQ(2u, os->_buf_list.size());

        ASSERT_EQ(_capacity * 2, os->Reserve(_capacity));
        ASSERT_EQ(_capacity * 3, os->ByteCount());
        ASSERT_EQ(2u, os->_buf_list.size());

        ASSERT_EQ(_capacity * 3, os->Reserve(_capacity));
        ASSERT_EQ(_capacity * 4, os->ByteCount());
        ASSERT_EQ(3u, os->_buf_list.size());
    }
}

TEST_F(WriteBufferTest, SetData)
{
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->Reserve(10));
        os->SetData(0, _block, 10);
    }
    {
        WriteBufferPtr os(new WriteBuffer());
        ASSERT_EQ(0, os->Reserve(10));
        ASSERT_EQ(10, os->Reserve(32));
        os->SetData(10, _block, 32);
    }
    {
        WriteBufferPtr os(new WriteBuffer());
        std::string str1(2000, 'x');
        std::string str2(2000, 'y');
        str1 += str2;
        int head_pos = os->Reserve(str1.size());
        os->SetData(head_pos, str1.c_str(), str1.size());
        ReadBufferPtr is(new ReadBuffer());
        os->SwapOut(is.get());
        ASSERT_EQ(str1, is->ToString());
    }
}

TEST_F(WriteBufferTest, Append)
{
    {
        WriteBufferPtr os(new WriteBuffer());
        std::string str1(2000, 'x');
        os->Append(str1);
        std::string str2(2000, 'y');
        os->Append(str2);
        str1 += str2;
        ReadBufferPtr is(new ReadBuffer());
        os->SwapOut(is.get());
        ASSERT_EQ(str1, is->ToString());
    }

    {
        WriteBufferPtr os(new WriteBuffer());
        std::string str1(2000, 'x');
        int head_pos = os->Reserve(str1.size());
        os->SetData(head_pos, str1.c_str(), str1.size());
        std::string str2(2000, 'y');
        os->Append(str2);
        str1 += str2;
        ReadBufferPtr is(new ReadBuffer());
        os->SwapOut(is.get());
        ASSERT_EQ(str1, is->ToString());
    }
}

class PBSerDeserTest : public ::testing::Test
{
public:
    PBSerDeserTest() {}
    virtual ~PBSerDeserTest() {}
    virtual void SetUp() {
        _block = static_cast<char*>(TranBufPool::malloc(4));
        _capacity = TranBufPool::capacity(_block);
        ASSERT_TRUE(_block != NULL);

        std::string str(5000, 'x');
        _test_data.set_v1(12);
        _test_data.set_v2(34);
        _test_data.set_v3(true);
        _test_data.set_v4(str);
        _test_data.set_v5("hello");
    }
    virtual void TearDown() {
        TranBufPool::free(_block);
    }

    int _capacity;
    char* _block;
    ::sofa::pbrpc::test::TestData _test_data;
};

TEST_F(PBSerDeserTest, SerDeser)
{
    RpcMessageHeader header;
    int header_size = sizeof(header);
    RpcMeta meta;
    meta.set_type(RpcMeta::REQUEST);
    meta.set_sequence_id(1);
    meta.set_method("TestService.Echo");

    WriteBufferPtr os(new WriteBuffer());
    ASSERT_EQ(0, os->Reserve(header_size));
    ASSERT_EQ(header_size, os->ByteCount());
    ASSERT_TRUE(meta.SerializeToZeroCopyStream(os.get()));
    int meta_size = os->ByteCount() - header_size;
    ASSERT_TRUE(_test_data.SerializeToZeroCopyStream(os.get()));
    int data_size = os->ByteCount() - header_size - meta_size;
    SLOG(NOTICE, "os->TotalCapacity = %d, os->ByteCount = %d",
            os->TotalCapacity(), os->ByteCount());
    int total_size = header_size + meta_size + data_size;

    ReadBufferPtr is(new ReadBuffer());
    os->SwapOut(is.get());
    ASSERT_EQ(0, os->TotalCapacity());
    ASSERT_EQ(0, os->ByteCount());
    ASSERT_EQ(total_size, is->TotalCount());
    ASSERT_EQ(0, is->ByteCount());
    SLOG(NOTICE, "is->TotalCount = %d, os->ByteCount = %d",
            is->TotalCount(), is->ByteCount());

    ASSERT_TRUE(is->Skip(header_size));
    ASSERT_EQ(header_size, is->ByteCount());

    RpcMeta new_meta;
    ASSERT_TRUE(new_meta.ParseFromBoundedZeroCopyStream(is.get(), meta_size));
    ASSERT_EQ(header_size + meta_size, is->ByteCount());
    ASSERT_EQ(RpcMeta::REQUEST, new_meta.type());
    ASSERT_EQ(1u, new_meta.sequence_id());
    ASSERT_EQ("TestService.Echo", new_meta.method());
    ASSERT_EQ(meta.method().size(), new_meta.method().size());

    ::sofa::pbrpc::test::TestData new_data;
    ASSERT_TRUE(new_data.ParseFromBoundedZeroCopyStream(is.get(), data_size));
    ASSERT_EQ(total_size, is->ByteCount());
    ASSERT_EQ(12, new_data.v1());
    ASSERT_EQ(_test_data.v5().size(), new_data.v5().size());
    std::string old_str, new_str;
    ASSERT_TRUE(_test_data.SerializeToString(&old_str));
    ASSERT_TRUE(new_data.SerializeToString(&new_str));
    ASSERT_EQ(old_str, new_str);

    SLOG(NOTICE, "meta_size=%d, data_size=%lld", meta_size, data_size);
}

int main(int argc, char **argv)
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

/* vim: set ts=4 sw=4 sts=4 tw=100 */
