// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef SOFA_PBRPC_SERVICE_CC_H
#define SOFA_PBRPC_SERVICE_CC_H

namespace sofa {
namespace pbrpc {
namespace code_gen {

static const char* service_cc = 
    "#include \"$INCLUDE$\""
    "\n"
    "$METHODS$"
    "\n"
    "\n"
    "/* vim: set ts=4 sw=4 sts=4 tw=100 */"
    "\n";
}
}
}

#endif
