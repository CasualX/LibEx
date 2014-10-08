#ifndef HGUARD_LIBEX_MATH_SSE_VECTOR4f_HPP
#define HGUARD_LIBEX_MATH_SSE_VECTOR4f_HPP
#pragma once

#include "vector4f.h"
#include "../vector.hpp"

namespace math
{


// Constructors

inline vector4f::vector() {
}
inline vector4f::vector( float a ) {
	assign( dup4( a ) );
}
inline vector4f::vector( float x, float y, float z, float w ) {
	assign( set( x, y, z, w ) );
}
inline vector4f::vector( __m128 xmm ) : reg( xmm ) {
}
inline vector4f::vector( const_ref vec ) : reg( vec.reg ) {
}
inline vector4f& vector4f::assign( const_ref vec ) {
	reg = vec.reg;
	return *this;
}


// Assignment

inline vector4f vector4f::dup3( const float f ) {
	__m128 reg = _mm_set_ss( f );
	return vector4f( _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(1,0,0,0) ) );
}
inline vector4f vector4f::dup4( const float f ) {
	return vector4f( _mm_load_ps1( &f ) );
}
inline vector4f vector4f::set( const float x, const float y, const float z, const float w ) {
	return vector4f( _mm_set_ps( w, z, y, x ) );
}
inline vector4f vector4f::zero() {
	return vector4f( _mm_setzero_ps() );
}
inline vector4f vector4f::xmm( __m128 xmm ) {
	return vector4f( xmm );
}
inline vector4f vector4f::vector3( const float xyz[3] ) {
	// load all 3 values independently ( 5 instructions )
	__m128 xy = _mm_movelh_ps( _mm_load_ss( &xyz[0] ), _mm_load_ss( &xyz[1] ) );
	__m128 reg = _mm_shuffle_ps( xy, _mm_load_ss( &xyz[2] ), _MM_SHUFFLE(2,0,2,0) );
	return vector4f( reg );
}
inline vector4f vector4f::aligned3( const float xyz[3] ) {
	// load x, y with a 64 bit integer load (00YX)
	__m128i xy = _mm_loadl_epi64( (const __m128i*)xyz );
	// now load the z element using a 32 bit float load (000Z)
	__m128 z = _mm_load_ss( &xyz[2] );
	// we now need to cast the __m128i register into a __m128 one (0ZYX)
	return vector4f( _mm_movelh_ps( _mm_castsi128_ps(xy), z ) );
}
inline vector4f vector4f::vector4( const float xyzw[4] ) {
	return vector4f( _mm_loadu_ps( xyzw ) );
}
inline vector4f vector4f::aligned4( const float xyzw[4] ) {
	return vector4f( _mm_load_ps( xyzw ) );
}
inline vector4f vector4f::int4( const int xyzw[4] ) {
	return vector4f( _mm_cvtepi32_ps( _mm_loadu_si128( (const __m128i*)xyzw ) ) );
}
inline vector4f vector4f::int2( const int xy[2] ) {
	__m128 reg;
	// x y * *
	reg = _mm_cvtepi32_ps( _mm_loadl_epi64( (const __m128i*)xy ) );
	// x y 1 0
	reg = _mm_shuffle_ps( reg, _mm_set_ps(0.0f,1.0f,0.0f,1.0f), _MM_SHUFFLE(1,0,1,0) );
	return vector4f( reg );
}
inline vector4f vector4f::color( unsigned int color ) {
	__m128i xmm = _mm_cvtsi32_si128( color );
	__m128i zeros = _mm_setzero_si128();
	// Unpack the bytes
	xmm = _mm_unpacklo_epi8( xmm, zeros );
	xmm = _mm_unpacklo_epi16( xmm, zeros );
	// Convert to float and make the components within [0,1] range
	return vector4f( _mm_mul_ps( _mm_cvtepi32_ps( xmm ), _mm_set_ps(1/255.0f,1/255.0f,1/255.0f,1/255.0f) ) );
}
inline vector4f& vector4f::setx( const float x ) {
	reg.m128_f32[0] = x;
	return *this;
}
inline vector4f& vector4f::sety( const float y ) {
	reg.m128_f32[1] = y;
	return *this;
}
inline vector4f& vector4f::setz( const float z ) {
	reg.m128_f32[2] = z;
	return *this;
}
inline vector4f& vector4f::setw( const float w ) {
	reg.m128_f32[3] = w;
	return *this;
}
inline void vector4f::load_2vector3( float src[6], vector4f& a_, vector4f& b_ ) {
	__m128 xyza = _mm_loadu_ps( src+0 ); // = {X0, Y0, Z0, X1}
	__m128 b =    _mm_load_ss( src+4 );  // = {Y1, 0, 0, 0}
	__m128 c =    _mm_load_ss( src+5 );  // = {Z1, 0, 0, 0}
	__m128 ab =  _mm_shuffle_ps(b, xyza, _MM_SHUFFLE(0, 3, 0, 0));
	// ab = {Y1, Y1, X1, X0}
	__m128 abc = _mm_shuffle_ps(ab, c, _MM_SHUFFLE(3, 0, 0, 2));
	// abc = {X1, Y1, Z1, 0}
	__m128 sum = _mm_add_ps(xyza, abc);
	a_ = xyza;
	b_ = sum;
}
inline void vector4f::load_4vector3( float src[12], vector4f& a, vector4f& b, vector4f& c, vector4f& d ) {
	__m128 p0 = _mm_loadu_ps( src+0 ); // X1Y1Z1X2
	__m128 p1 = _mm_loadu_ps( src+4 ); // Y2Z2X3Y3
	__m128 p2 = _mm_loadu_ps( src+8 ); // Z3X4Y4Z4

	__m128 tmp = _mm_shuffle_ps( p0, p1, _MM_SHUFFLE(0,1,0,3) ); // X2??Z2Y2

	a = p0;                                               // X1Y1Z1 
	b = _mm_shuffle_ps( tmp, tmp, _MM_SHUFFLE(0,2,3,0) ); // X2Y2Z2
	c = _mm_shuffle_ps( p1, p2, _MM_SHUFFLE(0,0,3,2) );   // X3Y3Z3
	d = _mm_shuffle_ps( p2, p2, _MM_SHUFFLE(0,3,2,1) );   // X4Y4Z4
}
template< dim x, dim y, dim z, dim w >
inline vector4f vector4f::shuffle() const {
	static_assert( x<4 && y<4 && z<4 && w<4, "invalid arguments for _MM_SHUFFLE()" );
	return vector4f( _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(w,z,y,x) ) );
}


// Extraction

inline float vector4f::getx() const {
	return get<0>();
}
inline float vector4f::gety() const {
	return get<1>();
}
inline float vector4f::getz() const {
	return get<2>();
}
inline float vector4f::getw() const {
	return get<3>();
}
template< dim D >
inline float vector4f::get() const {
	// FIXME! Optimize shuffle out if D is zero (x component).
	return _mm_cvtss_f32( _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(D,D,D,D) ) );
}
inline float vector4f::extract() const {
	return _mm_cvtss_f32( reg );
}
inline unsigned int vector4f::packc() const {
	// Scale back to [0,255] and convert to integers
	__m128i i = _mm_cvtps_epi32( _mm_mul_ps( reg, _mm_set_ps(255.0f,255.0f,255.0f,255.0f) ) );
	__m128i zeros = _mm_setzero_si128();
	// Pack them back together and extract
	return _mm_cvtsi128_si32( _mm_packus_epi16( _mm_packs_epi32( i, zeros ), zeros ) );
}
inline float* vector4f::data() {
	return reg.m128_f32;
}
inline const float* vector4f::data() const {
	return reg.m128_f32;
}
inline void vector4f::export3( float xyz[3] ) const {
	_mm_store_ss( &xyz[0], reg );
	_mm_store_ss( &xyz[1], _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,2,1,1)) );
	_mm_store_ss( &xyz[2], _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,2,1,2)) );
}
inline void vector4f::export4( float xyzw[4] ) const {
	_mm_storeu_ps( xyzw, reg );
}
inline vector4f vector4f::epsilon() {
	return vector4f( 1.0e-7f, 1.0e-7f, 1.0e-7f, 1.0e-7f );
}



// Basic operations

inline vector4f vector4f::neg() const {
	// use a mask to flip the sign bit
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	return vector4f( _mm_xor_ps( mask, reg ) );
}
inline vector4f vector4f::abs() const {
	// use a mask to clear the sign bit
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	return vector4f( _mm_andnot_ps( mask, reg ) );
}
inline vector4f vector4f::abs_neg() const {
	// use a mask to set the sign bit
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	return vector4f( _mm_or_ps( mask, reg ) );
}
inline vector4f vector4f::signum() const {
	__m128 reg = this->reg;
	// later we need to zero arguments that were already zero
	__m128 mask = _mm_cmpneq_ps( reg, _mm_setzero_ps() );
	// keep the sign bit of the components
	reg = _mm_and_ps( _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) ), reg );
	// or in the hex value of 1, if previously the sign was set then we get -1
	reg = _mm_or_ps( _mm_castsi128_ps( _mm_set_epi32( 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000 ) ), reg );
	// mask the zeros back to zero
	reg = _mm_and_ps( mask, reg );
	return vector4f( reg );
}
inline vector4f vector4f::copysign( const_ref vec ) const {
	__m128 reg = this->reg;
	// sign mask
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	// extract sign bits from input
	__m128 vecsgn = _mm_and_ps( mask, vec.reg );
	// clear our sign bits
	reg = _mm_andnot_ps( mask, reg );
	// set sign bits from argument
	reg = _mm_or_ps( reg, vecsgn );
	return vector4f( reg );
}

inline vector4f vector4f::min MATH_MINMAX_FIX ( const_ref vec ) const {
	return vector4f( _mm_min_ps( vec.reg, reg ) );
}
inline vector4f vector4f::max MATH_MINMAX_FIX ( const_ref vec ) const {
	return vector4f( _mm_max_ps( vec.reg, reg ) );
}



// Basic math

inline vector4f vector4f::add( const_ref vec ) const {
	return vector4f( _mm_add_ps( vec.reg, reg ) );
}
inline vector4f vector4f::sub( const_ref vec ) const {
	return vector4f( _mm_sub_ps( reg, vec.reg ) );
}
inline vector4f vector4f::hadd() const {
	__m128 reg = this->reg;
	// assign all components x+y+z+w
#if MATH_SSE_SUPPORT>=30
	reg = _mm_hadd_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
#else
	reg = _mm_add_ps( reg, _mm_movehl_ps(reg,reg) );
	reg = _mm_add_ss( reg, _mm_shuffle_ps(reg,reg,1) );
#endif
	return vector4f( reg );
}
inline vector4f vector4f::hsub() const {
	__m128 reg = this->reg;
#if MATH_SSE_SUPPORT>=30
	// assign all components x-y-(z-w)
	reg = _mm_hsub_ps( reg, reg );
	reg = _mm_hsub_ps( reg, reg );
#else
	reg = _mm_sub_ps( reg, _mm_movehl_ps(reg,reg) );
	reg = _mm_sub_ss( reg, _mm_shuffle_ps(reg,reg,1) );
#endif
	return vector4f( reg );
}

inline vector4f vector4f::mul( const_ref vec ) const {
	return vector4f( _mm_mul_ps( vec.reg, reg ) );
}
inline vector4f vector4f::madd( const_ref vec, const_ref a ) const {
	return vector4f( _mm_add_ps( _mm_mul_ps( vec.reg, a.reg ), reg ) );
}
inline vector4f vector4f::div( const_ref vec ) const {
	return vector4f( _mm_div_ps( vec.reg, reg ) );
}
inline vector4f vector4f::fmod( const_ref vec ) const {
	// Divide, truncate, multiply, subtract
	return sub( div(vec).trunc().mul(vec) );
}

inline vector4f vector4f::sqr() const {
	return vector4f( _mm_mul_ps( reg, reg ) );
}
inline vector4f vector4f::sqrt() const {
	return vector4f( _mm_sqrt_ps( reg ) );
}
inline vector4f vector4f::rsqrt() const {
	return vector4f( _mm_rsqrt_ps( reg ) );
}
inline vector4f vector4f::fsqrt() const {
	return vector4f( _mm_mul_ps( reg, _mm_rsqrt_ps( reg ) ) );
}



// Rounding and stuff

inline vector4f vector4f::ceil() const {
#if MATH_SSE_SUPPORT>=41
	return round<_MM_FROUND_CEIL>();
#else
	__m128 reg = this->reg;
	// Change rounding mode
	unsigned int old = _mm_getcsr();
	_mm_setcsr( (old&~_MM_ROUND_MASK)|_MM_ROUND_UP );
	// Do rounding
	reg = _mm_cvtepi32_ps( _mm_cvtps_epi32( reg ) );
	// Change back
	_mm_setcsr( old );
	return vector4f( reg );
#endif
}
inline vector4f vector4f::floor() const {
#if MATH_SSE_SUPPORT>=41
	return round<_MM_FROUND_FLOOR>();
#else
	//// Change rounding mode
	//unsigned int old = _mm_getcsr();
	//_mm_setcsr( (old&~_MM_ROUND_MASK)|_MM_ROUND_DOWN );
	//// Do rounding
	//reg = _mm_cvtepi32_ps( _mm_cvtps_epi32( reg ) );
	//// Change back
	//_mm_setcsr( old );
	//return *this;

	// Truncate to packed int32
	__m128i i = _mm_cvttps_epi32( reg );
	// Create a mask for elements already integral
	__m128i mask = _mm_castps_si128( _mm_cmpneq_ps( _mm_cvtepi32_ps( i ), reg ) );
	// Subtract the sign bit to correct truncation
	i = _mm_sub_epi32( i, _mm_and_si128( _mm_srli_epi32( _mm_castps_si128( reg ), 31 ), mask ) );
	// Convert back to packed single
	return vector4f( _mm_cvtepi32_ps( i ) );
#endif
}
inline vector4f vector4f::round() const {
#if MATH_SSE_SUPPORT>=41
	return round<_MM_FROUND_NINT>();
#else
	return vector4f( _mm_cvtepi32_ps( _mm_cvtps_epi32( reg ) ) );
#endif
}
inline vector4f vector4f::trunc() const {
#if MATH_SSE_SUPPORT>=41
	return round<_MM_FROUND_TRUNC>();
#else
	return vector4f( _mm_cvtepi32_ps( _mm_cvttps_epi32( reg ) ) );
#endif
}
inline vector4f vector4f::frac() const {
	// FIXME! What is the fractional part of a negative number?
	//        Wikipedia defined it as frac(x) = x-floor(x)
	//        Wolfram says that x-trunc(x) (resulting in a negative fraction) is also acceptable
	return sub( trunc() );
}
#if MATH_SSE_SUPPORT>=41
template< int mode >
inline vector4f vector4f::round() const {
	static_assert( mode>=0 && mode<16, "invalid round mode, see _MM_FROUND_* macros!" );
	return vector4f( _mm_round_ps( reg, mode ) );
}
#endif



// Distance and length

inline vector4f vector4f::length() const {
	return sqr().hadd().sqrt();
}
inline vector4f vector4f::lensqr() const {
	return sqr().hadd();
}
inline vector4f vector4f::lenfast() const {
	return sqr().hadd().fsqrt();
}

inline vector4f vector4f::resize( float len ) const {
	return mul( dup4( len ) / ( length() + epsilon() ) );
}

inline vector4f vector4f::dist( const_ref vec ) const {
	return sub( vec ).length();
}
inline vector4f vector4f::distsqr( const_ref vec ) const {
	return sub( vec ).lensqr();
}
inline vector4f vector4f::distfast( const_ref vec ) const {
	return sub( vec ).lenfast();
}

inline vector4f vector4f::len2d() const {
	return len2dsqr().sqrt();
}
inline vector4f vector4f::len2dsqr() const {
#if MATH_SSE_SUPPORT>=30
	__m128 reg = this->reg;
	reg = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(0,1,0,1) );
	reg = _mm_mul_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
	return vector4f( reg );
#else
	// FIXME! Figure out efficient hadd()!
	return shuffle<1,0,1,0>().sqr().hadd();
#endif
}
inline vector4f vector4f::len2dfast() const {
	return len2dsqr().fsqrt();
}



// Vector operations

inline vector4f vector4f::norm() const {
	return div( length() + epsilon() );
}
inline vector4f vector4f::dot( const_ref vec ) const {
	return mul( vec ).hadd();
}
inline vector4f vector4f::cross( const_ref vec ) const {
	return vector4f( _mm_sub_ps(
		_mm_mul_ps( _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,0,2,1)), _mm_shuffle_ps(vec.reg,vec.reg,_MM_SHUFFLE(3,1,0,2)) ), 
		_mm_mul_ps( _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,1,0,2)), _mm_shuffle_ps(vec.reg,vec.reg,_MM_SHUFFLE(3,0,2,1)) )) );
}



// Comparison

inline vector4f::bools vector4f::cmpeq( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpeq_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmplt( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmplt_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmple( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmple_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpgt( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpgt_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpge( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpge_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpord() const {
	return bools( _mm_movemask_ps( _mm_cmpord_ps( reg, reg ) ) );
}
inline vector4f::bools vector4f::cmpneq( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpneq_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpnlt( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpnlt_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpnle( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpnle_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpngt( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpngt_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpnge( const_ref vec ) const {
	return bools( _mm_movemask_ps( _mm_cmpnge_ps( reg, vec.reg ) ) );
}
inline vector4f::bools vector4f::cmpunord() const {
	return bools( _mm_movemask_ps( _mm_cmpunord_ps( reg, reg ) ) );
}


// Operator convenience

inline vector4f& vector4f::operator= ( __m128 xmm ) {
	reg = xmm;
	return *this;
}
inline vector4f& vector4f::operator= ( const_ref vec ) {
	return assign( vec );
}
inline vector4f& vector4f::operator+= ( const_ref vec ) {
	return assign( add( vec ) );
}
inline vector4f& vector4f::operator-= ( const_ref vec ) {
	return assign( sub( vec ) );
}
inline vector4f& vector4f::operator*= ( const_ref vec ) {
	return assign( mul( vec ) );
}
inline vector4f& vector4f::operator/= ( const_ref vec ) {
	return assign( div( vec ) );
}
inline vector4f& vector4f::operator%= ( const_ref vec ) {
	return assign( fmod( vec ) );
}

inline vector4f vector4f::operator+ ( const_ref vec ) const {
	return add( vec );
}
inline vector4f vector4f::operator- ( const_ref vec ) const {
	return sub( vec );
}
inline vector4f vector4f::operator- () const {
	return neg();
}
inline vector4f vector4f::operator* ( const_ref vec ) const {
	return mul( vec );
}
inline vector4f vector4f::operator/ ( const_ref vec ) const {
	return div( vec );
}
inline vector4f vector4f::operator% ( const_ref vec ) const {
	return fmod( vec );
}

inline vector4f::bools vector4f::operator< ( const_ref vec ) const {
	return cmplt( vec );
}
inline vector4f::bools vector4f::operator> ( const_ref vec ) const {
	return cmpgt( vec );
}
inline vector4f::bools vector4f::operator<= ( const_ref vec ) const {
	return cmple( vec );
}
inline vector4f::bools vector4f::operator>= ( const_ref vec ) const {
	return cmpge( vec );
}
inline vector4f::bools vector4f::operator== ( const_ref vec ) const {
	return cmpeq( vec );
}
inline vector4f::bools vector4f::operator!= ( const_ref vec ) const {
	return cmpneq( vec );
}

inline float& vector4f::operator[] ( dim i ) {
	assert( i<4 );
	return reg.m128_f32[i];
}
inline const float& vector4f::operator[] ( dim i ) const {
	assert( i<4 );
	return reg.m128_f32[i];
}
inline vector4f::operator __m128& () {
	return reg;
}
inline vector4f::operator const __m128& () const {
	return reg;
}

}

#endif // !HGUARD_LIBEX_MATH_SSE_VECTOR4f_HPP
