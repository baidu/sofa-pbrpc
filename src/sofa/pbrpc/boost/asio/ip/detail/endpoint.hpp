//
// ip/detail/endpoint.hpp
// ~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP
#define SOFA_PBRPC_BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>
#include <string>
#include <sofa/pbrpc/boost/asio/detail/socket_types.hpp>
//#include <sofa/pbrpc/boost/asio/detail/winsock_init.hpp>
#include <sofa/pbrpc/boost/system/error_code.hpp>
#include <sofa/pbrpc/boost/asio/ip/address.hpp>

#include <sofa/pbrpc/boost/asio/detail/push_options.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {
namespace ip {
namespace detail {

// Helper class for implementating an IP endpoint.
class endpoint
{
public:
  // Default constructor.
  BOOST_ASIO_DECL endpoint();

  // Construct an endpoint using a family and port number.
  BOOST_ASIO_DECL endpoint(int family, unsigned short port_num);

  // Construct an endpoint using an address and port number.
  BOOST_ASIO_DECL endpoint(const sofa::pbrpc::boost::asio::ip::address& addr,
      unsigned short port_num);

  // Copy constructor.
  endpoint(const endpoint& other)
    : data_(other.data_)
  {
  }

  // Assign from another endpoint.
  endpoint& operator=(const endpoint& other)
  {
    data_ = other.data_;
    return *this;
  }

  // Get the underlying endpoint in the native type.
  sofa::pbrpc::boost::asio::detail::socket_addr_type* data()
  {
    return &data_.base;
  }

  // Get the underlying endpoint in the native type.
  const sofa::pbrpc::boost::asio::detail::socket_addr_type* data() const
  {
    return &data_.base;
  }

  // Get the underlying size of the endpoint in the native type.
  std::size_t size() const
  {
    if (is_v4())
      return sizeof(sofa::pbrpc::boost::asio::detail::sockaddr_in4_type);
    else
      return sizeof(sofa::pbrpc::boost::asio::detail::sockaddr_in6_type);
  }

  // Set the underlying size of the endpoint in the native type.
  BOOST_ASIO_DECL void resize(std::size_t new_size);

  // Get the capacity of the endpoint in the native type.
  std::size_t capacity() const
  {
    return sizeof(data_);
  }

  // Get the port associated with the endpoint.
  BOOST_ASIO_DECL unsigned short port() const;

  // Set the port associated with the endpoint.
  BOOST_ASIO_DECL void port(unsigned short port_num);

  // Get the IP address associated with the endpoint.
  BOOST_ASIO_DECL sofa::pbrpc::boost::asio::ip::address address() const;

  // Set the IP address associated with the endpoint.
  BOOST_ASIO_DECL void address(const sofa::pbrpc::boost::asio::ip::address& addr);

  // Compare two endpoints for equality.
  BOOST_ASIO_DECL friend bool operator==(
      const endpoint& e1, const endpoint& e2);

  // Compare endpoints for ordering.
  BOOST_ASIO_DECL friend bool operator<(
      const endpoint& e1, const endpoint& e2);

  // Determine whether the endpoint is IPv4.
  bool is_v4() const
  {
    return data_.base.sa_family == BOOST_ASIO_OS_DEF(AF_INET);
  }

#if !defined(BOOST_ASIO_NO_IOSTREAM)
  // Convert to a string.
  BOOST_ASIO_DECL std::string to_string(sofa::pbrpc::boost::system::error_code& ec) const;
#endif // !defined(BOOST_ASIO_NO_IOSTREAM)

private:
  // The underlying IP socket address.
  union data_union
  {
    sofa::pbrpc::boost::asio::detail::socket_addr_type base;
    sofa::pbrpc::boost::asio::detail::sockaddr_in4_type v4;
    sofa::pbrpc::boost::asio::detail::sockaddr_in6_type v6;
  } data_;
};

} // namespace detail
} // namespace ip
} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/asio/detail/pop_options.hpp>

#if defined(BOOST_ASIO_HEADER_ONLY)
# include <sofa/pbrpc/boost/asio/ip/detail/impl/endpoint.ipp>
#endif // defined(BOOST_ASIO_HEADER_ONLY)

#endif // SOFA_PBRPC_BOOST_ASIO_IP_DETAIL_ENDPOINT_HPP
