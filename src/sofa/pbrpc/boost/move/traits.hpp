//////////////////////////////////////////////////////////////////////////////
//
// (C) Copyright Ion Gaztanaga 2009-2012.
// Distributed under the Boost Software License, Version 1.0.
// (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/move for documentation.
//
//////////////////////////////////////////////////////////////////////////////

//! \file

#ifndef SOFA_PBRPC_BOOST_MOVE_TRAITS_HPP
#define SOFA_PBRPC_BOOST_MOVE_TRAITS_HPP

#ifndef BOOST_CONFIG_HPP
#  include <sofa/pbrpc/boost/config.hpp>
#endif
#
#if defined(BOOST_HAS_PRAGMA_ONCE)
#  pragma once
#endif

#include <sofa/pbrpc/boost/move/detail/config_begin.hpp>

#ifndef BOOST_NO_CXX11_RVALUE_REFERENCES
#include <sofa/pbrpc/boost/move/core.hpp>
#endif
#include <sofa/pbrpc/boost/move/detail/meta_utils.hpp>
#include <sofa/pbrpc/boost/move/detail/type_traits.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {

//! If this trait yields to true
//! (<i>has_trivial_destructor_after_move &lt;T&gt;::value == true</i>)
//! means that if T is used as argument of a move construction/assignment,
//! there is no need to call T's destructor.
//! This optimization tipically is used to improve containers' performance.
//!
//! By default this trait is true if the type has trivial destructor,
//! every class should specialize this trait if it wants to improve performance
//! when inserted in containers.
template <class T>
struct has_trivial_destructor_after_move
   : ::sofa::pbrpc::boost::move_detail::is_trivially_destructible<T>
{};

//! By default this traits returns
//! <pre>sofa::pbrpc::boost::is_nothrow_move_constructible<T>::value && sofa::pbrpc::boost::is_nothrow_move_assignable<T>::value </pre>.
//! Classes with non-throwing move constructor
//! and assignment can specialize this trait to obtain some performance improvements.
template <class T>
struct has_nothrow_move
{
   static const bool value = sofa::pbrpc::boost::move_detail::is_nothrow_move_constructible<T>::value &&
                             sofa::pbrpc::boost::move_detail::is_nothrow_move_assignable<T>::value;
};

namespace move_detail {

template <class T>
struct is_nothrow_move_constructible_or_uncopyable
{
   //The standard requires is_nothrow_move_constructible for move_if_noexcept
   //but a user (usually in C++03) might specialize has_nothrow_move which includes it
   static const bool value = is_nothrow_move_constructible<T>::value ||
                             has_nothrow_move<T>::value ||
                            !is_copy_constructible<T>::value;
};

}  //namespace move_detail
}  //namespace boost
}  //namespace pbrpc 
}  //namespace sofa 

#include <sofa/pbrpc/boost/move/detail/config_end.hpp>

#endif //#ifndef SOFA_PBRPC_BOOST_MOVE_TRAITS_HPP
