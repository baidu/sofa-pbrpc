#ifndef SOFA_PBRPC_BOOST_SMART_PTR_SHARED_PTR_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_SMART_PTR_SHARED_PTR_HPP_INCLUDED

//
//  shared_ptr.hpp
//
//  (C) Copyright Greg Colvin and Beman Dawes 1998, 1999.
//  Copyright (c) 2001-2008 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0. (See
//  accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt)
//
//  See http://www.boost.org/libs/smart_ptr/shared_ptr.htm for documentation.
//

//#include <sofa/pbrpc/boost/config.hpp>   // for broken compiler workarounds

// In order to avoid circular dependencies with Boost.TR1
// we make sure that our include of <memory> doesn't try to
// pull in the TR1 headers: that's why we use this header 
// rather than including <memory> directly:
//#include <sofa/pbrpc/boost/config/no_tr1/memory.hpp>  // std::auto_ptr

//#include <sofa/pbrpc/boost/assert.hpp>
#include <sofa/pbrpc/boost/smart_ptr/checked_delete.hpp>
//#include <sofa/pbrpc/boost/throw_exception.hpp>
#include <sofa/pbrpc/boost/smart_ptr/detail/shared_count.hpp>
//#include <sofa/pbrpc/boost/detail/workaround.hpp>
#include <sofa/pbrpc/boost/smart_ptr/detail/sp_convertible.hpp>
//#include <sofa/pbrpc/boost/smart_ptr/detail/sp_nullptr_t.hpp>

#include <sofa/pbrpc/boost/smart_ptr/detail/spinlock_pool.hpp>

#include <algorithm>            // for std::swap
#include <functional>           // for std::less
#include <typeinfo>             // for std::bad_cast
#include <cstddef>              // for std::size_t

#include <iosfwd>               // for std::basic_ostream
#include <ostream>

namespace sofa {
namespace pbrpc {
namespace boost
{

template<class T> class shared_ptr;
template<class T> class weak_ptr;
template<class T> class enable_shared_from_this;
class enable_shared_from_raw;

namespace detail
{

// sp_element, element_type

template< class T > struct sp_element
{
    typedef T type;
};


template< class T > struct sp_element< T[] >
{
    typedef T type;
};

// sp_dereference, return type of operator*

template< class T > struct sp_dereference
{
    typedef T & type;
};

template<> struct sp_dereference< void >
{
    typedef void type;
};

template<> struct sp_dereference< void const >
{
    typedef void type;
};

template<> struct sp_dereference< void volatile >
{
    typedef void type;
};

template<> struct sp_dereference< void const volatile >
{
    typedef void type;
};

template< class T > struct sp_dereference< T[] >
{
    typedef void type;
};

// sp_member_access, return type of operator->

template< class T > struct sp_member_access
{
    typedef T * type;
};

template< class T > struct sp_member_access< T[] >
{
    typedef void type;
};

// sp_array_access, return type of operator[]

template< class T > struct sp_array_access
{
    typedef void type;
};

template< class T > struct sp_array_access< T[] >
{
    typedef T & type;
};

// sp_extent, for operator[] index check

template< class T > struct sp_extent
{
    enum _vt { value = 0 };
};

template< class T, std::size_t N > struct sp_extent< T[N] >
{
    enum _vt { value = N };
};

// enable_shared_from_this support

template< class X, class Y, class T > inline void sp_enable_shared_from_this( sofa::pbrpc::boost::shared_ptr<X> const * ppx, Y const * py, sofa::pbrpc::boost::enable_shared_from_this< T > const * pe )
{
    if( pe != 0 )
    {
        pe->_internal_accept_owner( ppx, const_cast< Y* >( py ) );
    }
}

template< class X, class Y > inline void sp_enable_shared_from_this( sofa::pbrpc::boost::shared_ptr<X> * ppx, Y const * py, sofa::pbrpc::boost::enable_shared_from_raw const * pe );

inline void sp_enable_shared_from_this( ... )
{
}

// rvalue auto_ptr support based on a technique by Dave Abrahams

template< class T, class R > struct sp_enable_if_auto_ptr
{
};

template< class T, class R > struct sp_enable_if_auto_ptr< std::auto_ptr< T >, R >
{
    typedef R type;
}; 


// sp_assert_convertible

template< class Y, class T > inline void sp_assert_convertible()
{
    // static_assert( sp_convertible< Y, T >::value );
    typedef char tmp[ sp_convertible< Y, T >::value? 1: -1 ];
    (void)sizeof( tmp );

}

// pointer constructor helper

template< class T, class Y > inline void sp_pointer_construct( sofa::pbrpc::boost::shared_ptr< T > * ppx, Y * p, sofa::pbrpc::boost::detail::shared_count & pn )
{
    sofa::pbrpc::boost::detail::shared_count( p ).swap( pn );
    sofa::pbrpc::boost::detail::sp_enable_shared_from_this( ppx, p, p );
}

template< class T, class Y > inline void sp_pointer_construct( sofa::pbrpc::boost::shared_ptr< T[] > * /*ppx*/, Y * p, sofa::pbrpc::boost::detail::shared_count & pn )
{
    sp_assert_convertible< Y[], T[] >();
    sofa::pbrpc::boost::detail::shared_count( p, sofa::pbrpc::boost::checked_array_deleter< T >() ).swap( pn );
}

template< class T, std::size_t N, class Y > inline void sp_pointer_construct( sofa::pbrpc::boost::shared_ptr< T[N] > * /*ppx*/, Y * p, sofa::pbrpc::boost::detail::shared_count & pn )
{
    sp_assert_convertible< Y[N], T[N] >();
    sofa::pbrpc::boost::detail::shared_count( p, sofa::pbrpc::boost::checked_array_deleter< T >() ).swap( pn );
}

// deleter constructor helper

template< class T, class Y > inline void sp_deleter_construct( sofa::pbrpc::boost::shared_ptr< T > * ppx, Y * p )
{
    sofa::pbrpc::boost::detail::sp_enable_shared_from_this( ppx, p, p );
}

template< class T, class Y > inline void sp_deleter_construct( sofa::pbrpc::boost::shared_ptr< T[] > * /*ppx*/, Y * /*p*/ )
{
    sp_assert_convertible< Y[], T[] >();
}

template< class T, std::size_t N, class Y > inline void sp_deleter_construct( sofa::pbrpc::boost::shared_ptr< T[N] > * /*ppx*/, Y * /*p*/ )
{
    sp_assert_convertible< Y[N], T[N] >();
}

} // namespace detail


//
//  shared_ptr
//
//  An enhanced relative of scoped_ptr with reference counted copy semantics.
//  The object pointed to is deleted when the last shared_ptr pointing to it
//  is destroyed or reset.
//

template<class T> class shared_ptr
{
private:

    // Borland 5.5.1 specific workaround
    typedef shared_ptr<T> this_type;

public:

    typedef typename sofa::pbrpc::boost::detail::sp_element< T >::type element_type;

    shared_ptr() : px( 0 ), pn() // never throws in 1.30+
    {
    }

    template<class Y>
    explicit shared_ptr( Y * p ): px( p ), pn() // Y must be complete
    {
        sofa::pbrpc::boost::detail::sp_pointer_construct( this, p, pn );
    }

    //
    // Requirements: D's copy constructor must not throw
    //
    // shared_ptr will release p by calling d(p)
    //

    template<class Y, class D> shared_ptr( Y * p, D d ): px( p ), pn( p, d )
    {
        sofa::pbrpc::boost::detail::sp_deleter_construct( this, p );
    }

    // As above, but with allocator. A's copy constructor shall not throw.

    template<class Y, class D, class A> shared_ptr( Y * p, D d, A a ): px( p ), pn( p, d, a )
    {
        sofa::pbrpc::boost::detail::sp_deleter_construct( this, p );
    }

//  generated copy constructor, destructor are fine...

    template<class Y>
    explicit shared_ptr( weak_ptr<Y> const & r ): pn( r.pn ) // may throw
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();

        // it is now safe to copy r.px, as pn(r.pn) did not throw
        px = r.px;
    }

    template<class Y>
    shared_ptr( weak_ptr<Y> const & r, sofa::pbrpc::boost::detail::sp_nothrow_tag )
    : px( 0 ), pn( r.pn, sofa::pbrpc::boost::detail::sp_nothrow_tag() )
    {
        if( !pn.empty() )
        {
            px = r.px;
        }
    }

    template<class Y>
    shared_ptr( shared_ptr<Y> const & r, typename sofa::pbrpc::boost::detail::sp_enable_if_convertible<Y,T>::type = sofa::pbrpc::boost::detail::sp_empty() )
    : px( r.px ), pn( r.pn )
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();
    }

    // aliasing
    template< class Y >
    shared_ptr( shared_ptr<Y> const & r, element_type * p ) : px( p ), pn( r.pn )
    {
    }

    template<class Y>
    explicit shared_ptr( std::auto_ptr<Y> & r ): px(r.get()), pn()
    {
        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();

        Y * tmp = r.get();
        pn = sofa::pbrpc::boost::detail::shared_count( r );

        sofa::pbrpc::boost::detail::sp_deleter_construct( this, tmp );
    }

    template<class Ap>
    explicit shared_ptr( Ap r, typename sofa::pbrpc::boost::detail::sp_enable_if_auto_ptr<Ap, int>::type = 0 ): px( r.get() ), pn()
    {
        typedef typename Ap::element_type Y;

        sofa::pbrpc::boost::detail::sp_assert_convertible< Y, T >();

        Y * tmp = r.get();
        pn = sofa::pbrpc::boost::detail::shared_count( r );

        sofa::pbrpc::boost::detail::sp_deleter_construct( this, tmp );
    }

    // assignment

    shared_ptr & operator=( shared_ptr const & r )
    {
        this_type(r).swap(*this);
        return *this;
    }

    template<class Y>
    shared_ptr & operator=(shared_ptr<Y> const & r)
    {
        this_type(r).swap(*this);
        return *this;
    }

    template<class Y>
    shared_ptr & operator=( std::auto_ptr<Y> & r )
    {
        this_type( r ).swap( *this );
        return *this;
    }

    template<class Ap>
    typename sofa::pbrpc::boost::detail::sp_enable_if_auto_ptr< Ap, shared_ptr & >::type operator=( Ap r )
    {
        this_type( r ).swap( *this );
        return *this;
    }

    void reset() // never throws in 1.30+
    {
        this_type().swap(*this);
    }

    template<class Y> void reset( Y * p ) // Y must be complete
    {
        assert( p == 0 || p != px ); // catch self-reset errors
        this_type( p ).swap( *this );
    }

    template<class Y, class D> void reset( Y * p, D d )
    {
        this_type( p, d ).swap( *this );
    }

    template<class Y, class D, class A> void reset( Y * p, D d, A a )
    {
        this_type( p, d, a ).swap( *this );
    }

    template<class Y> void reset( shared_ptr<Y> const & r, element_type * p )
    {
        this_type( r, p ).swap( *this );
    }
    
    // never throws (but has a assert in it, so not marked with BOOST_NOEXCEPT)
    typename sofa::pbrpc::boost::detail::sp_dereference< T >::type operator* () const
    {
        assert( px != 0 );
        return *px;
    }
    
    // never throws (but has a assert in it, so not marked with BOOST_NOEXCEPT)
    typename sofa::pbrpc::boost::detail::sp_member_access< T >::type operator-> () const 
    {
        assert( px != 0 );
        return px;
    }
    
    // never throws (but has a assert in it, so not marked with BOOST_NOEXCEPT)
    typename sofa::pbrpc::boost::detail::sp_array_access< T >::type operator[] ( std::ptrdiff_t i ) const
    {
        assert( px != 0 );
        assert( i >= 0 && ( i < sofa::pbrpc::boost::detail::sp_extent< T >::value || sofa::pbrpc::boost::detail::sp_extent< T >::value == 0 ) );

        return static_cast< typename sofa::pbrpc::boost::detail::sp_array_access< T >::type >( px[ i ] );
    }

    element_type * get() const
    {
        return px;
    }

// implicit conversion to "bool"
#include <sofa/pbrpc/boost/smart_ptr/detail/operator_bool.hpp>

    bool unique() const
    {
        return pn.unique();
    }

    long use_count() const
    {
        return pn.use_count();
    }

    void swap( shared_ptr & other )
    {
        std::swap(px, other.px);
        pn.swap(other.pn);
    }

    template<class Y> bool owner_before( shared_ptr<Y> const & rhs ) const
    {
        return pn < rhs.pn;
    }

    template<class Y> bool owner_before( weak_ptr<Y> const & rhs ) const
    {
        return pn < rhs.pn;
    }

    void * _internal_get_deleter( std::type_info const & ti ) const
    {
        return pn.get_deleter( ti );
    }

    void * _internal_get_untyped_deleter() const
    {
        return pn.get_untyped_deleter();
    }

    bool _internal_equiv( shared_ptr const & r ) const
    {
        return px == r.px && pn == r.pn;
    }

// Tasteless as this may seem, making all members public allows member templates
// to work in the absence of member template friends. (Matthew Langston)

private:

    template<class Y> friend class shared_ptr;
    template<class Y> friend class weak_ptr;


    element_type * px;                 // contained pointer
    sofa::pbrpc::boost::detail::shared_count pn;    // reference counter

};  // shared_ptr

template<class T, class U> inline bool operator==(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() == b.get();
}

template<class T, class U> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.get() != b.get();
}

// Resolve the ambiguity between our op!= and the one in rel_ops

template<class T> inline bool operator!=(shared_ptr<T> const & a, shared_ptr<T> const & b)
{
    return a.get() != b.get();
}

template<class T, class U> inline bool operator<(shared_ptr<T> const & a, shared_ptr<U> const & b)
{
    return a.owner_before( b );
}

template<class T> inline void swap(shared_ptr<T> & a, shared_ptr<T> & b)
{
    a.swap(b);
}

template<class T, class U> shared_ptr<T> static_pointer_cast( shared_ptr<U> const & r )
{
    (void) static_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename shared_ptr<T>::element_type E;

    E * p = static_cast< E* >( r.get() );
    return shared_ptr<T>( r, p );
}

template<class T, class U> shared_ptr<T> const_pointer_cast( shared_ptr<U> const & r )
{
    (void) const_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename shared_ptr<T>::element_type E;

    E * p = const_cast< E* >( r.get() );
    return shared_ptr<T>( r, p );
}

template<class T, class U> shared_ptr<T> dynamic_pointer_cast( shared_ptr<U> const & r )
{
    (void) dynamic_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename shared_ptr<T>::element_type E;

    E * p = dynamic_cast< E* >( r.get() );
    return p? shared_ptr<T>( r, p ): shared_ptr<T>();
}

template<class T, class U> shared_ptr<T> reinterpret_pointer_cast( shared_ptr<U> const & r )
{
    (void) reinterpret_cast< T* >( static_cast< U* >( 0 ) );

    typedef typename shared_ptr<T>::element_type E;

    E * p = reinterpret_cast< E* >( r.get() );
    return shared_ptr<T>( r, p );
}

// get_pointer() enables sofa::pbrpc::boost::mem_fn to recognize shared_ptr

template<class T> inline typename shared_ptr<T>::element_type * get_pointer(shared_ptr<T> const & p)
{
    return p.get();
}

// operator<<


template<class E, class T, class Y> std::basic_ostream<E, T> & operator<< (std::basic_ostream<E, T> & os, shared_ptr<Y> const & p)
{
    os << p.get();
    return os;
}

// get_deleter

namespace detail
{

template<class D, class T> D * basic_get_deleter(shared_ptr<T> const & p)
{
    void const * q = p._internal_get_deleter(typeid(D));
    return const_cast<D *>(static_cast<D const *>(q));
}

class esft2_deleter_wrapper
{
private:

    shared_ptr<void const volatile> deleter_;

public:

    esft2_deleter_wrapper()
    {
    }

    template< class T > void set_deleter( shared_ptr<T> const & deleter )
    {
        deleter_ = deleter;
    }

    template<typename D> D* get_deleter() const
    {
        return sofa::pbrpc::boost::detail::basic_get_deleter<D>( deleter_ );
    }

    template< class T> void operator()( T* )
    {
        assert( deleter_.use_count() <= 1 );
        deleter_.reset();
    }
};

} // namespace detail

template<class D, class T> D * get_deleter( shared_ptr<T> const & p )
{
    D *del = sofa::pbrpc::boost::detail::basic_get_deleter<D>(p);

    if(del == 0)
    {
        sofa::pbrpc::boost::detail::esft2_deleter_wrapper *del_wrapper = sofa::pbrpc::boost::detail::basic_get_deleter<sofa::pbrpc::boost::detail::esft2_deleter_wrapper>(p);
// The following get_deleter method call is fully qualified because
// older versions of gcc (2.95, 3.2.3) fail to compile it when written del_wrapper->get_deleter<D>()
        if(del_wrapper) del = del_wrapper->::sofa::pbrpc::boost::detail::esft2_deleter_wrapper::get_deleter<D>();
    }

    return del;
}

// atomic access

template<class T> inline bool atomic_is_lock_free( shared_ptr<T> const * /*p*/ )
{
    return false;
}

template<class T> shared_ptr<T> atomic_load( shared_ptr<T> const * p )
{
    sofa::pbrpc::boost::detail::spinlock_pool<2>::scoped_lock lock( p );
    return *p;
}

template<class T> inline shared_ptr<T> atomic_load_explicit( shared_ptr<T> const * p, /*memory_order mo*/ int )
{
    return atomic_load( p );
}

template<class T> void atomic_store( shared_ptr<T> * p, shared_ptr<T> r )
{
    sofa::pbrpc::boost::detail::spinlock_pool<2>::scoped_lock lock( p );
    p->swap( r );
}

template<class T> inline void atomic_store_explicit( shared_ptr<T> * p, shared_ptr<T> r, /*memory_order mo*/ int )
{
    atomic_store( p, r ); // std::move( r )
}

template<class T> shared_ptr<T> atomic_exchange( shared_ptr<T> * p, shared_ptr<T> r )
{
    sofa::pbrpc::boost::detail::spinlock & sp = sofa::pbrpc::boost::detail::spinlock_pool<2>::spinlock_for( p );

    sp.lock();
    p->swap( r );
    sp.unlock();

    return r; // return std::move( r )
}

template<class T> shared_ptr<T> atomic_exchange_explicit( shared_ptr<T> * p, shared_ptr<T> r, /*memory_order mo*/ int )
{
    return atomic_exchange( p, r ); // std::move( r )
}

template<class T> bool atomic_compare_exchange( shared_ptr<T> * p, shared_ptr<T> * v, shared_ptr<T> w )
{
    sofa::pbrpc::boost::detail::spinlock & sp = sofa::pbrpc::boost::detail::spinlock_pool<2>::spinlock_for( p );

    sp.lock();

    if( p->_internal_equiv( *v ) )
    {
        p->swap( w );

        sp.unlock();

        return true;
    }
    else
    {
        shared_ptr<T> tmp( *p );

        sp.unlock();

        tmp.swap( *v );
        return false;
    }
}

template<class T> inline bool atomic_compare_exchange_explicit( shared_ptr<T> * p, shared_ptr<T> * v, shared_ptr<T> w, /*memory_order success*/ int, /*memory_order failure*/ int )
{
    return atomic_compare_exchange( p, v, w ); // std::move( w )
}

// hash_value

template< class T > struct hash;

template< class T > std::size_t hash_value( sofa::pbrpc::boost::shared_ptr<T> const & p )
{
    return sofa::pbrpc::boost::hash< T* >()( p.get() );
}

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif  // #ifndef SOFA_PBRPC_BOOST_SMART_PTR_SHARED_PTR_HPP_INCLUDED
