
#include <cstdio>
#include "md5.h"

namespace hash
{

MD5::MD5( const char* text ) : _finalized(false)
{
	MD5Init( &_context );
	size_t len = strlen(text);
	assert( len<0x100000000 );
	MD5Update( &_context, text, static_cast<unsigned int>(len) );
	MD5Final( &_context );
	_finalized = true;
}

MD5& MD5::Update( const void* buf, unsigned int length )
{
	if ( !_finalized )
	{
		MD5Update( &_context, buf, length );
	}
	return *this;
}
MD5& MD5::Finalize()
{
	if ( !_finalized )
	{
		MD5Final( &_context );
		_finalized = true;
	}
	return *this;
}

void MD5::Digest( char* out )
{
	Finalize();

	for ( int i = 0; i<16; i++ )
	{
		unsigned char d, c;
		d = _context.digest[i];

		c = (d>>4) + '0';
		if ( c>'9' ) c = c - '0' - 10 + 'a';
		out[i*2] = c;

		c = (d&0xF) + '0';
		if ( c>'9' ) c = c - '0' - 10 + 'a';
		out[i*2+1] = c;
	}
}


}
