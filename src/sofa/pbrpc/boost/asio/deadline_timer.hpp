//
// deadline_timer.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_DEADLINE_TIMER_HPP
#define SOFA_PBRPC_BOOST_ASIO_DEADLINE_TIMER_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_BOOST_DATE_TIME) \
  || defined(GENERATING_DOCUMENTATION)

#include <sofa/pbrpc/boost/asio/detail/socket_types.hpp> // Must come before posix_time.
#include <sofa/pbrpc/boost/asio/basic_deadline_timer.hpp>

#include <sofa/pbrpc/boost/date_time/posix_time/posix_time_types.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {

/// Typedef for the typical usage of timer. Uses a UTC clock.
typedef basic_deadline_timer<sofa::pbrpc::boost::posix_time::ptime> deadline_timer;

} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif // defined(BOOST_ASIO_HAS_BOOST_DATE_TIME)
       // || defined(GENERATING_DOCUMENTATION)

#endif // SOFA_PBRPC_BOOST_ASIO_DEADLINE_TIMER_HPP
