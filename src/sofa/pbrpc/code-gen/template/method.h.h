// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef SOFA_PBRPC_METHOD_H_H
#define SOFA_PBRPC_METHOD_H_H

namespace sofa {
namespace pbrpc {
namespace code_gen {

static const char* method_h = 
    "\n    virtual void $METHOD_NAME$(google::protobuf::RpcController* controller,"
    "\n                      const $INPUT_TYPE$* request,"
    "\n                      $OUTPUT_TYPE$* response,"
    "\n                      google::protobuf::Closure* done);"
    "\n";

}
}
}

#endif
