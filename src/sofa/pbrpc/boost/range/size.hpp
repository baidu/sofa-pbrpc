// Boost.Range library
//
//  Copyright Thorsten Ottosen 2003-2004. Use, modification and
//  distribution is subject to the Boost Software License, Version
//  1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
// For more information, see http://www.boost.org/libs/range/
//

#ifndef SOFA_PBRPC_BOOST_RANGE_SIZE_HPP
#define SOFA_PBRPC_BOOST_RANGE_SIZE_HPP

#if defined(_MSC_VER)
# pragma once
#endif

#include <sofa/pbrpc/boost/range/config.hpp>
#include <sofa/pbrpc/boost/range/begin.hpp>
#include <sofa/pbrpc/boost/range/end.hpp>
#include <sofa/pbrpc/boost/range/size_type.hpp>
#include <sofa/pbrpc/boost/range/detail/has_member_size.hpp>
#include <sofa/pbrpc/boost/assert.hpp>
#include <sofa/pbrpc/boost/cstdint.hpp>
#include <sofa/pbrpc/boost/utility.hpp>

namespace sofa {
namespace pbrpc {
namespace boost
{
    namespace range_detail
    {

        template<class SinglePassRange>
        inline typename ::sofa::pbrpc::boost::enable_if<
            has_member_size<SinglePassRange>,
            typename range_size<const SinglePassRange>::type
        >::type
        range_calculate_size(const SinglePassRange& rng)
        {
            return rng.size();
        }

        template<class SinglePassRange>
        inline typename disable_if<
            has_member_size<SinglePassRange>,
            typename range_size<const SinglePassRange>::type
        >::type
        range_calculate_size(const SinglePassRange& rng)
        {
            return std::distance(sofa::pbrpc::boost::begin(rng), sofa::pbrpc::boost::end(rng));
        }
    }

    template<class SinglePassRange>
    inline typename range_size<const SinglePassRange>::type
    size(const SinglePassRange& rng)
    {
#if !BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x564)) && \
    !BOOST_WORKAROUND(__GNUC__, < 3) \
    /**/
        using namespace range_detail;
#endif
        return range_calculate_size(rng);
    }

} // namespace 'boost'
} // namespace 'pbrpc'
} // namespace 'sofa'

#endif
