#ifndef HGUARD_LIBEX_HASH_MD5CORE
#define HGUARD_LIBEX_HASH_MD5CORE

//------------------------------------------------
// MD5 Internal operations
//------------------------------------------------

#include "../libex.h"

struct MD5Context
{
	enum { blocksize = 64, };
	types::byte buffer[blocksize]; // bytes that didn't fit in last 64 byte chunk
	types::dword count[2]; // 64bit counter for number of bits (lo, hi)
	union
	{
		types::dword state[4]; // digest so far
		types::byte digest[16]; // the result
	};
};

void MD5Init( MD5Context* ctx );
void MD5Update( MD5Context* ctx, const void* src, unsigned int len );
void MD5Final( MD5Context* ctx );
unsigned int MD5Random( unsigned int seed );

#endif // !HGUARD_LIBEX_HASH_MD5CORE
