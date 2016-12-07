
#ifndef SOFA_PBRPC_BOOST_MPL_LIST_LIST0_C_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_LIST_LIST0_C_HPP_INCLUDED

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

#include <sofa/pbrpc/boost/mpl/list/list0.hpp>
#include <sofa/pbrpc/boost/mpl/integral_c.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl {

template< typename T > struct list0_c
    : l_end
{
    typedef l_end type;
    typedef T value_type;
};

}}}}

#endif // SOFA_PBRPC_BOOST_MPL_LIST_LIST0_C_HPP_INCLUDED
