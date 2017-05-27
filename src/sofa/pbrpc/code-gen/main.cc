// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <cstdio>
#include "cpp_code_generator.h"

int main(int argc, const char *argv[])
{
    if (argc < 4)
    {
        fprintf(stderr, 
                "%s [proto_file] [pbrpc_root_path] [output_dir]\n", argv[0]);
        exit(0);
    }

    sofa::pbrpc::code_gen::CodeGenerator* gen 
        = new sofa::pbrpc::code_gen::CppCodeGenerator(argv[1],
                                                      argv[2],
                                                      argv[3]);
    gen->Generate();
    delete gen;
    return 0;
}
