// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_HTTP_H_
#define _SOFA_PBRPC_HTTP_H_

#include <map>
#include <string>

#include <sofa/pbrpc/buffer.h>

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

    Type type;                                                      // method in http header

    const std::map<std::string, std::string>* headers;              // http request headers

    const std::map<std::string, std::string>* query_params;         // query parameters 
                                                                    // http://www.baidu.com/s?k=123
                                                                    // will be parsed to {"k":"123"}

    std::string path;                                               // PATH field in http request
                                                                    // for example, http://www.baidu.com/s?k=123
                                                                    // path is "/s"

    std::string ip;                                                 // server ip adddress

    uint16_t port;                                                  // server port

    ReadBufferPtr body;                                             // the body field describes HTTP post body
                                                                    // using body->ToString() to get the std::string

    HTTPRequest() : type(GET)
                  , headers(NULL)
                  , query_params(NULL)
                  , path() 
                  , ip()
                  , port(0)
                  , body()
    { }
};

/**
 * @brief http response to client or web browser
 */
struct HTTPResponse
{
    std::string status_line;                                        // HTTP server status line, reference to RFC2616
                                                                    // default value is 200 OK, means this request dealed 
                                                                    // normally
                                                                    
    std::string content_type;                                       // content-type field in http response header
                                                                    // default is "text/html", return a plain text to 
                                                                    // http client

    WriteBufferPtr content;                                         // page content will return to http client 
                                                                    // which maybe a web browser
                                                                    // using content->Append(std::string) to set response body
                                                            
    HTTPResponse() : status_line("HTTP/1.1 200 OK")
                   , content_type("text/html; charset=UTF-8")
                   , content(new WriteBuffer()) 
    { }
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_HTTP_H_
