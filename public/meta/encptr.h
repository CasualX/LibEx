#ifndef HGUARD_LIBEX_META_ENCPTR
#define HGUARD_LIBEX_META_ENCPTR
#pragma once

//----------------------------------------------------------------
// Meta: Pointer Encoding
//----------------------------------------------------------------
// Makes encoding pointers easy.
// The idea is to break up a pointer in 2 halves and pass both halves to a function
//
// Added true xref-masking
//

#include "../libex.h"

namespace meta
{

template< typename T >
INLINE T encode( T ptr, int magic )
{
	return reinterpret_cast<T>( (char*)ptr - magic );
}
template< typename T >
INLINE T decode( T ptr, int magic )
{
	return reinterpret_cast<T>( (char*)ptr + magic );
}



//------------------------------------------------
// Xref masking
//------------------------------------------------
//
// Normally xor and add aren't commutative, but in certain cases they are.
// This can be used to avoid the compiler from pre-computing the result (which we don't want when xref-masking).
//
// t is assumed to be even, this should be the case for most static addresses.
// If A is even, xor 1 is equal to add 1
// If A is odd, xor 1 is equal to sub 1
//
// The compiler doesn't know this and therefore doesn't optimize it.
//
// Final code will look similar to:
//
//   mov eax, t + A   ; Optimized by the compiler
//   xor eax, 1       ; Compiler can't optimize this, but we know the outcome
//   sub eax, A +/- 1 ; Depending if A is even or odd
//

template< int A, typename T >
inline T maskxref( T t )
{
#ifdef NDEBUG
	return reinterpret_cast<T>( (((uintptr_t)t - A) ^ 1) + A + ((A%2)?1:-1) );
#else
	assert( ((uintptr_t)t&1)==0 );
	return t;
#endif // NDEBUG
}

template< int A, typename T, T t >
inline T maskfnxref()
{
	// Required for functions
	static const T s = (T)( (uintptr_t)t + A );
	return (T)( (uintptr_t)s - A );
}

}

#endif // !HGUARD_LIBEX_META_ENCPTR
