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

/**
 * @brief request from http client, include http headers, query_params, and so
 * on.
 */
struct HTTPRequest
{
    enum Type
    {
        GET = 1,
        POST = 2
    };

    Type type;                                              // method in http header

    std::map<std::string, std::string> headers;             // http request headers

    std::map<std::string, std::string> query_params;        // query parameters 
                                                            // http://www.baidu.com/s?k=123
                                                            // will be parsed to {"k":"123"}

    std::string path;                                       // PATH field in http request
                                                            // for example, http://www.baidu.com/s?k=123
                                                            // path is "/s"

    std::string body;                                       // the body field describes HTTP post body

    HTTPRequest() : type(GET)
                  , headers()
                  , query_params()
                  , path() 
                  , body()
    { }
};

/**
 * @brief http response to client or web browser
 */
struct HTTPResponse
{
    std::string content;                                    // page content will return to http client 
                                                            // which maybe a web browser

    std::string host;                                       // server host, auto filled by web service

    std::string ip;                                         // server ip address, auto filled by web service

    uint32_t port;                                          // server port, auto filled by web service

    std::string content_type;                               // content-type field in http response header
                                                            // default is "text/html", return a plain text to 
                                                            // http client

    std::string status_line;                                // HTTP server status line, reference to RFC2616
                                                            // default value is 200 OK, means this request dealed 
                                                            // normally

    HTTPResponse() : content()
                   , host()
                   , ip()
                   , port(0)
                   , content_type("text/html; charset=UTF-8")
                   , status_line("HTTP/1.1 200 OK")
    { }
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_HTTP_H_
