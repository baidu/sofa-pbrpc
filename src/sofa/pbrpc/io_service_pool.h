// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_BINARY_IO_SERVICE_POOL_H_
#define _SOFA_PBRPC_BINARY_IO_SERVICE_POOL_H_

#include <vector>

#include <sofa/pbrpc/thread_group_impl.h>

namespace sofa {
namespace pbrpc {

class IOServicePool
{
public:
    IOServicePool(std::size_t pool_size, std::size_t pool_thread_num);

    bool Run();

    void Stop();

    IOService& GetIOService();

    void set_init_func(ThreadInitFunc init_func)
    {
        _init_func = init_func;
    }

    void set_dest_func(ThreadDestFunc dest_func)
    {
        _dest_func = dest_func;
    }

private:

    std::vector<ThreadGroupImplPtr> _pool;

    size_t _next_service;

    ThreadInitFunc _init_func;
    ThreadDestFunc _dest_func;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(IOServicePool);
};

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_BINARY_IO_SERVICE_POOL_H_
