#ifndef SOFA_PBRPC_BOOST_DETAIL_SP_TYPEINFO_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_DETAIL_SP_TYPEINFO_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//  detail/sp_typeinfo.hpp
//
//  Deprecated, please use boost/core/typeinfo.hpp
//
//  Copyright 2007 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

#include <sofa/pbrpc/boost/core/typeinfo.hpp>

namespace sofa {
namespace pbrpc {
namespace boost
{

namespace detail
{

typedef sofa::pbrpc::boost::core::typeinfo sp_typeinfo;

} // namespace detail

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#define BOOST_SP_TYPEID(T) BOOST_CORE_TYPEID(T)

#endif  // #ifndef SOFA_PBRPC_BOOST_DETAIL_SP_TYPEINFO_HPP_INCLUDED
