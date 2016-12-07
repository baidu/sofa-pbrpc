
#ifndef SOFA_PBRPC_BOOST_MPL_IS_PLACEHOLDER_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_IS_PLACEHOLDER_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <sofa/pbrpc/boost/mpl/arg_fwd.hpp>
#include <sofa/pbrpc/boost/mpl/bool.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/yes_no.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/type_wrapper.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/nttp_decl.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/config/ctps.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/config/static_constant.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl {

#if !defined(BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION)

template< typename T >
struct is_placeholder
    : bool_<false>
{
};

template< BOOST_MPL_AUX_NTTP_DECL(int, N) >
struct is_placeholder< arg<N> >
    : bool_<true>
{
};

#else

namespace aux {

aux::no_tag is_placeholder_helper(...);

template< BOOST_MPL_AUX_NTTP_DECL(int, N) >
aux::yes_tag is_placeholder_helper(aux::type_wrapper< arg<N> >*);

} // namespace aux

template< typename T >
struct is_placeholder
{
    static aux::type_wrapper<T>* get();
    BOOST_STATIC_CONSTANT(bool, value = 
          sizeof(aux::is_placeholder_helper(get())) == sizeof(aux::yes_tag)
        );
    
    typedef bool_<value> type;
};

#endif // BOOST_NO_TEMPLATE_PARTIAL_SPECIALIZATION

}}}}

#endif // SOFA_PBRPC_BOOST_MPL_IS_PLACEHOLDER_HPP_INCLUDED
