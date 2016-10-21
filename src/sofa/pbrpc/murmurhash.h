// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBPRC_MURMUR_HASH_H_
#define _SOFA_PBPRC_MURMUR_HASH_H_

#include <stdint.h>

namespace sofa {
namespace pbrpc {

inline uint64_t murmurhash(const void * key, int len)
{
    const uint64_t m = 0xc6a4a7935bd1e995;
    const int r = 47; 
    uint64_t h = 0x5bd1e995;

    const uint64_t * data = (const uint64_t *)key;
    const uint64_t * end = data + (len/8);

    while(data != end) {
        uint64_t k = *data++;

        k *= m;  
        k ^= k >> r;  
        k *= m;  

        h ^= k;
        h *= m;  
    }   

    const unsigned char * data2 = (const unsigned char*)data;

    switch(len & 7) {
        case 7:
            h ^= uint64_t(data2[6]) << 48;
        case 6:
            h ^= uint64_t(data2[5]) << 40; 
        case 5:
            h ^= uint64_t(data2[4]) << 32; 
        case 4:
            h ^= uint64_t(data2[3]) << 24; 
        case 3:
            h ^= uint64_t(data2[2]) << 16; 
        case 2:
            h ^= uint64_t(data2[1]) << 8;
        case 1:
            h ^= uint64_t(data2[0]);
            h *= m;
            break;
        default:
            break;
    };  

    h ^= h >> r;
    h *= m;
    h ^= h >> r;

    return h;
}

inline uint64_t murmurhash(const char* str)
{
    return murmurhash((const void*)str, strlen(str));
}

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBPRC_MURMUR_HASH_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
