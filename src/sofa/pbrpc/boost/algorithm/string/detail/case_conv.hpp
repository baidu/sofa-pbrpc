//  Boost string_algo library string_funct.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef SOFA_PBRPC_BOOST_STRING_CASE_CONV_DETAIL_HPP
#define SOFA_PBRPC_BOOST_STRING_CASE_CONV_DETAIL_HPP

#include <sofa/pbrpc/boost/algorithm/string/config.hpp>
#include <locale>
#include <functional>

#include <sofa/pbrpc/boost/type_traits/make_unsigned.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
    namespace algorithm {
        namespace detail {

//  case conversion functors -----------------------------------------------//

            // a tolower functor
            template<typename CharT>
            struct to_lowerF : public std::unary_function<CharT, CharT>
            {
                // Constructor
                to_lowerF( const std::locale& Loc ) : m_Loc( &Loc ) {}

                // Operation
                CharT operator ()( CharT Ch ) const
                {
                    return std::tolower<CharT>( Ch, *m_Loc );
                }
            private:
                const std::locale* m_Loc;
            };

            // a toupper functor
            template<typename CharT>
            struct to_upperF : public std::unary_function<CharT, CharT>
            {
                // Constructor
                to_upperF( const std::locale& Loc ) : m_Loc( &Loc ) {}

                // Operation
                CharT operator ()( CharT Ch ) const
                {
                    return std::toupper<CharT>( Ch, *m_Loc );
                }
            private:
                const std::locale* m_Loc;
            };

// algorithm implementation -------------------------------------------------------------------------

            // Transform a range
            template<typename OutputIteratorT, typename RangeT, typename FunctorT>
            OutputIteratorT transform_range_copy(
                OutputIteratorT Output,
                const RangeT& Input,
                FunctorT Functor)
            {
                return std::transform( 
                    ::sofa::pbrpc::boost::begin(Input), 
                    ::sofa::pbrpc::boost::end(Input), 
                    Output,
                    Functor);
            }

            // Transform a range (in-place)
            template<typename RangeT, typename FunctorT>
            void transform_range(
                const RangeT& Input,
                FunctorT Functor)
            {
                std::transform( 
                    ::sofa::pbrpc::boost::begin(Input), 
                    ::sofa::pbrpc::boost::end(Input), 
                    ::sofa::pbrpc::boost::begin(Input),
                    Functor);
            }

            template<typename SequenceT, typename RangeT, typename FunctorT>
            inline SequenceT transform_range_copy( 
                const RangeT& Input, 
                FunctorT Functor)
            {
                return SequenceT(
                    ::sofa::pbrpc::boost::make_transform_iterator(
                        ::sofa::pbrpc::boost::begin(Input),
                        Functor),
                    ::sofa::pbrpc::boost::make_transform_iterator(
                        ::sofa::pbrpc::boost::end(Input), 
                        Functor));
            }

        } // namespace detail
    } // namespace algorithm
} // namespace boost
} // namespace pbrpc
} // namespace sofa


#endif  // SOFA_PBRPC_BOOST_STRING_CASE_CONV_DETAIL_HPP
