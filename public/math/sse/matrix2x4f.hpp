#ifndef HGUARD_LIBEX_MATH_SSE_MATRIX2x4f_HPP
#define HGUARD_LIBEX_MATH_SSE_MATRIX2x4f_HPP
#pragma once

#include "matrix2x4f.h"

namespace math
{

inline vector4f& matrix2x4f::operator[] ( dim i ) {
	assert( i<2 );
	return data[i];
}
inline const vector4f& matrix2x4f::operator[] ( dim i ) const {
	assert( i<2 );
	return data[i];
}
inline float& matrix2x4f::operator() ( dim row, dim col ) {
	assert( row<2 && col<4 );
	return data[row][col];
}
inline const float& matrix2x4f::operator() ( dim row, dim col ) const {
	assert( row<2 && col<4 );
	return data[row][col];
}

}

#endif // !HGUARD_LIBEX_MATH_SSE_MATRIX2x4f_HPP
