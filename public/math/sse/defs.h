#ifndef HGUARD_LIBEX_MATH_SSE_DEFS
#define HGUARD_LIBEX_MATH_SSE_DEFS
#pragma once

// Highest version of SSE supported: SSE2: 20, SSE3: 30, SSSE3: 31, SSE4.1: 41, SSE4.2: 42
#define MATH_SSE_SUPPORT 30

#if MATH_SSE_SUPPORT>=42
#include <nmmintrin.h>
#elif MATH_SSE_SUPPORT>=41
#include <smmintrin.h>
#elif MATH_SSE_SUPPORT>=31
#include <tmmintrin.h>
#elif MATH_SSE_SUPPORT>=30
#include <pmmintrin.h>
#elif MATH_SSE_SUPPORT>=20
#include <emmintrin.h>
#else
#error SSE2 minimum requirement!
#include <xmmintrin.h>
#endif


#endif // !HGUARD_LIBEX_MATH_SSE_DEFS
