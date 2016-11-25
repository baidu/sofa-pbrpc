// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

// This file is modified from boost.
//
// Copyright Beman Dawes 2002, 2006
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// See library home page at http://www.boost.org/libs/system

#ifndef _SOFA_PBRPC_SMART_PTR_SCOPED_ARRAY_
#define _SOFA_PBRPC_SMART_PTR_SCOPED_ARRAY_

#include <sofa/pbrpc/smart_ptr/checked_delete.hpp>

#include <cstddef> // for std::ptrdiff_t

namespace sofa {
namespace pbrpc {

//  scoped_array extends scoped_ptr to arrays. Deletion of the array pointed to
//  is guaranteed, either on destruction of the scoped_array or via an explicit
//  reset(). Use shared_array or std::vector if your needs are more complex.

template<class T> class scoped_array // noncopyable
{
private:

    T * px;

    scoped_array(scoped_array const &);
    scoped_array & operator=(scoped_array const &);

    typedef scoped_array<T> this_type;

    void operator==( scoped_array const& ) const;
    void operator!=( scoped_array const& ) const;

public:

    typedef T element_type;

    explicit scoped_array( T * p = 0 ) : px( p ) // never throws
    {
    }

    ~scoped_array() // never throws
    {
        sofa::pbrpc::checked_array_delete( px );
    }

    void reset(T * p = 0) // never throws
    {
        this_type(p).swap(*this);
    }

    T & operator[](std::ptrdiff_t i) const // never throws
    {
        return px[i];
    }

    T * get() const // never throws
    {
        return px;
    }

// implicit conversion to "bool"
#include <sofa/pbrpc/smart_ptr/detail/operator_bool.hpp>

    void swap(scoped_array & b) // never throws
    {
        T * tmp = b.px;
        b.px = px;
        px = tmp;
    }
};

template<class T> inline void swap(scoped_array<T> & a, scoped_array<T> & b) // never throws
{
    a.swap(b);
}

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_SMART_PTR_SCOPED_ARRAY_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
