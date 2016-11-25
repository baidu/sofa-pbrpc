// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_TIMER_WORKER_H_
#define _SOFA_PBRPC_TIMER_WORKER_H_

#include <sofa/pbrpc/common_internal.h>

namespace sofa {
namespace pbrpc {

class TimerWorker : public sofa::pbrpc::enable_shared_from_this<TimerWorker>
{
public:
    typedef boost::function<void(const PTime& /* now */)> WorkRoutine;

public:
    TimerWorker(IOService& io_service)
        : _io_service(io_service)
        , _is_running(false)
        , _time_duration(time_duration_seconds(1))
        , _work_routine(NULL)
        , _timer(io_service)
        , _strand(io_service)
    {}

    ~TimerWorker()
    {
        SOFA_PBRPC_FUNCTION_TRACE;
        stop();
    }

    bool is_running()
    {
        return _is_running;
    }

    void set_time_duration(const TimeDuration& time_duration)
    {
        _time_duration = time_duration;
    }

    void set_work_routine(const WorkRoutine& work_routine)
    {
        _work_routine = work_routine;
    }

    void start()
    {
        if (_is_running) return;
        _is_running = true;

        ScopedLocker<MutexLock> _(_timer_lock);
        _timer.expires_from_now(_time_duration);
        _timer.async_wait(_strand.wrap(boost::bind(
                &TimerWorker::on_timeout, shared_from_this(), _1)));
    }

    void stop()
    {
        if (!_is_running) return;
        _is_running = false;

        ScopedLocker<MutexLock> _(_timer_lock);
        _timer.cancel();
    }

private:
    void on_timeout(const boost::system::error_code& ec)
    {
        if (_is_running)
        {
            PTime now = ptime_now();

            if (ec != boost::asio::error::operation_aborted && _work_routine)
            {
                _work_routine(now);
            }

            ScopedLocker<MutexLock> _(_timer_lock);
            _timer.expires_at(now + _time_duration);
            _timer.async_wait(_strand.wrap(boost::bind(
                            &TimerWorker::on_timeout, shared_from_this(), _1)));
        }
    }

private:
    IOService& _io_service;
    volatile bool _is_running;

    TimeDuration _time_duration;
    WorkRoutine _work_routine;

    IOServiceTimer _timer;
    MutexLock _timer_lock;
    IOServiceStrand _strand;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(TimerWorker);
}; // class TimerWorker

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_TIMER_WORKER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
