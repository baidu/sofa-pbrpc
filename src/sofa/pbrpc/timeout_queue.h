// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_TIMEOUT_QUEUE_H_
#define _SOFA_PBRPC_TIMEOUT_QUEUE_H_

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/ext_closure.h>
#include <boost/multi_index_container.hpp>
#include <boost/multi_index/indexed_by.hpp>
#include <boost/multi_index/ordered_index.hpp>
#include <boost/multi_index/member.hpp>

namespace sofa {
namespace pbrpc {

// Callback.

class TimeoutQueue {
public:
    typedef ExtClosure<void(uint64 /*id*/, int64 /*now*/)> Callback;

    TimeoutQueue() : _next_id(1) { }
    ~TimeoutQueue();

    /**
     * Add a one-time timeout event that will fire "delay" time units from "now"
     * (that is, the first time that run*() is called with a time value >= now
     * + delay).
     *
     * The "callback" should be a self delete closure (created by
     * NewExtClosure()).
     */
    uint64 add(int64 now, int64 delay, Callback* callback);

    /**
     * Add a repeating timeout event that will fire every "interval" time units
     * (it will first fire when run*() is called with a time value >=
     * now + interval).
     *
     * run*() will always invoke each repeating event at most once, even if
     * more than one "interval" period has passed.
     *
     * The "callback" should be a permanent closure (created by
     * NewPermanentExtClosure()).
     */
    uint64 add_repeating(int64 now, int64 interval, Callback* callback);

    /**
     * Erase a given timeout event, returns true if the event was actually
     * erased and false if it didn't exist in our queue.
     */
    bool erase(uint64 id);

    /**
     * Process all events that are due at times <= "now" by calling their
     * callbacks.
     *
     * Callbacks are allowed to call back into the queue and add / erase events;
     * they might create more events that are already due.  In this case,
     * run_once() will only go through the queue once, and return a "next
     * expiration" time in the past or present (<= now); run_loop()
     * will process the queue again, until there are no events already due.
     *
     * Note that it is then possible for runLoop to never return if
     * callbacks re-add themselves to the queue (or if you have repeating
     * callbacks with an interval of 0).
     *
     * Return the time that the next event will be due (same as
     * next_expiration(), below)
     */
    int64 run_once(int64 now) { return run_internal(now, true); }
    int64 run_loop(int64 now) { return run_internal(now, false); }

    /**
     * Return the time that the next event will be due.
     */
    int64 next_expiration() const;

private:
    // noncopyable
    TimeoutQueue(const TimeoutQueue&);
    TimeoutQueue& operator=(const TimeoutQueue&);

    int64 run_internal(int64 now, bool run_once);

    struct Event {
        uint64 id;
        int64 expiration;
        int64 repeat_interval;
        Callback* callback;
        Event(uint64 i, int64 e, int64 r, Callback* c)
            : id(i), expiration(e), repeat_interval(r), callback(c) {}
    };

    typedef boost::multi_index_container<
        Event,
        boost::multi_index::indexed_by<
            boost::multi_index::ordered_unique<boost::multi_index::member<
            Event, uint64, &Event::id
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

    typedef Set::nth_index<BY_EXPIRATION>::type ExpirationIndex;
    typedef Set::nth_index<BY_ID>::type IdIndex;

    Set _timeouts;
    uint64 _next_id;
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_TIMEOUT_QUEUE_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
