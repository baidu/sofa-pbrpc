#ifndef SOFA_PBRPC_BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED

// MS compatible compilers support #pragma once

#if defined(_MSC_VER) && (_MSC_VER >= 1020)
# pragma once
#endif

//
//  bind/placeholders.hpp - _N definitions
//
//  Copyright (c) 2002 Peter Dimov and Multi Media Ltd.
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/bind/bind.html for documentation.
//

#include <sofa/pbrpc/boost/bind/arg.hpp>
#include <sofa/pbrpc/boost/config.hpp>

namespace
{

#if defined(__BORLANDC__) || defined(__GNUC__) && (__GNUC__ < 4)

static inline sofa::pbrpc::boost::arg<1> _1() { return sofa::pbrpc::boost::arg<1>(); }
static inline sofa::pbrpc::boost::arg<2> _2() { return sofa::pbrpc::boost::arg<2>(); }
static inline sofa::pbrpc::boost::arg<3> _3() { return sofa::pbrpc::boost::arg<3>(); }
static inline sofa::pbrpc::boost::arg<4> _4() { return sofa::pbrpc::boost::arg<4>(); }
static inline sofa::pbrpc::boost::arg<5> _5() { return sofa::pbrpc::boost::arg<5>(); }
static inline sofa::pbrpc::boost::arg<6> _6() { return sofa::pbrpc::boost::arg<6>(); }
static inline sofa::pbrpc::boost::arg<7> _7() { return sofa::pbrpc::boost::arg<7>(); }
static inline sofa::pbrpc::boost::arg<8> _8() { return sofa::pbrpc::boost::arg<8>(); }
static inline sofa::pbrpc::boost::arg<9> _9() { return sofa::pbrpc::boost::arg<9>(); }

#elif defined(BOOST_MSVC) || (defined(__DECCXX_VER) && __DECCXX_VER <= 60590031) || defined(__MWERKS__) || \
    defined(__GNUC__) && (__GNUC__ == 4 && __GNUC_MINOR__ < 2)  

static sofa::pbrpc::boost::arg<1> _1;
static sofa::pbrpc::boost::arg<2> _2;
static sofa::pbrpc::boost::arg<3> _3;
static sofa::pbrpc::boost::arg<4> _4;
static sofa::pbrpc::boost::arg<5> _5;
static sofa::pbrpc::boost::arg<6> _6;
static sofa::pbrpc::boost::arg<7> _7;
static sofa::pbrpc::boost::arg<8> _8;
static sofa::pbrpc::boost::arg<9> _9;

#else

sofa::pbrpc::boost::arg<1> _1;
sofa::pbrpc::boost::arg<2> _2;
sofa::pbrpc::boost::arg<3> _3;
sofa::pbrpc::boost::arg<4> _4;
sofa::pbrpc::boost::arg<5> _5;
sofa::pbrpc::boost::arg<6> _6;
sofa::pbrpc::boost::arg<7> _7;
sofa::pbrpc::boost::arg<8> _8;
sofa::pbrpc::boost::arg<9> _9;

#endif

} // unnamed namespace

#endif // #ifndef SOFA_PBRPC_BOOST_BIND_PLACEHOLDERS_HPP_INCLUDED
