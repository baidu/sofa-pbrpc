#ifndef SOFA_PBRPC_BOOST_CURRENT_FUNCTION_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_CURRENT_FUNCTION_HPP_INCLUDED

//
//  boost/current_function.hpp - BOOST_CURRENT_FUNCTION
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  http://www.boost.org/libs/assert/current_function.html
//

namespace sofa {
namespace pbrpc {
namespace boost
{

namespace detail
{

inline void current_function_helper()
{
#define BOOST_CURRENT_FUNCTION __func__
}

} // namespace detail

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif // #ifndef SOFA_PBRPC_BOOST_CURRENT_FUNCTION_HPP_INCLUDED
