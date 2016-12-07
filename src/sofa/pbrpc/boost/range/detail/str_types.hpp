// Boost.Range library
//
//  Copyright Thorsten Ottosen 2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef SOFA_PBRPC_BOOST_RANGE_DETAIL_STR_TYPES_HPP
#define SOFA_PBRPC_BOOST_RANGE_DETAIL_STR_TYPES_HPP

#include <sofa/pbrpc/boost/range/size_type.hpp>
#include <sofa/pbrpc/boost/range/iterator.hpp>

namespace sofa {
namespace pbrpc {
namespace boost
{
    template< class T >
    struct range_mutable_iterator<T*>
    {
        typedef T* type;
    };

    template< class T >
    struct range_const_iterator<T*>
    {
        typedef const T* type;
    };

    template< class T >
    struct range_size<T*>
    {
       typedef std::size_t type;
    };    
}
}
}

#endif
