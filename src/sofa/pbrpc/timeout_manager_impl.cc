// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/timeout_manager_impl.h>

namespace sofa {
namespace pbrpc {

TimeoutManagerImpl::TimeoutManagerImpl()
    : _is_running(false)
    , _epoch_time(ptime_now())
    , _last_ticks(0)
    , _rectify_ticks(time_duration_milliseconds(kTimerGranularity).ticks())
    , _next_id(1)
{
}

TimeoutManagerImpl::~TimeoutManagerImpl()
{
    stop();
}

bool TimeoutManagerImpl::is_running()
{
    return _is_running;
}

void TimeoutManagerImpl::start()
{
    ScopedLocker<MutexLock> _(_start_stop_lock);
    if (_is_running)
        return;
    _is_running = true;

    _thread_group.reset(new ThreadGroupImpl(kThreadCount));
    _thread_group->start();

    _timer_worker.reset(new TimerWorker(_thread_group->io_service()));
    _timer_worker->set_time_duration(time_duration_milliseconds(kTimerGranularity));
    _timer_worker->set_work_routine(boost::bind(
                &TimeoutManagerImpl::timer_run, shared_from_this(), _1));
    _timer_worker->start();
}

void TimeoutManagerImpl::stop()
{
    ScopedLocker<MutexLock> _(_start_stop_lock);
    if (!_is_running) return;
    _is_running = false;

    _timer_worker->stop();
    _thread_group->stop();

    clear();

    _timer_worker.reset();
    _thread_group.reset();
}

void TimeoutManagerImpl::clear()
{
    EventVec cleared;
    {
        ScopedLocker<MutexLock> _(_timeouts_lock);
        if (!_timeouts.empty()) {
            IdIndex& by_id = _timeouts.get<BY_ID>();
            cleared.insert(cleared.end(), by_id.begin(), by_id.end());
            _timeouts.clear();
        }
    }
    for (EventVec::iterator it = cleared.begin(); it != cleared.end(); ++it) {
        Callback* callback = it->callback;
        bool should_delete = !callback->IsSelfDelete();
        callback->Run(it->id, TimeoutManager::CLEARED);
        if (should_delete) delete callback;
    }
}

TimeoutManagerImpl::Id TimeoutManagerImpl::add(int64 interval, Callback* callback)
{
    SCHECK_GE(interval, 0);
    SCHECK(callback->IsSelfDelete());
    ScopedLocker<MutexLock> _(_timeouts_lock);
    Id id = _next_id++;
    _timeouts.insert(Event(id, calc_expiration(interval), -1, callback));
    return id;
}

TimeoutManagerImpl::Id TimeoutManagerImpl::add_repeating(int64 interval, Callback* callback)
{
    SCHECK_GE(interval, 0);
    SCHECK(!callback->IsSelfDelete());
    ScopedLocker<MutexLock> _(_timeouts_lock);
    Id id = _next_id++;
    _timeouts.insert(Event(id, calc_expiration(interval), interval, callback));
    return id;
}

bool TimeoutManagerImpl::erase(Id id)
{
    Callback* callback = NULL;
    {
        ScopedLocker<MutexLock> _(_timeouts_lock);
        IdIndex& by_id = _timeouts.get<BY_ID>();
        IdIndex::iterator find = by_id.find(id);
        if (find == by_id.end()) return false;
        callback = find->callback;
        by_id.erase(find);
    }
    bool should_delete = !callback->IsSelfDelete();
    callback->Run(id, TimeoutManager::ERASED);
    if (should_delete) delete callback;
    return true;
}

void TimeoutManagerImpl::timer_run(const PTime& now)
{
    if (!_is_running) return;
    int64 now_ticks = (now - _epoch_time).ticks();
    _last_ticks = now_ticks;

    EventVec expired;
    {
        ScopedLocker<MutexLock> _(_timeouts_lock);
        if (!_timeouts.empty()) {
            ExpirationIndex& by_expiration = _timeouts.get<BY_EXPIRATION>();
            ExpirationIndex::iterator exp_end = by_expiration.upper_bound(now_ticks);
            expired.insert(expired.end(), by_expiration.begin(), exp_end);
            by_expiration.erase(by_expiration.begin(), exp_end);
        }
    }

    if (expired.empty()) return;

    // Reinsert if repeating, do this before executing callbacks
    // so the callbacks have a chance to call erase
    EventVec repeated;
    for (EventVec::iterator it = expired.begin(); it != expired.end(); ++it) {
        if (it->repeat_interval >= 0) {
            repeated.push_back(Event(it->id, calc_expiration(it->repeat_interval),
                        it->repeat_interval, it->callback));
        }
    }
    if (!repeated.empty()) {
        ScopedLocker<MutexLock> _(_timeouts_lock);
        _timeouts.insert(repeated.begin(), repeated.end());
    }

    // Execute callbacks
    for (EventVec::iterator it = expired.begin(); it != expired.end(); ++it) {
        it->callback->Run(it->id, TimeoutManager::TIMEOUTED);
    }
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
