#ifndef HGUARD_LIBEX_MATH_MATRIX
#define HGUARD_LIBEX_MATH_MATRIX
#pragma once

#include "defs.h"
#include "vector.h"

namespace math
{

// Generic matrix class.
// All matrices must be row-major!

template< typename T, dim H, dim W >
class matrix {
	typedef typename T type;
	static const dim ROWS = H;
	static const dim COLUMNS = W;
	typedef vector<typename T,W> vector;

	// Row major order
	vector data[H];
};

}

#endif // !HGUARD_LIBEX_MATH_MATRIX
