//
//! Copyright (c) 2011
//! Brandon Kohn
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
#ifndef SOFA_PBRPC_BOOST_NUMERIC_CAST_TRAITS_HPP
#define SOFA_PBRPC_BOOST_NUMERIC_CAST_TRAITS_HPP

#include <sofa/pbrpc/boost/numeric/conversion/converter_policies.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace numeric {

    template <typename Target, typename Source, typename EnableIf = void>
    struct numeric_cast_traits
    {
        typedef def_overflow_handler    overflow_policy;
        typedef UseInternalRangeChecker range_checking_policy;
        typedef Trunc<Source>           rounding_policy;
    };

}}}}//namespace sofa::pbrpc::boost::numeric;

#if !defined( BOOST_NUMERIC_CONVERSION_RELAX_BUILT_IN_CAST_TRAITS )
#include <sofa/pbrpc/boost/cstdint.hpp>
#include <sofa/pbrpc/boost/numeric/conversion/detail/numeric_cast_traits.hpp>
#endif//!defined BOOST_NUMERIC_CONVERSION_RELAX_BUILT_IN_CAST_TRAITS

#endif//BOOST_NUMERIC_CAST_TRAITS_HPP
