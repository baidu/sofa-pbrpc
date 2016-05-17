// Copyright (c) 2016 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: shichengyi@baidu.com (Shi Chengyi)

#ifndef _SOFA_PBRPC_CODE_GENERATOR_COMMON_H_
#define _SOFA_PBRPC_CODE_GENERATOR_COMMON_H_

#include <sys/stat.h>
#include <sys/types.h>
#include <errno.h>
#include <unistd.h>

#define LOG_INFO(format, ...) do { \
    fprintf(stdout, "[INFO %s:%d]  ", __FILE__, __LINE__); \
    fprintf(stdout, format, ##__VA_ARGS__); \
    fprintf(stdout, "\n"); \
} while (0)

#define LOG_WARN(format, ...) do { \
    fprintf(stderr, "[WARNING %s:%d]  ", __FILE__, __LINE__); \
    fprintf(stderr, format, ##__VA_ARGS__); \
    fprintf(stderr, "\n"); \
} while (0)

namespace 
{

bool Mkdir(const std::string& path)
{
    const int mode = 0777;
    int ret = ::mkdir(path.c_str(), mode);
    if (ret == 0 || errno == EEXIST)
    {
        return true;
    }

    LOG_WARN("Mkdir(): create dir {%s}:%s", 
             path.c_str(), strerror(errno));
    return false;
}

bool IsFileExist(const std::string& path)
{
    std::ifstream ifs(path.c_str());
    if (ifs.good())
    {
        ifs.close();
        return true;
    }
    else
    {
        ifs.close();
        return false;
    }
}

}

#endif
