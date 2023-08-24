// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)

// This file is modified from boost.
//
// Copyright Beman Dawes 2002, 2006
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See library home page at http://www.boost.org/libs/system

#ifndef _SOFA_PBRPC_SMART_PTR_DETAIL_SP_COUNTED_BASE_GCC_LOONGARCH64_
#define _SOFA_PBRPC_SMART_PTR_DETAIL_SP_COUNTED_BASE_GCC_LOONGARCH64_

#include <typeinfo>
#include <iostream>
namespace sofa {
namespace pbrpc {
namespace detail {

inline int atomic_exchange_and_add( int * pw, int dv )
{
    // int r = *pw;
    // *pw += dv;
    // return r;


    int r ;
    __asm__ __volatile__
    (
       "ll.w $r12, %0\n\t"
       "add.w %2, $r0, $r12\n\t"
       "sc.w $r12, %0\n\t"
       "ll.w $r12, %0\n\t"
       "add.w $r12, $r12, %1\n\t"
       "sc.w $r12, %0"
       :"+m"( *pw ), "+r"( dv ), "+r"(r)
       ::"memory", "cc", "$r12"
    );
    return r;
}

inline void atomic_increment( int * pw )
{
  atomic_exchange_and_add( pw, 1 );
}

inline int atomic_conditional_increment( int * pw )
{
    // int rv = *pw;
    // if( rv != 0 ) ++*pw;
    // return rv;

    int rv, tmp;
    asm(
                      "ll.w $r12, %0\n\t"
                      "add.w %1, $r0, $r12\n\t"
                      "sc.w $r12, %0\n\t"
                      : "+m"(*pw), "+r"(rv)
                      :: "cc", "$r12"
                      );
    asm(
		      "ll.w $r12, %0\n\t"
		      "beqz %1, L\n\t"
		      "addi.w $r12, $r12, 1\n\t"
		      "sc.w $r12, %0\n\t"
		      "L:nop\n\t"
		      : "+m"(*pw), "+r"(rv), "+r"(tmp)
		      :: "cc", "$r12"
		      );
    return rv;
}

class sp_counted_base
{
private:

    sp_counted_base( sp_counted_base const & );
    sp_counted_base & operator= ( sp_counted_base const & );

    int use_count_;        // #shared
    int weak_count_;       // #weak + (#shared != 0)

public:

    sp_counted_base(): use_count_( 1 ), weak_count_( 1 )
    {
    }

    virtual ~sp_counted_base() // nothrow
    {
    }

    // dispose() is called when use_count_ drops to zero, to release
    // the resources managed by *this.

    virtual void dispose() = 0; // nothrow

    // destroy() is called when weak_count_ drops to zero.

    virtual void destroy() // nothrow
    {
        delete this;
    }

    virtual void * get_deleter( std::type_info const & ti ) = 0;

    void add_ref_copy()
    {
	atomic_increment( &use_count_ );
    }

    bool add_ref_lock() // true on success
    {
        return atomic_conditional_increment( &use_count_ ) != 0;
    }

    void release() // nothrow
    {
        if( atomic_exchange_and_add( &use_count_, -1 ) == 1 )
        {
            dispose();
            weak_release();
        }
    }

    void weak_add_ref() // nothrow
    {
        atomic_increment( &weak_count_ );
    }

    void weak_release() // nothrow
    {
        if( atomic_exchange_and_add( &weak_count_, -1 ) == 1 )
        {
            destroy();
        }
    }

    long use_count() const // nothrow
    {
        return static_cast<int const volatile &>( use_count_ );
    }
};

} // namespace detail
} // namespace pbrpc
} // namespace sofa

#endif _SOFA_PBRPC_SMART_PTR_DETAIL_SP_COUNTED_BASE_GCC_LOONGARCH64_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
