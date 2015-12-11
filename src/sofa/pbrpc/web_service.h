// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_WEB_SERVICE_H_
#define _SOFA_PBRPC_WEB_SERVICE_H_

#include <sofa/pbrpc/common_internal.h>
#include <sofa/pbrpc/ext_closure.h>
#include <sofa/pbrpc/rpc_request.h>

namespace sofa {
namespace pbrpc {

class HTTPRequest;
class HTTPResponse;
typedef ExtClosure<bool(const HTTPRequest&, HTTPResponse&)>* MethodType;

class WebService
{
public:
    void RegisterMethod(MethodType method);

    bool Dispatch(
        const RpcServerStreamWPtr& server_stream,
        const RpcRequestPtr& rpc_request);

private:
   MethodType _method; 
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_WEB_SERVICE_H_
