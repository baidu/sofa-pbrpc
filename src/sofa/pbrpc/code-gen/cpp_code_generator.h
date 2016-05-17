// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_CPP_CODE_GENERATOR_H_
#define _SOFA_PBRPC_CPP_CODE_GENERATOR_H_

#include <map>
#include <string>
#include <sofa/pbrpc/buffer.h>
#include "parser.h"
#include "code_generator.h"

namespace sofa {
namespace pbrpc {
namespace code_gen {

class CppCodeGenerator : public CodeGenerator
{
public:
    CppCodeGenerator(const std::string& proto_path, 
                     const std::string& pbrpc_path,
                     const std::string& output_path)
        : _proto_path(proto_path),
          _pbrpc_path(pbrpc_path),
          _output_path(output_path)
    {
        _parser = new Parser(_proto_path);
    }

    ~CppCodeGenerator() 
    {
        delete _parser;
        _parser = NULL;
    }

    /**
     * @brief Generate rpc sever & client code
     *
     * @return 
     */
    int Generate();

private:
    enum Type 
    {
        HEADER = 0,
        BODY = 1
    };

    int GenerateMethod(const std::string& svc_full_name, 
                       Type type,
                       std::string& method_content);

    int GenerateService(const std::string& full_name);

    int GenerateMakefile();

    std::string Render(const std::map<std::string, std::string>& variants, 
                       const char* form, 
                       const char delimiter = '$');

    int DumpFile(const std::string& filename, const std::string& content);

    std::string GetClassName(const std::string& svc_full_name);

    std::string GetFileName(const std::string& svc_full_name, Type type);

    std::string GetRegisterService(const std::string& svc_full_name);

    std::string GetIncludeFiles(const std::string& svc_full_name);

private:
    Parser* _parser;

    std::string _proto_path;
    std::string _pbrpc_path;
    std::string _output_path;
};

}
}
}

#endif
