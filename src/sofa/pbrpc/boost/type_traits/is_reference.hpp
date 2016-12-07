
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, 
//      Howard Hinnant and John Maddock 2000, 2010. 
//  (C) Copyright Mat Marcus, Jesse Jones and Adobe Systems Inc 2001

//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef SOFA_PBRPC_BOOST_TT_IS_REFERENCE_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_TT_IS_REFERENCE_HPP_INCLUDED

#include <sofa/pbrpc/boost/type_traits/config.hpp>
#include <sofa/pbrpc/boost/type_traits/is_lvalue_reference.hpp>
#include <sofa/pbrpc/boost/type_traits/is_rvalue_reference.hpp>
#include <sofa/pbrpc/boost/type_traits/ice.hpp>

// should be the last #include
#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_def.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {

namespace detail {

template <typename T>
struct is_reference_impl
{
   BOOST_STATIC_CONSTANT(bool, value =
      (::sofa::pbrpc::boost::type_traits::ice_or<
         ::sofa::pbrpc::boost::is_lvalue_reference<T>::value, ::sofa::pbrpc::boost::is_rvalue_reference<T>::value
       >::value));
};

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_reference,T,::sofa::pbrpc::boost::detail::is_reference_impl<T>::value)

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_undef.hpp>

#endif // SOFA_PBRPC_BOOST_TT_IS_REFERENCE_HPP_INCLUDED

