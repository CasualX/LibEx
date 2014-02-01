#ifndef HGUARD_MATH_SSE_DEFS
#define HGUARD_MATH_SSE_DEFS
#pragma once

#include <xmmintrin.h>
#include <pmmintrin.h>
#include <cmath>

#define MATH_SSE_ALIGN16 __declspec(align(16))
#define MATH_SSE_MINMAX_FIX

namespace math_sse
{

	class vector4f;
	class matrix3x4f;
}

#endif // !HGUARD_MATH_SSE_DEFS
