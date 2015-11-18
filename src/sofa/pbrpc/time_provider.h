// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_TIME_PROVIDER_H_
#define _SOFA_PBRPC_TIME_PROVIDER_H_

#include <sofa/pbrpc/common.h>

namespace sofa {
namespace pbrpc {

typedef boost::posix_time::ptime PTime;

class TimeProvider
{
public:
    static TimeProvider* Instance();

    PTime GetNow();

private:
    TimeProvider();

    ~TimeProvider();

    static void Init();

    static void Destroy();

    static void* ThreadFunc(void* arg)
    {
        static_cast<TimeProvider*>(arg)->Run();
        return NULL;
    }

    void Run();

private:
    static pthread_once_t _init_once;

    static TimeProvider* _instance;

    static pthread_t _tid;

    PTime _time[2];
    
    volatile int _time_index;

    volatile int _start;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(TimeProvider);
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_TIME_PROVIDER_H_
