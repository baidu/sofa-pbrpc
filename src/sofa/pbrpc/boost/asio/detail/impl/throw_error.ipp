//
// detail/impl/throw_error.ipp
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_DETAIL_IMPL_THROW_ERROR_IPP
#define SOFA_PBRPC_BOOST_ASIO_DETAIL_IMPL_THROW_ERROR_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>
#include <sofa/pbrpc/boost/asio/detail/throw_error.hpp>
#include <sofa/pbrpc/boost/asio/detail/throw_exception.hpp>
#include <sofa/pbrpc/boost/system/system_error.hpp>

#include <sofa/pbrpc/boost/asio/detail/push_options.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {
namespace detail {

void do_throw_error(const sofa::pbrpc::boost::system::error_code& err)
{
  sofa::pbrpc::boost::system::system_error e(err);
  sofa::pbrpc::boost::asio::detail::throw_exception(e);
}

void do_throw_error(const sofa::pbrpc::boost::system::error_code& err, const char* location)
{
  sofa::pbrpc::boost::system::system_error e(err, location);
  sofa::pbrpc::boost::asio::detail::throw_exception(e);
}

} // namespace detail
} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/asio/detail/pop_options.hpp>

#endif // SOFA_PBRPC_BOOST_ASIO_DETAIL_IMPL_THROW_ERROR_IPP
