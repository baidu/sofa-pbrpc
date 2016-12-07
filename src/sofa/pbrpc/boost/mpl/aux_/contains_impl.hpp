
#ifndef SOFA_PBRPC_BOOST_MPL_AUX_CONTAINS_IMPL_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_AUX_CONTAINS_IMPL_HPP_INCLUDED

// Copyright Eric Friedman 2002
// Copyright Aleksey Gurtovoy 2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <sofa/pbrpc/boost/mpl/contains_fwd.hpp>
#include <sofa/pbrpc/boost/mpl/begin_end.hpp>
#include <sofa/pbrpc/boost/mpl/find.hpp>
#include <sofa/pbrpc/boost/mpl/not.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/traits_lambda_spec.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/config/forwarding.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/config/static_constant.hpp>

#include <sofa/pbrpc/boost/type_traits/is_same.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl {

template< typename Tag >
struct contains_impl
{
    template< typename Sequence, typename T > struct apply
#if !defined(BOOST_MPL_CFG_NO_NESTED_FORWARDING)
        : not_< is_same<
              typename find<Sequence,T>::type
            , typename end<Sequence>::type
            > >
    {
#else
    {
        typedef not_< is_same<
              typename find<Sequence,T>::type
            , typename end<Sequence>::type
            > > type;

        BOOST_STATIC_CONSTANT(bool, value = 
              (not_< is_same<
                  typename find<Sequence,T>::type
                , typename end<Sequence>::type
                > >::value)
            );
#endif
    };
};

BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(2,contains_impl)

}}}}

#endif // SOFA_PBRPC_BOOST_MPL_AUX_CONTAINS_IMPL_HPP_INCLUDED
