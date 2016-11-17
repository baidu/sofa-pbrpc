// Copyright (c) 2015 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/io_service_pool.h>

namespace sofa {
namespace pbrpc {

IOServicePool::IOServicePool(std::size_t pool_size, 
        size_t pool_thread_num) 
    : _next_service(0)
    , _init_func(NULL)
    , _dest_func(NULL)
{
    assert(pool_size > 0);

    for (size_t i = 0; i < pool_size; ++i)
    {
        ThreadGroupImplPtr service(new ThreadGroupImpl(
                pool_thread_num, "io_service worker thread"));
        _pool.push_back(service);
    }
}

bool IOServicePool::Run()
{
    size_t pool_size = _pool.size();
    for (size_t i = 0; i < pool_size; ++i)
    {
        _pool[i]->set_init_func(_init_func);
        _pool[i]->set_dest_func(_dest_func);
        if (!_pool[i]->start())
        {
#if defined( LOG )
            LOG(ERROR) << "Start(): start work thread group failed";
#else
            SLOG(ERROR, "Start(): start work thread group failed");
#endif
            return false;
        }
    }
    return true;
}

void IOServicePool::Stop()
{
    size_t pool_size = _pool.size();
    for (size_t i = 0; i < pool_size; ++i)
    {
        _pool[i]->stop();
        _pool[i].reset();
    }
}

IOService& IOServicePool::GetIOService()
{
    IOService& io_service = _pool[_next_service]->io_service();
    ++_next_service;
    if (_next_service == _pool.size())
    {
        _next_service = 0;
    }
    return io_service;
}

} // namespace pbrpc
} // namespace sofa
