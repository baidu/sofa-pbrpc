// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#include <sofa/pbrpc/timeout_manager.h>
#include <sofa/pbrpc/timeout_manager_impl.h>

namespace sofa {
namespace pbrpc {

TimeoutManager::TimeoutManager()
{
    _imp.reset(new TimeoutManagerImpl());
    _imp->start();
}

TimeoutManager::~TimeoutManager()
{
    _imp->stop();
    _imp.reset();
}

void TimeoutManager::clear()
{
    _imp->clear();
}

TimeoutManager::Id TimeoutManager::add(int64 interval, Callback* callback)
{
    return _imp->add(interval, callback);
}

TimeoutManager::Id TimeoutManager::add_repeating(int64 interval, Callback* callback)
{
    return _imp->add_repeating(interval, callback);
}

bool TimeoutManager::erase(Id id)
{
    return _imp->erase(id);
}

} // namespace pbrpc
} // namespace sofa

/* vim: set ts=4 sw=4 sts=4 tw=100 */
