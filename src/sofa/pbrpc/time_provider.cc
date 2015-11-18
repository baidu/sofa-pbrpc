// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <boost/date_time/posix_time/posix_time.hpp>
#include <sofa/pbrpc/time_provider.h>

namespace sofa {
namespace pbrpc {

pthread_once_t TimeProvider::_init_once = PTHREAD_ONCE_INIT;
TimeProvider* TimeProvider::_instance = NULL;
pthread_t TimeProvider::_tid = 0;

TimeProvider* TimeProvider::Instance()
{
    pthread_once(&_init_once, &TimeProvider::Init);
    return _instance;
}

TimeProvider::TimeProvider() 
    : _time_index(0)
    , _start(0)
{
    PTime now = boost::posix_time::microsec_clock::universal_time();
    _time[0] = now;
    _time[1] = now;
}

TimeProvider::~TimeProvider()
{
    if (_start == 1)
    {
        _start = 0;
        pthread_join(TimeProvider::_tid, NULL);
    }
}

void TimeProvider::Init()
{
    _instance = new TimeProvider();
    atexit(Destroy);

    int ret = pthread_create(
        &TimeProvider::_tid, NULL, &TimeProvider::ThreadFunc, _instance);
    assert(ret == 0);
}

void TimeProvider::Destroy()
{
    delete _instance;
    _instance = NULL;
}

PTime TimeProvider::GetNow()
{
    int index = _time_index;
    return _time[index];
}

void TimeProvider::Run()
{
    _start = 1;
    while (_start)
    {
        PTime& pt = _time[!_time_index];
        pt = boost::posix_time::microsec_clock::universal_time();
        _time_index = !_time_index;
        usleep(1);
    }
}

} // namespace pbrpc
} // namespace sofa
