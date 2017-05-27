// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_SERVICE_H_H_
#define _SOFA_PBRPC_SERVICE_H_H_

namespace sofa {
namespace pbrpc {
namespace code_gen {

static const char* service_h = 
    "#ifndef TEMPLATE_$CLASS_NAME$_H"
    "\n#define TEMPLATE_$CLASS_NAME$_H"
    "\n"
    "\n#include \"$PB_FILE_NAME$.pb.h\""
    "\n"
    "\nclass $CLASS_NAME$ : public $SERVICE_NAME$"
    "\n{"
    "\npublic:"
    "\n    $CLASS_NAME$() {}"
    "\n    virtual ~$CLASS_NAME$() {}"
    "\n"
    "\nprivate:"
    "$METHODS$"
    "\n};"
    "\n"
    "\n#endif"
    "\n"
    "/* vim: set ts=4 sw=4 sts=4 tw=100 */"
    "\n";
}
}
}

#endif
