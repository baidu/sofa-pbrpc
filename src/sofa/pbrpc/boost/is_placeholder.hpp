#ifndef SOFA_PBRPC_BOOST_IS_PLACEHOLDER_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_IS_PLACEHOLDER_HPP_INCLUDED

//  is_placeholder.hpp - TR1 is_placeholder metafunction
//
//  Copyright (c) 2006 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt


namespace sofa {
namespace pbrpc {
namespace boost
{

template< class T > struct is_placeholder
{
    enum _vt { value = 0 };
};

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif // #ifndef SOFA_PBRPC_BOOST_IS_PLACEHOLDER_HPP_INCLUDED
