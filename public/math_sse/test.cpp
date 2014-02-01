
#include <Windows.h>
#include "vector4f.h"
#include "vector4f_math.h"

namespace math_sse
{




__declspec(noinline) bool UnitTest()
{
	//__m128 a, b, c, d, e, f;
	////a = _mm_xor_ps( a, a );
	//b = _mm_set_ps( 3.0f, 4.0f, 0.0f, 0.0f );
	//c = _mm_set_ps1( 4.53f );

	//__m128 len = b;
	//len = _mm_mul_ps( len, len );
	//len = _mm_hadd_ps( len, len );
	//len = _mm_hadd_ps( len, len );
	//len = _mm_sqrt_ps( len );


	//{
	//VectorSSE a, b, c, d;
	//a.SetZero();
	//b.SetConst( 1.0f, 2.0f, 3.0f, 4.0f );
	//c.SetDup( 4.53f );

	//b.Add( c ).Mult( -0.5f );
	//}

	vector4f a, b, c, d, e, f, g, h, k;
	const float fl[3] = { 1.2f, 2.3f, 3.4f };
	const int i[4] = { 1, 3, 5, 0 };

	a.set_zero();
	b.set_const( 3.0f, -4.0f, 0.0f );
	c.set_dup3( 4.53f );
	d.set_vector3( fl );

	e.set_int4( i );
	f.set_const( 2.0f, 4.0f, 6.0f );
	e.cross_const( f );

	LARGE_INTEGER begin, end, freq;
	::QueryPerformanceFrequency( &freq );
	::QueryPerformanceCounter( &begin );
	for ( int i = 0; i<10000000; ++i )
	{
		k = b;
		b = sin4f_rd( b );
		//vector4f( b ).sin2();
	}
	::QueryPerformanceCounter( &end );
	double time = static_cast<double>( *(__int64*)&end - *(__int64*)&begin ) / static_cast<double>( *(__int64*)&freq );
	printf( "Took %f seconds!\n", static_cast<float>(time) );

	float l = ( ( b + d ) * c / 4.53f ).extract_z();

	{vector4f* t = (vector4f*)0; *t = a; *t = b; *t = c; *t = d; }
	{float* t = (float*)0; *t = l; }
	
	return true;
}

}
