#ifndef SOFA_PBRPC_BOOST_SMART_PTR_SCOPED_ARRAY_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_SMART_PTR_SCOPED_ARRAY_HPP_INCLUDED

//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001, 2002 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  http://www.boost.org/libs/smart_ptr/scoped_array.htm
//

#include <cassert>
#include <sofa/pbrpc/boost/smart_ptr/checked_delete.hpp>
#include <cstddef>            // for std::ptrdiff_t

namespace sofa {
namespace pbrpc {
namespace boost
{

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

    explicit scoped_array( T * p = 0 ) : px( p )
    {
    }

    ~scoped_array() // never throws
    {
        sofa::pbrpc::boost::checked_array_delete( px );
    }

    void reset(T * p = 0) // never throws
    {
        assert( p == 0 || p != px ); // catch self-reset errors
        this_type(p).swap(*this);
    }

    T & operator[](std::ptrdiff_t i) const // never throws
    {
        assert( px != 0 );
        assert( i >= 0 );
        return px[i];
    }

    T * get() const
    {
        return px;
    }

// implicit conversion to "bool"
#include <sofa/pbrpc/boost/smart_ptr/detail/operator_bool.hpp>

    void swap(scoped_array & b)
    {
        T * tmp = b.px;
        b.px = px;
        px = tmp;
    }
};

template<class T> inline void swap(scoped_array<T> & a, scoped_array<T> & b)
{
    a.swap(b);
}

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif  // #ifndef SOFA_PBRPC_BOOST_SMART_PTR_SCOPED_ARRAY_HPP_INCLUDED
