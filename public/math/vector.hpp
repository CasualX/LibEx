#ifndef HGUARD_LIBEX_MATH_VECTOR_HPP
#define HGUARD_LIBEX_MATH_VECTOR_HPP
#pragma once

#include "vector.h"
#include "bools.hpp"

#include <functional>

namespace math
{


template< typename T, dim D >
vector<T,D>::vector() {
}
template< typename T, dim D >
vector<T,D>::vector( T t ) {
	dup( t );
}
template< typename T, dim D >
vector<T,D>::vector( const_ref vec ) {
	assign( vec );
}
template< typename T, dim D >
typename vector<T,D>::ref vector<T,D>::assign( const_ref ) {
	for ( dim i = 0; i<D; ++i ) {
		data[i] = vec.data[i];
	}
	return *this;
}
template< typename T, dim D >
typename vector<T,D>::ref vector<T,D>::dup( T t ) {
	for ( dim i = 0; i<D; ++i ) {
		data[i] = t;
	}
	return *this;
}
template< typename T, dim D >
typename vector<T,D>::ref vector<T,D>::zero() {
	for ( dim i = 0; i<D; ++i ) {
		data[i] = T();
	}
	return *this;
}
template< typename T, dim D >
typename vector<T,D>::ref vector<T,D>::load( const T (&mem)[D] ) {
	for ( dim i = 0; i<D; ++i ) {
		data[i] = mem[i];
	}
	return *this;
}
template< typename T, dim D >
template< dim I >
typename vector<T,D>::ref vector<T,D>::set( T t ) {
	data[I] = t;
	return *this;
}
template< typename T, dim D >
template< dim I >
T vector<T,D>::get() const {
	return data[I];
}
template< typename T, dim D >
T vector<T,D>::extract() const {
	return data[0];
}
template< typename T, dim D >
void vector<T,D>::store( T (&mem)[D] ) const {
	for ( dim i = 0; i<D; ++i ) {
		mem[i] = data[i];
	}
}



template< typename T, dim D >
template< typename Op >
inline typename vector<T,D>::bools vector<T,D>::_cmp( const_ref vec ) const {
	bools::type res = 0;
	Op op;
	for ( dim i = 0; i<D; ++i ) {
		if ( op( data[i], vec.data[i] ) )
			res |= (1<<i);
	}
	return bools( res );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpeq( const_ref vec ) const {
	return _cmp<std::equal_to<T> >( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmplt( const_ref vec ) const {
	return _cmp<std::less<T> >( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmple( const_ref vec ) const {
	return _cmp<std::less_equal<T> >( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpgt( const_ref vec ) const {
	return _cmp<std::greater<T> >( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpge( const_ref vec ) const {
	return _cmp<std::greater_equal<T> >( vec );
}
//template< typename T, dim D >
//typename vector<T,D>::bools vector<T,D>::cmpord() const {
//}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpneq( const_ref vec ) const {
	return _cmp<std::not_equal_to<T> >( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpnlt( const_ref vec ) const {
	return cmpge( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpnle( const_ref vec ) const {
	return cmpgt( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpngt( const_ref vec ) const {
	return cmple( vec );
}
template< typename T, dim D >
typename vector<T,D>::bools vector<T,D>::cmpnge( const_ref vec ) const {
	return cmplt( vec );
}
//template< typename T, dim D >
//typename vector<T,D>::bools vector<T,D>::cmpunord() const {
//}



template< typename T, dim D >
inline T dot( const vector<T,D>& left, const vector<T,D>& right ) {
	return left.dot(right).extract();
}

template< typename T, dim D >
inline vector<T,D> cross( const vector<T,D>& left, const vector<T,D>& right ) {
	return left.cross(right);
}

template< typename T, dim D >
inline vector<T,D> clamp( const vector<T,D>& vec, const vector<T,D>& min, const vector<T,D>& max ) {
	return vec.min MATH_MINMAX_FIX (min).max MATH_MINMAX_FIX (max);
}

template< typename T, dim D >
inline vector<T,D> lerp( const vector<T,D>& left, const vector<T,D>& right, T scale ) {
	return left + ( right - left ) * scale;
}

template< typename T, dim D >
inline vector<T,D> slerp( const vector<T,D>& left, const vector<T,D>& right, T scale ) {
	// TODO!
}

}

#endif // !HGUARD_LIBEX_MATH_VECTOR_HPP
