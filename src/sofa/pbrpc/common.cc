// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#if defined( SOFA_PBRPC_ENABLE_DETAILED_LOGGING )
#include <time.h>
#include <sys/time.h>
#include <pthread.h>
#endif

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
#if defined( SOFA_PBRPC_ENABLE_DETAILED_LOGGING )
    struct timeval now_tv;
    gettimeofday(&now_tv, NULL);
    const time_t seconds = now_tv.tv_sec;
    struct tm t;
    localtime_r(&seconds, &t);
    fprintf(stderr, "libsofa_pbrpc %s %04d/%02d/%02d-%02d:%02d:%02d.%06d %llx %s:%d] %s\n",
            level_names[level],
            t.tm_year + 1900,
            t.tm_mon + 1,
            t.tm_mday,
            t.tm_hour,
            t.tm_min,
            t.tm_sec,
            static_cast<int>(now_tv.tv_usec),
#ifdef __linux__
            static_cast<long long unsigned int>(pthread_self()),
#else
            reinterpret_cast<long long unsigned int>(pthread_self()),
#endif
            filename, line, buf);
#else
    fprintf(stderr, "libsofa_pbrpc %s %s:%d] %s\n",
            level_names[level],
            filename, line, buf);
#endif
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
