//  abi_sufffix header  -------------------------------------------------------//

// (c) Copyright John Maddock 2003
   
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt).

// This header should be #included AFTER code that was preceded by a #include
// <sofa/pbrpc/boost/config/abi_prefix.hpp>.

#ifndef SOFA_PBRPC_BOOST_CONFIG_ABI_PREFIX_HPP
# error Header sofa/pbrpc/boost/config/abi_suffix.hpp must only be used after sofa/pbrpc/boost/config/abi_prefix.hpp
#else
# undef SOFA_PBRPC_BOOST_CONFIG_ABI_PREFIX_HPP
#endif

// the suffix header occurs after all of our code:
#ifdef BOOST_HAS_ABI_HEADERS
#  include BOOST_ABI_SUFFIX
#endif
