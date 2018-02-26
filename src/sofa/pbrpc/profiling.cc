// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

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

#ifndef SOFA_PBRPC_PROFILING_SAMPLE_TIME
#define SOFA_PBRPC_PROFILING_SAMPLE_TIME 10
#endif

bool PROFILING_LINKER_FALSE = false;

#if defined(SOFA_PBRPC_PROFILING)
void __attribute__((weak)) TCMallocGetHeapSample(std::string* writer);
#endif // SOFA_PBRPC_PROFILING

namespace sofa {
namespace pbrpc {

static const std::string CPU_PROFILING_PATH = "/rpc_profiling/cpu/";
static const std::string MEMORY_PROFILING_PATH = "/rpc_profiling/memory/";

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

static void ListFile(const std::string& path, std::set<std::string>& files)
{
    struct dirent* dir_entry;
    DIR* dir = opendir(path.c_str());
    if (dir == NULL)
    {
#if defined( LOG )
        LOG(ERROR) << "ListFile(): open dir {"
            << path << "} failed: " << strerror(errno);
#else
        SLOG(ERROR, "ListFile(): open dir {%s} failed: %s",
            path.c_str(), strerror(errno));
#endif
        return;
    }
    while ((dir_entry = readdir(dir)) != NULL)
    {
        if (dir_entry->d_type == DT_REG)
        {
            files.insert(dir_entry->d_name);
        }
    }
    closedir(dir);
}

static bool WriteFile(const std::string& path, const char* buffer, size_t size)
{
    if (buffer == NULL)
    {
        return false;
    }
    FILE* fp = fopen(path.c_str(), "w");
    if (fp == NULL)
    {
#if defined( LOG )
        LOG(ERROR) << "WriteFile(): open file {"
            << path << "} failed: "  << strerror(errno);
#else
        SLOG(ERROR, "WriteFile(): open file {%s} failed: %s",
                path.c_str(), strerror(errno));
#endif
        return false;
    }
    fwrite(buffer, size, 1, fp);
    fclose(fp);
    return true;
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

static std::string Exec(const std::string& cmd)
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

static std::string Readlink(const char* symbol_link)
{
    char buf[PATH_MAX + 1];
    ssize_t len = ::readlink(symbol_link, buf, PATH_MAX);
    if (len <= 0)
    {
        return std::string();
    }

    buf[len] = '\0';
    return std::string(buf);
}

int Profiling::Init()
{
    if (_is_initialized)
    {
        return 0;
    }

    if (!_profiling_thread_group->start())
    {
#if defined( LOG )
        LOG(ERROR) << "Init(): start profiling thread group failed";
#else
        SLOG(ERROR, "Init(): start profiling thread group failed");
#endif
        _profiling_thread_group.reset();
        return -1;
    }

    std::string path = Readlink("/proc/self/exe");
    int pos = path.rfind("/");
    if (pos < 0)
    {
#if defined( LOG )
        LOG(ERROR) << "Init(): bad executed file path: " << path;
#else
        SLOG(ERROR, "Init(): bad executed file path: %s", path.c_str());
#endif
        return -1;
    }
    _dir.path = path.substr(0, pos);
    _dir.name = path.substr(pos + 1, path.size());

    if (!Mkdir(_dir.path + "/rpc_profiling"))
    {
        return -1;
    }
    InitOperationMap();
    _is_initialized = true;
    return 0;
}

Profiling* Profiling::Instance()
{
    pthread_once(&_init_once, &Profiling::InitProfiling);
    return _instance;
}

Profiling::OperationType Profiling::FindOperationType(const std::string& operation_type)
{
    OperationMap::iterator it = _operation_map.find(operation_type);
    if (it != _operation_map.end())
    {
        return it->second;
    }
    else
    {
        return Profiling::PAGE;
    }
}

std::string Profiling::ProfilingPage(ProfilingType profiling_type, 
                                     OperationType operation_type,
                                     std::string& view_prof,
                                     std::string& base_prof)
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
            if (operation_type == GRAPH)
            {
                oss.str("");
                oss.clear();
                std::string cmd = "perl " + _dir.path + "/rpc_profiling/pprof.perl --dot "
                    + _dir.path + "/" + _dir.name + " " + _dir.path
                    + CPU_PROFILING_PATH + view_prof;
                if (!base_prof.empty())
                {
                    cmd.append(" --base " + _dir.path + CPU_PROFILING_PATH + base_prof);
                }
                std::string dot = Exec(cmd);
                oss << dot;
                return oss.str();
            }
            else if (operation_type == CLEANUP)
            {
                std::set<std::string> profiling_set;
                ListFile(_dir.path + CPU_PROFILING_PATH, profiling_set);
                for (std::set<std::string>::iterator it = profiling_set.begin();
                        it != profiling_set.end(); ++it)
                {
                    ::remove(std::string(_dir.path + CPU_PROFILING_PATH + *it).c_str());
                }
                oss << ShowResult(profiling_type, view_prof, base_prof);
            }
            else if (operation_type == DIFF)
            {
                oss << ShowResult(profiling_type, view_prof, base_prof);
            }
            else
            {
                Status ret = DoCpuProfiling(operation_type, view_prof);
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
                    oss << ShowResult(profiling_type, view_prof, base_prof);
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
            if (operation_type == GRAPH)
            {
                oss.str("");
                oss.clear();
                std::string cmd = "perl " + _dir.path + "/rpc_profiling/pprof.perl --dot "
                    + _dir.path + "/" + _dir.name + " " + _dir.path
                    + MEMORY_PROFILING_PATH + view_prof;
                if (!base_prof.empty())
                {
                    cmd.append(" --base " + _dir.path + MEMORY_PROFILING_PATH + base_prof);
                }
                std::string dot = Exec(cmd);
                oss << dot;
                return oss.str();
            }
            else if (operation_type == CLEANUP)
            {
                std::set<std::string> profiling_set;
                ListFile(_dir.path + MEMORY_PROFILING_PATH, profiling_set);
                for (std::set<std::string>::iterator it = profiling_set.begin();
                        it != profiling_set.end(); ++it)
                {
                    ::remove(std::string(_dir.path + MEMORY_PROFILING_PATH + *it).c_str());
                }
                oss << ShowResult(profiling_type, view_prof, base_prof);
            }
            else if (operation_type == DIFF)
            {
                oss << ShowResult(profiling_type, view_prof, base_prof);
            }
            else
            {
                Status ret = DoMemoryProfiling(operation_type, view_prof);
                if (ret == DISABLE)
                {
                    oss << "<h2>To enable memory profiling, please add compile and link options when compiling binary:</h2>";
                    oss << "<ol>";
                    oss << "<li>Install gperftools:</li>";
                    oss << "<ul>";
                    oss << "<li>For Ubuntu: sudo apt-get install libgoogle-perftools-dev</li>";
                    oss << "<li>For CentOS: sudo yum install google-perftools-devel</li>";
                    oss << "</ul>";
                    oss << "<li>Add '-DSOFA_PBRPC_PROFILING' to CXXFLAGS</li>";
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
                    oss << ShowResult(profiling_type, view_prof, base_prof);
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

Profiling::Status Profiling::DoCpuProfiling(OperationType operation_type,
                                            std::string& profiling_file)
{
#if defined(SOFA_PBRPC_PROFILING)

    if (_is_cpu_profiling == true)
    {
        return PROFILING;
    }

    std::set<std::string> profiling_set;
    ListFile(_dir.path + CPU_PROFILING_PATH, profiling_set);

    if (profiling_file == "default" && !profiling_set.empty() && operation_type != NEW_GRAPH)
    {
        profiling_file = *(profiling_set.rbegin());
        return FINISHED;
    }
    if (IsFileExist(_dir.path + CPU_PROFILING_PATH + profiling_file))
    {
        return FINISHED;
    }
    _is_cpu_profiling = true;
    google::protobuf::Closure* done = 
        sofa::pbrpc::NewClosure(Profiling::Instance(), 
                                &Profiling::CpuProfilingFunc);
    _profiling_thread_group->post(done);
    return OK;
#else
    return DISABLE;
#endif // SOFA_PBRPC_PROFILING
}

Profiling::Status Profiling::DoMemoryProfiling(OperationType operation_type,
                                               std::string& profiling_file)
{
#if defined(SOFA_PBRPC_PROFILING)

    if (_is_mem_profiling == true)
    {
        return PROFILING;
    }

    std::set<std::string> profiling_set;
    ListFile(_dir.path + MEMORY_PROFILING_PATH, profiling_set);

    if (profiling_file == "default" && !profiling_set.empty() && operation_type != NEW_GRAPH)
    {
        profiling_file = *(profiling_set.rbegin());
        return FINISHED;
    }
    if (IsFileExist(_dir.path + MEMORY_PROFILING_PATH + profiling_file))
    {
        return FINISHED;
    }
    _is_mem_profiling = true;
    google::protobuf::Closure* done =
        sofa::pbrpc::NewClosure(Profiling::Instance(),
                                &Profiling::MemoryProfilingFunc);
    _profiling_thread_group->post(done);
    return OK;
#else
    return DISABLE;
#endif // SOFA_PBRPC_PROFILING
}

Profiling::Profiling()
    : _is_cpu_profiling(false)
    , _is_mem_profiling(false)
    , _is_initialized(false)
{
    _profiling_thread_group.reset(new ThreadGroupImpl(
            1, "sofa_pbrpc_profiling_thread_group"));
}

Profiling::~Profiling()
{
    if (_is_initialized)
    {
        _profiling_thread_group->stop();
        _profiling_thread_group.reset();
    }
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

void Profiling::InitOperationMap()
{
    _operation_map["graph"] = GRAPH;
    _operation_map["newgraph"] = NEW_GRAPH;
    _operation_map["diff"] = DIFF;
    _operation_map["cleanup"] = CLEANUP;
}

void Profiling::CpuProfilingFunc()
{
    if (!IsFileExist(_dir.path + CPU_PROFILING_PATH))
    {
        if (!Mkdir(_dir.path + CPU_PROFILING_PATH))
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
    sleep(SOFA_PBRPC_PROFILING_SAMPLE_TIME);
    ProfilerStop();
    _is_cpu_profiling = false;
}

void Profiling::MemoryProfilingFunc()
{
#if defined(SOFA_PBRPC_PROFILING)
    if (!IsFileExist(_dir.path + MEMORY_PROFILING_PATH))
    {
        if (!Mkdir(_dir.path + MEMORY_PROFILING_PATH))
        {
            return;
        }
    }
    struct timeval tval;
    gettimeofday(&tval, NULL);
    std::string path = _dir.path
        + MEMORY_PROFILING_PATH + "tmp."
        + StringUtils::uint64_to_string(tval.tv_sec)
        + ".heap";
    std::string writer;
    TCMallocGetHeapSample(&writer);
    if (!WriteFile(path, writer.c_str(), writer.size()))
    {
#if defined( LOG )
        LOG(ERROR) << "WriteFile(): write file {" << path << "} failed";
#else
        SLOG(ERROR, "WriteFile(): write file {%s} failed", path.c_str());
#endif
        return;
    }
    _is_mem_profiling = false;
#endif // SOFA_PBRPC_PROFILING
}

std::string Profiling::ShowResult(ProfilingType profiling_type,
                                  const std::string& view_prof, 
                                  const std::string& base_prof)
{
#if defined(SOFA_PBRPC_PROFILING)
    std::ostringstream oss;
    std::string path = _dir.path + "/rpc_profiling/pprof.perl";
    if (!IsFileExist(path))
    {
        std::ofstream ofs(path.c_str());
        ofs << pprof_perl;
        ofs.close();
    }

    std::string profiling_type_str;
    std::string profiling_path;
    if (profiling_type == CPU)
    {
        profiling_type_str = "cpu";
        profiling_path = _dir.path + CPU_PROFILING_PATH;
    }
    else if (profiling_type == MEMORY)
    {
        profiling_type_str = "memory";
        profiling_path = _dir.path + MEMORY_PROFILING_PATH;
    }
    else
    {
        profiling_type_str = "";
    }

    std::set<std::string> profiling_set;
    ListFile(profiling_path, profiling_set);

    oss << "<head>";
    oss << "<script src='http://apps.bdimg.com/libs/jquery/2.1.4/jquery.min.js'></script>";
    oss << "<script>";
    oss << viz_min_js;
    oss << "</script>";
    oss << "<script type=\"text/javascript\">\n";
    oss << "function onViewChanged(obj) {\n"
        << "  window.location.href = "
        << "'/profiling?' + obj.title + '=page&prof=' + obj.value;\n"
        << "}\n";
    oss << "function onDiffChanged(obj) {\n"
        << "  var view_prof = document.getElementById('view_" << profiling_type_str << "').value;\n"
        << "  window.location.href = "
        << "'/profiling?' + obj.title + '=diff&prof=' + view_prof + '&base=' + obj.value;\n"
        << "}\n";
    oss << "</script>";
    oss << "</head>";

    oss << "<div><a href='/profiling?" << profiling_type_str
        << "=newgraph'>start new " << profiling_type_str << " profiling</a></div>";

    oss << "<div><a href='/profiling?" << profiling_type_str
        << "=cleanup'>remove all " << profiling_type_str << " profiling</a></div>";

    oss << "<div>exec path:[" << _dir.path << "]</div>";
    oss << "<div>exec binary:[" << _dir.name << "]</div>";

    oss << "<p></p>";
    oss << "<pre style='display:inline'>View </pre>";
    oss << "<select id=view_" << profiling_type_str
        << " title=" << profiling_type_str << " onchange='onViewChanged(this)'>";
    oss << "<option value=''> profiler file</option>";
    for (std::set<std::string>::iterator it = profiling_set.begin();
            it != profiling_set.end(); ++it)
    {
        oss << "<option value='" << *it << "' ";
        if (view_prof == *it)
        {
            oss << "selected";
        }
        oss << ">" << *it;
    }
    oss << "</select>";
    oss << "<pre style='display:inline'>Diff </pre>";
    oss << "<select id=diff_" << profiling_type_str
        << " title=" << profiling_type_str << " onchange='onDiffChanged(this)'>";
    oss << "<option value=''> base profiler file</option>";
    for (std::set<std::string>::iterator it = profiling_set.begin();
            it != profiling_set.end(); ++it)
    {
        if (view_prof == *it)
        {
            continue;
        }
        oss << "<option value='" << *it << "' ";
        if (base_prof == *it)
        {
            oss << "selected";
        }
        oss << ">" << *it;
    }
    oss << "</select>";

    oss << "<div id='result'></div>";
    oss << "<script>";
    oss << "$.ajax({ type:'GET', url:'/profiling?" << profiling_type_str << "=graph&prof=" << view_prof;
    if (!base_prof.empty())
    {
        oss << "&base=" << base_prof;
    }
    oss << "', ";
    oss << "success:function(data){";
    oss << "$('#result').html(Viz(data, \"svg\"));";
    oss << "}});";
    oss << "</script>";

    return oss.str();
#else
    return "";
#endif // SOFA_PBRPC_PROFILING
}

} // namespace pbrpc
} // namespace sofa
