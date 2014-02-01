#ifndef HGUARD_MATH_SSE_VECTOR4f
#define HGUARD_MATH_SSE_VECTOR4f
#pragma once

#include "defs.h"

namespace math_sse
{

void __fastcall sin4f( float* vec );

	
MATH_SSE_ALIGN16 class vector4f
{
public:
	typedef const vector4f& const_ref_t;
	
	//----------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------

	vector4f();
	explicit vector4f( float f );
	vector4f( float x, float y, float z, float w = 0.0f );
	vector4f( __m128 xmm );
	explicit vector4f( const float xyz[3] );
	vector4f( const_ref_t vec );
	
	//----------------------------------------------------------------
	// Assignment
	//----------------------------------------------------------------

	// Copy a vector4f
	vector4f& set( const_ref_t vec );
	// Assign to x, y, z; leave w zero.
	vector4f& set_dup3( const float f );
	// Assign all components the same constant.
	vector4f& set_dup4( const float f );
	// Assign individual components, may be expensive if used incorrectly!
	vector4f& set_const( const float x, const float y, const float z, const float w = 0.0f );
	// Zero the vector.
	vector4f& set_zero();
	// Assign directly from xmm register.
	vector4f& set_xmm( const __m128& xmm );
	// Set from unaligned memory vector, w = 0.
	vector4f& set_vector3( const float xyz[3] );
	// Load aligned(8) vector3, w = 0.
	// Works with unaligned memory, but slower than set_vector3.
	vector4f& set_aligned3( const float xyz[3] );
	// Load from unaligned memory vector.
	vector4f& set_vector4( const float xyzw[4] );
	// Load aligned(16) vector.
	// Will segfault on unaligned memory!
	vector4f& set_aligned4( const float xyzw[4] );
	// Load unaligned integers (cvt to float)
	vector4f& set_int4( const int xyzw[4] );
	// Load unaligned integers (cvt to float), z = 1, w = 0
	vector4f& set_int2( const int xy[2] );
	// Set from 32bit argb color
	vector4f& set_color( unsigned long clr );
	// Set individual X component. FIXME! Inefficient!
	vector4f& set_x( const float x );
	// Set individual Y component. FIXME! Inefficient!
	vector4f& set_y( const float y );
	// Set individual Z component. FIXME! Inefficient!
	vector4f& set_z( const float z );
	// Set individual W component. FIXME! Inefficient!
	vector4f& set_w( const float w );

	// Load 2 3D vectors in a row
	static void load_2vector3( float src[6], vector4f& a, vector4f& b );
	// Load 4 3D vectors in a row
	static void load_4vector3( float src[12], vector4f& a, vector4f& b, vector4f& c, vector4f& d );

	// Shuffle the components around, specify where the components should go: x = 0, y = 1, z = 2, w = 3
	// Example: (xyzw).shuffle<2,0,1,3>() means, move x to position 2 (z), y to position 0 (x) and z to position 1 (y); resulting in (yzxw).
	template< int x, int y, int z, int w >
	vector4f& shuffle();

	//----------------------------------------------------------------
	// Extraction
	//----------------------------------------------------------------
	
	// Get the X component.
	float extract_x() const;
	// Get the Y component.
	float extract_y() const;
	// Get the Z component.
	float extract_z() const;
	// Get the W component.
	float extract_w() const;
	// Get any component, useful for operations that ensure every component is the same.
	// In practice this returns a single component but makes no guaranties which component it is.
	// Use with length*(), len2d*(), dist*() and dot() operations.
	float extract() const;

	// Generates a clone.
	vector4f clone() const;

	// Direct access to internal data floats.
	// NOTE! Will flush the register to memory before you can start reading them!
	// FIXME! Aliasing problem! (what happens if you modify them directly, how does the compiler know to re-read them from memory?)
	float* data();
	const float* data() const;

	// Write x,y,z to unaligned memory.
	void export3( float xyz[3] ) const;
	// Write x,y,z,w to unaligned memory.
	void export4( float xyzw[4] ) const;

	// Get epsilon vector.
	static vector4f epsilon();

	//----------------------------------------------------------------
	// Basic operations
	//----------------------------------------------------------------

	// Negate the vector.
	vector4f& neg();
	// Absolute value of the vector.
	vector4f& abs();
	// Absolute value then negate the vector (make negative).
	vector4f& abs_neg();
	// Store the sign of each component, -1 for negative, 0 for zero and 1 for positive.
	vector4f& signum();
	// Copy the sign of the argument.
	vector4f& copysign( const_ref_t vec );
	
	// Take the minimum components of ourself and vec.
	vector4f& min MATH_SSE_MINMAX_FIX ( const_ref_t vec );
	// Take the maximum components of ourself and vec.
	vector4f& max MATH_SSE_MINMAX_FIX ( const_ref_t vec );
	// Clamp between min and max.
	vector4f& clamp( const_ref_t min, const_ref_t max );

	// Add vector.
	vector4f& add( const_ref_t vec );
	// Subtract vector.
	vector4f& sub( const_ref_t vec );
	// Horizontally add components.
	vector4f& hadd();
	// Horizontally subtract components.
	vector4f& hsub();

	// Multiply vector component wise.
	vector4f& mul( const_ref_t vec );
	// Multiply vector with a scalar.
	vector4f& mul( float scalar );

	// Fused multiply add with a vector.
	vector4f& madd( const_ref_t vec, const_ref_t scale );
	// Fused multiply add with a scalar.
	vector4f& madd( const_ref_t vec, float scalar );

	// Divide vector.
	vector4f& div( const_ref_t vec );
	// Divide by scalar.
	vector4f& div( float scalar );

	// Square the vector.
	vector4f& sqr();
	// Square root.
	vector4f& sqrt();
	// Inverse square root.
	vector4f& rsqrt();
	// Fast square root.
	vector4f& sqrt_fast();

	//----------------------------------------------------------------
	// Length and distance operations
	//----------------------------------------------------------------

	// Length of the vector.
	vector4f& length();
	// Squared length of the vector.
	vector4f& length_sqr();
	// Fast length (less accurate) of the vector.
	vector4f& length_fast();
	
	float length_const() const;
	float length_sqr_const() const;
	float length_fast_const() const;

	vector4f& resize( float len );

	vector4f& dist( const_ref_t vec );
	vector4f& dist_sqr( const_ref_t vec );
	vector4f dist_fast( const_ref_t vec );
	
	float dist_const( const_ref_t vec ) const;
	float dist_sqr_const( const_ref_t vec ) const;
	float dist_fast_const( const_ref_t vec ) const;
	
	// 2D length of the vector.
	vector4f& len2d();
	// 2D squared length of the vector.
	vector4f& len2d_sqr();
	// 2D fast length (less accurate) of the vector.
	vector4f& len2d_fast();
	// 2D length of the vector, constant.
	float len2d_const() const;
	// 2D squared length of the vector, constant.
	float len2d_sqr_const() const;
	// 2D fast length (less accurate) of the vector, constant.
	float len2d_fast_const() const;
	
	//----------------------------------------------------------------
	// Vector operations
	//----------------------------------------------------------------

	// Normalize and return our length.
	float norm();
	// Dot product.
	vector4f& dot( const_ref_t vec );
	// Cross product.
	vector4f& cross( const_ref_t vec );
	// Lerp from this (alpha=0) to vec (alpha=1)
	vector4f& lerp( const_ref_t vec, float alpha );
	// Slerp from this (alpha=0) to vec (alpha=1)
	vector4f& slerp( const_ref_t vec, float alpha );

	// Dot product.
	float dot_const( const_ref_t vec ) const;
	// Cross product.
	vector4f cross_const( const_ref_t vec ) const;
	
	//----------------------------------------------------------------
	// Format conversion
	//----------------------------------------------------------------

	// Convert axis angles (radians!) to vectors.
	void cvt_ang2vecs( vector4f& forward, vector4f& up, vector4f& right ) const;
	// Convert axis angles (radians!) to vector.
	vector4f& cvt_ang2vec();
	// Convert vector to axis angles (radians!).
	vector4f& cvt_vec2ang();

	// Convert degree angles to radians angles.
	vector4f& cvt_deg2rad();
	// Convert radians angles to degree angles.
	vector4f& cvt_rad2deg();
	
	//----------------------------------------------------------------
	// Comparison
	//----------------------------------------------------------------

	// Equal
	vector4f cmpeq( const vector4f& vec ) const;
	// Less than
	vector4f cmplt( const vector4f& vec ) const;
	// Less than or equal
	vector4f cmple( const vector4f& vec ) const;
	// Greater than
	vector4f cmpgt( const vector4f& vec ) const;
	// Greater than or equal
	vector4f cmpge( const vector4f& vec ) const;
	// Is finite
	vector4f cmpord() const;

	// Not equal
	vector4f cmpneq( const vector4f& vec ) const;
	// Not less than
	vector4f cmpnlt( const vector4f& vec ) const;
	// Not less than or equal
	vector4f cmpnle( const vector4f& vec ) const;
	// Not greater than
	vector4f cmpngt( const vector4f& vec ) const;
	// Not greater than or equal
	vector4f cmpnge( const vector4f& vec ) const;
	// Is not finite
	vector4f cmpunord() const;

	// Any comparison was true
	bool any( int mask ) const;
	// All comparisons were true
	bool all( int mask ) const;

	//----------------------------------------------------------------
	// Operator convenience
	//----------------------------------------------------------------

	vector4f& operator= ( const_ref_t vec );
	vector4f& operator+= ( const_ref_t vec );
	vector4f& operator-= ( const_ref_t vec );
	vector4f& operator*= ( const_ref_t vec );
	vector4f& operator*= ( float f );
	vector4f& operator/= ( const_ref_t vec );
	vector4f& operator/= ( float f );

	vector4f operator+ ( const_ref_t vec ) const;
	vector4f operator- ( const_ref_t vec ) const;
	vector4f operator- () const;
	vector4f operator* ( const_ref_t vec ) const;
	vector4f operator* ( float f ) const;
	vector4f operator/ ( const_ref_t vec ) const;
	vector4f operator/ ( float f ) const;

	float& operator[] ( unsigned i );
	const float& operator[] ( unsigned i ) const;

	operator __m128& ();
	operator const __m128& () const;

private:
	__m128 reg;
};














// Constructors

inline vector4f::vector4f() { }
inline vector4f::vector4f( float f )
{
	set_dup3( f );
}
inline vector4f::vector4f( float x, float y, float z, float w )
{
	set_const( x, y, z, w );
}
inline vector4f::vector4f( __m128 xmm ) : reg( xmm ) { }
inline vector4f::vector4f( const float* xyz )
{
	set_vector3( xyz );
}
inline vector4f::vector4f( const_ref_t vec ) : reg( vec.reg ) { }



// Assignment

inline vector4f& vector4f::set( const_ref_t vec )
{
	reg = vec.reg;
	return *this;
}
inline vector4f& vector4f::set_dup3( const float f )
{
	reg = _mm_set_ss( f );
	reg = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(1,0,0,0) );
	return *this;
}
inline vector4f& vector4f::set_dup4( const float f )
{
	reg = _mm_load_ps1( &f );
	return *this;
}
inline vector4f& vector4f::set_const( const float x, const float y, const float z, const float w )
{
	reg = _mm_set_ps( w, z, y, x );
	return *this;
}
inline vector4f& vector4f::set_zero()
{
	reg = _mm_setzero_ps();
	return *this;
}
inline vector4f& vector4f::set_xmm( const __m128& xmm )
{
	reg = xmm;
	return *this;
}
inline vector4f& vector4f::set_vector3( const float xyz[3] )
{
	// load all 3 values independently ( 5 instructions )
	__m128 x = _mm_load_ss( &xyz[0] );
	__m128 y = _mm_load_ss( &xyz[1] );
	__m128 xy = _mm_movelh_ps( x, y );
	__m128 z = _mm_load_ss( &xyz[2] );
	reg = _mm_shuffle_ps( xy, z, _MM_SHUFFLE(2,0,2,0) );
	return *this;
}
inline vector4f& vector4f::set_aligned3( const float xyz[3] )
{
	// load x, y with a 64 bit integer load (00YX)
	__m128i xy = _mm_loadl_epi64( (const __m128i*)xyz );
	// now load the z element using a 32 bit float load (000Z)
	__m128 z = _mm_load_ss( &xyz[2] );
	// we now need to cast the __m128i register into a __m128 one (0ZYX)
	reg = _mm_movelh_ps( _mm_castsi128_ps(xy), z );
	return *this;
}
inline vector4f& vector4f::set_vector4( const float xyzw[4] )
{
	reg = _mm_loadu_ps( xyzw );
	return *this;
}
inline vector4f& vector4f::set_aligned4( const float xyzw[4] )
{
	reg = _mm_load_ps( xyzw );
	return *this;
}
inline vector4f& vector4f::set_int4( const int xyzw[4] )
{
	reg = _mm_cvtepi32_ps( _mm_loadu_si128( (const __m128i*)xyzw ) );
	return *this;
}
inline vector4f& vector4f::set_int2( const int xy[2] )
{
	// x y * *
	reg = _mm_cvtepi32_ps( _mm_loadl_epi64( (const __m128i*)xy ) );
	// x y 1 0
	reg = _mm_shuffle_ps( reg, _mm_set_ps(0.0f,1.0f,0.0f,1.0f), _MM_SHUFFLE(1,0,1,0) );
	return *this;
}
inline vector4f& vector4f::set_color( unsigned long color )
{
	__m128i xmm = _mm_cvtsi32_si128( color );
	// Unpack the bytes
	xmm = _mm_unpacklo_epi8( xmm, _mm_setzero_si128() );
	xmm = _mm_unpacklo_epi16( xmm, _mm_setzero_si128() );
	// Convert to float and make the components within [0,1] range
	reg = _mm_cvtepi32_ps( xmm );
	reg = _mm_mul_ps( reg, _mm_set_ps(1/255.0f,1/255.0f,1/255.0f,1/255.0f) );
	return *this;
}
inline vector4f& vector4f::set_x( const float x )
{
	reg.m128_f32[0] = x;
	return *this;
}
inline vector4f& vector4f::set_y( const float y )
{
	reg.m128_f32[1] = y;
	return *this;
}
inline vector4f& vector4f::set_z( const float z )
{
	reg.m128_f32[2] = z;
	return *this;
}
inline vector4f& vector4f::set_w( const float w )
{
	reg.m128_f32[3] = w;
	return *this;
}
inline void vector4f::load_2vector3( float src[6], vector4f& a_, vector4f& b_ )
{
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
inline void vector4f::load_4vector3( float src[12], vector4f& a, vector4f& b, vector4f& c, vector4f& d )
{
	__m128 p0 = _mm_loadu_ps( src+0 ); // X1Y1Z1X2
	__m128 p1 = _mm_loadu_ps( src+4 ); // Y2Z2X3Y3
	__m128 p2 = _mm_loadu_ps( src+8 ); // Z3X4Y4Z4

	__m128 tmp = _mm_shuffle_ps( p0, p1, _MM_SHUFFLE(0,1,0,3) ); // X2??Z2Y2

	a = p0;                                               // X1Y1Z1 
	b = _mm_shuffle_ps( tmp, tmp, _MM_SHUFFLE(0,2,3,0) ); // X2Y2Z2
	c = _mm_shuffle_ps( p1, p2, _MM_SHUFFLE(0,0,3,2) );   // X3Y3Z3
	d = _mm_shuffle_ps( p2, p2, _MM_SHUFFLE(0,3,2,1) );   // X4Y4Z4
}
template< int x, int y, int z, int w >
inline vector4f& vector4f::shuffle()
{
	static_assert( x>=0 && x<4 && y>=0 && y<4 && z>=0 && z<4 && w>=0 && w<4, "invalid arguments for _MM_SHUFFLE()" );
	reg = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(w,z,y,x) );
	return *this;
}



// Extraction

inline float vector4f::extract_x() const
{
	__m128 x = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(0,0,0,0) );
	return _mm_cvtss_f32( x );
}
inline float vector4f::extract_y() const
{
	__m128 y = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(1,1,1,1) );
	return _mm_cvtss_f32( y );
}
inline float vector4f::extract_z() const
{
	__m128 z = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(2,2,2,2) );
	return _mm_cvtss_f32( z );
}
inline float vector4f::extract_w() const
{
	__m128 w = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(3,3,3,3) );
	return _mm_cvtss_f32( w );
}
inline float vector4f::extract() const
{
	return _mm_cvtss_f32( reg );
}
inline vector4f vector4f::clone() const
{
	return vector4f(*this);
}
inline float* vector4f::data()
{
	return reg.m128_f32;
}
inline const float* vector4f::data() const
{
	return reg.m128_f32;
}
inline void vector4f::export3( float xyz[3] ) const
{
	_mm_store_ss( &xyz[0], reg );
	_mm_store_ss( &xyz[1], _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,2,1,1)) );
	_mm_store_ss( &xyz[2], _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,2,1,2)) );
}
inline void vector4f::export4( float xyzw[4] ) const
{
	_mm_storeu_ps( xyzw, reg );
}
inline vector4f vector4f::epsilon()
{
	return vector4f( 1.0e-7f, 1.0e-7f, 1.0e-7f, 1.0e-7f );
}



// Basic operations

inline vector4f& vector4f::neg()
{
	// use a mask to flip the sign bit
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	reg = _mm_xor_ps( mask, reg );
	return *this;
}
inline vector4f& vector4f::abs()
{
	// use a mask to clear the sign bit
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	reg = _mm_andnot_ps( mask, reg );
	return *this;
}
inline vector4f& vector4f::abs_neg()
{
	// use a mask to set the sign bit
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	reg = _mm_or_ps( mask, reg );
	return *this;
}
inline vector4f& vector4f::signum()
{
	// later we need to zero arguments that were already zero
	__m128 mask = _mm_cmpneq_ps( reg, _mm_setzero_ps() );
	// keep the sign bit of the components
	reg = _mm_and_ps( _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) ), reg );
	// or in the hex value of 1, if previously the sign was set then we get -1
	reg = _mm_or_ps( _mm_castsi128_ps( _mm_set_epi32( 0x3f800000, 0x3f800000, 0x3f800000, 0x3f800000 ) ), reg );
	// mask the zeros back to zero
	reg = _mm_and_ps( mask, reg );
	return *this;
}
inline vector4f& vector4f::copysign( const_ref_t vec )
{
	// sign mask
	__m128 mask = _mm_castsi128_ps( _mm_set_epi32( 0x80000000, 0x80000000, 0x80000000, 0x80000000 ) );
	// extract sign bits from input
	__m128 vecsgn = _mm_and_ps( mask, vec.reg );
	// clear our sign bits
	reg = _mm_andnot_ps( mask, reg );
	// set sign bits from argument
	reg = _mm_or_ps( reg, vecsgn );
	return *this;
}

inline vector4f& vector4f::min MATH_SSE_MINMAX_FIX ( const_ref_t vec )
{
	reg = _mm_min_ps( vec.reg, reg );
	return *this;
}
inline vector4f& vector4f::max MATH_SSE_MINMAX_FIX ( const_ref_t vec )
{
	reg = _mm_max_ps( vec.reg, reg );
	return *this;
}
inline vector4f& vector4f::clamp( const_ref_t min, const_ref_t max )
{
	return this->min MATH_SSE_MINMAX_FIX ( max ).max MATH_SSE_MINMAX_FIX ( min );
}

inline vector4f& vector4f::add( const_ref_t vec )
{
	reg = _mm_add_ps( vec.reg, reg );
	return *this;
}
inline vector4f& vector4f::sub( const_ref_t vec )
{
	reg = _mm_sub_ps( vec.reg, reg );
	return *this;
}
inline vector4f& vector4f::hadd()
{
	// assign all components x+y+z+w
	reg = _mm_hadd_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
	return *this;
}
inline vector4f& vector4f::hsub()
{
	// assign all components x-y-(z-w)
	reg = _mm_hsub_ps( reg, reg );
	reg = _mm_hsub_ps( reg, reg );
	return *this;
}

inline vector4f& vector4f::mul( const_ref_t vec )
{
	reg = _mm_mul_ps( vec.reg, reg );
	return *this;
}
inline vector4f& vector4f::mul( float scalar )
{
	return mul( vector4f().set_dup4(scalar) );
}

inline vector4f& vector4f::madd( const_ref_t vec, const_ref_t a )
{
	reg = _mm_add_ps( _mm_mul_ps( vec.reg, a.reg ), reg );
	return *this;
}
inline vector4f& vector4f::madd( const_ref_t vec, float scalar )
{
	return madd( vec, vector4f().set_dup4(scalar) );
}

inline vector4f& vector4f::div( const_ref_t vec )
{
	reg = _mm_div_ps( vec.reg, reg );
	return *this;
}
inline vector4f& vector4f::div( float a )
{
	return div( vector4f().set_dup4( a ) );
}

inline vector4f& vector4f::sqr()
{
	reg = _mm_mul_ps( reg, reg );
	return *this;
}
inline vector4f& vector4f::sqrt()
{
	reg = _mm_sqrt_ps( reg );
	return *this;
}
inline vector4f& vector4f::rsqrt()
{
	reg = _mm_rsqrt_ps( reg );
	return *this;
}
inline vector4f& vector4f::sqrt_fast()
{
	reg = _mm_mul_ps( reg, _mm_rsqrt_ps( reg ) );
	return *this;
}



// Distance and length

inline vector4f& vector4f::length()
{
	return sqr().hadd().sqrt();
}
inline vector4f& vector4f::length_sqr()
{
	return sqr().hadd();
}
inline vector4f& vector4f::length_fast()
{
	return sqr().hadd().sqrt_fast();
}
	
inline float vector4f::length_const() const
{
	return clone().length().extract();
}
inline float vector4f::length_sqr_const() const
{
	return clone().length_sqr().extract();
}
inline float vector4f::length_fast_const() const
{
	return clone().length_fast().extract();
}

inline vector4f& vector4f::resize( float len )
{
	return mul( vector4f( len ) / ( clone().length() += epsilon() ) );
}

inline vector4f& vector4f::dist( const_ref_t vec )
{
	return sub( vec ).length();
}
inline vector4f& vector4f::dist_sqr( const_ref_t vec )
{
	return sub( vec ).length_sqr();
}
inline vector4f vector4f::dist_fast( const_ref_t vec )
{
	return sub( vec ).length_fast();
}
	
inline float vector4f::dist_const( const_ref_t vec ) const
{
	return clone().dist( vec ).extract();
}
inline float vector4f::dist_sqr_const( const_ref_t vec ) const
{
	return clone().dist_sqr( vec ).extract();
}
inline float vector4f::dist_fast_const( const_ref_t vec ) const
{
	return clone().dist_fast( vec ).extract();
}
	
inline vector4f& vector4f::len2d()
{
	reg = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(0,1,0,1) );
	reg = _mm_mul_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
	reg = _mm_sqrt_ps( reg );
	return *this;
}
inline vector4f& vector4f::len2d_sqr()
{
	reg = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(0,1,0,1) );
	reg = _mm_mul_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
	return *this;
}
inline vector4f& vector4f::len2d_fast()
{
	reg = _mm_shuffle_ps( reg, reg, _MM_SHUFFLE(0,1,0,1) );
	reg = _mm_mul_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
	reg = _mm_mul_ps( reg, _mm_rsqrt_ps( reg ) );
	return *this;
}
inline float vector4f::len2d_const() const
{
	return clone().len2d().extract();
}
inline float vector4f::len2d_sqr_const() const
{
	return clone().len2d_sqr().extract();
}
inline float vector4f::len2d_fast_const() const
{
	return clone().len2d_fast().extract();
}



// Vector operations

inline float vector4f::norm()
{
	vector4f len = clone().length().add( epsilon() );
	div( len );
	return len.extract();
}
inline vector4f& vector4f::dot( const_ref_t vec )
{
	reg = _mm_mul_ps( reg, vec.reg );
	reg = _mm_hadd_ps( reg, reg );
	reg = _mm_hadd_ps( reg, reg );
	return *this;
}
inline vector4f& vector4f::cross( const_ref_t vec )
{
	reg = _mm_sub_ps(
		_mm_mul_ps( _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,0,2,1)), _mm_shuffle_ps(vec.reg,vec.reg,_MM_SHUFFLE(3,1,0,2)) ), 
		_mm_mul_ps( _mm_shuffle_ps(reg,reg,_MM_SHUFFLE(3,1,0,2)), _mm_shuffle_ps(vec.reg,vec.reg,_MM_SHUFFLE(3,0,2,1)) ));
	return *this;
}
inline vector4f& vector4f::lerp( const_ref_t vec, float alpha )
{
	return add( ( vec - *this ) *= alpha );
}
inline vector4f& vector4f::slerp( const_ref_t vec, float alpha )
{
	// This isn't exactly correct though :)
	vector4f len = vector4f(*this).length().lerp( vec, alpha );
	return lerp( vec, alpha ).div( length_fast() ).mul( len );
}

inline float vector4f::dot_const( const_ref_t vec ) const
{
	return clone().dot( vec ).extract();
}
inline vector4f vector4f::cross_const( const_ref_t vec ) const
{
	return clone().cross( vec );
}



// Format conversions

inline vector4f& vector4f::cvt_deg2rad()
{
	static const float cvt = 2.f * 3.14159265358979323846f / 360.f;
	return mul( cvt );
}
inline vector4f& vector4f::cvt_rad2deg()
{
	static const float cvt = 360.f / ( 2.f * 3.14159265358979323846f );
	return mul( cvt );
}



// Comparison
// FIXME! Move this to a separate class?

inline vector4f vector4f::cmpeq( const vector4f& vec ) const
{
	return vector4f( _mm_cmpeq_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmplt( const vector4f& vec ) const
{
	return vector4f( _mm_cmplt_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmple( const vector4f& vec ) const
{
	return vector4f( _mm_cmple_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpgt( const vector4f& vec ) const
{
	return vector4f( _mm_cmpgt_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpge( const vector4f& vec ) const
{
	return vector4f( _mm_cmpge_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpord() const
{
	return vector4f( _mm_cmpord_ps( reg, reg ) );
}
inline vector4f vector4f::cmpneq( const vector4f& vec ) const
{
	return vector4f( _mm_cmpneq_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpnlt( const vector4f& vec ) const
{
	return vector4f( _mm_cmpnlt_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpnle( const vector4f& vec ) const
{
	return vector4f( _mm_cmpnle_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpngt( const vector4f& vec ) const
{
	return vector4f( _mm_cmpngt_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpnge( const vector4f& vec ) const
{
	return vector4f( _mm_cmpnge_ps( reg, vec.reg ) );
}
inline vector4f vector4f::cmpunord() const
{
	return vector4f( _mm_cmpunord_ps( reg, reg ) );
}
//inline int vector4f::movemask() const
//{
//	return _mm_movemask_ps( reg );
//}
inline bool vector4f::any( int mask ) const
{
	return (_mm_movemask_ps( reg )&mask)!=0;
}
inline bool vector4f::all( int mask ) const
{
	return (_mm_movemask_ps( reg )&mask)==(0xF&mask);
}


// Operator convenience

inline vector4f& vector4f::operator= ( const_ref_t vec )
{
	reg = vec.reg;
	return *this;
}
inline vector4f& vector4f::operator+= ( const_ref_t vec )
{
	return add( vec );
}
inline vector4f& vector4f::operator-= ( const_ref_t vec )
{
	return sub( vec );
}
inline vector4f& vector4f::operator*= ( const_ref_t vec )
{
	return mul( vec );
}
inline vector4f& vector4f::operator*= ( float f )
{
	return mul( f );
}
inline vector4f& vector4f::operator/= ( const_ref_t vec )
{
	return div( vec );
}
inline vector4f& vector4f::operator/= ( float f )
{
	return div( f );
}

inline vector4f vector4f::operator+ ( const_ref_t vec ) const
{
	return clone().add( vec );
}
inline vector4f vector4f::operator- ( const_ref_t vec ) const
{
	return clone().sub( vec );
}
inline vector4f vector4f::operator- () const
{
	return clone().neg();
}
inline vector4f vector4f::operator* ( const_ref_t vec ) const
{
	return clone().mul( vec );
}
inline vector4f vector4f::operator* ( float f ) const
{
	return clone().mul( f );
}
inline vector4f vector4f::operator/ ( const_ref_t vec ) const
{
	return clone().div( vec );
}
inline vector4f vector4f::operator/ ( float f ) const
{
	return clone().div( f );
}
inline float& vector4f::operator[] ( unsigned i )
{
	assert( i<4 );
	return reg.m128_f32[i];
}
inline const float& vector4f::operator[] ( unsigned i ) const
{
	assert( i<4 );
	return reg.m128_f32[i];
}
inline vector4f::operator __m128& ()
{
	return reg;
}
inline vector4f::operator const __m128& () const
{
	return reg;
}

}

#endif // HGUARD_MATH_SSE_VECTOR4f
