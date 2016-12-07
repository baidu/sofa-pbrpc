// Copyright David Abrahams 2003. Use, modification and distribution is
// subject to the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#ifndef SOFA_PBRPC_BOOST_ITERATOR_MINIMUM_CATEGORY_HPP_INCLUDED_
# define SOFA_PBRPC_BOOST_ITERATOR_MINIMUM_CATEGORY_HPP_INCLUDED_

# include <sofa/pbrpc/boost/static_assert.hpp>
# include <sofa/pbrpc/boost/type_traits/is_convertible.hpp>
# include <sofa/pbrpc/boost/type_traits/is_same.hpp>

# include <sofa/pbrpc/boost/mpl/placeholders.hpp>
# include <sofa/pbrpc/boost/mpl/aux_/lambda_support.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
namespace iterators {
namespace detail {

template <bool GreaterEqual, bool LessEqual>
struct minimum_category_impl;

template <class T1, class T2>
struct error_not_related_by_convertibility;

template <>
struct minimum_category_impl<true,false>
{
    template <class T1, class T2> struct apply
    {
        typedef T2 type;
    };
};

template <>
struct minimum_category_impl<false,true>
{
    template <class T1, class T2> struct apply
    {
        typedef T1 type;
    };
};

template <>
struct minimum_category_impl<true,true>
{
    template <class T1, class T2> struct apply
    {
        BOOST_STATIC_ASSERT((is_same<T1,T2>::value));
        typedef T1 type;
    };
};

template <>
struct minimum_category_impl<false,false>
{
    template <class T1, class T2> struct apply
      : error_not_related_by_convertibility<T1,T2>
    {
    };
};

} // namespace detail

//
// Returns the minimum category type or fails to compile
// if T1 and T2 are unrelated.
//
template <class T1 = mpl::_1, class T2 = mpl::_2>
struct minimum_category
{
    typedef sofa::pbrpc::boost::iterators::detail::minimum_category_impl<
        ::sofa::pbrpc::boost::is_convertible<T1,T2>::value
      , ::sofa::pbrpc::boost::is_convertible<T2,T1>::value
    > outer;

    typedef typename outer::template apply<T1,T2> inner;
    typedef typename inner::type type;

    BOOST_MPL_AUX_LAMBDA_SUPPORT(2,minimum_category,(T1,T2))
};

template <>
struct minimum_category<mpl::_1,mpl::_2>
{
    template <class T1, class T2>
    struct apply : minimum_category<T1,T2>
    {};

    BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(2,minimum_category,(mpl::_1,mpl::_2))
};

} // namespace iterators

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif // SOFA_PBRPC_BOOST_ITERATOR_MINIMUM_CATEGORY_HPP_INCLUDED_
