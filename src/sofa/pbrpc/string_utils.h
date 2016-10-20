// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_STRING_UTILS_H_
#define _SOFA_PBRPC_STRING_UTILS_H_

#include <stdint.h>
#include <string>
#include <vector>
#include <ostream>

namespace sofa {
namespace pbrpc {

class StringUtils
{
public:
    // Transfer string to lower string.
    static std::string to_lower(const std::string& str);

    // Transfer string to upper string.
    static std::string to_upper(const std::string& str);

    static bool end_with(const std::string& str, const std::string& match);

    static bool begin_with(const std::string& str, const std::string& match);

    // Remove all leading and trailing spaces from the input "str".
    static std::string trim(const std::string& str);

    // Split the string "str" with delimiter "delim".  Empty string is also count.
    // For sample, delim="@":
    //   "" => [""]
    //   "a" => ["a"]
    //   "a@b" => ["a","b"]
    //   "@" => ["",""]
    //   "a@" => ["a",""]
    //   "@b" => ["","b"]
    //   "a@@b" => ["a","","b"]
    static void split(const std::string& str, const std::string& delim,
            std::vector<std::string>* substr_list);

    // Joins all strings in "components" into one long string.
    // Segments are concatenated by given separator "delim".
    static std::string join(const std::vector<std::string>& components,
            const std::string& delim);

    // Replace the first "old" pattern with the "new" pattern in a string.
    static std::string replace(const std::string& str,
            const std::string& oldsub, const std::string& newsub);

    // Replace all the "old" pattern with the "new" pattern in a string.
    static std::string replace_all(const std::string& str,
            const std::string& oldsub, const std::string& newsub);

    // Compute the hash value for the given string.
    // This is copied from gcc 3.4.3 "hash_fun.h".
    static size_t hash_string(const char *str)
    {
        unsigned long hash = 0;
        for ( ; *str; ++str) hash = 5*hash + *str;
        return size_t(hash);
    }

    // Return a mutable char* pointing to a string's internal buffer.
    // Writing through this pointer will modify the string.
    static char* string_as_array(std::string* str)
    {
        return &*str->begin();
    }

    // Resize the string without initializing the new bytes.
    // In some cases doing this can significantly improve performance. 
    static void resize_uninitialized(std::string* str, size_t new_size)
    {
        struct Rep
        {
            std::string::size_type length;
            std::string::size_type capacity;
            int refcount; // _Atomic_word
        };
        // We need always call reserve() for two reasons:
        //   - if new_size > capacity, to realloc space.
        //   - if new_size <= capacity, to make data unshared.
        // More, if new_size < capacity, we call reserve() with the old capacity
        // to avoid unnecessary copies.
        str->reserve(std::max(new_size, str->capacity()));
        char* base = &*str->begin();
        reinterpret_cast<Rep*>(base)[-1].length = new_size;
        base[new_size] = '\0';
    }

    // Format uint64_t to string.
    static std::string uint64_to_string(uint64_t value);

    // Escape a binary string to a c-escaped string.
    static std::string c_escape_string(const char* src, int src_len);
    static std::string c_escape_string(const std::string& src)
    {
        return c_escape_string(src.data(), src.length());
    }

    // Unescape a c-escaped string to a binary string.
    static std::string unescape_c_escape_string(const std::string& src);

    // Escape a binary string to xml style string.
    static void xml_escape_string(std::ostream& os, const char* src, int src_len);
    static std::string xml_escape_string(const char* src, int src_len);
    static std::string xml_escape_string(const std::string& src)
    {
        return xml_escape_string(src.data(), src.length());
    }

    // Escape a binary string to json style string.
    static void json_escape_string(std::ostream& os, const char* src, int src_len);
    static std::string json_escape_string(const char* src, int src_len);
    static std::string json_escape_string(const std::string& src)
    {
        return json_escape_string(src.data(), src.length());
    }

    // Encode URL. Not encode reserved char by default.
    // Set E_ENCODE_RESERVED_CHAR if you want use the URL as file name.
    // Set E_NOT_ENCODE_HIGH_CHAR if you want reserve the chinese characters.
    enum
    {
        E_ENCODE_RESERVED_CHAR = 0x01,      // encode reserved char
        E_NOT_ENCODE_HIGH_CHAR = 0x02       // not encode high char (ch > 127)
    };
    static void encode_url(std::ostream& os, const std::string& str, int flag = 0);
    static std::string encode_url(const std::string& str, int flag = 0);

    // Decode URL. Not decode reserved char and precent sign char by default.
    enum
    {
        E_DECODE_RESERVED_CHAR = 0x10,      // decode reserved char
        E_DECODE_PERCENT_SIGN_CHAR = 0x20   // decode precent sign char '%'
    };
    static void decode_url(std::ostream& os, const std::string& str, int flag = 0);
    static std::string decode_url(const std::string& str, int flag = 0);
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_STRING_UTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
