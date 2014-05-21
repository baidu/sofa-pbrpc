// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#include <vector>

#include <sofa/pbrpc/timeout_queue.h>

namespace sofa {
namespace pbrpc {

TimeoutQueue::~TimeoutQueue()
{
}

uint64 TimeoutQueue::add(
        int64 now,
        int64 delay,
        Callback* callback)
{
    SCHECK(callback->IsSelfDelete());
    uint64 id = _next_id++;
    _timeouts.insert(Event(id, now + delay, -1, callback));
    return id;
}

uint64 TimeoutQueue::add_repeating(
        int64 now,
        int64 interval,
        Callback* callback)
{
    SCHECK(!callback->IsSelfDelete());
    uint64 id = _next_id++;
    _timeouts.insert(Event(id, now + interval, interval, callback));
    return id;
}

int64 TimeoutQueue::next_expiration() const
{
    return (_timeouts.empty() ? kint64max :
            _timeouts.get<BY_EXPIRATION>().begin()->expiration);
}

bool TimeoutQueue::erase(uint64 id)
{
    IdIndex& by_id = _timeouts.get<BY_ID>();
    IdIndex::iterator find = by_id.find(id);
    if (find == by_id.end()) return false;
    delete find->callback;
    by_id.erase(find);
    return true;
}

int64 TimeoutQueue::run_internal(int64 now, bool once_only)
{
    ExpirationIndex& by_expiration = _timeouts.get<BY_EXPIRATION>();
    int64 next_exp;
    do {
        ExpirationIndex::iterator exp_end = by_expiration.upper_bound(now);
        std::vector<Event> expired(by_expiration.begin(), exp_end);
        by_expiration.erase(by_expiration.begin(), exp_end);
        for (std::vector<Event>::iterator it = expired.begin();
                it != expired.end(); ++it) {
            Event& event = *it;
            // Reinsert if repeating, do this before executing callbacks
            // so the callbacks have a chance to call erase
            if (event.repeat_interval >= 0) {
                _timeouts.insert(Event(event.id, now + event.repeat_interval,
                        event.repeat_interval, event.callback));
            }
        }

        // Call callbacks
        for (std::vector<Event>::iterator it = expired.begin();
                it != expired.end(); ++it) {
            Event& event = *it;
            event.callback->Run(event.id, now);
        }
        next_exp = next_expiration();
    } while (!once_only && next_exp <= now);
    return next_exp;
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
