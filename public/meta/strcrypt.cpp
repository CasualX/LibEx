
#include <cstdlib>
#include <cstring>

#include "strcrypt.h"
#include "../pelite/image.h"

namespace meta
{

	
strcrypt_t::key_t strcrypt_t::_global = 0;
inline void strcrypt_t::setgkey( key_t g )
{
#if STRCRYPT_ENABLE!=0
	// Should only ever be called once to set the global key...
	assert(_global==0);
#endif
	_global = g;
}
inline bool strcrypt_t::hasprefix() const
{
	__int64 prefix = this->_prefix;
	return prefix==(*(const __int64*)STRCRYPT_PREFIX)
		|| prefix==(*(const __int64*)WSTRCRYPT_PREFIX);
}
unsigned long strcrypt_t::getlen() const
{
	return this->_len&255;
}
	
NOINLINE void* strcrypt_t::_decrypt( void* buf, unsigned long size ) const
{
	// Note: size parameter should get optimized out since it's only used in asserts...

	if ( this->_prefix==(*(const __int64*)STRCRYPT_PREFIX) )
	{
		// Binary wasn't processed for encrypted strings...
		assert( strlen(this->_str)<size );
		strcpy( (char*)buf, this->_str );
	}
	else if ( this->_prefix==(*(const __int64*)WSTRCRYPT_PREFIX) )
	{
		assert( wcslen((const wchar_t*)this->_str)*2<size );
		wcscpy( (wchar_t*)buf, (const wchar_t*)this->_str );
	}
	else
	{
		unsigned long len = this->getlen();
		assert( (len*4)<=size );
		_decrypt( this->getkey(), this->getiv(), (const key_t*)this->_str, (key_t*) buf, len );
	}
	return buf;
}
NOINLINE const void* strcrypt_t::_decrypt() const
{
	if ( this->hasprefix() )
	{
		// Binary wasn't processed for encrypted strings...
		return this->_str;
	}
	else
	{
		unsigned long len = this->getlen();
		key_t* buf = (key_t*) ::malloc( len*sizeof(key_t) );
		_decrypt( this->getkey(), this->getiv(), (const key_t*)this->_str, buf, len );
		// Who is responsible for cleaning up? ... Leak memory? :)
		return buf;
	}
}
void strcrypt_t::_decrypt( key_t key, key_t iv, const key_t* in, key_t* out, unsigned long len )
{
	// Shitty XOR 'decryption' with CBC chaining
	const key_t* end = in+len;
	do
	{
		key_t plain = *in ^ key ^ iv;
		iv = *in;
		*out = plain;
	}
	while ( ++in, ++out, in<end );
}


NOINLINE void strcrypt_t::_encrypt( key_t key )
{
	// String length in dwords
	unsigned long len;
	if ( this->_prefix==*(const __int64*)STRCRYPT_PREFIX )
	{
		len = static_cast<unsigned int>( strlen(this->_str)+1 );
		len = (((len-1)&~3)+4)/4;
	}
	else if ( this->_prefix==*(const __int64*)WSTRCRYPT_PREFIX )
	{
		len = static_cast<unsigned int>( wcslen((const wchar_t*)this->_str)+1 );
		len = (((len*2-2)&~3)+4)/4;
	}
	else
	{
		// Should never reach this...
		__debugbreak();
		len = 0;
	}

	// Encode the length in random data
	unsigned long bits = (key&~255)|len;

	// Initialize members
	this->_basekey = (~key)*22695477+1;
	this->_len = bits;

	// If accidentally the same as a prefix...
	if ( this->hasprefix() )
		this->_basekey = ~this->_basekey;

	// Encrypt
	_encrypt( this->getkey(), this->getiv(), (key_t*)this->_str, len );
}
void strcrypt_t::_encrypt( key_t key, key_t iv, key_t* str, unsigned long len )
{
	// Shitty XOR 'encryption' with CBC chaining
	for ( unsigned i = 0; i<len; ++i )
	{
		key_t cipher = str[i] ^ iv ^ key;
		iv = cipher;
		str[i] = cipher;
	}
}
void strcrypt_t::encryptall( unsigned long* it, unsigned long* end, key_t basekey, key_t gkey )
{
	setgkey( gkey );
	for ( ; it<end; ++it )
	{
		strcrypt_t* src = (strcrypt_t*)it;
		if ( src->hasprefix() )
		{
			basekey = basekey*22695477+1;
			src->_encrypt( basekey );
		}
	}
}

}
