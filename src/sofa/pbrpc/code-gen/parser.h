// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_PARSER_H_
#define _SOFA_PBRPC_PARSER_H_

#include <fstream>
#include <map>
#include <string>
#include <vector>

namespace google {
namespace protobuf {

class DescriptorPool;
class FileDescriptor;

namespace compiler {

class Importer;

}

}
}

namespace sofa {
namespace pbrpc {
namespace code_gen {

class Parser 
{
public:
    explicit Parser(const std::string& full_path)
        : _init(false),
          _pool(NULL),
          _file_desc(NULL),
          _importer(NULL)
    { 
        size_t found = full_path.rfind("/");
        if (found == std::string::npos)
        {
            _path = "./";
            _filename = full_path;
        }
        else
        {
            _path = full_path.substr(0, found);
            _filename = full_path.substr(found + 1);
        }
    }

    ~Parser();

     int Init();

     typedef std::vector<std::string> Variants;

     int ParseService(Variants& services);

     int ParseServiceMethod(const std::string& svc_name, Variants& methods);

     int ParseMethodIOType(const std::string& method_name, Variants& iotype);

     std::string GetServiceShortName(const std::string& svc_name);

     std::string GetMethodShortName(const std::string& svc_name, 
                                    const int index);

     std::string GetProtoFileName();

     std::string GetProtoFileShortName();

     void GetDependency(std::vector<std::string>& dependency);

private:
    bool _init;
    
    std::string _filename;
    std::string _path;

    const google::protobuf::DescriptorPool* _pool;
    const google::protobuf::FileDescriptor* _file_desc;
    
    google::protobuf::compiler::Importer* _importer;
};

}
}
}

#endif
