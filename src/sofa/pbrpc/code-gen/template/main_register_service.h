// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_MAIN_REGISTER_SERVICE_H_
#define _SOFA_PBRPC_MAIN_REGISTER_SERVICE_H_

namespace sofa {
namespace pbrpc {
namespace code_gen {

static const char* main_register_service = 
    "\n    $SERVICE_NAME$* $VAR_NAME$ = new $CLASS_NAME$();"
    "\n    if (!rpc_server.RegisterService($VAR_NAME$)) {"
    "\n        SLOG(ERROR, \"export service failed\");"
    "\n        return EXIT_FAILURE;"
    "\n    }"
    "\n";
}
}
}

#endif
