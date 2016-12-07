//
// streambuf.hpp
// ~~~~~~~~~~~~~
//
// Copyright (c) 2003-2015 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef SOFA_PBRPC_BOOST_ASIO_STREAMBUF_HPP
#define SOFA_PBRPC_BOOST_ASIO_STREAMBUF_HPP

#if defined(_MSC_VER) && (_MSC_VER >= 1200)
# pragma once
#endif // defined(_MSC_VER) && (_MSC_VER >= 1200)

#include <sofa/pbrpc/boost/asio/detail/config.hpp>

#if !defined(BOOST_ASIO_NO_IOSTREAM)

#include <sofa/pbrpc/boost/asio/basic_streambuf.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace asio {

/// Typedef for the typical usage of basic_streambuf.
typedef basic_streambuf<> streambuf;

} // namespace asio
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif // !defined(BOOST_ASIO_NO_IOSTREAM)

#endif // SOFA_PBRPC_BOOST_ASIO_STREAMBUF_HPP
