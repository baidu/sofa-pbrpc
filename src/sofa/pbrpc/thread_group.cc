// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/thread_group.h>
#include <sofa/pbrpc/thread_group_impl.h>

namespace sofa {
namespace pbrpc {

ThreadGroup::ThreadGroup(int thread_num)
{
    _imp.reset(new ThreadGroupImpl(thread_num));
    _imp->start();
}

ThreadGroup::~ThreadGroup()
{
    _imp->stop();
    _imp.reset();
}

int ThreadGroup::thread_num() const
{
    return _imp->thread_num();
}

void ThreadGroup::dispatch(google::protobuf::Closure* handle)
{
    _imp->dispatch(handle);
}

void ThreadGroup::post(google::protobuf::Closure* handle)
{
    _imp->post(handle);
}

void ThreadGroup::dispatch(ExtClosure<void()>* handle)
{
    _imp->dispatch(handle);
}

void ThreadGroup::post(ExtClosure<void()>* handle)
{
    _imp->post(handle);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
