#ifndef HGUARD_LIBEX_META_RNDHEADER
#define HGUARD_LIBEX_META_RNDHEADER
#pragma once

// Best I can do...

// Ignore the overflow warnings...
#define RANDOMBITS(bits) ((RANDOMRAW>>(32-bits))&((1<<bits)-1))
#define RANDOMBYTE static_cast<unsigned char>( RANDOMBITS(8) )
#define RANDOMSHORT static_cast<short>( RANDOMBITS(16) )
#define RANDOMINT static_cast<int>( (RANDOMBITS(16)<<16) | RANDOMBITS(16) )
//#define RANDOMLONG static_cast<__int64>( static_cast<unsigned __int64>(static_cast<unsigned __int64>(RANDOMINT)<<32) | static_cast<unsigned __int64>(RANDOMINT) )
#define RANDOMFLOAT RANDOMBITS(30)/1073741824.0f

// Uses __LINE__, __COUNTER__, __TIME__, __DATE__ to generate compile time random numbers
#define RANDOMRAW (( (( (( (( (( (( (( (( (__LINE__*100+__COUNTER__) \
	^__TIME__[0]^__DATE__[0])*1103515245+12345) \
	^__TIME__[1]^__DATE__[1])*1103515245+12345) \
	^__TIME__[2]^__DATE__[2])*1103515245+12345) \
	^__TIME__[3]^__DATE__[3])*1103515245+12345) \
	^__TIME__[4]^__DATE__[4])*1103515245+12345) \
	^__TIME__[5]^__DATE__[5])*1103515245+12345) \
	^__TIME__[6]^__DATE__[6])*1103515245+12345) \
	^__TIME__[7]^__DATE__[7])*1103515245+12345)

#endif // !HGUARD_LIBEX_META_RNDHEADER
