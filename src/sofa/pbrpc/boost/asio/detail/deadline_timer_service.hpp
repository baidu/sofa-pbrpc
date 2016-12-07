//
// detail/deadline_timer_service.hpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_DETAIL_DEADLINE_TIMER_SERVICE_HPP
#define SOFA_PBRPC_BOOST_ASIO_DETAIL_DEADLINE_TIMER_SERVICE_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>
#include <cstddef>
#include <sofa/pbrpc/boost/asio/error.hpp>
#include <sofa/pbrpc/boost/asio/io_service.hpp>
#include <sofa/pbrpc/boost/asio/detail/addressof.hpp>
#include <sofa/pbrpc/boost/asio/detail/bind_handler.hpp>
#include <sofa/pbrpc/boost/asio/detail/fenced_block.hpp>
#include <sofa/pbrpc/boost/asio/detail/noncopyable.hpp>
#include <sofa/pbrpc/boost/asio/detail/socket_ops.hpp>
#include <sofa/pbrpc/boost/asio/detail/socket_types.hpp>
#include <sofa/pbrpc/boost/asio/detail/timer_queue.hpp>
#include <sofa/pbrpc/boost/asio/detail/timer_scheduler.hpp>
#include <sofa/pbrpc/boost/asio/detail/wait_handler.hpp>
#include <sofa/pbrpc/boost/asio/detail/wait_op.hpp>

#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
# include <chrono>
# include <thread>
#endif // defined(BOOST_ASIO_WINDOWS_RUNTIME)

#include <sofa/pbrpc/boost/asio/detail/push_options.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {
namespace detail {

template <typename Time_Traits>
class deadline_timer_service
{
public:
  // The time type.
  typedef typename Time_Traits::time_type time_type;

  // The duration type.
  typedef typename Time_Traits::duration_type duration_type;

  // The implementation type of the timer. This type is dependent on the
  // underlying implementation of the timer service.
  struct implementation_type
    : private sofa::pbrpc::boost::asio::detail::noncopyable
  {
    time_type expiry;
    bool might_have_pending_waits;
    typename timer_queue<Time_Traits>::per_timer_data timer_data;
  };

  // Constructor.
  deadline_timer_service(sofa::pbrpc::boost::asio::io_service& io_service)
    : scheduler_(sofa::pbrpc::boost::asio::use_service<timer_scheduler>(io_service))
  {
    scheduler_.init_task();
    scheduler_.add_timer_queue(timer_queue_);
  }

  // Destructor.
  ~deadline_timer_service()
  {
    scheduler_.remove_timer_queue(timer_queue_);
  }

  // Destroy all user-defined handler objects owned by the service.
  void shutdown_service()
  {
  }

  // Construct a new timer implementation.
  void construct(implementation_type& impl)
  {
    impl.expiry = time_type();
    impl.might_have_pending_waits = false;
  }

  // Destroy a timer implementation.
  void destroy(implementation_type& impl)
  {
    sofa::pbrpc::boost::system::error_code ec;
    cancel(impl, ec);
  }

  // Cancel any asynchronous wait operations associated with the timer.
  std::size_t cancel(implementation_type& impl, sofa::pbrpc::boost::system::error_code& ec)
  {
    if (!impl.might_have_pending_waits)
    {
      ec = sofa::pbrpc::boost::system::error_code();
      return 0;
    }

    BOOST_ASIO_HANDLER_OPERATION(("deadline_timer", &impl, "cancel"));

    std::size_t count = scheduler_.cancel_timer(timer_queue_, impl.timer_data);
    impl.might_have_pending_waits = false;
    ec = sofa::pbrpc::boost::system::error_code();
    return count;
  }

  // Cancels one asynchronous wait operation associated with the timer.
  std::size_t cancel_one(implementation_type& impl,
      sofa::pbrpc::boost::system::error_code& ec)
  {
    if (!impl.might_have_pending_waits)
    {
      ec = sofa::pbrpc::boost::system::error_code();
      return 0;
    }

    BOOST_ASIO_HANDLER_OPERATION(("deadline_timer", &impl, "cancel_one"));

    std::size_t count = scheduler_.cancel_timer(
        timer_queue_, impl.timer_data, 1);
    if (count == 0)
      impl.might_have_pending_waits = false;
    ec = sofa::pbrpc::boost::system::error_code();
    return count;
  }

  // Get the expiry time for the timer as an absolute time.
  time_type expires_at(const implementation_type& impl) const
  {
    return impl.expiry;
  }

  // Set the expiry time for the timer as an absolute time.
  std::size_t expires_at(implementation_type& impl,
      const time_type& expiry_time, sofa::pbrpc::boost::system::error_code& ec)
  {
    std::size_t count = cancel(impl, ec);
    impl.expiry = expiry_time;
    ec = sofa::pbrpc::boost::system::error_code();
    return count;
  }

  // Get the expiry time for the timer relative to now.
  duration_type expires_from_now(const implementation_type& impl) const
  {
    return Time_Traits::subtract(expires_at(impl), Time_Traits::now());
  }

  // Set the expiry time for the timer relative to now.
  std::size_t expires_from_now(implementation_type& impl,
      const duration_type& expiry_time, sofa::pbrpc::boost::system::error_code& ec)
  {
    return expires_at(impl,
        Time_Traits::add(Time_Traits::now(), expiry_time), ec);
  }

  // Perform a blocking wait on the timer.
  void wait(implementation_type& impl, sofa::pbrpc::boost::system::error_code& ec)
  {
    time_type now = Time_Traits::now();
    ec = sofa::pbrpc::boost::system::error_code();
    while (Time_Traits::less_than(now, impl.expiry) && !ec)
    {
      this->do_wait(Time_Traits::to_posix_duration(
            Time_Traits::subtract(impl.expiry, now)), ec);
      now = Time_Traits::now();
    }
  }

  // Start an asynchronous wait on the timer.
  template <typename Handler>
  void async_wait(implementation_type& impl, Handler& handler)
  {
    // Allocate and construct an operation to wrap the handler.
    typedef wait_handler<Handler> op;
    typename op::ptr p = { sofa::pbrpc::boost::asio::detail::addressof(handler),
      boost_asio_handler_alloc_helpers::allocate(
        sizeof(op), handler), 0 };
    p.p = new (p.v) op(handler);

    impl.might_have_pending_waits = true;

    BOOST_ASIO_HANDLER_CREATION((p.p, "deadline_timer", &impl, "async_wait"));

    scheduler_.schedule_timer(timer_queue_, impl.expiry, impl.timer_data, p.p);
    p.v = p.p = 0;
  }

private:
  // Helper function to wait given a duration type. The duration type should
  // either be of type sofa::pbrpc::boost::posix_time::time_duration, or implement the
  // required subset of its interface.
  template <typename Duration>
  void do_wait(const Duration& timeout, sofa::pbrpc::boost::system::error_code& ec)
  {
#if defined(BOOST_ASIO_WINDOWS_RUNTIME)
    std::this_thread::sleep_for(
        std::chrono::seconds(timeout.total_seconds())
        + std::chrono::microseconds(timeout.total_microseconds()));
    ec = sofa::pbrpc::boost::system::error_code();
#else // defined(BOOST_ASIO_WINDOWS_RUNTIME)
    ::timeval tv;
    tv.tv_sec = timeout.total_seconds();
    tv.tv_usec = timeout.total_microseconds() % 1000000;
    socket_ops::select(0, 0, 0, 0, &tv, ec);
#endif // defined(BOOST_ASIO_WINDOWS_RUNTIME)
  }

  // The queue of timers.
  timer_queue<Time_Traits> timer_queue_;

  // The object that schedules and executes timers. Usually a reactor.
  timer_scheduler& scheduler_;
};

} // namespace detail
} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/asio/detail/pop_options.hpp>

#endif // SOFA_PBRPC_BOOST_ASIO_DETAIL_DEADLINE_TIMER_SERVICE_HPP
