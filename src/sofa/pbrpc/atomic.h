// Copyright (c) 2014 Baidu.com, Inc. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.
//
// Author: qinzuoyan01@baidu.com (Qin Zuoyan)
// `atomic.h` serves as the header file for inline assembly functions in this project, providing support for assembly statements. 
// Currently, some functions that support LoongArch architecture assembly instructions have been modified, including `atomic_add_ret_old`, `atomic_inc_ret_old`, and `atomic_dec_ret_old`.
//
#ifndef _SOFA_PBRPC_ATOMIC_H_
#define _SOFA_PBRPC_ATOMIC_H_

#if !defined(__i386__) && !defined(__x86_64__) && !defined(__loongarch__)
#error    "Arch not supprot!"
#endif

#include <stdint.h>
#include <iostream>
namespace sofa {
namespace pbrpc {

template <typename T>
inline void atomic_inc(volatile T* n)
{
  
    //std::cout<< "atomic_inc()" << std::endl;	
    // asm volatile (" incl %0;":"+m"(*n)::"cc");
    asm volatile (
		    "ll.w $r12, %0\n\t"
		    "addi.w $r12, $r12, 1\n\t"
		    "sc.w $r12, %0"
		    :"+m"(*n)
		    ::"cc", "$r12"
		    );
}
template <typename T>
inline void atomic_dec(volatile T* n)
{
    //std::cout<< "atomic_dec()" << std::endl;
    //asm volatile (" decl %0;":"+m"(*n)::"cc");
    asm volatile (
                    "ll.w $r12, %0\n\t"
                    "addi.w $r12, $r12, -1\n\t"
                    "sc.w $r12, %0"
                    :"+m"(*n)
                    ::"cc", "$r12"
                    );
}
template <typename T>
inline T atomic_add_ret_old(volatile T* n, T v)
{
    T* k = &v;
    T temp = *n;
    //std::cout<< "atomic_add_ret_old()" << std::endl;
    //std::cout <<"n=" <<*n <<"v="<<v<<"temp="<<temp<<std::endl;
    //asm volatile (" xaddl %1, %0;":"+m"(*n),"+r"(v)::"cc");
    asm volatile(
    "ll.w $r12, %[n]\n\t"
    "add.w $r12, $r12, %[v]\n\t"
    "add.w %[v], $r0, %[temp]\n\t"
    "sc.w  $r12, %[n] \n\t"
    : [n] "+m" (*n), [v]"+r"(v), [k] "+m" (*k),[temp]"+r"(temp)
    :: "$r12", "$r13", "cc"
    );
    //std::cout <<"n=" <<*n <<"v="<<v<<"temp"<<temp<<std::endl; 
    return v;
}
template <typename T>
inline T atomic_inc_ret_old(volatile T* n)
{
    T r = 1;
    T* k = &r;
    T temp = *n;
    //std::cout<< "atomic_inc_ret_old()" << std::endl;
    //std::cout <<"n=" <<*n <<"r="<<r<<std::endl;
    // asm volatile (" xaddl %1, %0;":"+m"(*n), "+r"(r)::"cc");
    asm volatile(
    "ll.w $r12, %[n]\n\t"
    "add.w $r12, $r12, %[r]\n\t"
    "add.w %[r], $r0, %[temp]\n\t"
    "sc.w  $r12, %[n] \n\t"
    : [n] "+m" (*n), [r]"+r"(r), [k] "+m" (*k),[temp]"+r"(temp)
    :: "$r12", "$r13", "cc"
    );
    //std::cout <<"n=" <<*n <<"r="<<r<<std::endl;
    return r;
}
template <typename T>
inline T atomic_dec_ret_old(volatile T* n)
{
    T r = (T)-1;
    T* k = &r;
    T temp = *n;
    //std::cout<< "atomic_dec_ret_old()" << std::endl;
    //std::cout <<"n=" <<*n <<"r="<<r<<std::endl;
    //asm volatile (" xaddl %1, %0;":"+m"(*n), "+r"(r)::"cc");
    asm volatile(
    "ll.w $r12, %[n]\n\t"
    "add.w $r12, $r12, %[r]\n\t"
    "add.w %[r], $r0, %[temp]\n\t"
    "sc.w  $r12, %[n] \n\t"
    : [n] "+m" (*n), [r]"+r"(r), [k] "+m" (*k),[temp]"+r"(temp)
    :: "$r12", "$r13", "cc"
    );
    //std::cout <<"n=" <<*n <<"r="<<r<<std::endl;
    return r;
}
template <typename T>
inline T atomic_add_ret_old64(volatile T* n, T v)
{
    //std::cout<< "atomic_add_ret_old64()" << std::endl;
    //asm volatile (" xaddq %1, %0;":"+m"(*n),"+r"(v)::"cc");
    return v;
}
template <typename T>
inline T atomic_inc_ret_old64(volatile T* n)
{
    T r = 1;
    //std::cout<< "atomic_inc_ret_old64()" << std::endl;
    //asm volatile (" xaddq %1, %0;":"+m"(*n), "+r"(r)::"cc");
    return r;
}
template <typename T>
inline T atomic_dec_ret_old64(volatile T* n)
{
    //std::cout<< "atomic_dec_ret_old64()" << std::endl;
    T r = (T)-1;
    //asm volatile ("xaddq %1, %0;":"+m"(*n), "+r"(r)::"cc");
    return r;
}
template <typename T>
inline void atomic_add(volatile T* n, T v)
{
    //std::cout<< "atomic_add()" << std::endl;
    //asm volatile (" addl %1, %0;":"+m"(*n):"r"(v):"cc");
    T* k = &v;
    asm volatile (
	"ll.w $r12, %1"	
	"add.w $r12, %0, %1"
	"sc.w $r12, %2"
	: "+m"(*n), "+r"(v), "+m"(k)
	::"cc"
		    );
}
template <typename T>
inline void atomic_sub(volatile T* n, T v)
{
    std::cout<< "atomic_sub()" << std::endl;
    //asm volatile (" subl %1, %0;":"+m"(*n):"r"(v):"cc");
}
template <typename T, typename C, typename D>
inline T atomic_cmpxchg(volatile T* n, C cmp, D dest)
{
    //std::cout<< "atomic_cmpxchg()" << std::endl;
    //std::cout<< "n=" << *n << "cmp=" << cmp << "dest=" << dest << std::endl;
    //asm volatile (" cmpxchgl %1, %0":"+m"(*n), "+r"(dest), "+a"(cmp)::"cc");
    // n == cmp ? n=dest:cmp=n
    //C* k = &cmp;
    if (*n == cmp) {
      *n = dest;
    } else {
      cmp = *n;
    }
   // asm volatile (
   //		    " ll.w $r12, %0\n\t"
   //		    " beq  $r12, %2, equal \n\t"
   //		    //" b end\n\t"
   //                    
   //		    //" end:\n\t"
                    //" sc.w $r12, %3\n\t"

   //		    " equal:\n\t"
		    //" sc.w %1, %0\n\t"
		    
   //		    :"+m"(*n), "+r"(dest), "+r"(cmp), "+m"(*k)
   //		    ::"cc"
   //		    );
    //std::cout<< "n=" << *n << "cmp=" << cmp << "dest=" << dest << std::endl;
    return cmp;
}
// return old value
template <typename T>
inline T atomic_swap(volatile T* lockword, T value)
{
    //std::cout<< "atomic_swap()" << std::endl;
    T temp = *lockword;
    //asm volatile (" xchg %0, %1;" : "+r"(value), "+m"(*lockword));
    asm volatile(
	"sc.w  %0, %1 \n\t"
        "add.w %0, $r0, %2\n\t"
	:"+r"(value), "+m"(*lockword), "+r"(temp)
		    );
    return value;
}
template <typename T, typename E, typename C>
inline T atomic_comp_swap(volatile T* lockword, E exchange, C comperand)
{
    //std::cout<< "atomic_comp_swap()" << std::endl;
    return atomic_cmpxchg(lockword, comperand, exchange);
}

} // namespace pbrpc
} // namespace sofa

#endif // _SOFA_PBRPC_ATOMIC_H_

/* vim: set ts=4 sw=4 sts=4 tw=100 */
