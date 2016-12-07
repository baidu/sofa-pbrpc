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
# ifndef SOFA_PBRPC_BOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
# define SOFA_PBRPC_BOOST_PREPROCESSOR_ITERATION_LOCAL_HPP
#
# include <sofa/pbrpc/boost/preprocessor/config/config.hpp>
# include <sofa/pbrpc/boost/preprocessor/slot/slot.hpp>
# include <sofa/pbrpc/boost/preprocessor/tuple/elem.hpp>
#
# /* BOOST_PP_LOCAL_ITERATE */
#
# define BOOST_PP_LOCAL_ITERATE() <sofa/pbrpc/boost/preprocessor/iteration/detail/local.hpp>
#
# define BOOST_PP_LOCAL_C(n) (BOOST_PP_LOCAL_S) <= n && (BOOST_PP_LOCAL_F) >= n
# define BOOST_PP_LOCAL_R(n) (BOOST_PP_LOCAL_F) <= n && (BOOST_PP_LOCAL_S) >= n
#
# endif
