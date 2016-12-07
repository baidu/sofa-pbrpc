// Boost.Range library
//
//  Copyright Thorsten Ottosen 2006. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef SOFA_PBRPC_BOOST_RANGE_DETAIL_AS_LITERAL_HPP
#define SOFA_PBRPC_BOOST_RANGE_DETAIL_AS_LITERAL_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <sofa/pbrpc/boost/range/detail/detail_str.hpp>
#include <sofa/pbrpc/boost/range/iterator_range.hpp>

namespace sofa {
namespace pbrpc {
namespace boost
{
    template< class Range >
    inline iterator_range<BOOST_DEDUCED_TYPENAME range_iterator<Range>::type> 
    as_literal( Range& r )
    {
        return ::sofa::pbrpc::boost::make_iterator_range( ::sofa::pbrpc::boost::range_detail::str_begin(r),
                                             ::sofa::pbrpc::boost::range_detail::str_end(r) );
    }

}
}
}

#endif
