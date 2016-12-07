
#ifndef SOFA_PBRPC_BOOST_MPL_AUX_CLEAR_IMPL_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_AUX_CLEAR_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <sofa/pbrpc/boost/mpl/clear_fwd.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/traits_lambda_spec.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/config/eti.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl {

// no default implementation; the definition is needed to make MSVC happy

template< typename Tag >
struct clear_impl
{
    template< typename Sequence > struct apply;
};

BOOST_MPL_ALGORITM_TRAITS_LAMBDA_SPEC(1, clear_impl)

}}}}

#endif // SOFA_PBRPC_BOOST_MPL_AUX_CLEAR_IMPL_HPP_INCLUDED
