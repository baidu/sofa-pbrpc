// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <cstdio> // for sprintf()

#include <sofa/pbrpc/string_utils.h>
#include <sofa/pbrpc/ascii.h>

namespace sofa {
namespace pbrpc {

int CEscapeInternal(const char* src, int src_len, char* dest, int dest_len)
{
    int used = 0;
    const char* src_end = src + src_len;
    for (; src < src_end; src++)
    {
        if (dest_len - used < 2)   // at least two chars needed.
        {
            return -1;
        }
        switch (*src)
        {
            case '\a': dest[used++] = '\\'; dest[used++] = 'a';  break;
            case '\b': dest[used++] = '\\'; dest[used++] = 'b';  break;
            case '\f': dest[used++] = '\\'; dest[used++] = 'f';  break;
            case '\n': dest[used++] = '\\'; dest[used++] = 'n';  break;
            case '\r': dest[used++] = '\\'; dest[used++] = 'r';  break;
            case '\t': dest[used++] = '\\'; dest[used++] = 't';  break;
            case '\v': dest[used++] = '\\'; dest[used++] = 'v';  break;
                       case '\"': dest[used++] = '\\'; dest[used++] = '\"'; break;
            case '\?': dest[used++] = '\\'; dest[used++] = '\?'; break;
            case '\'': dest[used++] = '\\'; dest[used++] = '\''; break;
            case '\\': dest[used++] = '\\'; dest[used++] = '\\'; break;
            default:
                       if (Ascii::IsPrint(*src))
                       {
                           dest[used++] = *src;
                       }
                       else
                       {
                           if (dest_len - used < 4)
                               return -1;
                           sprintf(dest + used, "\\x%02x", static_cast<uint8_t>(*src));
                           used += 4;
                       }
        }
    }
    if (dest_len - used < 1)
        return -1;
    dest[used] = '\0';
    return used;
}

int CEscapeString(const char* src, int src_len, char* dest, int dest_len)
{
    return CEscapeInternal(src, src_len, dest, dest_len);
}

std::string CEscapeString(const char* src, int src_len)
{
    const int dest_length = src_len * 4 + 1; // Maximum space needed
    char* dest = new char[dest_length];
    const int len = CEscapeInternal(src, src_len, dest, dest_length);
    std::string str;
    if (len >= 0)
        str = dest;
    return str;
}

std::string CEscapeString(const std::string& src)
{
    return CEscapeString(src.data(), src.length());
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
