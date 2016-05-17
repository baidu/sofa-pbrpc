// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <cstdio>
#include <vector>
#include <google/protobuf/descriptor.h>
#include <google/protobuf/compiler/importer.h>
#include <sofa/pbrpc/string_utils.h>
#include "parser.h"
#include "common.h"

namespace sofa {
namespace pbrpc {
namespace code_gen {

Parser::~Parser()
{
    delete _importer;
    _importer = NULL;
}

int Parser::Init()
{
    using namespace ::google::protobuf;
    using namespace ::google::protobuf::compiler;
    if (_init)
    {
        return 0;
    }
    DiskSourceTree source_tree;
    source_tree.MapPath("", _path);
    _importer = new Importer(&source_tree, NULL);
    _importer->Import(_filename);
    _pool = _importer->pool();
    if (_pool == NULL)
    {
        LOG_WARN("import proto[%s/%s] error\n", 
                 _path.c_str(), _filename.c_str());
        return -1;
    }

    _file_desc = _pool->FindFileByName(_filename);
    if (_file_desc == NULL)
    {
        LOG_WARN("parse proto[%s/%s] error\n", 
                 _path.c_str(), _filename.c_str());
        return -1;
    }

    _init = true;
    return 0;
}

int Parser::ParseService(Variants& services)
{
    using namespace ::google::protobuf;
    if (_file_desc == NULL)
    {
        LOG_WARN("%s/%s\n", _path.c_str(), _filename.c_str());
        return -1;
    }

    if (_file_desc->service_count() == 0)
    {
        LOG_WARN("WARNING:no service exist in proto[%s/%s]\n", 
                 _path.c_str(), _filename.c_str());
        return 0;
    }

    for (int i = 0; i < _file_desc->service_count(); ++i)
    {
        const ServiceDescriptor* svc_desc = _file_desc->service(i);
        services.push_back(svc_desc->full_name());
    }
    return 0;
}

int Parser::ParseServiceMethod(const std::string& svc_name, Variants& methods)
{
    using namespace ::google::protobuf;
    const ServiceDescriptor* svc_desc = _pool->FindServiceByName(svc_name);
    if (svc_desc == NULL)
    {
        return -1;
    }
    for (int i = 0; i < svc_desc->method_count(); ++i)
    {
        methods.push_back(svc_desc->method(i)->full_name());
    }
    return 0;
}

int Parser::ParseMethodIOType(const std::string& method_name, Variants& iotype)
{
    using namespace ::google::protobuf;
    const MethodDescriptor* method_desc = 
        _pool->FindMethodByName(method_name);
    if (method_desc == NULL)
    {
        return -1;
    }
    iotype.push_back(method_desc->input_type()->full_name());
    iotype.push_back(method_desc->output_type()->full_name());
    return 0;
}

std::string Parser::GetServiceShortName(const std::string& svc_name)
{
    using namespace ::google::protobuf;
    const ServiceDescriptor* svc_desc = _pool->FindServiceByName(svc_name);
    if (svc_desc == NULL)
    {
        return "";
    }
    return svc_desc->name();
}

std::string Parser::GetMethodShortName(const std::string& svc_name, const int index)
{
    using namespace ::google::protobuf;
    const ServiceDescriptor* svc_desc = _pool->FindServiceByName(svc_name);
    if (svc_desc == NULL || index >= svc_desc->method_count())
    {
        return "";
    }
    return svc_desc->method(index)->name();
}

std::string Parser::GetProtoFileName()
{
    return _filename;
}

std::string Parser::GetProtoFileShortName()
{
    std::vector<std::string> tokens;
    sofa::pbrpc::StringUtils::split(_filename, ".", &tokens);
    return tokens[0];
}

void Parser::GetDependency(std::vector<std::string>& dependency)
{
    for (int i = 0; i < _file_desc->dependency_count(); ++i)
    {
        dependency.push_back(_file_desc->dependency(i)->name());
    }
}

}
}
}
