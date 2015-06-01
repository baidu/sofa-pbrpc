// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/ptime.h>

namespace sofa {
namespace pbrpc {

const char* compile_info()
{
    return __DATE__ " " __TIME__ " by gcc "
        SOFA_PBRPC_STR(__GNUC__) "." SOFA_PBRPC_STR(__GNUC_MINOR__)
        "." SOFA_PBRPC_STR(__GNUC_PATCHLEVEL__);
}

namespace internal {

static LogLevel s_log_level = ::sofa::pbrpc::LOG_LEVEL_ERROR;

LogLevel get_log_level()
{
    return s_log_level;
}

void set_log_level(LogLevel level)
{
    s_log_level = level;
}

void default_log_handler(
    LogLevel level, const char* filename, int line, const char *fmt, va_list ap)
{
    static const char* level_names[] = { "FATAL", "ERROR", "WARNNING",
                                         "INFO", "TRACE", "DEBUG" };
    char buf[1024];
    vsnprintf(buf, 1024, fmt, ap);
#if 0
    fprintf(stderr, "libsofa_pbrpc %s %s %s:%d] %s\n",
            level_names[level],
            boost::posix_time::to_simple_string(
                boost::posix_time::microsec_clock::local_time()).c_str(),
            filename, line, buf);
#endif
    fprintf(stderr, "libsofa_pbrpc %s %s:%d] %s\n",
            level_names[level],
            filename, line, buf);
    fflush(stderr);

    if (level == ::sofa::pbrpc::LOG_LEVEL_FATAL)
    {
        abort();
    }
}

void null_log_handler(LogLevel, const char*, int, const char *, va_list)
{
    // Nothing
}

static LogHandler* s_log_handler = default_log_handler;

void log_handler(LogLevel level, const char* filename, int line, const char *fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    s_log_handler(level, filename, line, fmt, ap);
    va_end(ap);
}

} // namespace internal

LogHandler* set_log_handler(LogHandler* new_func) {
    LogHandler* old = internal::s_log_handler;
    if (old == &internal::null_log_handler) {
        old = NULL;
    }
    if (new_func == NULL) {
        internal::s_log_handler = &internal::null_log_handler;
    } else {
        internal::s_log_handler = new_func;
    }
    return old;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
