#ifndef HGUARD_LIBEX_MATH_SSE_VECTOR4f
#define HGUARD_LIBEX_MATH_SSE_VECTOR4f
#pragma once

#include "defs.h"
#include "../vector.h"

namespace math
{

template<>
class MATH_ALIGN16 vector<float,4> {
public:
	typedef float type;
	static const dim dims = 4;

	typedef vector<float,4> value;
	typedef vector<float,4>& ref;
	typedef const vector<float,4>& const_ref;
	typedef bools<4> bools;

	//----------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------

	vector();
	vector( float a );
	vector( float x, float y, float z, float w = 0.0f );
	vector( __m128 xmm );
	vector( const_ref vec );
	ref assign( const_ref vec );

	//----------------------------------------------------------------
	// Assignment
	//----------------------------------------------------------------

	// Assign to x, y, z; leave w zero.
	static value dup3( const float f );
	// Assign all components the same constant.
	static value dup4( const float f );
	// Assign individual components, may be expensive if used incorrectly!
	static value set( const float x, const float y, const float z, const float w = 0.0f );
	// Zero the vector.
	static value zero();
	// Assign directly from xmm register.
	static value xmm( __m128 xmm );
	// Set from unaligned memory vector, w = 0.
	static value vector3( const float xyz[3] );
	// Load aligned(8) vector3, w = 0. Works with unaligned memory, but slower than set_vector3.
	static value aligned3( const float xyz[3] );
	// Load from unaligned memory vector.
	static value vector4( const float xyzw[4] );
	// Load aligned(16) vector. Will segfault on unaligned memory!
	static value aligned4( const float xyzw[4] );
	// Load unaligned integers (cvt to float)
	static value int4( const int xyzw[4] );
	// Load unaligned integers (cvt to float), z = 1, w = 0
	static value int2( const int xy[2] );
	// Set from 32bit argb color
	static value color( unsigned int clr );
	// Set individual X component.
	ref setx( const float x );
	// Set individual Y component.
	ref sety( const float y );
	// Set individual Z component.
	ref setz( const float z );
	// Set individual W component.
	ref setw( const float w );

	// Load 2 3D vectors in a row.
	static void load_2vector3( float src[6], ref a, ref b );
	// Load 4 3D vectors in a row.
	static void load_4vector3( float src[12], ref a, ref b, ref c, ref d );

	// Shuffle the components around, specify source of component: x = 0, y = 1, z = 2, w = 3.
	// Example: (xyzw).shuffle<2,0,1,1>() means, load position 2 (z) into x, load position 0 (x) into y, etc... Resulting in (zxyy)
	template< dim x, dim y, dim z, dim w >
	value shuffle() const;

	//----------------------------------------------------------------
	// Extraction
	//----------------------------------------------------------------

	// Get the X component.
	float getx() const;
	// Get the Y component.
	float gety() const;
	// Get the Z component.
	float getz() const;
	// Get the W component.
	float getw() const;
	// Get a component
	template< dim D >
	float get() const;
	// Get any component, useful for operations that ensure every component is the same.
	// In practice this returns a single component but makes no guaranties which component it is.
	// Use with length*(), len2d*(), dist*() and dot() operations.
	float extract() const;

	// Get as color.
	unsigned int packc() const;

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
	static value epsilon();

	//----------------------------------------------------------------
	// Basic operations
	//----------------------------------------------------------------

	// Negate the vector.
	value neg() const;
	// Absolute value of the vector.
	value abs() const;
	// Absolute value then negate the vector (make negative).
	value abs_neg() const;
	// Store the sign of each component, -1 for negative, 0 for zero and 1 for positive.
	value signum() const;
	// Copy the sign of the argument.
	value copysign( const_ref vec ) const;

	// Take the minimum components of ourself and vec.
	value min MATH_MINMAX_FIX ( const_ref vec ) const;
	// Take the maximum components of ourself and vec.
	value max MATH_MINMAX_FIX ( const_ref vec ) const;

	//----------------------------------------------------------------
	// Basic math
	//----------------------------------------------------------------

	// Add vector.
	value add( const_ref vec ) const;
	// Subtract vector.
	value sub( const_ref vec ) const;
	// Horizontally add components.
	value hadd() const;
	// Horizontally subtract components.
	value hsub() const;

	// Multiply vector component wise.
	value mul( const_ref vec ) const;
	// Fused multiply arguments then add.
	value madd( const_ref vec, const_ref scale ) const;
	// Divide vector.
	value div( const_ref vec ) const;
	// Modulus vector.
	value fmod( const_ref vec ) const;

	// Square the vector.
	value sqr() const;
	// Square root, accurate.
	value sqrt() const;
	// Inverse square root. Computes sqrt(vec)/vec.
	value rsqrt() const;
	// Fast square root. Computes rsqrt(vec)*vec.
	value fsqrt() const;

	//----------------------------------------------------------------
	// Rounding stuff
	//----------------------------------------------------------------
	
	// Round up.
	value ceil() const;
	// Round down.
	value floor() const;
	// Round to nearest int.
	value round() const;
	// Truncate, round to zero.
	value trunc() const;
	// Get fractional parts.
	value frac() const;

#if MATH_SSE_SUPPORT>=41
	// Generic round, see _MM_FROUND_* macros.
	template< int mode >
	value round() const;
#endif

	//----------------------------------------------------------------
	// Length and distance operations
	//----------------------------------------------------------------

	// Length of the vector.
	value length() const;
	// Squared length of the vector.
	value lensqr() const;
	// Fast length (less accurate) of the vector.
	value lenfast() const;

	// Resize the length of the vector to a given length, deals with zero length by adding epsilon().
	value resize( float len ) const;

	// Distance to vector.
	value dist( const_ref vec ) const;
	// Squared distance to vector.
	value distsqr( const_ref vec ) const;
	// Fast distance (less accurate) to vector.
	value distfast( const_ref vec ) const;

	// 2D length of the vector.
	value len2d() const;
	// 2D squared length of the vector.
	value len2dsqr() const;
	// 2D fast length (less accurate) of the vector.
	value len2dfast() const;

	//----------------------------------------------------------------
	// Vector operations
	//----------------------------------------------------------------

	// Normalize and return our length. Adds epsilon() to avoid div by zero.
	value norm() const;
	// Dot product.
	value dot( const_ref vec ) const;
	// Cross product.
	value cross( const_ref vec ) const;

	//----------------------------------------------------------------
	// Comparison
	//----------------------------------------------------------------

	// Equal
	bools cmpeq( const_ref vec ) const;
	// Less than
	bools cmplt( const_ref vec ) const;
	// Less than or equal
	bools cmple( const_ref vec ) const;
	// Greater than
	bools cmpgt( const_ref vec ) const;
	// Greater than or equal
	bools cmpge( const_ref vec ) const;
	// Is finite
	bools cmpord() const;

	// Not equal
	bools cmpneq( const_ref vec ) const;
	// Not less than
	bools cmpnlt( const_ref vec ) const;
	// Not less than or equal
	bools cmpnle( const_ref vec ) const;
	// Not greater than
	bools cmpngt( const_ref vec ) const;
	// Not greater than or equal
	bools cmpnge( const_ref vec ) const;
	// Is not finite
	bools cmpunord() const;

	//----------------------------------------------------------------
	// Operator convenience
	//----------------------------------------------------------------

	ref operator= ( __m128 xmm );

	ref operator= ( const_ref vec );
	ref operator+= ( const_ref vec );
	ref operator-= ( const_ref vec );
	ref operator*= ( const_ref vec );
	ref operator/= ( const_ref vec );
	ref operator%= ( const_ref vec );

	value operator+ ( const_ref vec ) const;
	value operator- ( const_ref vec ) const;
	value operator- () const;
	value operator* ( const_ref vec ) const;
	value operator/ ( const_ref vec ) const;
	value operator% ( const_ref vec ) const;

	bools operator< ( const_ref vec ) const;
	bools operator> ( const_ref vec ) const;
	bools operator<= ( const_ref vec ) const;
	bools operator>= ( const_ref vec ) const;
	bools operator== ( const_ref vec ) const;
	bools operator!= ( const_ref vec ) const;

	float& operator[] ( dim i );
	const float& operator[] ( dim i ) const;

	operator __m128& ();
	operator const __m128& () const;

private:
	__m128 reg;
};

typedef vector<float,4> vector4f;

}

#endif // HGUARD_LIBEX_MATH_SSE_VECTOR4f
