// Copyright Peter Dimov and David Abrahams 2002.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
#ifndef SOFA_PBRPC_BOOST_GET_POINTER_DWA20021219_HPP
#define SOFA_PBRPC_BOOST_GET_POINTER_DWA20021219_HPP

#include <sofa/pbrpc/boost/config.hpp>

// In order to avoid circular dependencies with Boost.TR1
// we make sure that our include of <memory> doesn't try to
// pull in the TR1 headers: that's why we use this header 
// rather than including <memory> directly:
#include <sofa/pbrpc/boost/config/no_tr1/memory.hpp>  // std::auto_ptr

namespace sofa {
namespace pbrpc {
namespace boost { 

// get_pointer(p) extracts a ->* capable pointer from p

template<class T> T * get_pointer(T * p)
{
    return p;
}

// get_pointer(shared_ptr<T> const & p) has been moved to shared_ptr.hpp

template<class T> T * get_pointer(std::auto_ptr<T> const& p)
{
    return p.get();
}

#if !defined( BOOST_NO_CXX11_SMART_PTR )

template<class T> T * get_pointer( std::unique_ptr<T> const& p )
{
    return p.get();
}

template<class T> T * get_pointer( std::shared_ptr<T> const& p )
{
    return p.get();
}

#endif

} // namespace boost
} // namespace pbrpc
} // namespace sofa

#endif // SOFA_PBRPC_BOOST_GET_POINTER_DWA20021219_HPP
