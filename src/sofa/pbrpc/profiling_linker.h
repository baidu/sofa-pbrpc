#ifndef SOFA_PBRPC_PROFILING_LINKER_H
#define SOFA_PBRPC_PROFILING_LINKER_H

#if defined(SOFA_PBRPC_CPU_PROFILING)
#include <gperftools/profiler.h>
#endif // SOFA_PBRPC_CPU_PROFILING

#if defined(SOFA_PBRPC_HEAP_PROFILING)
#include <gperftools/heap-profiler.h>
#endif // SOFA_PBRPC_HEAP_PROFILING

extern bool PROFILING_LINKER_FALSE;

class ProfilingLinker
{
public:
    ProfilingLinker()
    {
        // make libprofiler be linked
        if (PROFILING_LINKER_FALSE != false)
        {
#if defined(SOFA_PBRPC_CPU_PROFILING)
            ProfilerStart(NULL);
#endif // SOFA_PBRPC_CPU_PROFILING

#if defined(SOFA_PBRPC_HEAP_PROFILING)
            HeapProfilerStart(NULL);
#endif // SOFA_PBRPC_HEAP_PROFILING
        }
    }
};

#endif // SOFA_PBRPC_PROFILING_LINKER_H
