// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_PROFILING_H_
#define _SOFA_PBRPC_PROFILING_H_

#include <string>
#include <boost/atomic.hpp>
#include <sofa/pbrpc/http.h>
#include <sofa/pbrpc/thread_group_impl.h>

namespace sofa {
namespace pbrpc {

class Profiling
{
public:

    enum Type
    {
        DEFAULT = 1, 
        CPU = 2, 
        MEMORY = 3,
        GRAPH = 4
    };

    enum ErrorCode
    {
        OK = 1,
        PROFILING = 2, 
        DISABLE = 3,
        FINISHED = 4
    };

    std::string ProfilingPage(Type type);

    ErrorCode DoCpuProfiling();

    int DoMemoryProfiling();

    static Profiling* Instance();

    int Init();

private:
    Profiling();

    ~Profiling();

    void CpuProfilingFunc();

    void MemoryProfilingFunc();

    std::string ShowResult();

    static void InitProfiling();
    
    static void DestroyProfiling();

private:
    boost::atomic<bool> _is_profiling;

    boost::atomic<bool> _is_initialized;

    ThreadGroupImplPtr _profiling_thread_group;

    std::string _self_path;

    static pthread_once_t _init_once;

    static Profiling* _instance;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(Profiling);
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_WEB_SERVICE_H_
