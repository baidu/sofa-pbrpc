// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cassert>
#include <cstdio>
#include <sstream>
#include <stdexcept>

#include <sofa/pbrpc/string_utils.h>
#include <sofa/pbrpc/ascii.h>
#include <sofa/pbrpc/smart_ptr/scoped_array.hpp>

namespace sofa {
namespace pbrpc {

std::string StringUtils::to_lower(const std::string& str)
{
    std::string ret(str);
    for (size_t i = 0; i < ret.size(); ++i) {
        if (Ascii::is_upper(ret[i])) {
            ret[i] = Ascii::to_lower(ret[i]); 
        }
    }
    return ret;
}

std::string StringUtils::to_upper(const std::string& str)
{
    std::string ret(str);
    for (size_t i = 0; i < ret.size(); ++i) {
        if (Ascii::is_lower(ret[i])) {
            ret[i] = Ascii::to_upper(ret[i]); 
        }
    }
    return ret;
}

bool StringUtils::end_with(const std::string& str, const std::string& match)
{
    return str.size() >= match.size()
        && str.rfind(match) + match.size() == str.size(); 
}

bool StringUtils::begin_with(const std::string& str, const std::string& match)
{
    return str.size() >= match.size()
        && str.substr(0, match.size()) == match;
}

std::string StringUtils::trim(const std::string& str)
{
    size_t start_pos = 0;
    size_t end_pos = str.length();
    while (start_pos != end_pos && Ascii::is_space(str[start_pos]))
        start_pos++;
    if (start_pos == end_pos) {
        return "";
    }
    end_pos--;
    while (Ascii::is_space(str[end_pos])) // end_pos always >= 0
        end_pos--;
    return str.substr(start_pos, end_pos - start_pos + 1);
}

void StringUtils::split(const std::string& str, const std::string& delim,
        std::vector<std::string>* substr_list)
{
    substr_list->clear();
    if (str.empty() || delim.empty()) {
        substr_list->push_back(str);
        return;
    }
    std::string::size_type start = 0;
    while (true) {
        std::string::size_type pos = str.find(delim, start);
        if (pos == start) {
            substr_list->push_back(std::string());
            start += delim.length();
            continue;
        }
        else if (pos == std::string::npos) {
            substr_list->push_back(str.substr(start));
            break;
        }
        else {
            substr_list->push_back(str.substr(start, pos - start));
            start = pos + delim.length();
        }
    }
}

std::string StringUtils::join(const std::vector<std::string>& components,
        const std::string& delim)
{
    std::ostringstream oss;
    for (std::vector<std::string>::const_iterator it = components.begin();
         it != components.end(); ++it)
    {
        if (it != components.begin())
            oss << delim;
        oss << *it;
    }
    return oss.str();
}

std::string StringUtils::replace(const std::string& str,
        const std::string& oldsub, const std::string& newsub)
{
    if (oldsub.empty()) return str;
    std::string res;
    std::string::size_type pos = str.find(oldsub);
    if (pos == std::string::npos) {
        return str;
    }
    else {
        res.append(str.data(), pos);
        res.append(newsub.data(), newsub.size());
        res.append(str.data() + pos + oldsub.size(), str.size() - pos - oldsub.size());
    }
    return res;
}

std::string StringUtils::replace_all(const std::string& str,
        const std::string& oldsub, const std::string& newsub)
{
    if (oldsub.empty()) return str;
    std::string res;
    std::string::size_type start_pos = 0;
    std::string::size_type pos;
    do {
        pos = str.find(oldsub, start_pos);
        if (pos == std::string::npos) {
            break;
        }
        res.append(str.data() + start_pos, pos - start_pos);
        res.append(newsub.data(), newsub.size());
        start_pos = pos + oldsub.size();
    } while (true);
    res.append(str.data() + start_pos, str.size() - start_pos);
    return res;
}

std::string StringUtils::uint64_to_string(uint64_t value)
{
    char buf[32];
    sprintf(buf, "0x%016lX", value);
    return std::string(buf);
}

////////////// c_escape ///////////////////////////////
#define IS_OCTAL_DIGIT(c) (((c) >= '0') && ((c) <= '7'))

inline bool isxdigit(char c) {
    return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F');
}

inline bool isprint(char c) {
    return c >= 0x20 && c <= 0x7E;
}

static int c_escape_string_internal(const char* src, int src_len, char* dest, int dest_len) {
    int used = 0;
    const char* src_end = src + src_len;
    for (; src < src_end; src++) {
        // at least two chars needed.
        if (dest_len - used < 2) {
            return -1;
        }
        unsigned char c = *src;
        switch (c) {
            case '\n': dest[used++] = '\\'; dest[used++] = 'n';  break;
            case '\r': dest[used++] = '\\'; dest[used++] = 'r';  break;
            case '\t': dest[used++] = '\\'; dest[used++] = 't';  break;
            case '\"': dest[used++] = '\\'; dest[used++] = '\"'; break;
            case '\'': dest[used++] = '\\'; dest[used++] = '\''; break;
            case '\\': dest[used++] = '\\'; dest[used++] = '\\'; break;
            default:
                if (c == '\0' && (src + 1 == src_end || !IS_OCTAL_DIGIT(src[1]))) {
                    dest[used++] = '\\';
                    dest[used++] = '0';
                }
                else if (c < ' ' || c > '~') {
                    if (dest_len - used < 4) {
                        return -1;
                    }
                    sprintf(dest + used, "\\%03o", c);
                    used += 4;
                }
                else {
                    dest[used++] = c;
                }
        }
    }
    if (dest_len - used < 1)
        return -1;
    dest[used] = '\0';
    return used;
}

std::string StringUtils::c_escape_string(const char* src, int src_len)
{
    int dest_length = src_len * 4 + 1; // Maximum space needed
    sofa::pbrpc::scoped_array<char> dest(new char[dest_length]);
    int len = c_escape_string_internal(src, src_len, dest.get(), dest_length);
    assert(len >= 0);
    return std::string(dest.get(), len);
}

////////////// c_unescape ///////////////////////////////
////// Come from google/protobuf/stubs/strutils.cc //////
inline int hex_digit_to_int(char c) {
    /* Assume ASCII. */
    assert('0' == 0x30 && 'A' == 0x41 && 'a' == 0x61);
    assert(isxdigit(c));
    int x = static_cast<unsigned char>(c);
    if (x > '9') {
        x += 9;
    }
    return x & 0xf;
}

static int unescape_c_escape_sequences(const char* source, char* dest)
{
    char* d = dest;
    const char* p = source;

    // Small optimization for case where source = dest and there's no escaping
    while ( p == d && *p != '\0' && *p != '\\' ) {
        p++;
        d++;
    }

    while (*p != '\0') {
        if (*p != '\\') {
            *d++ = *p++;
        } else {
            switch ( *++p ) {                    // skip past the '\\'
                case '\0':
                    throw std::runtime_error("bad c-escape string");
                case 'a':  *d++ = '\a';  break;
                case 'b':  *d++ = '\b';  break;
                case 'f':  *d++ = '\f';  break;
                case 'n':  *d++ = '\n';  break;
                case 'r':  *d++ = '\r';  break;
                case 't':  *d++ = '\t';  break;
                case 'v':  *d++ = '\v';  break;
                case '\\': *d++ = '\\';  break;
                case '?':  *d++ = '\?';  break;    // \?  Who knew?
                case '\'': *d++ = '\'';  break;
                case '"':  *d++ = '\"';  break;
                case '0': case '1': case '2': case '3':  // octal digit: 1 to 3 digits
                case '4': case '5': case '6': case '7': {
                    char ch = *p - '0';
                    if ( IS_OCTAL_DIGIT(p[1]) ) {
                        ch = ch * 8 + *++p - '0';
                    }
                    if ( IS_OCTAL_DIGIT(p[1]) ) {      // safe (and easy) to do this twice
                        ch = ch * 8 + *++p - '0';      // now points at last digit
                    }
                    *d++ = ch;
                    break;
                }
                case 'x': case 'X': {
                    if (!isxdigit(p[1])) {
                        if (p[1] == '\0') {
                            throw std::runtime_error("bad c-escape string");
                        }
                        else {
                            throw std::runtime_error("bad c-escape string");
                        }
                        break;
                    }
                    unsigned int ch = 0;
                    while (isxdigit(p[1])) { // arbitrarily many hex digits
                        ch = (ch << 4) + hex_digit_to_int(*++p);
                    }
                    if (ch > 0xFF) {
                        throw std::runtime_error("bad c-escape string");
                    }
                    *d++ = ch;
                    break;
                }
                case 'u': case 'U': {
                    // TODO: support \u and \U ?
                    throw std::runtime_error("bad c-escape string");
                }
                default:
                    throw std::runtime_error("bad c-escape string");
            }
            p++; // read past letter we escaped
        }
    }
    *d = '\0';
    return d - dest;
}

std::string StringUtils::unescape_c_escape_string(const std::string& src)
{
    sofa::pbrpc::scoped_array<char> unescaped(new char[src.size() + 1]);
    int len = unescape_c_escape_sequences(src.c_str(), unescaped.get());
    return std::string(unescaped.get(), len);
}

////////////// xml_escape ///////////////////////////////
void StringUtils::xml_escape_string(std::ostream& os, const char* src, int src_len)
{
    for (int i = 0; i < src_len; ++i) {
        char ch = *(src + i);
        switch (ch) {
            case '&': os << "&amp;"; break;
            case '\'': os << "&apos;"; break;
            case '"': os << "&quot;"; break;
            case '<': os << "&lt;"; break;
            case '>': os << "&gt;"; break;
            default: os << ch;
        }
    }
}

std::string StringUtils::xml_escape_string(const char* src, int src_len)
{
    std::stringstream os;
    xml_escape_string(os, src, src_len);
    return os.str();
}

////////////// json_escape ///////////////////////////////
void StringUtils::json_escape_string(std::ostream& os, const char* src, int src_len)
{
    for (int i = 0; i < src_len; ++i) {
        char c = *(src + i);
        switch (c)
        {
            case '"': os << "\\\""; break;
            case '\\': os  << "\\\\"; break;
            case '/': os << "\\/"; break;
            case '\b': os << "\\b"; break;
            case '\f': os << "\\f"; break;
            case '\n': os << "\\n"; break;
            case '\r': os << "\\r"; break;
            case '\t': os << "\\t"; break;
            case '\0': os << "\\u0000"; break;
            default: os << c;
        }
    }
}

std::string StringUtils::json_escape_string(const char* src, int src_len)
{
    std::stringstream os;
    json_escape_string(os, src, src_len);
    return os.str();
}

////////////// encode_url ///////////////////////////////
#define URL_CHR_TEST(c, mask)   (URL_CHR_TABLE[(unsigned char)(c)] & (mask))
#define URL_RESERVED_CHAR(c)    URL_CHR_TEST(c, E_URL_CHR_RESERVED)
#define URL_UNSAFE_CHAR(c)      URL_CHR_TEST(c, E_URL_UNSAFE)

#define XNUM_TO_DIGIT(x)        ("0123456789ABCDEF"[x])
#define XDIGIT_TO_NUM(h)        ((h) < 'A' ? (h) - '0' : toupper(h) - 'A' + 10)
#define X2DIGITS_TO_NUM(h1, h2) ((XDIGIT_TO_NUM (h1) << 4) + XDIGIT_TO_NUM (h2))

// Url character type.
enum
{
    E_URL_CHR_RESERVED = 1,
    E_URL_UNSAFE = 2
};

// Shorthands for the table.
#define R   E_URL_CHR_RESERVED
#define U   E_URL_UNSAFE
#define RU  R|U

// Characters defined by RFC 3986.
const unsigned char URL_CHR_TABLE[256] =
{
    U,  U,  U,  U,   U,  U,  U,  U,   /* NUL SOH STX ETX  EOT ENQ ACK BEL */
    U,  U,  U,  U,   U,  U,  U,  U,   /* BS  HT  LF  VT   FF  CR  SO  SI  */
    U,  U,  U,  U,   U,  U,  U,  U,   /* DLE DC1 DC2 DC3  DC4 NAK SYN ETB */
    U,  U,  U,  U,   U,  U,  U,  U,   /* CAN EM  SUB ESC  FS  GS  RS  US  */
    U,  R,  U, RU,   R,  U,  R,  R,   /* SP  !   "   #    $   %   &   '   */
    R,  R,  R,  R,   R,  0,  0,  R,   /* (   )   *   +    ,   -   .   /   */
    0,  0,  0,  0,   0,  0,  0,  0,   /* 0   1   2   3    4   5   6   7   */
    0,  0, RU,  R,   U,  R,  U,  R,   /* 8   9   :   ;    <   =   >   ?   */
    RU, 0,  0,  0,   0,  0,  0,  0,   /* @   A   B   C    D   E   F   G   */
    0,  0,  0,  0,   0,  0,  0,  0,   /* H   I   J   K    L   M   N   O   */
    0,  0,  0,  0,   0,  0,  0,  0,   /* P   Q   R   S    T   U   V   W   */
    0,  0,  0, RU,   U, RU,  U,  0,   /* X   Y   Z   [    \   ]   ^   _   */
    U,  0,  0,  0,   0,  0,  0,  0,   /* `   a   b   c    d   e   f   g   */
    0,  0,  0,  0,   0,  0,  0,  0,   /* h   i   j   k    l   m   n   o   */
    0,  0,  0,  0,   0,  0,  0,  0,   /* p   q   r   s    t   u   v   w   */
    0,  0,  0,  U,   U,  U,  0,  U,   /* x   y   z   {    |   }   ~   DEL */

    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,

    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
    U, U, U, U,  U, U, U, U,  U, U, U, U,  U, U, U, U,
};

#undef RU
#undef U
#undef R

void StringUtils::encode_url(std::ostream& os, const std::string& str, int flag)
{
    size_t len = str.size();
    for (size_t pos = 0; pos < len; pos++)
    {
        unsigned char ch = str[pos];
        if ('%' == ch)
        {
            if (((pos + 2) < len) && isxdigit(str[pos + 1]) && isxdigit(str[pos + 2]))
            {
                os << ch;
            }
            else
            {
                os << '%' << XNUM_TO_DIGIT(ch >> 4) << XNUM_TO_DIGIT(ch & 0xf);
            }
        }
        else if (URL_UNSAFE_CHAR (ch))
        {
            if ((!(flag & E_ENCODE_RESERVED_CHAR) && URL_RESERVED_CHAR (ch))
                    || ((flag & E_NOT_ENCODE_HIGH_CHAR) && (ch > 127)))
            {
                os << ch;
            }
            else
            {
                os << '%' << XNUM_TO_DIGIT(ch >> 4) << XNUM_TO_DIGIT(ch & 0xf);
            }
        }
        else
        {
            if ((flag & E_ENCODE_RESERVED_CHAR) && URL_RESERVED_CHAR(ch))
            {
                os << '%' << XNUM_TO_DIGIT(ch >> 4) << XNUM_TO_DIGIT(ch & 0xf);
            }
            else
            {
                os << ch;
            }
        }
    }
}

std::string StringUtils::encode_url(const std::string& str, int flag)
{
    std::stringstream os;
    encode_url(os, str, flag);
    return os.str();
}

void StringUtils::decode_url(std::ostream& os, const std::string& str, int flag)
{
    const char* cs = str.c_str();
    size_t len = str.size();

    for (size_t pos = 0; pos < len; pos++)
    {
        if (('%' == cs[pos]) && (pos + 2 < len)
                && isxdigit(cs[pos + 1]) && isxdigit(cs[pos + 2]))
        {
            unsigned char ch = X2DIGITS_TO_NUM(cs[pos + 1], cs[pos + 2]);

            if ((!(flag & E_DECODE_RESERVED_CHAR) && URL_RESERVED_CHAR(ch))
                || (!(flag & E_DECODE_PERCENT_SIGN_CHAR) && ('%' == ch)))
            {
                os << cs[pos] << cs[pos + 1] << cs[pos + 2];
            }
            else
            {
                os << ch;
            }

            pos += 2;
        }
        else
        {
            os << cs[pos];
        }
    }
}

std::string StringUtils::decode_url(const std::string& str, int flag)
{
    std::stringstream os;
    decode_url(os, str, flag);
    return os.str();
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
