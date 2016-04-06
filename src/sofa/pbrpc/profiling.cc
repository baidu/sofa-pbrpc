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
    // TODO
    bool ret = Mkdir(_dir.path + "/rpc_profiling");
    if (ret == false)
    {
        return;
    }
    std::string path = _dir.path + "/rpc_profiling/tmp.prof";
    ProfilerStart(path.c_str());
    sleep(10);
    ProfilerStop();
    _is_profiling = false;
}

void Profiling::MemoryProfilingFunc()
{
    // TODO
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

    char buf[PATH_MAX];
    memset(buf, 0, PATH_MAX);
    const char* link = "/proc/self/exe";
    ssize_t r = readlink(link, buf, PATH_MAX);
    if (r == -1)
    {
        return -1;
    }

    // TODO
    std::string path(buf);
    int pos = path.rfind("/");
    if (pos < 0)
    {
        return -1;
    }
    _dir.path = path.substr(0, pos);
    _dir.name = path.substr(pos + 1, path.size());

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

std::string Profiling::ProfilingPage(ProfilingType profiling_type, 
                                     DataType data_type)
{
    std::ostringstream oss;
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

    switch (profiling_type) 
    {
        case CPU:
            if (data_type == GRAPH)
            {
                oss.str("");
                oss.clear();
                std::string dot = 
                    exec("perl " + _dir.path + "/rpc_profiling/pprof.perl --dot " 
                         + _dir.path + "/"+ _dir.name + " " + _dir.path 
                         + "/rpc_profiling/tmp.prof");
                oss << dot;
                return oss.str();
            }
            else
            {
                Status ret = DoCpuProfiling(data_type);
                if (ret == DISABLE)
                {
                    oss << "<h2>To enable profiling, please add compile and link options when compiling binary:</h2>";
                    oss << "<ol>";
                    oss << "<li>Install gperftools:</li>";
                    oss << "<ul>";
                    oss << "<li>For Ubuntu: sudo apt-get install libgoogle-perftools-dev</li>";
                    oss << "<li>For CentOS: sudo yum install google-perftools-devel</li>";
                    oss << "</ul>";
                    oss << "<li>Add '-DSOFA_PBRPC_PROFILING' to CXXFLAGS</li>";
                    oss << "<li>Add '-lprofiler' to LDFLAGS</li>";
                    oss << "</ol>";
                }
                else if (ret == PROFILING)
                {
                    oss << "<h2>Profiling is in processing, please wait ...</h2>";
                    oss << "<script>setTimeout(function(){"
                           "window.location.href='/profiling?cpu=page';}, 1000);</script>";
                }
                else if (ret == FINISHED)
                {
                    oss << ShowResult();
                }
                else
                {
                    oss << "<h2>Profiling is starting, please wait ...</h2>";
                    oss << "<script>setTimeout(function(){"
                           "window.location.href='/profiling?cpu=page';}, 1000);</script>";
                }
            }
            break;
        case MEMORY:
            // TODO
            oss << "<h2>Memory profiling not supported now</h2>";
            break;
        default:
            oss << "<div>";
            oss << "<a href=\"/profiling?cpu=page\">cpu</a>";
            oss << "<br>";
            oss << "<a href=\"/profiling?memory=page\">memory</a>";
            oss << "</div>";
            break;
    }
    oss << "</html>";
    return oss.str();
}

Profiling::Status Profiling::DoCpuProfiling(DataType data_type)
{
    if (ProfilerStart == NULL)
    {
        return DISABLE;
    }

    if (_is_profiling == true)
    {
        return PROFILING;
    }

    if (data_type == NEW_GRAPH)
    {
        std::string profiling_path(_dir.path + "/rpc_profiling/tmp.prof");
        ::remove(profiling_path.c_str());
    }

    if (IsFileExist(_dir.path + "/rpc_profiling/tmp.prof"))
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
    std::string path = _dir.path + "/rpc_profiling/pprof.perl";
    if (!IsFileExist(path))
    {
        std::ofstream ofs(path.c_str());
        ofs << pprof_perl;
        ofs.close();
    }

    oss << "<head>";
    oss << "<script src='http://apps.bdimg.com/libs/jquery/2.1.4/jquery.min.js'></script>";
    oss << "<script>";
    oss << viz_min_js;
    oss << "</script>";
    oss << "</head>";

    oss << "<div><a href='/profiling?cpu=newgraph'>start new cpu profiling</a></div>";
    oss << "<div>exec path:[" << _dir.path << "]</div>";
    oss << "<div>exec binary:[" << _dir.name << "]</div>";

    oss << "<div id='result'></div>";
    oss << "<script>";
    oss << "$.ajax({ type:'GET', url:'/profiling?cpu=graph', success:function(data){";
    oss << "$('#result').html(Viz(data, \"svg\"));";
    oss << "}});";
    oss << "</script>";

    return oss.str();
}

int Profiling::DoMemoryProfiling()
{
    // TODO
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
