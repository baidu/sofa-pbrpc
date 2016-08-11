#ifndef SOFA_PBRPC_PROFILING_LINKER_H
#define SOFA_PBRPC_PROFILING_LINKER_H

#if defined(SOFA_PBRPC_PROFILING)
#include <gperftools/profiler.h>
void TCMallocGetHeapSample(std::string* writer);
#endif // SOFA_PBRPC_PROFILING

extern bool PROFILING_LINKER_FALSE;

class ProfilingLinker
{
public:
    ProfilingLinker()
    {
        // make libprofiler be linked
        if (PROFILING_LINKER_FALSE != false)
        {
#if defined(SOFA_PBRPC_PROFILING)
            ProfilerStart(NULL);
            TCMallocGetHeapSample(NULL);
#endif // SOFA_PBRPC_PROFILING
        }
    }
};

#endif // SOFA_PBRPC_PROFILING_LINKER_H
