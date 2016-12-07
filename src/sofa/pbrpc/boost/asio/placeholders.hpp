//
// placeholders.hpp
// ~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_PLACEHOLDERS_HPP
#define SOFA_PBRPC_BOOST_ASIO_PLACEHOLDERS_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>

#if defined(BOOST_ASIO_HAS_BOOST_BIND)
# include <sofa/pbrpc/boost/bind/arg.hpp>
#endif // defined(BOOST_ASIO_HAS_BOOST_BIND)

#include <sofa/pbrpc/boost/asio/detail/push_options.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {
namespace placeholders {

#if defined(GENERATING_DOCUMENTATION)

/// An argument placeholder, for use with sofa::pbrpc::boost::bind(), that corresponds to
/// the error argument of a handler for any of the asynchronous functions.
unspecified error;

/// An argument placeholder, for use with sofa::pbrpc::boost::bind(), that corresponds to
/// the bytes_transferred argument of a handler for asynchronous functions such
/// as sofa::pbrpc::boost::asio::basic_stream_socket::async_write_some or
/// sofa::pbrpc::boost::asio::async_write.
unspecified bytes_transferred;

/// An argument placeholder, for use with sofa::pbrpc::boost::bind(), that corresponds to
/// the iterator argument of a handler for asynchronous functions such as
/// sofa::pbrpc::boost::asio::basic_resolver::async_resolve.
unspecified iterator;

/// An argument placeholder, for use with sofa::pbrpc::boost::bind(), that corresponds to
/// the signal_number argument of a handler for asynchronous functions such as
/// sofa::pbrpc::boost::asio::signal_set::async_wait.
unspecified signal_number;

#elif defined(BOOST_ASIO_HAS_BOOST_BIND)
# if defined(__BORLANDC__) || defined(__GNUC__)

inline sofa::pbrpc::boost::arg<1> error()
{
  return sofa::pbrpc::boost::arg<1>();
}

inline sofa::pbrpc::boost::arg<2> bytes_transferred()
{
  return sofa::pbrpc::boost::arg<2>();
}

inline sofa::pbrpc::boost::arg<2> iterator()
{
  return sofa::pbrpc::boost::arg<2>();
}

inline sofa::pbrpc::boost::arg<2> signal_number()
{
  return sofa::pbrpc::boost::arg<2>();
}

# else

namespace detail
{
  template <int Number>
  struct placeholder
  {
    static sofa::pbrpc::boost::arg<Number>& get()
    {
      static sofa::pbrpc::boost::arg<Number> result;
      return result;
    }
  };
}

#  if defined(BOOST_ASIO_MSVC) && (BOOST_ASIO_MSVC < 1400)

static sofa::pbrpc::boost::arg<1>& error
  = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<1>::get();
static sofa::pbrpc::boost::arg<2>& bytes_transferred
  = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<2>::get();
static sofa::pbrpc::boost::arg<2>& iterator
  = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<2>::get();
static sofa::pbrpc::boost::arg<2>& signal_number
  = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<2>::get();

#  else

namespace
{
  sofa::pbrpc::boost::arg<1>& error
    = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<1>::get();
  sofa::pbrpc::boost::arg<2>& bytes_transferred
    = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<2>::get();
  sofa::pbrpc::boost::arg<2>& iterator
    = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<2>::get();
  sofa::pbrpc::boost::arg<2>& signal_number
    = sofa::pbrpc::boost::asio::placeholders::detail::placeholder<2>::get();
} // namespace

#  endif
# endif
#endif

} // namespace placeholders
} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/asio/detail/pop_options.hpp>

#endif // SOFA_PBRPC_BOOST_ASIO_PLACEHOLDERS_HPP
