#ifndef SOFA_PBRPC_PROFILING_LINKER_H
#define SOFA_PBRPC_PROFILING_LINKER_H

#if defined(SOFA_PBRPC_PROFILING)
#include <gperftools/profiler.h>
#endif // SOFA_PBRPC_PROFILING

class ProfilingLinker
{
public:
    ProfilingLinker()
    {
#if defined(SOFA_PBRPC_PROFILING)
        ProfilerStart("never_run");
        ProfilerStop();
#endif // SOFA_PBRPC_PROFILING
    }
};

#endif // SOFA_PBRPC_PROFILING_LINKER_H
