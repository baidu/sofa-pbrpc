// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_MAIN_CC_H_
#define _SOFA_PBRPC_MAIN_CC_H_

namespace sofa {
namespace pbrpc {
namespace code_gen {

static const char* main_cc = 
    "#include <signal.h>"
    "\n#include <unistd.h>"
    "\n#include <sofa/pbrpc/pbrpc.h>"
    "$INCLUDE_FILES$"
    "\n"
    "\nbool WebServlet(const sofa::pbrpc::HTTPRequest& request, sofa::pbrpc::HTTPResponse& response)"
    "\n{"
    "\n    return response.content->Append(\"<h1>Hello from sofa-pbrpc web server</h1>\");"
    "\n}"
    "\n"
    "\nbool thread_init_func()"
    "\n{"
    "\n    sleep(1);"
    "\n    SLOG(INFO, \"Init work thread succeed\");"
    "\n    return true;"
    "\n}"
    "\n"
    "\nvoid thread_dest_func()"
    "\n{"
    "\n    SLOG(INFO, \"Destroy work thread succeed\");"
    "\n}"
    "\n"
    "\nint main()"
    "\n{"
    "\n    SOFA_PBRPC_SET_LOG_LEVEL(NOTICE);"
    "\n"
    "\n    // Define an rpc server."
    "\n    sofa::pbrpc::RpcServerOptions options;"
    "\n    options.work_thread_init_func = sofa::pbrpc::NewPermanentExtClosure(&thread_init_func);"
    "\n    options.work_thread_dest_func = sofa::pbrpc::NewPermanentExtClosure(&thread_dest_func);"
    "\n    sofa::pbrpc::RpcServer rpc_server(options);"
    "\n"
    "\n    sofa::pbrpc::Servlet servlet = sofa::pbrpc::NewPermanentExtClosure(&WebServlet);"
    "\n    rpc_server.RegisterWebServlet(\"/hello\", servlet);"
    "\n"
    "\n    // Start rpc server."
    "\n    if (!rpc_server.Start(\"0.0.0.0:12321\")) {"
    "\n        SLOG(ERROR, \"start server failed\");"
    "\n        return EXIT_FAILURE;"
    "\n    }"
    "\n    "
    "\n    // Register service."
    "$REGISTER_SERVICE$"
    "\n    // Wait signal."
    "\n    rpc_server.Run();"
    "\n"
    "\n    // Stop rpc server."
    "\n    rpc_server.Stop();"
    "\n"
    "\n    // Delete closures."
    "\n    // Attention: should delete the closures after server stopped, or may be crash."
    "\n    delete options.work_thread_init_func;"
    "\n    delete options.work_thread_dest_func;"
    "\n"
    "\n    return EXIT_SUCCESS;"
    "\n}"
    "\n"
    "\n"
    "/* vim: set ts=4 sw=4 sts=4 tw=100 */"
    "\n";
}
}
}

#endif
