// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_TIMEOUT_MANAGER_IMPL_H_
#define _SOFA_PBRPC_TIMEOUT_MANAGER_IMPL_H_

#include <vector>

#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/timeout_manager.h>
#include <sofa/pbrpc/thread_group_impl.h>
#include <sofa/pbrpc/timer_worker.h>

namespace sofa {
namespace pbrpc {

// Defined in this file.
class TimeoutManagerImpl;
typedef sofa::pbrpc::shared_ptr<TimeoutManagerImpl> TimeoutManagerImplPtr;

class TimeoutManagerImpl : public sofa::pbrpc::enable_shared_from_this<TimeoutManagerImpl>
{
public:
    // Thread number for timer and callbacks.
    const static int kThreadCount = 1;
    // Timeout granularity of timer in milli-seconds.
    const static int64 kTimerGranularity = 10; 

    typedef TimeoutManager::Id Id;
    typedef TimeoutManager::Type Type;
    typedef TimeoutManager::Callback Callback;

    TimeoutManagerImpl();
    ~TimeoutManagerImpl();

    bool is_running();

    void start();

    void stop();

    void clear();

    Id add(int64 interval, Callback* callback);

    Id add_repeating(int64 interval, Callback* callback);

    bool erase(Id id);

private:
    // Given interval in milli-seconds, calculate expiration ticks.
    inline int64 calc_expiration(int64 interval)
    {
        return _last_ticks + time_duration_milliseconds(interval).ticks() + _rectify_ticks;
    }

    void timer_run(const PTime& now);

private:
    struct Event {
        Id id;
        int64 expiration;
        int64 repeat_interval;
        Callback* callback;
        Event(Id i, int64 e, int64 r, Callback* c)
            : id(i), expiration(e), repeat_interval(r), callback(c) {}
    };

    typedef boost::multi_index_container<
        Event,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::member<
            Event, Id, &Event::id
            > >,
        boost::multi_index::ordered_non_unique<boost::multi_index::member<
            Event, int64, &Event::expiration
            > >
        >
    > Set;

    enum {
        BY_ID=0,
        BY_EXPIRATION=1
    };

    typedef Set::nth_index<BY_ID>::type IdIndex;
    typedef Set::nth_index<BY_EXPIRATION>::type ExpirationIndex;
    typedef std::vector<Event> EventVec;

    volatile bool _is_running;
    MutexLock _start_stop_lock;
    PTime _epoch_time;
    volatile int64 _last_ticks; 
    int64 _rectify_ticks;

    ThreadGroupImplPtr _thread_group;
    TimerWorkerPtr _timer_worker;

    Set _timeouts;
    Id _next_id;
    MutexLock _timeouts_lock;
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_TIMEOUT_MANAGER_IMPL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
