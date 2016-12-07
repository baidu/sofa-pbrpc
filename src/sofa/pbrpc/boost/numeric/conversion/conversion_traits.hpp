//  (c) Copyright Fernando Luis Cacciola Carballal 2000-2004
//  Use, modification, and distribution is subject to the Boost Software
//  License, Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)

//  See library home page at http://www.boost.org/libs/numeric/conversion
//
// Contact the author at: fernando_cacciola@hotmail.com
// 
#ifndef SOFA_PBRPC_BOOST_NUMERIC_CONVERSION_CONVERSION_TRAITS_FLC_12NOV2002_HPP
#define SOFA_PBRPC_BOOST_NUMERIC_CONVERSION_CONVERSION_TRAITS_FLC_12NOV2002_HPP

#include "sofa/pbrpc/boost/numeric/conversion/detail/conversion_traits.hpp"
#include "sofa/pbrpc/boost/detail/workaround.hpp"
#include "sofa/pbrpc/boost/config.hpp"

namespace sofa {
namespace pbrpc {
namespace boost { namespace numeric
{

template<class T, class S>
struct conversion_traits 
    : convdetail::get_conversion_traits<T,S>::type 
{
} ;

} } } } // namespace sofa::pbrpc::boost::numeric

#endif
//
///////////////////////////////////////////////////////////////////////////////////////////////


