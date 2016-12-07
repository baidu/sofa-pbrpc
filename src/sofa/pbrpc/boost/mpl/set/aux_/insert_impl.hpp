
#ifndef BOOST_MPL_SET_AUX_INSERT_IMPL_HPP_INCLUDED
#define BOOST_MPL_SET_AUX_INSERT_IMPL_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2003-2007
// Copyright David Abrahams 2003-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <sofa/pbrpc/boost/mpl/insert_fwd.hpp>
#include <sofa/pbrpc/boost/mpl/set/aux_/has_key_impl.hpp>
#include <sofa/pbrpc/boost/mpl/set/aux_/item.hpp>
#include <sofa/pbrpc/boost/mpl/set/aux_/tag.hpp>
#include <sofa/pbrpc/boost/mpl/identity.hpp>
#include <sofa/pbrpc/boost/mpl/base.hpp>
#include <sofa/pbrpc/boost/mpl/eval_if.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/na.hpp>

#include <sofa/pbrpc/boost/type_traits/is_same.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl {

namespace aux {
template<  typename Set, typename T > struct set_insert_impl
    : eval_if< 
          has_key_impl<aux::set_tag>::apply<Set,T>
        , identity<Set>
        , eval_if< 
              is_same< T,typename Set::last_masked_ > 
            , base<Set>
            , identity< s_item<T,typename Set::item_> >
            >
        >
{
};
}

template<>
struct insert_impl< aux::set_tag >
{
    template< 
          typename Set
        , typename PosOrKey
        , typename KeyOrNA
        > 
    struct apply
        : aux::set_insert_impl<
              Set
            , typename if_na<KeyOrNA,PosOrKey>::type
            >
    {
    };
};

}}}}

#endif // BOOST_MPL_SET_AUX_INSERT_IMPL_HPP_INCLUDED
