#ifndef HGUARD_LIBEX_HASH_MD5
#define HGUARD_LIBEX_HASH_MD5

//------------------------------------------------
// MD5 hashing
//------------------------------------------------
// Do NOT use when security is important!

#include "md5core.h"

namespace hash
{

class MD5
{
public:
	MD5( const char* text );

	MD5() : _finalized(false) { MD5Init( &_context ); }
	MD5& Update( const void* buf, unsigned int length );
	MD5& Finalize();

	// Render the final hash in text.
	void Digest( char* out );
	// FIXME! Does not check if final!
	inline const types::byte* Raw() const { return _context.digest; }

private:
	MD5Context _context;
	bool _finalized;
};

}


#endif // !HGUARD_LIBEX_HASH_MD5
