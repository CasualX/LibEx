#ifndef HGUARD_LIBEX_MATH_DEFS
#define HGUARD_LIBEX_MATH_DEFS
#pragma once

#include <cmath>

#define MATH_ALIGN16 __declspec(align(16))
#define MATH_MINMAX_FIX
#define MATH_NO_VARIADIC_TEMPLATES

namespace math
{

// Integer type for # of dimensions
typedef unsigned int dim;




// Helper to force loop unrolling
template< dim N, typename Fn, Fn pfn, dim I >
class repeat_t
{
public:
	inline static void invoke()
	{
		repeat_t<N,Fn,pfn,I-1>::invoke();
		pfn( I );
	}
};
template< dim N, typename Fn, Fn pfn >
class repeat_t<N,Fn,pfn,0>
{
public:
	inline static void invoke()
	{
		pfn( 0 );
	}
};
template< unsigned int N, typename Fn, Fn pfn >
inline void repeat()
{
	repeat_t<N,Fn,pfn,N-1>::invoke();
}


}

#endif // !HGUARD_LIBEX_MATH_DEFS
