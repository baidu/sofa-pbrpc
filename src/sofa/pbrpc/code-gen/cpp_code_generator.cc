// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <map>
#include <fstream>
#include <stdlib.h>
#include <sofa/pbrpc/buffer.h>
#include <sofa/pbrpc/string_utils.h>
#include <google/protobuf/io/printer.h>
#include "cpp_code_generator.h"
#include "common.h"
#include "parser.h"
#include "template/service.h.h"
#include "template/service.cc.h"
#include "template/method.h.h"
#include "template/method.cc.h"
#include "template/main_register_service.h"
#include "template/main_include_files.h"
#include "template/main.cc.h"
#include "template/Makefile.h"

namespace sofa {
namespace pbrpc {
namespace code_gen {
CppCodeGenerator::CppCodeGenerator(const std::string& proto_path, 
                                   const std::string& pbrpc_path,
                                   const std::string& output_path)
    : _proto_path(proto_path),
      _pbrpc_path(pbrpc_path),
      _output_path(output_path)
    {
        _parser = new Parser(_proto_path);
    }

CppCodeGenerator::~CppCodeGenerator() 
{
    delete _parser;
    _parser = NULL;
}

int CppCodeGenerator::Generate()
{
    int ret = _parser->Init();
    if (ret != 0)
    {
        LOG_WARN("init parser error");
        return -1;
    }

    Parser::Variants services;
    ret = _parser->ParseService(services);
    if (ret != 0)
    {
        return -1;
    }

    LOG_INFO("start generate service");
    std::string register_service;
    std::string include_files;
    for (size_t i = 0; i < services.size(); ++i)
    {
        ret = GenerateService(services[i]);
        if (ret != 0)
        {
            LOG_WARN("deal service[%s] error", services[i].c_str());
            continue;
        }
        register_service.append(GetRegisterService(services[i]));
        include_files.append(GetIncludeFiles(services[i]));
    }
    LOG_INFO("generate service finish");

    LOG_INFO("start generate Makefile");
    ret = GenerateMakefile();
    if (ret != 0)
    {
        LOG_WARN("generate makefile error");
        return -1;
    }

    LOG_INFO("generate Makefile finish");

    LOG_INFO("start generate main.cc");
    std::map<std::string, std::string> main_vars;
    main_vars["REGISTER_SERVICE"] = register_service;
    main_vars["INCLUDE_FILES"] = include_files;
    DumpFile("main.cc", Render(main_vars, main_cc));
    LOG_INFO("generate main.cc finish");
    return 0;
}

std::string CppCodeGenerator::Render(
    const std::map<std::string, std::string>& variants, 
    const char* form, 
    const char delimiter)
{
    sofa::pbrpc::WriteBufferPtr write_buffer(new sofa::pbrpc::WriteBuffer());
    google::protobuf::io::Printer* printer = 
        new google::protobuf::io::Printer(write_buffer.get(), delimiter);
    printer->Print(variants, form);
    delete printer;
    sofa::pbrpc::ReadBufferPtr read_buffer(new sofa::pbrpc::ReadBuffer());
    write_buffer->SwapOut(read_buffer.get());
    return read_buffer->ToString();
}

int CppCodeGenerator::GenerateMethod(const std::string& svc_full_name, 
                                     Type type,
                                     std::string& method_content)
{
    Parser::Variants methods;
    int ret = _parser->ParseServiceMethod(svc_full_name, methods);
    if (ret != 0)
    {
        LOG_WARN("parse service[%s] error", svc_full_name.c_str());
        return -1;
    }

    for (size_t i = 0; i < methods.size(); ++i)
    {
        std::map<std::string, std::string> method_vars;
        method_vars["METHOD_NAME"] = 
            _parser->GetMethodShortName(svc_full_name, i);
        Parser::Variants iotype;
        ret = _parser->ParseMethodIOType(methods[i], iotype);
        if (ret != 0 || iotype.size() != 2)
        {
            return -1;
        }
        method_vars["INPUT_TYPE"] = 
            sofa::pbrpc::StringUtils::replace_all(iotype[0], ".", "::");
        method_vars["OUTPUT_TYPE"] = 
            sofa::pbrpc::StringUtils::replace_all(iotype[1], ".", "::");
        if (type == HEADER)
        {
            const std::string& content = 
                Render(method_vars, method_h);
            method_content.append(content);
        }
        else
        {
            method_vars["CLASS_NAME"] = GetClassName(svc_full_name);
            const std::string& content = 
                Render(method_vars, method_cc);
            method_content.append(content);
        }
    }
    return 0;
}

int CppCodeGenerator::GenerateService(const std::string& svc_full_name)
{
    std::map<std::string, std::string> service_vars;
    std::string svc_name = 
        sofa::pbrpc::StringUtils::replace_all(svc_full_name, ".", "::");
    service_vars["SERVICE_NAME"] = svc_name;
    const std::string& class_name = GetClassName(svc_full_name);
    LOG_INFO("start generate class[%s] declaration", class_name.c_str());
    service_vars["CLASS_NAME"] = class_name;
    service_vars["PB_FILE_NAME"] = _parser->GetProtoFileShortName();
    std::string method_content;
    int ret = GenerateMethod(svc_full_name, HEADER, method_content);
    if (ret != 0)
    {
        return ret;
    }
    service_vars["METHODS"] = method_content;
    const std::string& svc_header = Render(service_vars, service_h);
    const std::string& header_file = GetFileName(svc_full_name, HEADER);
    DumpFile(header_file, svc_header);
    
    LOG_INFO("start generate class[%s] implementation", class_name.c_str());
    service_vars.clear();
    service_vars["INCLUDE"] = header_file;
    method_content.clear();
    ret = GenerateMethod(svc_full_name, BODY, method_content);
    if (ret != 0)
    {
        return ret;
    }
    service_vars["METHODS"] = method_content;
    const std::string& svc_body = Render(service_vars, service_cc);
    DumpFile(GetFileName(svc_full_name, BODY), svc_body);
    return 0;
}

int CppCodeGenerator::GenerateMakefile()
{
    std::vector<std::string> dependency;
    _parser->GetDependency(dependency);
    std::string protos;
    for (size_t i = 0; i < dependency.size(); ++i)
    {
        protos.append(dependency[i] + " ");
    }
    protos.append(_parser->GetProtoFileName());
    std::map<std::string, std::string> vars;
    vars["PROTOS"] = protos;
    char resolved_path[PATH_MAX];
    char* ret = realpath(_pbrpc_path.c_str(), resolved_path);
    if (ret == NULL)
    {
        LOG_WARN("resolve pbrpc path[%s] error", _pbrpc_path.c_str());
        return -1;
    }
    vars["PBRPC_ROOT"] = resolved_path;
    return DumpFile("Makefile", Render(vars, Makefile, '&'));
}

std::string CppCodeGenerator::GetRegisterService(const std::string& full_name)
{
    std::map<std::string, std::string> vars;
    std::string svc_name = 
        sofa::pbrpc::StringUtils::replace_all(full_name, ".", "::");
    vars["SERVICE_NAME"] = svc_name;
    const std::string& class_name = GetClassName(full_name);
    LOG_INFO("start generate class[%s] declaration", class_name.c_str());
    vars["CLASS_NAME"] = class_name;
    vars["VAR_NAME"] = sofa::pbrpc::StringUtils::to_lower(class_name);
    return Render(vars, main_register_service);
}

int CppCodeGenerator::DumpFile(const std::string& filename, 
                               const std::string& content)
{
    bool is_exist = IsFileExist(_output_path);
    if (!is_exist)
    {
        bool ret = Mkdir(_output_path);
        if (!ret)
        {
            LOG_WARN("create path[%s] error\n", _output_path.c_str());
            return -1;
        }
    }
    const std::string& file = _output_path + "/" + filename;
    std::ofstream ofs(file.c_str());
    if (!ofs.is_open())
    {
        LOG_WARN("open file[%s] error", file.c_str());
        return -1;
    }
    ofs << content;
    ofs.close();
    return 0;
}

std::string CppCodeGenerator::GetClassName(const std::string& svc_name)
{
    std::string class_name = _parser->GetServiceShortName(svc_name);
    LOG_INFO("class name[%s], svc_name[%s]", 
             class_name.c_str(), svc_name.c_str());
    class_name.append("Impl");
    return class_name;
}

std::string CppCodeGenerator::GetFileName(const std::string& svc_name, Type type)
{
    return type == HEADER ? GetClassName(svc_name) + ".h" : 
        GetClassName(svc_name) + ".cc";
}

std::string CppCodeGenerator::GetIncludeFiles(
    const std::string& svc_full_name)
{
    std::map<std::string, std::string> files;
    files["FILE_NAME"] = GetFileName(svc_full_name, HEADER);
    return Render(files, main_include_files);
}

}
}
}
