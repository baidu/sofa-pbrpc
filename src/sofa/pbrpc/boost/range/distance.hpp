// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef SOFA_PBRPC_BOOST_RANGE_DISTANCE_HPP
#define SOFA_PBRPC_BOOST_RANGE_DISTANCE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <sofa/pbrpc/boost/range/begin.hpp>
#include <sofa/pbrpc/boost/range/end.hpp>
#include <sofa/pbrpc/boost/range/difference_type.hpp>

namespace sofa {
namespace pbrpc {
namespace boost 
{

    template< class T >
    inline BOOST_DEDUCED_TYPENAME range_difference<T>::type 
    distance( const T& r )
    {
        return std::distance( sofa::pbrpc::boost::begin( r ), sofa::pbrpc::boost::end( r ) );
    }

}}} // namespace 'boost'

#endif
