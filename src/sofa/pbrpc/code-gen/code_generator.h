// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_CODE_GENERATOR_H_
#define _SOFA_PBRPC_CODE_GENERATOR_H_

namespace sofa {
namespace pbrpc {
namespace code_gen {

class CodeGenerator 
{
public:
    CodeGenerator() { }
    virtual ~CodeGenerator() { }

    /**
     * @brief Generate rpc sever & client code
     *
     * @return 
     */
    virtual int Generate() = 0;

};

}
}
}

#endif
