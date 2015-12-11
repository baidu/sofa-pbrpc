// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_HTTP_H_
#define _SOFA_PBRPC_HTTP_H_

#include <map>
#include <string>

namespace sofa {
namespace pbrpc {

struct HTTPRequest
{
    enum Type
    {
        GET = 1,
        POST = 2
    };

    Type type;

    std::map<std::string, std::string> headers;

    std::map<std::string, std::string> query_params;

    std::string path;

    std::string body;

    std::string decoded_path;

    HTTPRequest() : type(GET)
                  , headers()
                  , query_params()
                  , path() 
                  , body()
                  , decoded_path()
    { }
};

struct HTTPResponse
{
    std::string html;

    HTTPResponse() : html()
    { }
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_HTTP_H_
