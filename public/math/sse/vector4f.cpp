#include "vector4f.hpp"

namespace math
{

int TestVector4f() {
	int fail = 0;

	// Define some vectors for testing.

	vector4f a( 1.0f, 2.0f, 3.0f, 4.0f ),
		     b( 0.0f, 1.3f, -3.1f, -0.1f ),
			 c,
			 one( 1.0f, 1.0f, 1.0f, 1.0f ),
			 two( 2.0f, 2.0f, 2.0f, 2.0f ),
			 zero( 0.0f, 0.0f, 0.0f, 0.0f );

	MATH_ALIGN16 float xyz[3] = { -1.0f, 2.0f, 3.0f };
	MATH_ALIGN16 float xyzw[4] = { 0.1f, 0.2f, 0.3f, 0.4f };

	int int2[2] = { 34, 15 };
	int int4[4] = { 8, 16, 32, 64 };

	c.assign( a );
	c = vector4f::dup3( -2.0f );
	c = vector4f::dup4( 8.1f );
	c = vector4f::zero();
	c = vector4f::set( 2.0f, 1.0f, 4.0f, 3.0f );
	c = vector4f::xmm( _mm_setzero_ps() );
	c = vector4f::vector3( xyz );
	c = vector4f::vector4( xyzw );
	c = vector4f::aligned3( xyz );
	c = vector4f::aligned4( xyzw );
	c = vector4f::int2( int2 );
	c = vector4f::int4( int4 );
	c = vector4f::color( 0x8010FF22 ).shuffle<2,1,0,3>();
	c.setx( 0.25f );
	c.sety( 0.26f );
	c.setz( 0.27f );
	c.setw( 0.28f );
	c = c.shuffle<2,2,1,0>();

	c.getx();
	c.gety();
	c.getz();
	c.getw();
	c.extract();
	c.export3( xyz );
	c.export4( xyzw );
	fail += !!memcmp( xyzw, c.data(), 4*4 );

	c = c.neg();
	c = c.abs();
	c = c.abs_neg();
	c = c.signum();
	c = c.copysign( b );
	c = c.min MATH_MINMAX_FIX ( a );
	c = c.max MATH_MINMAX_FIX ( b );
	clamp( c, vector4f::aligned4( xyzw ), zero );
	c.setx(-10.0f);
	if ( !all(c.floor()==vector4f(-10,1,-1,-1)) ) {
		printf( "wrong!" );
	}

	return fail;
}


}
