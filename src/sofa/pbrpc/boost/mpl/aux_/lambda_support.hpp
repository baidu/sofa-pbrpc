
#ifndef SOFA_PBRPC_BOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED

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

#include <sofa/pbrpc/boost/mpl/aux_/config/lambda.hpp>

#if !defined(BOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT)

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) /**/
#   define BOOST_MPL_AUX_LAMBDA_SUPPORT(i,name,params) /**/

#else

#   include <sofa/pbrpc/boost/mpl/int_fwd.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/yes_no.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/na_fwd.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/preprocessor/params.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/preprocessor/enum.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/config/msvc.hpp>
#   include <sofa/pbrpc/boost/mpl/aux_/config/workaround.hpp>

#   include <sofa/pbrpc/boost/preprocessor/tuple/to_list.hpp>
#   include <sofa/pbrpc/boost/preprocessor/list/for_each_i.hpp>
#   include <sofa/pbrpc/boost/preprocessor/inc.hpp>
#   include <sofa/pbrpc/boost/preprocessor/cat.hpp>

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC(R,typedef_,i,param) \
    typedef_ param BOOST_PP_CAT(arg,BOOST_PP_INC(i)); \
    /**/

// agurt, 07/mar/03: restore an old revision for the sake of SGI MIPSpro C++
#if BOOST_WORKAROUND(__EDG_VERSION__, <= 238) 

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    typedef BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    BOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , BOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , BOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    struct rebind \
    { \
        template< BOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
            : name< BOOST_MPL_PP_PARAMS(i,U) > \
        { \
        }; \
    }; \
    /**/

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    BOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    /**/

#elif BOOST_WORKAROUND(__EDG_VERSION__, <= 244) && !defined(BOOST_INTEL_CXX_VERSION)
// agurt, 18/jan/03: old EDG-based compilers actually enforce 11.4 para 9
// (in strict mode), so we have to provide an alternative to the 
// MSVC-optimized implementation

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    typedef BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    BOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , BOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , BOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    struct rebind; \
/**/

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
template< BOOST_MPL_PP_PARAMS(i,typename T) > \
struct name<BOOST_MPL_PP_PARAMS(i,T)>::rebind \
{ \
    template< BOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
        : name< BOOST_MPL_PP_PARAMS(i,U) > \
    { \
    }; \
/**/

#else // __EDG_VERSION__

namespace sofa {
namespace pbrpc {
namespace boost { namespace mpl { namespace aux {
template< typename T > struct has_rebind_tag;
}}}}}

#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
    typedef BOOST_MPL_AUX_ADL_BARRIER_NAMESPACE::int_<i> arity; \
    BOOST_PP_LIST_FOR_EACH_I_R( \
          1 \
        , BOOST_MPL_AUX_LAMBDA_SUPPORT_ARG_TYPEDEF_FUNC \
        , typedef \
        , BOOST_PP_TUPLE_TO_LIST(i,params) \
        ) \
    friend class BOOST_PP_CAT(name,_rebind); \
    typedef BOOST_PP_CAT(name,_rebind) rebind; \
/**/

#if BOOST_WORKAROUND(__BORLANDC__, BOOST_TESTED_AT(0x610))
#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
template< BOOST_MPL_PP_PARAMS(i,typename T) > \
::sofa::pbrpc::boost::mpl::aux::yes_tag operator|( \
      ::sofa::pbrpc::boost::mpl::aux::has_rebind_tag<int> \
    , name<BOOST_MPL_PP_PARAMS(i,T)>* \
    ); \
::sofa::pbrpc::boost::mpl::aux::no_tag operator|( \
      ::sofa::pbrpc::boost::mpl::aux::has_rebind_tag<int> \
    , name< BOOST_MPL_PP_ENUM(i,::sofa::pbrpc::boost::mpl::na) >* \
    ); \
/**/
#elif !BOOST_WORKAROUND(BOOST_MSVC, < 1300)
#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
template< BOOST_MPL_PP_PARAMS(i,typename T) > \
::sofa::pbrpc::boost::mpl::aux::yes_tag operator|( \
      ::sofa::pbrpc::boost::mpl::aux::has_rebind_tag<int> \
    , ::sofa::pbrpc::boost::mpl::aux::has_rebind_tag< name<BOOST_MPL_PP_PARAMS(i,T)> >* \
    ); \
/**/
#else
#   define BOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) /**/
#endif

#   if !defined(__BORLANDC__)
#   define BOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
BOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
class BOOST_PP_CAT(name,_rebind) \
{ \
 public: \
    template< BOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
        : name< BOOST_MPL_PP_PARAMS(i,U) > \
    { \
    }; \
/**/
#   else
#   define BOOST_MPL_AUX_LAMBDA_SUPPORT(i, name, params) \
    BOOST_MPL_AUX_LAMBDA_SUPPORT_SPEC(i, name, params) \
}; \
BOOST_MPL_AUX_LAMBDA_SUPPORT_HAS_REBIND(i, name, params) \
class BOOST_PP_CAT(name,_rebind) \
{ \
 public: \
    template< BOOST_MPL_PP_PARAMS(i,typename U) > struct apply \
    { \
        typedef typename name< BOOST_MPL_PP_PARAMS(i,U) >::type type; \
    }; \
/**/
#   endif // __BORLANDC__

#endif // __EDG_VERSION__

#endif // BOOST_MPL_CFG_NO_FULL_LAMBDA_SUPPORT

#endif // SOFA_PBRPC_BOOST_MPL_AUX_LAMBDA_SUPPORT_HPP_INCLUDED
