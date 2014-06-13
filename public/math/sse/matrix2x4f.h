#ifndef HGUARD_LIBEX_MATH_SSE_MATRIX2x4f
#define HGUARD_LIBEX_MATH_SSE_MATRIX2x4f
#pragma once

#include "defs.h"
#include "../matrix.h"
#include "vector4f.h"

namespace math
{

template<>
class MATH_ALIGN16 matrix<float,2,4> {
public:
	typedef float type;
	static const dim ROWS = 2;
	static const dim COLUMNS = 4;

	typedef matrix<float,2,4> value;
	typedef matrix<float,2,4>& ref;
	typedef const matrix<float,2,4>& const_ref;

	typedef vector4f vec;


	vec& operator[] ( dim i );
	const vec& operator[] ( dim i ) const;

	type& operator() ( dim row, dim col );
	const type& operator() ( dim row, dim col ) const;

private:
	vec data[2];
};

typedef matrix<float,2,4> matrix2x4f;

}

#endif // !HGUARD_LIBEX_MATH_SSE_MATRIX2x4f
