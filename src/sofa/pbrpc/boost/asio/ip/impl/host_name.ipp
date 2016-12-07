//
// ip/impl/host_name.ipp
// ~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_IP_IMPL_HOST_NAME_IPP
#define SOFA_PBRPC_BOOST_ASIO_IP_IMPL_HOST_NAME_IPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>
#include <sofa/pbrpc/boost/asio/detail/socket_ops.hpp>
#include <sofa/pbrpc/boost/asio/detail/throw_error.hpp>
//#include <sofa/pbrpc/boost/asio/detail/winsock_init.hpp>
#include <sofa/pbrpc/boost/asio/ip/host_name.hpp>

#include <sofa/pbrpc/boost/asio/detail/push_options.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {
namespace ip {

std::string host_name()
{
  char name[1024];
  sofa::pbrpc::boost::system::error_code ec;
  if (sofa::pbrpc::boost::asio::detail::socket_ops::gethostname(name, sizeof(name), ec) != 0)
  {
    sofa::pbrpc::boost::asio::detail::throw_error(ec);
    return std::string();
  }
  return std::string(name);
}

std::string host_name(sofa::pbrpc::boost::system::error_code& ec)
{
  char name[1024];
  if (sofa::pbrpc::boost::asio::detail::socket_ops::gethostname(name, sizeof(name), ec) != 0)
    return std::string();
  return std::string(name);
}

} // namespace ip
} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/asio/detail/pop_options.hpp>

#endif // SOFA_PBRPC_BOOST_ASIO_IP_IMPL_HOST_NAME_IPP
