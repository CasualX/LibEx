
#include "vector4f_math.h"

namespace math_sse
{

	enum { TRIG_ITERATIONS = 12, };

	
#ifdef _M_IX86
__declspec(naked) void __fastcall sin4f( float* vec )
{
__asm {

	fld [ecx]
	fsin
	fstp [ecx]

	fld [ecx+4]
	fsin
	fstp [ecx+4]

	fld [ecx+8]
	fsin
	fstp [ecx+8]

	fld [ecx+12]
	fsin
	fstp [ecx+12]

	retn
}
}
#endif
	
vector4f sin4f_rd( const vector4f& in )
{
	// Precomputed reciprocal factorials: 1/(3+2n)!
	static MATH_SSE_ALIGN16 const float table[16] = {
		0.1666666666666667f, 0.008333333333333f, 0.000198412698412f, 2.7557319224e-006f,
		2.505210838544171e-008f, 1.60590438368216146e-010f, 7.6471637318198164759e-013, 2.8114572543455207632e-015f,
		8.220635246624330e-018f, 1.9572941063391261231e-020f, 3.8681701706306840377e-023f, 6.4469502843844733962e-026f,
		9.183689863795546e-029f, 1.130996288644771693e-031f, 1.21612504155351794963e-034f, 1.15163356207719502805868815e-037f,
	};
	// Different precomputed table
	static MATH_SSE_ALIGN16 const float table2[16] = {
		1./(2*3), 1./(4*5), 1./(6*7), 1./(7*8),
		1./(9*10), 1./(11*12), 1./(13*14), 1./(15*16),
		1./(17*18), 1./(19*20), 1./(21*22), 1./(23*24),
		1./(25*26), 1./(27*28), 1./(29*30), 1./(31*32),
	};
	assert( TRIG_ITERATIONS>0 && TRIG_ITERATIONS<=16 );

	vector4f m( in ), n( m * m ), s( m );

	// Sine using taylor series
	for ( auto it = table, end = it+TRIG_ITERATIONS; it<end; it+=4 )
	{
		vector4f facts = vector4f().set_aligned4( it );

		// Next factor in series
		( m *= n ).neg();
		// Improve sine
		s += m * vector4f(facts).shuffle<0,0,0,0>();

		// Next factor in series
		( m *= n ).neg();
		// Improve sine
		s += m * vector4f(facts).shuffle<1,1,1,1>();

		// Next factor in series
		( m *= n ).neg();
		// Improve sine
		s += m * vector4f(facts).shuffle<2,2,2,2>();

		// Next factor in series
		( m *= n ).neg();
		// Improve sine
		s += m * vector4f(facts).shuffle<3,3,3,3>();

		//// Next factor in series
		//s += m.mul( n ).neg().mul( vector4f(facts).shuffle<0,0,0,0>() );

		//// Next factor in series
		//s += m.mul( n ).neg().mul( vector4f(facts).shuffle<1,1,1,1>() );

		//// Next factor in series
		//s += m.mul( n ).neg().mul( vector4f(facts).shuffle<2,2,2,2>() );

		//// Next factor in series
		//s += m.mul( n ).neg().mul( vector4f(facts).shuffle<3,3,3,3>() );
	}
	return s;
}
vector4f cos4f_rd( const vector4f& in )
{
	// Precomputed reciprocal factorials: 1/(2+2n)!
	static MATH_SSE_ALIGN16 const float table[16] = {
		0.5f, 0.041666666666666666666666667f, 0.0013888888888888888888888888888889f, 0.0000248015873015873015873015873015873f,
		2.7557319223985890652557319224e-007, 2.087675698786809897921e-009, 1.14707455977297247138517e-011, 4.7794773323873852974382075e-014,
		1.561920696858622646221636435e-016, 4.110317623312164858478e-019, 8.89679139245057328674890e-022, 1.611737571096118349048713305e-024,
		2.479596263224797460074943546e-027, 3.279889237069837910152e-030, 3.76998762881590564385292e-033, 3.8003907548547435925936708928e-036,
	};
	assert( TRIG_ITERATIONS>0 && TRIG_ITERATIONS<=16 );

	vector4f ones( 1.f, 1.f, 1.f, 1.f ), m( ones ), n( in * in ), c( ones );

	for ( auto it = table, end = it+TRIG_ITERATIONS; it<=end; it+=4 )
	{
		vector4f facts = vector4f().set_aligned4( it );
		
		// Next factor in series
		( m *= n ).neg();
		// Improve cosine
		c += m * vector4f(facts).shuffle<0,0,0,0>();
		
		// Next factor in series
		( m *= n ).neg();
		// Improve cosine
		c += m * vector4f(facts).shuffle<1,1,1,1>();
		
		// Next factor in series
		( m *= n ).neg();
		// Improve cosine
		c += m * vector4f(facts).shuffle<2,2,2,2>();
		
		// Next factor in series
		( m *= n ).neg();
		// Improve cosine
		c += m * vector4f(facts).shuffle<3,3,3,3>();
	}

	return c;
}


}
