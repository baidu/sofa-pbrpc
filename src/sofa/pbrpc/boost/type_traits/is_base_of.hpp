
//  (C) Copyright Rani Sharoni 2003-2005.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.
 
#ifndef SOFA_PBRPC_BOOST_TT_IS_BASE_OF_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_TT_IS_BASE_OF_HPP_INCLUDED

#include <sofa/pbrpc/boost/type_traits/is_base_and_derived.hpp>
#include <sofa/pbrpc/boost/type_traits/is_same.hpp>
#include <sofa/pbrpc/boost/type_traits/is_class.hpp>
#include <sofa/pbrpc/boost/type_traits/detail/ice_or.hpp>
#include <sofa/pbrpc/boost/type_traits/detail/ice_and.hpp>

// should be the last #include
#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_def.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {

   namespace detail{
      template <class B, class D>
      struct is_base_of_imp
      {
          typedef typename remove_cv<B>::type ncvB;
          typedef typename remove_cv<D>::type ncvD;
          BOOST_STATIC_CONSTANT(bool, value = (::sofa::pbrpc::boost::type_traits::ice_or<      
            (::sofa::pbrpc::boost::detail::is_base_and_derived_impl<ncvB,ncvD>::value),
            (::sofa::pbrpc::boost::type_traits::ice_and< ::sofa::pbrpc::boost::is_same<ncvB,ncvD>::value, ::sofa::pbrpc::boost::is_class<ncvB>::value>::value)>::value));
      };
   }

BOOST_TT_AUX_BOOL_TRAIT_DEF2(
      is_base_of
    , Base
    , Derived
    , (::sofa::pbrpc::boost::detail::is_base_of_imp<Base, Derived>::value))

BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_base_of,Base&,Derived,false)
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_base_of,Base,Derived&,false)
BOOST_TT_AUX_BOOL_TRAIT_PARTIAL_SPEC2_2(typename Base,typename Derived,is_base_of,Base&,Derived&,false)

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_undef.hpp>

#endif // SOFA_PBRPC_BOOST_TT_IS_BASE_AND_DERIVED_HPP_INCLUDED
