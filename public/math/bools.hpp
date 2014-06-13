#ifndef HGUARD_LIBEX_MATH_BOOLS_HPP
#define HGUARD_LIBEX_MATH_BOOLS_HPP
#pragma once

#include "bools.h"

namespace math
{

template< dim D >
inline bool any( const bools<D>& result ) {
	return result.any();
}
template< dim D >
inline bool any( const bools<D>& result, typename bools<D>::type mask ) {
	return result.any( mask );
}
template< dim D >
inline bool all( const bools<D>& result ) {
	return result.all();
}
template< dim D >
inline bool all( const bools<D>& result, typename bools<D>::type mask ) {
	return result.all( mask );
}



template< dim D >
inline bools<D>::bools( type bits ) : bits(bits) {
	// FIXME! Add assert() here to enforce (bits&~mask)==0 (ie, no bits outside mask are set)?
}
template< dim D >
inline bool bools<D>::any() const {
	return bits!=0;
}
template< dim D >
inline bool bools<D>::any( type mask ) const {
	return (bits&mask)!=0;
}
template< dim D >
inline bool bools<D>::all() const {
	return bits==mask;
}
template< dim D >
inline bool bools<D>::all( type mask ) const {
	return (bits&mask)==mask;
}
//template< dim D >
//inline bools<D>::operator bool () const {
//	return bits!=0;
//}

}

#endif // !HGUARD_LIBEX_MATH_BOOLS_HPP
