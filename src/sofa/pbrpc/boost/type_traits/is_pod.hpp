
//  (C) Copyright Steve Cleary, Beman Dawes, Howard Hinnant & John Maddock 2000.
//  Use, modification and distribution are subject to the Boost Software License,
//  Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt).
//
//  See http://www.boost.org/libs/type_traits for most recent version including documentation.

#ifndef SOFA_PBRPC_BOOST_TT_IS_POD_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_TT_IS_POD_HPP_INCLUDED

#include <sofa/pbrpc/boost/type_traits/config.hpp>
#include <sofa/pbrpc/boost/type_traits/is_void.hpp>
#include <sofa/pbrpc/boost/type_traits/is_scalar.hpp>
#include <sofa/pbrpc/boost/type_traits/detail/ice_or.hpp>
#include <sofa/pbrpc/boost/type_traits/intrinsics.hpp>

#include <cstddef>

// should be the last #include
#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_def.hpp>

#ifndef BOOST_IS_POD
#define BOOST_INTERNAL_IS_POD(T) false
#else
#define BOOST_INTERNAL_IS_POD(T) BOOST_IS_POD(T)
#endif

namespace sofa {
namespace pbrpc {
namespace boost {

// forward declaration, needed by 'is_pod_array_helper' template below
template< typename T > struct is_POD;

namespace detail {


template <typename T> struct is_pod_impl
{ 
    BOOST_STATIC_CONSTANT(
        bool, value =
        (::sofa::pbrpc::boost::type_traits::ice_or<
            ::sofa::pbrpc::boost::is_scalar<T>::value,
            ::sofa::pbrpc::boost::is_void<T>::value,
            BOOST_INTERNAL_IS_POD(T)
         >::value));
};

#if !defined(BOOST_NO_ARRAY_TYPE_SPECIALIZATIONS)
template <typename T, std::size_t sz>
struct is_pod_impl<T[sz]>
    : public is_pod_impl<T>
{
};
#endif


// the following help compilers without partial specialization support:
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void,true)

#ifndef BOOST_NO_CV_VOID_SPECIALIZATIONS
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void const,true)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void volatile,true)
BOOST_TT_AUX_BOOL_TRAIT_IMPL_SPEC1(is_pod,void const volatile,true)
#endif

} // namespace detail

BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_pod,T,::sofa::pbrpc::boost::detail::is_pod_impl<T>::value)
// is_POD is the old depricated name for this trait, do not use this as it may
// be removed in future without warning!!
BOOST_TT_AUX_BOOL_TRAIT_DEF1(is_POD,T,::sofa::pbrpc::boost::is_pod<T>::value)

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#include <sofa/pbrpc/boost/type_traits/detail/bool_trait_undef.hpp>

#undef BOOST_INTERNAL_IS_POD

#endif // SOFA_PBRPC_BOOST_TT_IS_POD_HPP_INCLUDED
