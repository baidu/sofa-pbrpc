// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <sofa/pbrpc/profiling.h>
#include <sofa/pbrpc/pprof_perl.h>
#include <sofa/pbrpc/viz_min_js.h>
#include <sofa/pbrpc/closure.h>

namespace sofa {
namespace pbrpc {

extern "C" 
{
    int __attribute__((weak)) ProfilerStart(const char* fname);
    void __attribute__((weak)) ProfilerStop();
}

pthread_once_t Profiling::_init_once = PTHREAD_ONCE_INIT;
Profiling* Profiling::_instance = NULL;

static bool Mkdir(const std::string& path)
{
    const int mode = 0777;
    int ret = ::mkdir(path.c_str(), mode);
    if (ret == 0 || errno == EEXIST)
    {
        return true;
    }

#if defined( LOG )
    LOG(ERROR) << "Mkdir(): create dir {" 
        << path << "}:" << strerror(errno);
#else
    SLOG(ERROR, "Mkdir(): create dir {%s}:%s", 
         path.c_str(), strerror(errno));
#endif
    return false;
}

static bool IsFileExist(const std::string& path)
{
    std::ifstream ifs(path.c_str());
    if (ifs.good())
    {
        ifs.close();
        return true;
    }
    else
    {
        ifs.close();
        return false;
    }
}

static std::string exec(const std::string& cmd)
{
    FILE* pipe = popen(cmd.c_str(), "r");
    if (!pipe) return "ERROR";
    char buffer[128];
    std::string result = "";
    while (!feof(pipe)) 
    {
        if (fgets(buffer, 128, pipe) != NULL)
            result += buffer;
    }
    pclose(pipe);
    return result;
}

void Profiling::CpuProfilingFunc()
{
    bool ret = Mkdir("./rpc_profiling");
    if (ret == false)
    {
        return;
    }
    ProfilerStart("./rpc_profiling/test.prof");
    sleep(10);
    ProfilerStop();
    _is_profiling = false;
}

void Profiling::MemoryProfilingFunc()
{
    bool ret = Mkdir("./rpc_profiling");
    if (ret == false)
    {
        return;
    }
    ProfilerStart("./rpc_profiling/test.prof");
    // TODO
    sleep(10);
    ProfilerStop();
}

Profiling::Profiling() 
: _is_profiling(false)
    , _is_initialized(false)
{
    _profiling_thread_group.reset(new ThreadGroupImpl(
            1, "sofa_pbrpc_profiling_thread_group"));
}

int Profiling::Init()
{
    if (_is_initialized)
    {
        return 0;
    }

    if (!_profiling_thread_group->start())
    {
        _profiling_thread_group.reset();
        return -1;
    }

    // _self_path.resize(PATH_MAX);
    char buf[PATH_MAX];
    const char* link = "/proc/self/exe";
    readlink(link, buf, PATH_MAX);
    _self_path = buf;
    _is_initialized = true;
    return 0;
}

Profiling::~Profiling()
{
    if (_is_initialized)
    {
        _profiling_thread_group->stop();
        _profiling_thread_group.reset();
    }
}

std::string Profiling::ProfilingPage(Type type)
{
    std::ostringstream oss;
    // TODO transfer request response
    if (type == GRAPH)
    {
        // TODO perl exist
        std::string dot = exec("perl ./rpc_profiling/pprof.perl --dot " + _self_path + " ./rpc_profiling/test.prof");
        oss << dot;
        return oss.str();
    }

    oss << "<html><h1>Sofa-pbrpc Profiling</h1>";

    if (!_is_initialized)
    {
        int ret = Init();
        if (ret < 0)
        {
            oss << "<h2>init profiling thread error</h2></html>";
            return oss.str();
        }
    }

    switch (type) 
    {
        case CPU:
            int ret = DoCpuProfiling();
            if (ret == DISABLE)
            {
                oss << "<h2>To enable profiling, link profiler and add -DSOFA_PBRPC_PROFILING to CXXFLAGS</h2>";
            }
            else if (ret == PROFILING)
            {
                oss << "<h2>other profiling in processing, please wait a minute</h2>";
            }
            else if (ret == FINISHED)
            {
                oss << ShowResult();
            }
            else
            {
                oss << "<h2>do cpu profiling</h2>";
            }
            break;
        case MEMORY:
            oss << "<h2>do memory profiling</h2>";
            break;
        default:
            oss << "<div>";
            oss << "<a href=\"/profiling?cpu=1\">cpu</a>";
            oss << "<br>";
            oss << "<a href=\"/profiling?memory=1\">memory</a>";
            oss << "</div>";
            break;
    }
    oss << "</html>";
    return oss.str();
}

Profiling::ErrorCode Profiling::DoCpuProfiling()
{
    if (ProfilerStart == NULL)
    {
        return DISABLE;
    }

    if (_is_profiling == true)
    {
        return PROFILING;
    }

    if (IsFileExist("./rpc_profiling/test.prof"))
    {
        return FINISHED;
    }

    _is_profiling = true;
    google::protobuf::Closure* done = 
        sofa::pbrpc::NewClosure(Profiling::Instance(), 
                                &Profiling::CpuProfilingFunc);
    _profiling_thread_group->post(done);
    return OK;
}

std::string Profiling::ShowResult()
{
    std::ostringstream oss;
    if (!IsFileExist("./rpc_profiling/pprof.perl"))
    {
        std::ofstream ofs("./rpc_profiling/pprof.perl");
        ofs << pprof_perl;
        ofs.close();
    }

    oss << "<head>";
    oss << "<script src='http://apps.bdimg.com/libs/jquery/2.1.4/jquery.min.js'></script>";
    oss << "<script>";
    oss << viz_min_js;
    oss << "</script>";
    oss << "</head>";

    oss << "<div> exec path:[" << _self_path << "]</div>";

    // std::string dot = exec("perl ./rpc_profiling/pprof.perl --dot " + _self_path + " ./rpc_profiling/test.prof");

    // dot = StringUtils::replace_all(dot, "\n", "");

    oss << "<div id='result'></div>";
    oss << "<script>";
    oss << "$.ajax({ type:'GET', url:'/profiling?cpu=4', success:function(data){";
    oss << "$('#result').html(Viz(data, \"svg\"));";
    oss << "}});";
    oss << "</script>";

    return oss.str();
}

int Profiling::DoMemoryProfiling()
{
    return 0;
}

Profiling* Profiling::Instance()
{
    pthread_once(&_init_once, &Profiling::InitProfiling);
    return _instance;
}

void Profiling::InitProfiling()
{
    _instance = new Profiling();
    ::atexit(DestroyProfiling);
}

void Profiling::DestroyProfiling()
{
    delete _instance;
    _instance = NULL;
}

} // namespace pbrpc
} // namespace sofa