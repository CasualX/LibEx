
#include "md5.h"
#include "hash.h"
#include <cstring>

namespace hash
{

bool test()
{
	int fail = 0;
	
	if ( HASH("UnitTest")!=0xb5217335 ) ++fail;
	if ( hashit<hash_t>("UnitTest")!=0xb5217335 ) ++fail;
	if ( hashit<djb2_t>("UnitTest")!=0xfe65a4e5 ) ++fail;
	if ( hashit<sdbm_t>("UnitTest")!=0x810d2876 ) ++fail;
	if ( hashit<lose_t>("UnitTest")!=0x00000340 ) ++fail;
	if ( hashit<crc32_t>("UnitTest")!=0xd9cd03c0 ) ++fail;

	char buf[33]; buf[32] = 0;
	MD5( "apple" ).Digest( buf );
	if ( !!strcmp( buf, "1f3870be274f6c49b3e31a0c6728957f" ) ) ++fail;

	return fail==0;
}

}
