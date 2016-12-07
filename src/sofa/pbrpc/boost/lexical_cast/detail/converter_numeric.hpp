// Copyright Kevlin Henney, 2000-2005.
// Copyright Alexander Nasonov, 2006-2010.
// Copyright Antony Polukhin, 2011-2014.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
// what:  lexical_cast custom keyword cast
// who:   contributed by Kevlin Henney,
//        enhanced with contributions from Terje Slettebo,
//        with additional fixes and suggestions from Gennaro Prota,
//        Beman Dawes, Dave Abrahams, Daryle Walker, Peter Dimov,
//        Alexander Nasonov, Antony Polukhin, Justin Viiret, Michael Hofmann,
//        Cheng Yang, Matthew Bradbury, David W. Birdsall, Pavel Korzh and other Boosters
// when:  November 2000, March 2003, June 2005, June 2006, March 2011 - 2014

#ifndef SOFA_PBRPC_BOOST_LEXICAL_CAST_DETAIL_CONVERTER_NUMERIC_HPP
#define SOFA_PBRPC_BOOST_LEXICAL_CAST_DETAIL_CONVERTER_NUMERIC_HPP

#include <sofa/pbrpc/boost/config.hpp>
#ifdef BOOST_HAS_PRAGMA_ONCE
#   pragma once
#endif

#include <sofa/pbrpc/boost/limits.hpp>
#include <sofa/pbrpc/boost/mpl/if.hpp>
#include <sofa/pbrpc/boost/type_traits/ice.hpp>
#include <sofa/pbrpc/boost/type_traits/make_unsigned.hpp>
#include <sofa/pbrpc/boost/type_traits/is_signed.hpp>
#include <sofa/pbrpc/boost/type_traits/is_integral.hpp>
#include <sofa/pbrpc/boost/type_traits/is_arithmetic.hpp>
#include <sofa/pbrpc/boost/type_traits/is_base_of.hpp>
#include <sofa/pbrpc/boost/type_traits/is_float.hpp>

#include <sofa/pbrpc/boost/numeric/conversion/cast.hpp>

namespace sofa {
namespace pbrpc {
namespace boost { namespace detail {

template <class Source >
struct detect_precision_loss
{
    typedef Source source_type;
    typedef sofa::pbrpc::boost::numeric::Trunc<Source> Rounder;
    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
        sofa::pbrpc::boost::is_arithmetic<Source>, Source, Source const&
    >::type argument_type ;

    static inline source_type nearbyint(argument_type s, bool& is_ok) BOOST_NOEXCEPT {
        const source_type near_int = Rounder::nearbyint(s);
        if (near_int && is_ok) {
            const source_type orig_div_round = s / near_int;
            const source_type eps = std::numeric_limits<source_type>::epsilon();

            is_ok = !((orig_div_round > 1 ? orig_div_round - 1 : 1 - orig_div_round) > eps);
        }

        return s;
    }

    typedef typename Rounder::round_style round_style;
};

template <typename Base, class Source>
struct fake_precision_loss: public Base
{
    typedef Source source_type ;
    typedef BOOST_DEDUCED_TYPENAME mpl::if_<
        sofa::pbrpc::boost::is_arithmetic<Source>, Source, Source const&
    >::type argument_type ;

    static inline source_type nearbyint(argument_type s, bool& /*is_ok*/) BOOST_NOEXCEPT {
        return s;
    }
};

struct nothrow_overflow_handler
{
    inline bool operator() ( sofa::pbrpc::boost::numeric::range_check_result r ) const BOOST_NOEXCEPT {
        return (r == sofa::pbrpc::boost::numeric::cInRange);
    }
};

template <typename Target, typename Source>
inline bool noexcept_numeric_convert(const Source& arg, Target& result) BOOST_NOEXCEPT {
    typedef sofa::pbrpc::boost::numeric::converter<
            Target,
            Source,
            sofa::pbrpc::boost::numeric::conversion_traits<Target, Source >,
            nothrow_overflow_handler,
            detect_precision_loss<Source >
    > converter_orig_t;

    typedef BOOST_DEDUCED_TYPENAME sofa::pbrpc::boost::mpl::if_c<
        sofa::pbrpc::boost::is_base_of< detect_precision_loss<Source >, converter_orig_t >::value,
        converter_orig_t,
        fake_precision_loss<converter_orig_t, Source>
    >::type converter_t;

    bool res = nothrow_overflow_handler()(converter_t::out_of_range(arg));
    result = converter_t::low_level_convert(converter_t::nearbyint(arg, res));
    return res;
}

template <typename Target, typename Source>
struct lexical_cast_dynamic_num_not_ignoring_minus
{
    static inline bool try_convert(const Source &arg, Target& result) BOOST_NOEXCEPT {
        return noexcept_numeric_convert<Target, Source >(arg, result);
    }
};

template <typename Target, typename Source>
struct lexical_cast_dynamic_num_ignoring_minus
{
    static inline bool try_convert(const Source &arg, Target& result) BOOST_NOEXCEPT {
        typedef BOOST_DEDUCED_TYPENAME sofa::pbrpc::boost::mpl::eval_if_c<
                sofa::pbrpc::boost::is_float<Source>::value,
                sofa::pbrpc::boost::mpl::identity<Source>,
                sofa::pbrpc::boost::make_unsigned<Source>
        >::type usource_t;

        if (arg < 0) {
            const bool res = noexcept_numeric_convert<Target, usource_t>(0u - arg, result);
            result = static_cast<Target>(0u - result);
            return res;
        } else {
            return noexcept_numeric_convert<Target, usource_t>(arg, result);
        }
    }
};

/*
 * lexical_cast_dynamic_num follows the rules:
 * 1) If Source can be converted to Target without precision loss and
 * without overflows, then assign Source to Target and return
 *
 * 2) If Source is less than 0 and Target is an unsigned integer,
 * then negate Source, check the requirements of rule 1) and if
 * successful, assign static_casted Source to Target and return
 *
 * 3) Otherwise throw a bad_lexical_cast exception
 *
 *
 * Rule 2) required because sofa::pbrpc::boost::lexical_cast has the behavior of
 * stringstream, which uses the rules of scanf for conversions. And
 * in the C99 standard for unsigned input value minus sign is
 * optional, so if a negative number is read, no errors will arise
 * and the result will be the two's complement.
 */
template <typename Target, typename Source>
struct dynamic_num_converter_impl
{
    static inline bool try_convert(const Source &arg, Target& result) BOOST_NOEXCEPT {
        typedef BOOST_DEDUCED_TYPENAME sofa::pbrpc::boost::mpl::if_c<
            sofa::pbrpc::boost::type_traits::ice_and<
                sofa::pbrpc::boost::is_unsigned<Target>::value,
                sofa::pbrpc::boost::type_traits::ice_or<
                    sofa::pbrpc::boost::is_signed<Source>::value,
                    sofa::pbrpc::boost::is_float<Source>::value
                >::value,
                sofa::pbrpc::boost::type_traits::ice_not<
                    sofa::pbrpc::boost::is_same<Source, bool>::value
                >::value,
                sofa::pbrpc::boost::type_traits::ice_not<
                    sofa::pbrpc::boost::is_same<Target, bool>::value
                >::value
            >::value,
            lexical_cast_dynamic_num_ignoring_minus<Target, Source>,
            lexical_cast_dynamic_num_not_ignoring_minus<Target, Source>
        >::type caster_type;

        return caster_type::try_convert(arg, result);
    }
};

}}}} // namespace sofa::pbrpc::boost::detail

#endif // SOFA_PBRPC_BOOST_LEXICAL_CAST_DETAIL_CONVERTER_NUMERIC_HPP

