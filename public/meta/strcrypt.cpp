
#include <cstdlib>
#include <cstring>

#include "strcrypt.h"
#include "../pelite/image.h"

namespace meta
{

	
inline void strcrypt_t::setgkey( unsigned g )
{
#if STRCRYPT_ENABLE!=0
	// Should only ever be called once to set the global key...
	assert(_global==0);
#endif
	_global = g;
}
unsigned strcrypt_t::_global = 0;
inline bool strcrypt_t::iscrypted() const
{
	return this->_prefix==*(const __int64*)STRCRYPT_PREFIX
		|| this->_prefix==*(const __int64*)WSTRCRYPT_PREFIX;
}
	
NOINLINE void* strcrypt_t::_decrypt( void* buf, unsigned size ) const
{
	// Note: size parameter should get optimized out since it's only used in asserts...

	if ( this->_prefix==*(const __int64*)STRCRYPT_PREFIX )
	{
		// Binary wasn't processed for encrypted strings...
		assert( strlen(this->_str)<size );
		strcpy( (char*)buf, this->_str );
	}
	else if ( this->_prefix==*(const __int64*)WSTRCRYPT_PREFIX )
	{
		assert( wcslen((const wchar_t*)this->_str)*2<size );
		wcscpy( (wchar_t*)buf, (const wchar_t*)this->_str );
	}
	else
	{
		assert( static_cast<unsigned>(this->_len*4)<=size );

		unsigned key = this->getkey();
		// Decrypt in chunks of dwords
		for ( unsigned i = 0, len = this->_len; i<len; ++i )
		{
			key = key*1103515245+12345;
			((unsigned*)buf)[i] = (((unsigned*)this->_str)[i]-this->_key2) ^ key;
		}
	}
	return buf;
}
NOINLINE const void* strcrypt_t::_decrypt() const
{
	if ( this->iscrypted() )
	{
		// Binary wasn't processed for encrypted strings...
		return this->_str;
	}
	else
	{
		unsigned* buf = (unsigned*) ::malloc( this->_len*4 );
		unsigned key = this->getkey();
		// Decrypt in chunks of dwords
		for ( unsigned i = 0, len = this->_len; i<len; ++i )
		{
			key = key*1103515245+12345;
			((unsigned*)buf)[i] = (((unsigned*)this->_str)[i]-this->_key2) ^ key;
		}
		// Who is responsible for cleaning up? ... Leak memory? :)
		return buf;
	}
}




void strencrypt( strcrypt_t* src, unsigned key )
{
	// String length in dwords
	unsigned len;
	if ( src->_prefix==*(const __int64*)STRCRYPT_PREFIX )
	{
		len = strlen(src->_str)+1;
		len = (len>>2)+((len&3)!=0);
	}
	else if ( src->_prefix==*(const __int64*)WSTRCRYPT_PREFIX )
	{
		len = wcslen((wchar_t*)src->_str)+1;
		len = (len>>1)+((len&1)!=0);
	}
	else assert(false);
	// Fixup key
	src->_key2 = (~key)*22695477+1;
	src->_key = key ^ src->_global;
	src->_len = len;
	if ( src->_prefix==*(const __int64*)STRCRYPT_PREFIX || src->_prefix==*(const __int64*)WSTRCRYPT_PREFIX )
		src->_key2 = ~src->_key2;

	key = src->_key ^ src->_global;
	// Encrypt
	for ( unsigned i = 0; i<len; ++i )
	{
		key = key*1103515245+12345;
		(((unsigned*)src->_str)[i] ^= key) += src->_key2;
	}
}
void strencryptall( unsigned* it, unsigned* end, unsigned basekey, unsigned gkey )
{
	strcrypt_t::setgkey( gkey );
	for ( ; it<end; ++it )
	{
		strcrypt_t* src = (strcrypt_t*)it;
		if ( src->iscrypted() )
		{
			basekey = basekey*22695477+1;
			strencrypt( src, basekey );
		}
	}
}

}
