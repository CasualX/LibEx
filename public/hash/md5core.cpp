/*
Copyright (C) 1991-2, RSA Data Security, Inc. Created 1991. All
rights reserved.

License to copy and use this software is granted provided that it
is identified as the "RSA Data Security, Inc. MD5 Message-Digest
Algorithm" in all material mentioning or referencing this software
or this function.

License is also granted to make and use derivative works provided
that such works are identified as "derived from the RSA Data
Security, Inc. MD5 Message-Digest Algorithm" in all material
mentioning or referencing the derived work.

RSA Data Security, Inc. makes no representations concerning either
the merchantability of this software or the suitability of this
software for any particular purpose. It is provided "as is"
without express or implied warranty of any kind.

These notices must be retained in any copies of any part of this
documentation and/or software.
*/


#include "md5core.h"

using types::byte;
using types::dword;


// F, G, H and I are basic MD5 functions.
FORCEINLINE dword MD5_F( dword x, dword y, dword z )
{
	return x&y | ~x&z;
}
FORCEINLINE dword MD5_G( dword x, dword y, dword z )
{
	return x&z | y&~z;
}
FORCEINLINE dword MD5_H( dword x, dword y, dword z )
{
	return x^y^z;
}
FORCEINLINE dword MD5_I( dword x, dword y, dword z )
{
	return y ^ (x | ~z);
}

// rotate_left rotates x left n bits.
FORCEINLINE dword MD5_rotate_left( dword x, dword n )
{
	return (x << n) | (x >> (32-n));
}

// FF, GG, HH, and II transformations for rounds 1, 2, 3, and 4.
// Rotation is separate from addition to prevent recomputation.
FORCEINLINE void MD5_FF( dword &a, dword b, dword c, dword d, dword x, dword s, dword ac )
{
	a = MD5_rotate_left( a + MD5_F(b,c,d) + x + ac, s ) + b;
}
FORCEINLINE void MD5_GG( dword &a, dword b, dword c, dword d, dword x, dword s, dword ac )
{
	a = MD5_rotate_left( a + MD5_G(b,c,d) + x + ac, s ) + b;
}
FORCEINLINE void MD5_HH( dword &a, dword b, dword c, dword d, dword x, dword s, dword ac )
{
	a = MD5_rotate_left( a + MD5_H(b,c,d) + x + ac, s ) + b;
}
FORCEINLINE void MD5_II( dword &a, dword b, dword c, dword d, dword x, dword s, dword ac )
{
	a = MD5_rotate_left( a + MD5_I(b,c,d) + x + ac, s ) + b;
}

// Constants for MD5Transform routine.
enum MD5Const
{
	MD5_S11 = 7,
	MD5_S12 = 12,
	MD5_S13 = 17,
	MD5_S14 = 22,
	MD5_S21 = 5,
	MD5_S22 = 9,
	MD5_S23 = 14,
	MD5_S24 = 20,
	MD5_S31 = 4,
	MD5_S32 = 11,
	MD5_S33 = 16,
	MD5_S34 = 23,
	MD5_S41 = 6,
	MD5_S42 = 10,
	MD5_S43 = 15,
	MD5_S44 = 21,
};




FORCEINLINE void MD5Encode( byte output[], const dword input[], unsigned int len )
{
		//for (unsigned int i = 0, j = 0; j < len; i++, j += 4) {
		//	output[j] = input[i] & 0xff;
		//	output[j+1] = (input[i] >> 8) & 0xff;
		//	output[j+2] = (input[i] >> 16) & 0xff;
		//	output[j+3] = (input[i] >> 24) & 0xff;
		//}
	memcpy( output, input, len );
}



void MD5Init( MD5Context* ctx )
{
	ctx->count[0] = 0;
	ctx->count[1] = 0;

	// Load magic initialization constants.
	ctx->state[0] = 0x67452301;
	ctx->state[1] = 0xefcdab89;
	ctx->state[2] = 0x98badcfe;
	ctx->state[3] = 0x10325476;
}


void MD5Transform( MD5Context* ctx, const byte block[MD5Context::blocksize] )
{
	dword a = ctx->state[0], b = ctx->state[1], c = ctx->state[2], d = ctx->state[3], x[16];
	memcpy( x, block, MD5Context::blocksize );

	/* Round 1 */
	MD5_FF( a, b, c, d, x[ 0], MD5_S11, 0xd76aa478 ); /* 1 */
	MD5_FF( d, a, b, c, x[ 1], MD5_S12, 0xe8c7b756 ); /* 2 */
	MD5_FF( c, d, a, b, x[ 2], MD5_S13, 0x242070db ); /* 3 */
	MD5_FF( b, c, d, a, x[ 3], MD5_S14, 0xc1bdceee ); /* 4 */
	MD5_FF( a, b, c, d, x[ 4], MD5_S11, 0xf57c0faf ); /* 5 */
	MD5_FF( d, a, b, c, x[ 5], MD5_S12, 0x4787c62a ); /* 6 */
	MD5_FF( c, d, a, b, x[ 6], MD5_S13, 0xa8304613 ); /* 7 */
	MD5_FF( b, c, d, a, x[ 7], MD5_S14, 0xfd469501 ); /* 8 */
	MD5_FF( a, b, c, d, x[ 8], MD5_S11, 0x698098d8 ); /* 9 */
	MD5_FF( d, a, b, c, x[ 9], MD5_S12, 0x8b44f7af ); /* 10 */
	MD5_FF( c, d, a, b, x[10], MD5_S13, 0xffff5bb1 ); /* 11 */
	MD5_FF( b, c, d, a, x[11], MD5_S14, 0x895cd7be ); /* 12 */
	MD5_FF( a, b, c, d, x[12], MD5_S11, 0x6b901122 ); /* 13 */
	MD5_FF( d, a, b, c, x[13], MD5_S12, 0xfd987193 ); /* 14 */
	MD5_FF( c, d, a, b, x[14], MD5_S13, 0xa679438e ); /* 15 */
	MD5_FF( b, c, d, a, x[15], MD5_S14, 0x49b40821 ); /* 16 */

	/* Round 2 */
	MD5_GG( a, b, c, d, x[ 1], MD5_S21, 0xf61e2562 ); /* 17 */
	MD5_GG( d, a, b, c, x[ 6], MD5_S22, 0xc040b340 ); /* 18 */
	MD5_GG( c, d, a, b, x[11], MD5_S23, 0x265e5a51 ); /* 19 */
	MD5_GG( b, c, d, a, x[ 0], MD5_S24, 0xe9b6c7aa ); /* 20 */
	MD5_GG( a, b, c, d, x[ 5], MD5_S21, 0xd62f105d ); /* 21 */
	MD5_GG( d, a, b, c, x[10], MD5_S22, 0x02441453 ); /* 22 */
	MD5_GG( c, d, a, b, x[15], MD5_S23, 0xd8a1e681 ); /* 23 */
	MD5_GG( b, c, d, a, x[ 4], MD5_S24, 0xe7d3fbc8 ); /* 24 */
	MD5_GG( a, b, c, d, x[ 9], MD5_S21, 0x21e1cde6 ); /* 25 */
	MD5_GG( d, a, b, c, x[14], MD5_S22, 0xc33707d6 ); /* 26 */
	MD5_GG( c, d, a, b, x[ 3], MD5_S23, 0xf4d50d87 ); /* 27 */
	MD5_GG( b, c, d, a, x[ 8], MD5_S24, 0x455a14ed ); /* 28 */
	MD5_GG( a, b, c, d, x[13], MD5_S21, 0xa9e3e905 ); /* 29 */
	MD5_GG( d, a, b, c, x[ 2], MD5_S22, 0xfcefa3f8 ); /* 30 */
	MD5_GG( c, d, a, b, x[ 7], MD5_S23, 0x676f02d9 ); /* 31 */
	MD5_GG( b, c, d, a, x[12], MD5_S24, 0x8d2a4c8a ); /* 32 */

	/* Round 3 */
	MD5_HH( a, b, c, d, x[ 5], MD5_S31, 0xfffa3942 ); /* 33 */
	MD5_HH( d, a, b, c, x[ 8], MD5_S32, 0x8771f681 ); /* 34 */
	MD5_HH( c, d, a, b, x[11], MD5_S33, 0x6d9d6122 ); /* 35 */
	MD5_HH( b, c, d, a, x[14], MD5_S34, 0xfde5380c ); /* 36 */
	MD5_HH( a, b, c, d, x[ 1], MD5_S31, 0xa4beea44 ); /* 37 */
	MD5_HH( d, a, b, c, x[ 4], MD5_S32, 0x4bdecfa9 ); /* 38 */
	MD5_HH( c, d, a, b, x[ 7], MD5_S33, 0xf6bb4b60 ); /* 39 */
	MD5_HH( b, c, d, a, x[10], MD5_S34, 0xbebfbc70 ); /* 40 */
	MD5_HH( a, b, c, d, x[13], MD5_S31, 0x289b7ec6 ); /* 41 */
	MD5_HH( d, a, b, c, x[ 0], MD5_S32, 0xeaa127fa ); /* 42 */
	MD5_HH( c, d, a, b, x[ 3], MD5_S33, 0xd4ef3085 ); /* 43 */
	MD5_HH( b, c, d, a, x[ 6], MD5_S34,  0x4881d05 ); /* 44 */
	MD5_HH( a, b, c, d, x[ 9], MD5_S31, 0xd9d4d039 ); /* 45 */
	MD5_HH( d, a, b, c, x[12], MD5_S32, 0xe6db99e5 ); /* 46 */
	MD5_HH( c, d, a, b, x[15], MD5_S33, 0x1fa27cf8 ); /* 47 */
	MD5_HH( b, c, d, a, x[ 2], MD5_S34, 0xc4ac5665 ); /* 48 */

	/* Round 4 */
	MD5_II( a, b, c, d, x[ 0], MD5_S41, 0xf4292244 ); /* 49 */
	MD5_II( d, a, b, c, x[ 7], MD5_S42, 0x432aff97 ); /* 50 */
	MD5_II( c, d, a, b, x[14], MD5_S43, 0xab9423a7 ); /* 51 */
	MD5_II( b, c, d, a, x[ 5], MD5_S44, 0xfc93a039 ); /* 52 */
	MD5_II( a, b, c, d, x[12], MD5_S41, 0x655b59c3 ); /* 53 */
	MD5_II( d, a, b, c, x[ 3], MD5_S42, 0x8f0ccc92 ); /* 54 */
	MD5_II( c, d, a, b, x[10], MD5_S43, 0xffeff47d ); /* 55 */
	MD5_II( b, c, d, a, x[ 1], MD5_S44, 0x85845dd1 ); /* 56 */
	MD5_II( a, b, c, d, x[ 8], MD5_S41, 0x6fa87e4f ); /* 57 */
	MD5_II( d, a, b, c, x[15], MD5_S42, 0xfe2ce6e0 ); /* 58 */
	MD5_II( c, d, a, b, x[ 6], MD5_S43, 0xa3014314 ); /* 59 */
	MD5_II( b, c, d, a, x[13], MD5_S44, 0x4e0811a1 ); /* 60 */
	MD5_II( a, b, c, d, x[ 4], MD5_S41, 0xf7537e82 ); /* 61 */
	MD5_II( d, a, b, c, x[11], MD5_S42, 0xbd3af235 ); /* 62 */
	MD5_II( c, d, a, b, x[ 2], MD5_S43, 0x2ad7d2bb ); /* 63 */
	MD5_II( b, c, d, a, x[ 9], MD5_S44, 0xeb86d391 ); /* 64 */

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;

	// Zeroize sensitive information.
	memset( x, 0, sizeof(x) );
}

__declspec(noinline)
void MD5Update( MD5Context* ctx, const void* src, unsigned int length )
{
	const byte* input = (const byte*) src;

	// compute number of bytes mod 64
	unsigned int index = ctx->count[0] / 8 % MD5Context::blocksize;

	// Update number of bits
	if ((ctx->count[0] += (length << 3)) < (length << 3))
		ctx->count[1]++;
	ctx->count[1] += (length >> 29);

	// number of bytes we need to fill in _buffer
	unsigned int firstpart = 64 - index;

	unsigned int i = 0;

	// transform as many times as possible.
	if (length >= firstpart)
	{
		// fill buffer first, transform
		memcpy( &ctx->buffer[index], input, firstpart );
		MD5Transform( ctx, ctx->buffer );

		// transform chunks of blocksize (64 bytes)
		for ( i = firstpart; i + MD5Context::blocksize <= length; i += MD5Context::blocksize )
			MD5Transform( ctx, &input[i] );

		index = 0;
	}

	// Buffer remaining input
	if ( length-i ) memcpy( &ctx->buffer[index], &input[i], length-i );
}


__declspec(noinline)
void MD5Final( MD5Context* ctx )
{
	static byte padding[64] =
	{
	 0x80, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0,
		0, 0, 0, 0, 0, 0, 0,  0, 0, 0, 0, 0, 0, 0, 0, 0
	};

	// Save number of bits
	byte bits[8];
	((dword*)bits)[0] = ctx->count[0];
	((dword*)bits)[1] = ctx->count[1];

	// pad out to 56 mod 64.
	unsigned int index = ctx->count[0] / 8 % 64;
	unsigned int padLen = (index < 56) ? (56 - index) : (120 - index);
	MD5Update( ctx, padding, padLen );

	// Append length (before padding)
	MD5Update( ctx, bits, 8 );

	// Zeroize sensitive information.
	memset( ctx->buffer, 0, MD5Context::blocksize );
	ctx->count[0] = 0;
	ctx->count[1] = 0;
}


unsigned int MD5Random( unsigned int seed )
{
	MD5Context ctx;
	MD5Init( &ctx );
	MD5Update( &ctx, &seed, sizeof(seed) );
	MD5Final( &ctx );
	return *reinterpret_cast<unsigned int*>( ctx.digest+6 );
}
