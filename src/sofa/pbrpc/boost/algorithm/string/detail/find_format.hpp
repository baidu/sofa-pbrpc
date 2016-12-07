//  Boost string_algo library find_format.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
// 
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef SOFA_PBRPC_BOOST_STRING_FIND_FORMAT_DETAIL_HPP
#define SOFA_PBRPC_BOOST_STRING_FIND_FORMAT_DETAIL_HPP

#include <sofa/pbrpc/boost/algorithm/string/config.hpp>
#include <sofa/pbrpc/boost/range/iterator_range_core.hpp>
#include <sofa/pbrpc/boost/range/const_iterator.hpp>
#include <sofa/pbrpc/boost/range/iterator.hpp>
#include <sofa/pbrpc/boost/algorithm/string/detail/find_format_store.hpp>
#include <sofa/pbrpc/boost/algorithm/string/detail/replace_storage.hpp>

namespace sofa {
namespace pbrpc {
namespace boost {
    namespace algorithm {
        namespace detail {

// find_format_copy (iterator variant) implementation -------------------------------//

           template< 
                typename OutputIteratorT,
                typename InputT,
                typename FormatterT,
                typename FindResultT,
                typename FormatResultT >
            inline OutputIteratorT find_format_copy_impl2(
                OutputIteratorT Output,
                const InputT& Input,
                FormatterT Formatter,
                const FindResultT& FindResult,
                const FormatResultT& FormatResult )
            {       
                typedef find_format_store<
                    BOOST_STRING_TYPENAME 
                        range_const_iterator<InputT>::type, 
                        FormatterT,
                        FormatResultT > store_type;

                // Create store for the find result
                store_type M( FindResult, FormatResult, Formatter );

                if ( !M )
                {
                    // Match not found - return original sequence
                    Output = std::copy( ::sofa::pbrpc::boost::begin(Input), ::sofa::pbrpc::boost::end(Input), Output );
                    return Output;
                }

                // Copy the beginning of the sequence
                Output = std::copy( ::sofa::pbrpc::boost::begin(Input), ::sofa::pbrpc::boost::begin(M), Output );
                // Format find result
                // Copy formatted result
                Output = std::copy( ::sofa::pbrpc::boost::begin(M.format_result()), ::sofa::pbrpc::boost::end(M.format_result()), Output );
                // Copy the rest of the sequence
                Output = std::copy( M.end(), ::sofa::pbrpc::boost::end(Input), Output );

                return Output;
            }

            template< 
                typename OutputIteratorT,
                typename InputT,
                typename FormatterT,
                typename FindResultT >
            inline OutputIteratorT find_format_copy_impl(
                OutputIteratorT Output,
                const InputT& Input,
                FormatterT Formatter,
                const FindResultT& FindResult )
            {   
                if( ::sofa::pbrpc::boost::algorithm::detail::check_find_result(Input, FindResult) ) {
                    return ::sofa::pbrpc::boost::algorithm::detail::find_format_copy_impl2( 
                        Output,
                        Input,
                        Formatter,
                        FindResult,
                        Formatter(FindResult) );
                } else {
                    return std::copy( ::sofa::pbrpc::boost::begin(Input), ::sofa::pbrpc::boost::end(Input), Output );
                }
            }

 
// find_format_copy implementation --------------------------------------------------//

           template< 
                typename InputT, 
                typename FormatterT,
                typename FindResultT,
                typename FormatResultT >
            inline InputT find_format_copy_impl2(
                const InputT& Input,
                FormatterT Formatter,
                const FindResultT& FindResult,
                const FormatResultT& FormatResult)
            {
                typedef find_format_store<
                    BOOST_STRING_TYPENAME 
                        range_const_iterator<InputT>::type, 
                        FormatterT,
                        FormatResultT > store_type;

                // Create store for the find result
                store_type M( FindResult, FormatResult, Formatter );

                if ( !M )
                {
                    // Match not found - return original sequence
                    return InputT( Input );
                }

                InputT Output;
                // Copy the beginning of the sequence
                sofa::pbrpc::boost::algorithm::detail::insert( Output, ::sofa::pbrpc::boost::end(Output), ::sofa::pbrpc::boost::begin(Input), M.begin() );
                // Copy formatted result
                sofa::pbrpc::boost::algorithm::detail::insert( Output, ::sofa::pbrpc::boost::end(Output), M.format_result() );
                // Copy the rest of the sequence
                sofa::pbrpc::boost::algorithm::detail::insert( Output, ::sofa::pbrpc::boost::end(Output), M.end(), ::sofa::pbrpc::boost::end(Input) );

                return Output;
            }

            template< 
                typename InputT, 
                typename FormatterT,
                typename FindResultT >
            inline InputT find_format_copy_impl(
                const InputT& Input,
                FormatterT Formatter,
                const FindResultT& FindResult)
            {
                if( ::sofa::pbrpc::boost::algorithm::detail::check_find_result(Input, FindResult) ) {
                    return ::sofa::pbrpc::boost::algorithm::detail::find_format_copy_impl2(
                        Input,
                        Formatter,
                        FindResult,
                        Formatter(FindResult) );
                } else {
                    return Input;
                }
            }

 // replace implementation ----------------------------------------------------//
        
            template<
                typename InputT,
                typename FormatterT,
                typename FindResultT,
                typename FormatResultT >
            inline void find_format_impl2( 
                InputT& Input,
                FormatterT Formatter,
                const FindResultT& FindResult,
                const FormatResultT& FormatResult)
            {
                typedef find_format_store<
                    BOOST_STRING_TYPENAME 
                        range_iterator<InputT>::type, 
                        FormatterT,
                        FormatResultT > store_type;

                // Create store for the find result
                store_type M( FindResult, FormatResult, Formatter );

                if ( !M )
                {
                    // Search not found - return original sequence
                    return;
                }

                // Replace match
                ::sofa::pbrpc::boost::algorithm::detail::replace( Input, M.begin(), M.end(), M.format_result() );
            }

            template<
                typename InputT,
                typename FormatterT,
                typename FindResultT >
            inline void find_format_impl( 
                InputT& Input,
                FormatterT Formatter,
                const FindResultT& FindResult)
            {
                if( ::sofa::pbrpc::boost::algorithm::detail::check_find_result(Input, FindResult) ) {
                    ::sofa::pbrpc::boost::algorithm::detail::find_format_impl2(
                        Input,
                        Formatter,
                        FindResult,
                        Formatter(FindResult) );
                }
            }

        } // namespace detail
    } // namespace algorithm
} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif  // SOFA_PBRPC_BOOST_STRING_FIND_FORMAT_DETAIL_HPP
