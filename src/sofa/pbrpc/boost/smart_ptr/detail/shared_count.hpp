#ifndef SOFA_PBRPC_BOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED
#define SOFA_PBRPC_BOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED

// MS compatible compilers support #pragma once

//
//  detail/shared_count.hpp
//
//  Copyright (c) 2001, 2002, 2003 Peter Dimov and Multi Media Ltd.
//  Copyright 2004-2005 Peter Dimov
//
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//

#include <sofa/pbrpc/boost/smart_ptr/checked_delete.hpp>
#include <sofa/pbrpc/boost/smart_ptr/bad_weak_ptr.hpp>
#include <sofa/pbrpc/boost/smart_ptr/detail/sp_counted_base.hpp>
#include <sofa/pbrpc/boost/smart_ptr/detail/sp_counted_impl.hpp>
#include <functional>       // std::less
#include <typeinfo>

//#ifdef BOOST_NO_EXCEPTIONS
//# include <new>              // std::bad_alloc
//#endif

namespace sofa {
namespace pbrpc {
namespace boost
{

namespace detail
{

//#if defined(BOOST_SP_ENABLE_DEBUG_HOOKS)
//
//int const shared_count_id = 0x2C35F101;
//int const   weak_count_id = 0x298C38A4;
//
//#endif

struct sp_nothrow_tag {};

template< class D > struct sp_inplace_tag
{
};

class weak_count;

class shared_count
{
private:

    sp_counted_base * pi_;

    friend class weak_count;

public:

    shared_count(): pi_(0) // nothrow
    {
    }

    template<class Y> explicit shared_count( Y * p ): pi_( 0 )
    {

        try
        {
            pi_ = new sp_counted_impl_p<Y>( p );
        }
        catch(...)
        {
            sofa::pbrpc::boost::checked_delete( p );
            throw;
        }
    }

    template<class P, class D> shared_count( P p, D d ): pi_(0)
    {
        try
        {
            pi_ = new sp_counted_impl_pd<P, D>(p, d);
        }
        catch(...)
        {
            d(p); // delete p
            throw;
        }
    }

    template< class P, class D > shared_count( P p, sp_inplace_tag<D> ): pi_( 0 )
    {
        try
        {
            pi_ = new sp_counted_impl_pd< P, D >( p );
        }
        catch( ... )
        {
            D::operator_fn( p ); // delete p
            throw;
        }
    }

    template<class P, class D, class A> shared_count( P p, D d, A a ): pi_( 0 )
    {
        typedef sp_counted_impl_pda<P, D, A> impl_type;

        typedef typename A::template rebind< impl_type >::other A2;

        A2 a2( a );

        try
        {
            pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
            ::new( static_cast< void* >( pi_ ) ) impl_type( p, d, a );

        }
        catch(...)
        {
            d( p );

            if( pi_ != 0 )
            {
                a2.deallocate( static_cast< impl_type* >( pi_ ), 1 );
            }

            throw;
        }
    }

    template< class P, class D, class A > shared_count( P p, sp_inplace_tag< D >, A a ): pi_( 0 )
    {
        typedef sp_counted_impl_pda< P, D, A > impl_type;
        typedef typename A::template rebind< impl_type >::other A2;

        A2 a2( a );

        try
        {
            pi_ = a2.allocate( 1, static_cast< impl_type* >( 0 ) );
            ::new( static_cast< void* >( pi_ ) ) impl_type( p, a );

        }
        catch(...)
        {
            D::operator_fn( p );

            if( pi_ != 0 )
            {
                a2.deallocate( static_cast< impl_type* >( pi_ ), 1 );
            }

            throw;
        }
    }

//#ifndef BOOST_NO_AUTO_PTR
//
//    // auto_ptr<Y> is special cased to provide the strong guarantee
//
//    template<class Y>
//    explicit shared_count( std::auto_ptr<Y> & r ): pi_( new sp_counted_impl_p<Y>( r.get() ) )
//    {
//#ifdef BOOST_NO_EXCEPTIONS
//
//        if( pi_ == 0 )
//        {
//            //sofa::pbrpc::boost::throw_exception(std::bad_alloc());
//            throw(std::bad_alloc());
//        }
//
//#endif
//
//        r.release();
//    }
//
//#endif 

//#if !defined( BOOST_NO_CXX11_SMART_PTR )
//
//    template<class Y, class D>
//    explicit shared_count( std::unique_ptr<Y, D> & r ): pi_( 0 )
//    {
//        typedef typename sp_convert_reference<D>::type D2;
//
//        D2 d2( r.get_deleter() );
//        pi_ = new sp_counted_impl_pd< typename std::unique_ptr<Y, D>::pointer, D2 >( r.get(), d2 );
//
//#ifdef BOOST_NO_EXCEPTIONS
//
//        if( pi_ == 0 )
//        {
//            //sofa::pbrpc::boost::throw_exception( std::bad_alloc() );
//            throw(std::bad_alloc());
//        }
//
//#endif
//
//        r.release();
//    }
//
//#endif

    ~shared_count() // nothrow
    {
        if( pi_ != 0 ) pi_->release();
    }

    shared_count(shared_count const & r): pi_(r.pi_) // nothrow
    {
        if( pi_ != 0 ) pi_->add_ref_copy();
    }

//#if !defined( BOOST_NO_CXX11_RVALUE_REFERENCES )
//
//    shared_count(shared_count && r): pi_(r.pi_) // nothrow
//    {
//        r.pi_ = 0;
//    }
//
//#endif

    explicit shared_count(weak_count const & r); // throws bad_weak_ptr when r.use_count() == 0
    shared_count( weak_count const & r, sp_nothrow_tag ); // constructs an empty *this when r.use_count() == 0

    shared_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if( tmp != 0 ) tmp->add_ref_copy();
            if( pi_ != 0 ) pi_->release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(shared_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool unique() const // nothrow
    {
        return use_count() == 1;
    }

    bool empty() const // nothrow
    {
        return pi_ == 0;
    }

    friend inline bool operator==(shared_count const & a, shared_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(shared_count const & a, shared_count const & b)
    {
        return std::less<sp_counted_base *>()( a.pi_, b.pi_ );
    }

    void * get_deleter( std::type_info const & ti ) const
    {
        return pi_? pi_->get_deleter( ti ): 0;
    }

    void * get_untyped_deleter() const
    {
        return pi_? pi_->get_untyped_deleter(): 0;
    }
};


class weak_count
{
private:

    sp_counted_base * pi_;

    friend class shared_count;

public:

    weak_count(): pi_(0) // nothrow
    {
    }

    weak_count(shared_count const & r): pi_(r.pi_) // nothrow
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    weak_count(weak_count const & r): pi_(r.pi_) // nothrow
    {
        if(pi_ != 0) pi_->weak_add_ref();
    }

    ~weak_count() // nothrow
    {
        if(pi_ != 0) pi_->weak_release();
    }

    weak_count & operator= (shared_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if(tmp != 0) tmp->weak_add_ref();
            if(pi_ != 0) pi_->weak_release();
            pi_ = tmp;
        }

        return *this;
    }

    weak_count & operator= (weak_count const & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;

        if( tmp != pi_ )
        {
            if(tmp != 0) tmp->weak_add_ref();
            if(pi_ != 0) pi_->weak_release();
            pi_ = tmp;
        }

        return *this;
    }

    void swap(weak_count & r) // nothrow
    {
        sp_counted_base * tmp = r.pi_;
        r.pi_ = pi_;
        pi_ = tmp;
    }

    long use_count() const // nothrow
    {
        return pi_ != 0? pi_->use_count(): 0;
    }

    bool empty() const // nothrow
    {
        return pi_ == 0;
    }

    friend inline bool operator==(weak_count const & a, weak_count const & b)
    {
        return a.pi_ == b.pi_;
    }

    friend inline bool operator<(weak_count const & a, weak_count const & b)
    {
        return std::less<sp_counted_base *>()(a.pi_, b.pi_);
    }
};

inline shared_count::shared_count( weak_count const & r ): pi_( r.pi_ )
{
    if( pi_ == 0 || !pi_->add_ref_lock() )
    {
        //sofa::pbrpc::boost::throw_exception( sofa::pbrpc::boost::bad_weak_ptr() );
        throw sofa::pbrpc::boost::bad_weak_ptr();
    }
}

inline shared_count::shared_count( weak_count const & r, sp_nothrow_tag ): pi_( r.pi_ )
{
    if( pi_ != 0 && !pi_->add_ref_lock() )
    {
        pi_ = 0;
    }
}

} // namespace detail

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif  // #ifndef SOFA_PBRPC_BOOST_SMART_PTR_DETAIL_SHARED_COUNT_HPP_INCLUDED
