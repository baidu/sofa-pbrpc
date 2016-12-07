
#ifndef SOFA_PBRPC_BOOST_MPL_LAMBDA_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_LAMBDA_HPP_INCLUDED

// Copyright Aleksey Gurtovoy 2001-2004
//
// Distributed under the Boost Software License, Version 1.0. 
// (See accompanying file LICENSE_1_0.txt or copy at 
// http://www.boost.org/LICENSE_1_0.txt)
//
// See http://www.boost.org/libs/mpl for documentation.

// $Id$
// $Date$
// $Revision$

#include <sofa/pbrpc/boost/mpl/lambda_fwd.hpp>
#include <sofa/pbrpc/boost/mpl/bind.hpp>
#include <sofa/pbrpc/boost/mpl/aux_/config/lambda.hpp>

#if !defined(BOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)
#   include <sofa/pbrpc/boost/mpl/aux_/full_lambda.hpp>
#else
#   include <sofa/pbrpc/boost/mpl/aux_/lambda_no_ctps.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/lambda_support.hpp>
#   define BOOST_MPL_CFG_NO_IMPLICIT_METAFUNCTIONS
#endif

#endif // SOFA_PBRPC_BOOST_MPL_LAMBDA_HPP_INCLUDED
