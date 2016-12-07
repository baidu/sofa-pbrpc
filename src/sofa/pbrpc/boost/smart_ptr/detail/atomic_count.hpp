#ifndef SOFA_PBRPC_BOOST_SMART_PTR_DETAIL_ATOMIC_COUNT_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_SMART_PTR_DETAIL_ATOMIC_COUNT_HPP_INCLUDED

//
//  boost/detail/atomic_count.hpp - thread/SMP safe reference counter
//
//  Copyright (c) 2001, 2002 Peter Dimov and Multi Media Ltd.
//  Copyright (c) 2013 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt
//
//  typedef <implementation-defined> sofa::pbrpc::boost::detail::atomic_count;
//
//  atomic_count a(n);
//
//    (n is convertible to long)
//
//    Effects: Constructs an atomic_count with an initial value of n
//
//  a;
//
//    Returns: (long) the current value of a
//    Memory Ordering: acquire
//
//  ++a;
//
//    Effects: Atomically increments the value of a
//    Returns: (long) the new value of a
//    Memory Ordering: acquire/release
//
//  --a;
//
//    Effects: Atomically decrements the value of a
//    Returns: (long) the new value of a
//    Memory Ordering: acquire/release
//

//#include <sofa/pbrpc/boost/config.hpp>
//#include <sofa/pbrpc/boost/smart_ptr/detail/sp_has_sync.hpp>
# include <sofa/pbrpc/boost/smart_ptr/detail/atomic_count_gcc_x86.hpp>

#endif // #ifndef SOFA_PBRPC_BOOST_SMART_PTR_DETAIL_ATOMIC_COUNT_HPP_INCLUDED
