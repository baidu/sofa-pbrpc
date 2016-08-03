// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <limits.h>
#include <stdio.h>
#include <unistd.h>
#include <fstream>
#include <dirent.h>
#include <set>
#include <sofa/pbrpc/profiling.h>
#include <sofa/pbrpc/pprof_perl.h>
#include <sofa/pbrpc/viz_min_js.h>
#include <sofa/pbrpc/closure.h>
#include <sofa/pbrpc/string_utils.h>

bool PROFILING_LINKER_FALSE = false;
static const std::string CPU_PROFILING_PATH = "/rpc_profiling/cpu/";
static const std::string HEAP_PROFILING_PATH = "/rpc_profiling/heap/";

namespace sofa {
namespace pbrpc {

extern "C" 
{
    int __attribute__((weak)) ProfilerStart(const char* fname);
    void __attribute__((weak)) ProfilerStop();
    void __attribute__((weak)) HeapProfilerStart(const char* prefix);
    void __attribute__((weak)) HeapProfilerStop();
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

static void ListFile(const std::string& path, std::set<std::string>& files)
{
    struct dirent* dir_entry;
    DIR* dir = opendir(path.c_str());
    if (dir == NULL)
    {
        return;
    }
    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (dir_entry->d_type == DT_REG)
        {
            files.insert(dir_entry->d_name);
        }
    }
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
    if (!IsFileExist(_dir.path + CPU_PROFILING_PATH))
    {
        bool ret = Mkdir(_dir.path + CPU_PROFILING_PATH);
        if (ret == false)
        {
            return;
        }
    }
    struct timeval tval;
    gettimeofday(&tval, NULL);
    std::string path = _dir.path
        + CPU_PROFILING_PATH + "tmp."
        + StringUtils::uint64_to_string(tval.tv_sec)
        + ".prof";
    ProfilerStart(path.c_str());
    sleep(10);
    ProfilerStop();
    _is_cpu_profiling = false;
}

void Profiling::MemoryProfilingFunc()
{
    // TODO
    if (!IsFileExist(_dir.path + HEAP_PROFILING_PATH))
    {
        bool ret = Mkdir(_dir.path + HEAP_PROFILING_PATH);
        if (ret == false)
        {
            return;
        }
    }
    std::string prefix = _dir.path + HEAP_PROFILING_PATH + "tmp";
    HeapProfilerStart(prefix.c_str());
    sleep(5);
    HeapProfilerStop();
    _is_mem_profiling = false;
}

Profiling::Profiling() 
: _is_cpu_profiling(false)
    , _is_mem_profiling(false)
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

    if (!Mkdir(_dir.path + "/rpc_profiling"))
    {
        return -1;
    }
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
                                     DataType data_type,
                                     std::string& prof_file,
                                     std::string& prof_base)
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
                std::string cmd = "perl " + _dir.path + "/rpc_profiling/pprof.perl --dot " 
                         + _dir.path + "/" + _dir.name + " " + _dir.path 
                         + CPU_PROFILING_PATH + prof_file;
                if (!prof_base.empty())
                {
                    cmd.append(" --base " + _dir.path + CPU_PROFILING_PATH + prof_base);
                }
                std::string dot = exec(cmd);
                oss << dot;
                return oss.str();
            }
            else if (data_type == CLEANUP)
            {
                std::set<std::string> prof_files;
                ListFile(_dir.path + CPU_PROFILING_PATH, prof_files);
                for (std::set<std::string>::iterator it = prof_files.begin();
                        it != prof_files.end(); ++it)
                {
                    std::string profiling_path(_dir.path + CPU_PROFILING_PATH + *it);
                    ::remove(profiling_path.c_str());
                }
                oss << ShowResult(profiling_type, prof_file, prof_base); 
            }
            else if (data_type == DIFF)
            {
                oss << ShowResult(profiling_type, prof_file, prof_base); 
            }
            else
            {
                Status ret = DoCpuProfiling(data_type, prof_file);
                if (ret == DISABLE)
                {
                    oss << "<h2>To enable profiling, please add compile and link options when compiling binary:</h2>";
                    oss << "<ol>";
                    oss << "<li>Install gperftools:</li>";
                    oss << "<ul>";
                    oss << "<li>For Ubuntu: sudo apt-get install libgoogle-perftools-dev</li>";
                    oss << "<li>For CentOS: sudo yum install google-perftools-devel</li>";
                    oss << "</ul>";
                    oss << "<li>Add '-DSOFA_PBRPC_CPU_PROFILING' to CXXFLAGS</li>";
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
                    oss << ShowResult(profiling_type, prof_file, prof_base);
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
            if (data_type == GRAPH)
            {
                oss.str("");
                oss.clear();
                std::string cmd = "perl " + _dir.path + "/rpc_profiling/pprof.perl --dot " 
                         + _dir.path + "/" + _dir.name + " " + _dir.path 
                         + HEAP_PROFILING_PATH + prof_file;
                if (!prof_base.empty())
                {
                    cmd.append(" --base " + _dir.path + HEAP_PROFILING_PATH + prof_base);
                }
                std::string dot = exec(cmd);
                oss << dot;
                return oss.str();
            }
            else if (data_type == CLEANUP)
            {
                std::set<std::string> prof_files;
                ListFile(_dir.path + HEAP_PROFILING_PATH, prof_files);
                for (std::set<std::string>::iterator it = prof_files.begin();
                        it != prof_files.end(); ++it)
                {
                    std::string profiling_path(_dir.path + HEAP_PROFILING_PATH + *it);
                    ::remove(profiling_path.c_str());
                }
                oss << ShowResult(profiling_type, prof_file, prof_base); 
            }
            else if (data_type == DIFF)
            {
                oss << ShowResult(profiling_type, prof_file, prof_base); 
            }
            else
            {
                Status ret = DoMemoryProfiling(data_type, prof_file);
                if (ret == DISABLE)
                {
                    oss << "<h2>To enable memory profiling, please add compile and link options when compiling binary:</h2>";
                    oss << "<ol>";
                    oss << "<li>Install gperftools:</li>";
                    oss << "<ul>";
                    oss << "<li>For Ubuntu: sudo apt-get install libgoogle-perftools-dev</li>";
                    oss << "<li>For CentOS: sudo yum install google-perftools-devel</li>";
                    oss << "</ul>";
                    oss << "<li>Add '-DSOFA_PBRPC_HEAP_PROFILING' to CXXFLAGS</li>";
                    oss << "<li>Add '-ltcmalloc_and_profiler' to LDFLAGS</li>";
                    oss << "</ol>";
                }
                else if (ret == PROFILING)
                {
                    oss << "<h2>Profiling is in processing, please wait ...</h2>";
                    oss << "<script>setTimeout(function(){"
                           "window.location.href='/profiling?memory=page';}, 1000);</script>";
                }
                else if (ret == FINISHED)
                {
                    oss << ShowResult(profiling_type, prof_file, prof_base);
                }
                else
                {
                    oss << "<h2>Profiling is starting, please wait ...</h2>";
                    oss << "<script>setTimeout(function(){"
                           "window.location.href='/profiling?memory=page';}, 1000);</script>";
                }
            }
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

Profiling::Status Profiling::DoCpuProfiling(DataType data_type, std::string& prof_file)
{
    if (ProfilerStart == NULL)
    {
        return DISABLE;
    }

    if (_is_cpu_profiling == true)
    {
        return PROFILING;
    }

    std::set<std::string> prof_files;
    ListFile(_dir.path + CPU_PROFILING_PATH, prof_files);

    if (prof_file == "default" && !prof_files.empty() && data_type != NEW_GRAPH)
    {
        prof_file = *(prof_files.rbegin());
        return FINISHED;
    }
    _is_cpu_profiling = true;
    google::protobuf::Closure* done = 
        sofa::pbrpc::NewClosure(Profiling::Instance(), 
                                &Profiling::CpuProfilingFunc);
    _profiling_thread_group->post(done);
    return OK;
}

std::string Profiling::ShowResult(ProfilingType profiling_type, 
        const std::string& prof_file, const std::string& prof_base)
{
    std::ostringstream oss;
    std::string path = _dir.path + "/rpc_profiling/pprof.perl";
    if (!IsFileExist(path))
    {
        std::ofstream ofs(path.c_str());
        ofs << pprof_perl;
        ofs.close();
    }

    std::string prof_type;
    std::string prof_path;
    if (profiling_type == CPU)
    {
        prof_type = "cpu";
        prof_path = _dir.path + CPU_PROFILING_PATH;
    }
    else if (profiling_type == MEMORY)
    {
        prof_type = "memory";
        prof_path = _dir.path + HEAP_PROFILING_PATH;
    }
    else
    {
        prof_type = "";
    }

    std::set<std::string> prof_files;
    ListFile(prof_path, prof_files);

    oss << "<head>";
    oss << "<script src='http://apps.bdimg.com/libs/jquery/2.1.4/jquery.min.js'></script>";
    oss << "<script>";
    oss << viz_min_js;
    oss << "</script>";
    oss << "<script type=\"text/javascript\">\n";
    oss << "function onProfChanged(obj) {\n";
    oss << "  window.location.href = "
        << "'/profiling?' + obj.title + '=page&prof=' + obj.value;\n";
    oss << "}\n";
    oss << "function onDiffChanged(obj) {\n";
    oss << "  var prof_file = document.getElementById('view_" << prof_type << "').value;";
    oss << "  window.location.href = " 
        << "'/profiling?' + obj.title + '=diff&prof=' + prof_file + '&base=' + obj.value;\n";
    oss << "}\n";
    oss << "</script>";
    oss << "</head>";

    oss << "<div><a href='/profiling?" << prof_type 
        << "=newgraph'>start new " << prof_type << " profiling</a></div>";

    oss << "<div><a href='/profiling?" << prof_type 
        << "=cleanup'>remove all " << prof_type << " profiling</a></div>";

    oss << "<div>exec path:[" << _dir.path << "]</div>";
    oss << "<div>exec binary:[" << _dir.name << "]</div>";

    oss << "<p></p>";
    oss << "<pre style='display:inline'>View </pre>";
    oss << "<select id=view_" << prof_type 
        << " title=" << prof_type << " onchange='onProfChanged(this)'>";
    oss << "<option value=''> profiler file</option>";
    for (std::set<std::string>::iterator it = prof_files.begin(); 
            it != prof_files.end(); ++it)
    {
        oss << "<option value='" << *it << "' "; 
        if (prof_file == *it)
        {
            oss << "selected";
        }
        oss << ">" << *it;
    }
    oss << "</select>";
    oss << "<pre style='display:inline'>Diff </pre>";
    oss << "<select id=diff_" << prof_type 
        << " title=" << prof_type << " onchange='onDiffChanged(this)'>";
    oss << "<option value=''> profiler file</option>";
    for (std::set<std::string>::iterator it = prof_files.begin(); 
            it != prof_files.end(); ++it)
    {
        if (prof_file == *it)
        {
            continue;
        }
        oss << "<option value='" << *it << "' ";
        if (prof_base == *it)
        {
            oss << "selected";
        }
        oss << ">" << *it;
    }
    oss << "</select>";

    oss << "<div id='result'></div>";
    oss << "<script>";
    oss << "$.ajax({ type:'GET', url:'/profiling?" << prof_type << "=graph&prof=" << prof_file;
    if (!prof_base.empty())
    {
        oss << "&base=" << prof_base;
    }
    oss << "', ";
    oss << "success:function(data){";
    oss << "$('#result').html(Viz(data, \"svg\"));";
    oss << "}});";
    oss << "</script>";

    return oss.str();
}

Profiling::Status Profiling::DoMemoryProfiling(DataType data_type, std::string& prof_file)
{
    // TODO
    if (HeapProfilerStart == NULL)
    {
        return DISABLE;
    }

    if (_is_mem_profiling == true)
    {
        return PROFILING;
    }

    std::set<std::string> prof_files;
    ListFile(_dir.path + HEAP_PROFILING_PATH, prof_files);

    if (prof_file == "default" && !prof_files.empty() && data_type != NEW_GRAPH)
    {
        prof_file = *(prof_files.rbegin());
        return FINISHED;
    }
    _is_mem_profiling = true;
    google::protobuf::Closure* done = 
        sofa::pbrpc::NewClosure(Profiling::Instance(), 
                                &Profiling::MemoryProfilingFunc);
    _profiling_thread_group->post(done);
    return OK;
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
