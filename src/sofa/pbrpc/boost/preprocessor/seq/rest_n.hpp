# /* **************************************************************************
#  *                                                                          *
#  *     (C) Copyright Paul Mensonides 2002.
#  *     Distributed under the Boost Software License, Version 1.0. (See
#  *     accompanying file LICENSE_1_0.txt or copy at
#  *     http://www.boost.org/LICENSE_1_0.txt)
#  *                                                                          *
#  ************************************************************************** */
#
# /* See http://www.boost.org for most recent version. */
#
# ifndef SOFA_PBRPC_BOOST_PREPROCESSOR_SEQ_REST_N_HPP
# define SOFA_PBRPC_BOOST_PREPROCESSOR_SEQ_REST_N_HPP
#
# include <sofa/pbrpc/boost/preprocessor/arithmetic/inc.hpp>
# include <sofa/pbrpc/boost/preprocessor/config/config.hpp>
# include <sofa/pbrpc/boost/preprocessor/facilities/identity.hpp>
# include <sofa/pbrpc/boost/preprocessor/seq/detail/split.hpp>
# include <sofa/pbrpc/boost/preprocessor/tuple/elem.hpp>
#
# /* BOOST_PP_SEQ_REST_N */
#
# if ~BOOST_PP_CONFIG_FLAGS() & BOOST_PP_CONFIG_EDG()
#    define BOOST_PP_SEQ_REST_N(n, seq) BOOST_PP_TUPLE_ELEM(2, 1, BOOST_PP_SEQ_SPLIT(BOOST_PP_INC(n), BOOST_PP_IDENTITY( (nil) seq )))()
# else
#    define BOOST_PP_SEQ_REST_N(n, seq) BOOST_PP_SEQ_REST_N_I(n, seq)
#    define BOOST_PP_SEQ_REST_N_I(n, seq) BOOST_PP_TUPLE_ELEM(2, 1, BOOST_PP_SEQ_SPLIT(BOOST_PP_INC(n), BOOST_PP_IDENTITY( (nil) seq )))()
# endif
#
# endif
