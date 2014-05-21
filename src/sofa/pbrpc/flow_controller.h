// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

#ifndef _SOFA_PBRPC_FLOW_CONTROLLER_H_
#define _SOFA_PBRPC_FLOW_CONTROLLER_H_

#include <sofa/pbrpc/common_internal.h>

namespace sofa {
namespace pbrpc {

class FlowController
{
public:
    FlowController(bool read_no_limit, int read_quota,
                   bool write_no_limit, int write_quota)
        : _read_no_limit(read_no_limit), _read_quota(read_quota)
        , _write_no_limit(write_no_limit), _write_quota(write_quota) {}

    ~FlowController() {}

    // Reset read quota.
    // @param read_no_limit  if set as no limit.
    // @param quota  the new quota, only useful when read_no_limit is false.
    void reset_read_quota(bool read_no_limit, int quota)
    {
        _read_no_limit = read_no_limit;
        _read_quota = quota;
    }

    // Reset read quota.
    // @param quota  the new quota.
    void reset_read_quota(int quota)
    {
        _read_quota = quota;
    }

    // Reset write quota.
    // @param write_no_limit  if set as no limit.
    // @param quota  the new quota, only useful when write_no_limit is false.
    void reset_write_quota(bool write_no_limit, int quota)
    {
        _write_no_limit = write_no_limit;
        _write_quota = quota;
    }

    // Reset write quota.
    // @param quota  the new quota.
    void reset_write_quota(int quota)
    {
        _write_quota = quota;
    }

    // Check if has more read quota.
    bool has_read_quota() const
    {
        return _read_no_limit || _read_quota > 0;
    }

    // Check if has more write quota.
    bool has_write_quota() const
    {
        return _write_no_limit || _write_quota > 0;
    }

    // Acquire some read quota.
    // @param quota  the quota expect to acquire.
    // @return >0  if acquire succeed.
    // @return <=0 if acquire failed, and the return value can be used as sequence number
    //             to sort trigger order: closer to zero, earlier to trigger.
    int acquire_read_quota(int quota)
    {
        return _read_no_limit ? 1 : atomic_add_ret_old(&_read_quota, -quota);
    }

    // Acquire some write quota.
    // @param quota  the quota expect to acquire.
    // @return >0  if acquire succeed.
    // @return <=0 if acquire failed, and the return value can be used as sequence number
    //             to sort trigger order: closer to zero, earlier to trigger.
    int acquire_write_quota(int quota)
    {
        return _write_no_limit ? 1 : atomic_add_ret_old(&_write_quota, -quota);
    }

private:
    bool _read_no_limit;
    volatile int _read_quota;
    bool _write_no_limit;
    volatile int _write_quota;

    SOFA_PBRPC_DISALLOW_EVIL_CONSTRUCTORS(FlowController);
}; // class FlowController

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_FLOW_CONTROLLER_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
