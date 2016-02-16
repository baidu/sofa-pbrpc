#ifndef SOFA_PBRPC_PROFILING_LINKER_H
#define SOFA_PBRPC_PROFILING_LINKER_H

#if defined(SOFA_PBRPC_PROFILING)

#include <gperftools/profiler.h>

static void touch_gperftools_profiler()
{
    SCHECK(false);
    ProfilerStart("function_never_run");
    ProfilerStop();
}

static void touch_profiler() 
{
    SCHECK(false);
    touch_gperftools_profiler();
}

#endif

#endif
