// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_STRING_UTILS_H_
#define _SOFA_PBRPC_STRING_UTILS_H_

#include <string>

namespace sofa {
namespace pbrpc {

int CEscapeString(const char* src, int src_len, char* dest, int dest_len);

std::string CEscapeString(const char* src, int src_len);

std::string CEscapeString(const std::string& src);

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_STRING_UTILS_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
