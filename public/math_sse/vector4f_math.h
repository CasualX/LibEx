#ifndef HGUARD_MATH_SSE_VECTOR4f_MATH
#define HGUARD_MATH_SSE_VECTOR4f_MATH
#pragma once

//----------------------------------------------------------------
// Additional vector4f math
//----------------------------------------------------------------

#include "vector4f.h"

namespace math_sse
{

	vector4f sin4f_rd( const vector4f& in );
	vector4f cos4f_rd( const vector4f& in );

	vector4f pow4f_rd( const vector4f& a, const vector4f& b );
	vector4f ln4f_rd( const vector4f& a, const vector4f& b );

}

#endif // !HGUARD_MATH_SSE_VECTOR4f_MATH
