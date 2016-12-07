
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef SOFA_PBRPC_BOOST_TT_IS_FUNDAMENTAL_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_TT_IS_FUNDAMENTAL_HPP_INCLUDED

#include <sofa/pbrpc/boost/type_traits/is_arithmetic.hpp>
#include <sofa/pbrpc/boost/type_traits/is_void.hpp>
#include <sofa/pbrpc/boost/type_traits/detail/ice_or.hpp>

// should be the last #include
#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_def.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {

namespace detail {

template <typename T> 
struct is_fundamental_impl
    : public ::sofa::pbrpc::boost::type_traits::ice_or< 
          ::sofa::pbrpc::boost::is_arithmetic<T>::value
        , ::sofa::pbrpc::boost::is_void<T>::value
        >
{ 
};

} // namespace detail

//* is a type T a fundamental type described in the standard (3.9.1)
#if defined( __CODEGEARC__ )
BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_fundamental,T,__is_fundamental(T))
#else
BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_fundamental,T,::sofa::pbrpc::boost::detail::is_fundamental_impl<T>::value)
#endif

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_undef.hpp>

#endif // SOFA_PBRPC_BOOST_TT_IS_FUNDAMENTAL_HPP_INCLUDED
