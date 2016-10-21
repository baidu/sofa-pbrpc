// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_PTIME_H_
#define _SOFA_PBRPC_PTIME_H_

#include <cstdio> // for snprintf()

#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/local_time_adjustor.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>

namespace sofa {
namespace pbrpc {

typedef boost::posix_time::ptime PTime;
typedef boost::posix_time::time_duration TimeDuration;

inline TimeDuration time_duration_microseconds(int64_t);
inline PTime ptime_now()
{
#ifdef __linux__
    struct timespec ts = { 0, 0 };
    clock_gettime(CLOCK_REALTIME, &ts);
    time_t microsec = ts.tv_sec * 1000000 + ts.tv_nsec / 1000;
#else
    struct timeval tv = { 0, 0 };
    gettimeofday(&tv, NULL);
    time_t microsec = tv.tv_sec * 1000000 + tv.tv_usec;
#endif
    TimeDuration td = time_duration_microseconds(microsec);
    PTime pt(boost::gregorian::date(1970, 1, 1));
    pt += td;
    return pt;
}

inline PTime ptime_infin()
{
    return boost::posix_time::ptime(boost::posix_time::pos_infin);
}

inline std::string ptime_to_string(const PTime& t)
{
    // see <http://www.boost.org/doc/libs/1_40_0/doc/html/date_time/examples.html>
    typedef boost::date_time::c_local_adjustor<PTime> local_adj;
    PTime lt = local_adj::utc_to_local(t);
    PTime::date_type date = lt.date();
    TimeDuration tod = lt.time_of_day();
    char buf[64];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d.%06ld",
            (int)date.year(),
            (int)date.month(),
            (int)date.day(),
            tod.hours(),
            tod.minutes(),
            tod.seconds(),
            tod.fractional_seconds());
    return buf; 
}

inline TimeDuration time_duration_hours(int64_t n)
{
    return boost::posix_time::hours(static_cast<long>(n));
}

inline TimeDuration time_duration_minutes(int64_t n)
{
    return boost::posix_time::minutes(static_cast<long>(n));
}

inline TimeDuration time_duration_seconds(int64_t n)
{
    return boost::posix_time::seconds(static_cast<long>(n));
}

inline TimeDuration time_duration_milliseconds(int64_t n)
{
    return boost::posix_time::milliseconds(static_cast<long>(n));
}

inline TimeDuration time_duration_microseconds(int64_t n)
{
    return boost::posix_time::microseconds(static_cast<long>(n));
}

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_PTIME_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
