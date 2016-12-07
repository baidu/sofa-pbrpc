
//  (C) Copyright Dave Abrahams, Steve Cleary, Beman Dawes, Howard
//  Hinnant & John Maddock 2000.  
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.


#ifndef SOFA_PBRPC_BOOST_TT_REMOVE_VOLATILE_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_TT_REMOVE_VOLATILE_HPP_INCLUDED

#include <sofa/pbrpc/boost/type_traits/is_const.hpp>
#include <sofa/pbrpc/boost/type_traits/detail/cv_traits_impl.hpp>
#include <sofa/pbrpc/boost/config.hpp>
#include <sofa/pbrpc/boost/detail/workaround.hpp>

#include <cstddef>

// should be the last #include
#include <sofa/pbrpc/boost/type_traits/detail/type_trait_def.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {


namespace detail {

template <typename T, bool is_const>
struct remove_volatile_helper
{
    typedef T type;
};

template <typename T>
struct remove_volatile_helper<T,true>
{
    typedef T const type;
};

template <typename T>
struct remove_volatile_impl
{
    typedef typename remove_volatile_helper<
       typename cv_traits_imp<BOOST_TT_AUX_CV_TRAITS_IMPL_PARAM(T)>::unqualified_type
        , ::sofa::pbrpc::boost::is_const<T>::value
        >::type type;
};

//
// We can't filter out rvalue_references at the same level as
// references or we get ambiguities from msvc:
//
#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
template <typename T>
struct remove_volatile_impl<T&&>
{
    typedef T&& type;
};
#endif
} // namespace detail

// * convert a type T to a non-volatile type - remove_volatile<T>

BOOST_TT_AUX_TYPE_TRAIT_DEF1(remove_volatile,T,typename sofa::pbrpc::boost::detail::remove_volatile_impl<T>::type)
BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_1(typename T,remove_volatile,T&,T&)
#if !defined(BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_volatile,T volatile[N],T type[N])
BOOST_TT_AUX_TYPE_TRAIT_PARTIAL_SPEC1_2(typename T,std::size_t N,remove_volatile,T const volatile[N],T const type[N])
#endif


} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/type_traits/detail/type_trait_undef.hpp>

#endif // SOFA_PBRPC_BOOST_TT_REMOVE_VOLATILE_HPP_INCLUDED
