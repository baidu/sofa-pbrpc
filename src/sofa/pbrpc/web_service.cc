// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#include <sofa/pbrpc/web_service.h>
#include <sofa/pbrpc/http.h>
#include <sofa/pbrpc/http_rpc_request.h>
#include <sofa/pbrpc/rpc_server_stream.h>

namespace sofa {
namespace pbrpc {

void WebService::RegisterMethod(MethodType method)
{
    _method = method;
}

bool WebService::Dispatch(
    const RpcServerStreamWPtr& server_stream,
    const RpcRequestPtr& rpc_request)
{

    const HTTPRpcRequest* http_rpc_request = 
        static_cast<const HTTPRpcRequest*>(rpc_request.get());

    HTTPRequest request;
    request.path = http_rpc_request->_path;
    request.body = http_rpc_request->_req_body->ToString();
    request.headers = http_rpc_request->_headers;
    request.query_params = http_rpc_request->_query_params;
    request.decoded_path = http_rpc_request->_decoded_path;
    HTTPResponse response;
    bool ret = false;
    if (_method)
    {
        ret = _method->Run(request, response);
        if (ret)
        {
            const_cast<HTTPRpcRequest*>(http_rpc_request)->SendPage(
                server_stream, response.html);
        }
    }
    return ret;
}

} // namespace pbrpc
} // namespace sofa
