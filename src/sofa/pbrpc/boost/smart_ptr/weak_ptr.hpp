#ifndef SOFA_PBRPC_BOOST_SMART_PTR_WEAK_PTR_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_SMART_PTR_WEAK_PTR_HPP_INCLUDED

//
//  weak_ptr.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/weak_ptr.htm for documentation.
//

#include <memory> // boost.TR1 include order fix
#include <sofa/pbrpc/boost/smart_ptr/detail/shared_count.hpp>
#include <sofa/pbrpc/boost/smart_ptr/shared_ptr.hpp>

namespace sofa {
namespace pbrpc {
namespace boost
{

template<class T> class weak_ptr
{
private:

    typedef weak_ptr<T> this_type;

public:

    typedef typename sofa::pbrpc::boost::detail::sp_element< T >::type element_type;

    weak_ptr() : px(0), pn() // never throws in 1.30+
    {
    }

    template<class Y>
    weak_ptr( weak_ptr<Y> const & r, typename sofa::pbrpc::boost::detail::sp_enable_if_convertible<Y,T>::type = sofa::pbrpc::boost::detail::sp_empty() )

     : px(r.lock().get()), pn(r.pn)
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();
    }

    template<class Y>
    weak_ptr( shared_ptr<Y> const & r, typename sofa::pbrpc::boost::detail::sp_enable_if_convertible<Y,T>::type = sofa::pbrpc::boost::detail::sp_empty() )

    : px( r.px ), pn( r.pn )
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();
    }

    template<class Y>
    weak_ptr & operator=( weak_ptr<Y> const & r )
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();

        px = r.lock().get();
        pn = r.pn;

        return *this;
    }

    template<class Y>
    weak_ptr & operator=( shared_ptr<Y> const & r )
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();

        px = r.px;
        pn = r.pn;

        return *this;
    }

    shared_ptr<T> lock() const
    {
        return shared_ptr<T>( *this, sofa::pbrpc::boost::detail::sp_nothrow_tag() );
    }

    long use_count() const
    {
        return pn.use_count();
    }

    bool expired() const
    {
        return pn.use_count() == 0;
    }

    bool _empty() const // extension, not in std::weak_ptr
    {
        return pn.empty();
    }

    void reset() // never throws in 1.30+
    {
        this_type().swap(*this);
    }

    void swap(this_type & other)
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    template<typename Y>
    void _internal_aliasing_assign(weak_ptr<Y> const & r, element_type * px2)
    {
        px = px2;
        pn = r.pn;
    }

    template<class Y> bool owner_before( weak_ptr<Y> const & rhs ) const
    {
        return pn < rhs.pn;
    }

    template<class Y> bool owner_before( shared_ptr<Y> const & rhs ) const
    {
        return pn < rhs.pn;
    }

// Tasteless as this may seem, making all members public allows member templates
// to work in the absence of member template friends. (Matthew Langston)

private:

    template<class Y> friend class weak_ptr;
    template<class Y> friend class shared_ptr;


    element_type * px;            // contained pointer
    sofa::pbrpc::boost::detail::weak_count pn; // reference counter

};  // weak_ptr

template<class T, class U> inline bool operator<(weak_ptr<T> const & a, weak_ptr<U> const & b)
{
    return a.owner_before( b );
}

template<class T> void swap(weak_ptr<T> & a, weak_ptr<T> & b)
{
    a.swap(b);
}

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif  // #ifndef SOFA_PBRPC_BOOST_SMART_PTR_WEAK_PTR_HPP_INCLUDED
