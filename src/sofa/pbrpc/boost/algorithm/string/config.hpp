//  Boost string_algo library config.hpp header file  ---------------------------//

//  Copyright Pavol Droba 2002-2003.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/ for updates, documentation, and revision history.

#ifndef SOFA_PBRPC_BOOST_STRING_CONFIG_HPP
#define SOFA_PBRPC_BOOST_STRING_CONFIG_HPP

#include <sofa/pbrpc/boost/config.hpp>
#include <sofa/pbrpc/boost/detail/workaround.hpp>

#ifdef BOOST_STRING_DEDUCED_TYPENAME
#   error "macro already defined!"
#endif

#define BOOST_STRING_TYPENAME BOOST_DEDUCED_TYPENAME

#endif  // SOFA_PBRPC_BOOST_STRING_CONFIG_HPP
