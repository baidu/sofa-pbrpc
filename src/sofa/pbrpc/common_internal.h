// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef _SOFA_PBRPC_COMMON_INTERNAL_H_
#define _SOFA_PBRPC_COMMON_INTERNAL_H_

#include <boost/bind.hpp>
#include <boost/function.hpp>

#include <sofa/pbrpc/common.h>
#include <sofa/pbrpc/atomic.h>
#include <sofa/pbrpc/counter.h>
#include <sofa/pbrpc/func_tracer.h>
#include <sofa/pbrpc/io_service.h>
#include <sofa/pbrpc/locks.h>
#include <sofa/pbrpc/ptime.h>

namespace sofa {
namespace pbrpc {

class RpcControllerImpl;
typedef sofa::pbrpc::shared_ptr<RpcControllerImpl> RpcControllerImplPtr;
typedef sofa::pbrpc::weak_ptr<RpcControllerImpl> RpcControllerImplWPtr;

class RpcChannelImpl;
typedef sofa::pbrpc::shared_ptr<RpcChannelImpl> RpcChannelImplPtr;

class RpcClientImpl;
typedef sofa::pbrpc::shared_ptr<RpcClientImpl> RpcClientImplPtr;

class RpcServerImpl;
typedef sofa::pbrpc::shared_ptr<RpcServerImpl> RpcServerImplPtr;
typedef sofa::pbrpc::weak_ptr<RpcServerImpl> RpcServerImplWPtr;

class RpcClientStream;
typedef sofa::pbrpc::shared_ptr<RpcClientStream> RpcClientStreamPtr;
typedef sofa::pbrpc::weak_ptr<RpcClientStream> RpcClientStreamWPtr;

class RpcServerStream;
typedef sofa::pbrpc::shared_ptr<RpcServerStream> RpcServerStreamPtr;
typedef sofa::pbrpc::weak_ptr<RpcServerStream> RpcServerStreamWPtr;

class RpcListener;
typedef sofa::pbrpc::shared_ptr<RpcListener> RpcListenerPtr;

class TimerWorker;
typedef sofa::pbrpc::shared_ptr<TimerWorker> TimerWorkerPtr;

class RpcTimeoutManager;
typedef sofa::pbrpc::shared_ptr<RpcTimeoutManager> RpcTimeoutManagerPtr;

class ThreadGroup;
typedef sofa::pbrpc::shared_ptr<ThreadGroup> ThreadGroupPtr;

class ServicePool;
typedef sofa::pbrpc::shared_ptr<ServicePool> ServicePoolPtr;
typedef sofa::pbrpc::weak_ptr<ServicePool> ServicePoolWPtr;

class FlowController;
typedef sofa::pbrpc::shared_ptr<FlowController> FlowControllerPtr;

class WaitEvent;
typedef sofa::pbrpc::shared_ptr<WaitEvent> WaitEventPtr;

class IOServicePool;
typedef sofa::pbrpc::shared_ptr<IOServicePool> IOServicePoolPtr;

class WebService;
typedef sofa::pbrpc::shared_ptr<WebService> WebServicePtr;

#define SOFA_PBRPC_DECLARE_RESOURCE_COUNTER(name_) \
    extern sofa::pbrpc::AtomicCounter g_sofa_counter_##name_
#define SOFA_PBRPC_DEFINE_RESOURCE_COUNTER(name_) \
    sofa::pbrpc::AtomicCounter g_sofa_counter_##name_(0)
#define SOFA_PBRPC_INC_RESOURCE_COUNTER(name_) \
    ++g_sofa_counter_##name_
#define SOFA_PBRPC_DEC_RESOURCE_COUNTER(name_) \
    --g_sofa_counter_##name_
#define SOFA_PBRPC_GET_RESOURCE_COUNTER(name_) \
    static_cast<int>(g_sofa_counter_##name_)

SOFA_PBRPC_DECLARE_RESOURCE_COUNTER(RpcByteStream);
SOFA_PBRPC_DECLARE_RESOURCE_COUNTER(RpcListener);

// Use for affecting global/static variables' construct/destruct order.
inline void touch_boost_error_category()
{
    (void)boost::system::system_category();
    (void)boost::system::generic_category();
    (void)boost::asio::error::get_addrinfo_category();
    (void)boost::asio::error::get_misc_category();
    (void)boost::asio::error::get_netdb_category();
}

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_COMMON_INTERNAL_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
