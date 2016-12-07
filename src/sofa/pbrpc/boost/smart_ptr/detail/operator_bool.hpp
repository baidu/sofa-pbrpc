//  This header intentionally has no include guards.
//
//  Copyright (c) 2001-2009, 2012 Peter Dimov
//
//  Distributed under the Boost Software License, Version 1.0.
//  See accompanying file LICENSE_1_0.txt or copy at
//  http://www.boost.org/LICENSE_1_0.txt

    typedef element_type * this_type::*unspecified_bool_type;

    operator unspecified_bool_type() const
    {
        return px == 0? 0: &this_type::px;
    }

    // operator! is redundant, but some compilers need it
    bool operator! () const
    {
        return px == 0;
    }
