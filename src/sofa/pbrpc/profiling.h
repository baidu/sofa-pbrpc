// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_PROFILING_H_
#define _SOFA_PBRPC_PROFILING_H_

#include <string>
#include <map>

#include <sofa/pbrpc/http.h>
#include <sofa/pbrpc/thread_group_impl.h>

namespace sofa {
namespace pbrpc {

class Profiling
{
public:
    enum ProfilingType
    {
        DEFAULT = 1,
        CPU = 2, 
        MEMORY = 4,
    };

    enum OperationType
    {
        PAGE = 1,
        GRAPH = 2,
        NEW_GRAPH = 3,
        DIFF = 4,
        CLEANUP = 5
    };

    typedef std::map<std::string, OperationType> OperationMap;

    enum Status
    {
        OK = 1,
        PROFILING = 2, 
        DISABLE = 3,
        FINISHED = 4
    };

    int Init();

    static Profiling* Instance();

    OperationType FindOperationType(const std::string& operation_type);

    std::string ProfilingPage(ProfilingType profiling_type, 
                              OperationType operation_type,
                              std::string& profiling_file,
                              std::string& profiling_base);

    Status DoCpuProfiling(OperationType operation_type,
                          std::string& profiling_file);

    Status DoMemoryProfiling(OperationType operation_type,
                             std::string& profiling_file);

private:
    Profiling();

    ~Profiling();

    static void InitProfiling();

    static void DestroyProfiling();

    void InitOperationMap();

    void CpuProfilingFunc();

    void MemoryProfilingFunc();

    std::string ShowResult(ProfilingType profiling_type,
                           const std::string& profiling_file,
                           const std::string& profiling_base);

    struct EXEDir
    {
        std::string path;
        std::string name;
    };

private:
    volatile bool _is_cpu_profiling;

    volatile bool _is_mem_profiling;

    volatile bool _is_initialized;

    ThreadGroupImplPtr _profiling_thread_group;

    EXEDir _dir;

    static pthread_once_t _init_once;

    static Profiling* _instance;

    OperationMap _operation_map;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(Profiling);
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_PROFILING_H_
