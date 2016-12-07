
#ifndef SOFA_PBRPC_BOOST_MPL_FIND_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_FIND_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2000-2002
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <sofa/pbrpc/boost/mpl/find_if.hpp>
#include <sofa/pbrpc/boost/mpl/same_as.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/na_spec.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/lambda_support.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl {

template<
      typename BOOST_MPL_AUX_NA_PARAM(Sequence)
    , typename BOOST_MPL_AUX_NA_PARAM(T)
    >
struct find
    : find_if< Sequence,same_as<T> >
{
    BOOST_MPL_AUX_LAMBDA_SUPPORT(2,find,(Sequence,T))
};

BOOST_MPL_AUX_NA_SPEC(2, find)

}}}}

#endif // SOFA_PBRPC_BOOST_MPL_FIND_HPP_INCLUDED
