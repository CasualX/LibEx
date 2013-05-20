
#include "enumstring.h"
#include <cstdio>
#include <exception>

namespace tools
{
	
template< typename C > struct _copy {
	static inline void f( const C* src, C*& buf, unsigned& len )
	{
		for ( ; len; ++src, ++buf, --len )
		{
			*buf = *src;
			if ( !*src )
				break;
		}
	}
};
template< typename C >
inline bool _isspace( C c )
{
	types::make_unsigned<C>::type k = c-1;
	if ( k>=32 )
		return false;
	unsigned int mask = (1<<(' '-1)) | (1<<('\t'-1)) | (1<<('\n'-1)) | (1<<('\r'-1));
	return ((1<<k)&mask)!=0;
}

	
int ES_enum_char_t::Parse( const char* s ) const
{
	if ( en.flags&1 )
	{
		// Flags enum
		const char sep = ',';
		int out = 0;
		const char* n;
		for ( bool b = true; b; s = n+1 )
		{
			// Find next separator
			for ( n = s; *n && *n!=sep; ++n );
			b = *n!=0;

			// Strip whitespace
			while ( _isspace( *s ) ) ++s;

			// Lookup identifier
			if ( enum_t::const_iterator it = en.lookup( s, sep ) )
				out |= it->value;
			else goto notfound;
		}
		// All success
		return out;
	}
	else
	{
		// Single enum
		if ( enum_t::const_iterator it = en.lookup( s, 0 ) )
			return it->value;
	}

notfound:
	char buf[512];
	_snprintf( buf, sizeof(buf), "Cannot parse \"%s\" to %s!", s, nullptr );
	throw std::exception( buf );
	return 0;
}
bool ES_enum_char_t::Parse( const char* s, int* value ) const
{
	if ( en.flags&1 )
	{
		// Flags enum
		const char sep = ',';
		*value = 0;
		const char* n;
		for ( bool b = true; b; s = n+1 )
		{
			// Find next separator
			for ( n = s; *n && *n!=sep; ++n );
			b = *n!=0;

			// Strip whitespace
			while ( _isspace( *s ) ) ++s;

			// Lookup identifier
			if ( enum_t::const_iterator it = en.lookup( s, sep ) )
				*value |= it->value;
			else return false;
		}
		// All success
		return true;
	}
	else
	{
		// Single enum
		if ( enum_t::const_iterator it = en.lookup( s, 0 ) )
			return *value = it->value, true;
		return false;
	}
}
int ES_enum_char_t::Parse( const char* s, int def ) const
{
	if ( en.flags&1 )
	{
		// Flags enum
		const char sep = ',';
		int out = 0;
		const char* n;
		for ( bool b = true; b; s = n+1 )
		{
			// Find next separator
			for ( n = s; *n && *n!=sep; ++n );
			b = *n!=0;

			// Strip whitespace
			while ( _isspace( *s ) ) ++s;

			// Lookup identifier
			enum_t::const_iterator it = en.lookup( s, sep );
			out |= it? it->value : def;
		}
		// All success
		return out;
	}
	else
	{
		// Single enum
		enum_t::const_iterator it = en.lookup( s, 0 );
		return it? it->value : def;
	}
}
bool ES_enum_char_t::Render( int value, char* buf, unsigned len, int type ) const
{
	if ( en.flags&1 )
	{
		const char sep = ',';

		for ( int x = 1; x; x = x<<1 )
		{
			// Test if this flag/bit is set
			if ( value&x )
			{
				// Remove bit so we can test if we converted everything
				value = value&~x;
				if ( enum_t::const_iterator it = en.lookup( x ) )
				{
					_copy<char>::f( it->str, buf, len-=2 );
addcomma:
					if ( value )
					{
						*buf++ = sep;
						*buf++ = ' ';
						*buf = 0;
					}
					else break;
				}
				else if ( type==0 )
				{
					value = x;
					goto notfound;
				}
				else if ( type==1 )
				{
					return false;
				}
				else //if ( type==2 )
				{
					len -= _snprintf( buf, len-=2, "%d", x );
					goto addcomma;
				}
			}
		}
		return true;
	}
	else
	{
		if ( enum_t::const_iterator it = en.lookup( value ) )
		{
			_copy<char>::f( it->str, buf, len );
			return true;
		}
		else if ( type==0 ) goto notfound;
		else if ( type==1 ) return false;
		else //if ( type==2 )
		{
			_snprintf( buf, len, "%d", value );
		}
	}

notfound:
	// TODO!!!
	//assert( false );
	*(int*)buf = *(const int*)"ERR\0";
	return false;
}
bool ES_enum_char_t::Lookup( int i, int& val, char* buf, unsigned len ) const
{
	enum_t::const_iterator it = &en.list[i];
	if ( it->str )
	{
		val = it->value;
		_copy<char>::f( it->str, buf, len );
		return true;
	}
	return false;
}

}

ENUMSTRING( bool )
{
	{ true, "true" },
	{ true, "yes" },
	{ true, "1\0" "0" },
	{ false, "false" },
	{ false, "no" },
	{ false, "1\0" "0"+2 },
	{ 0, nullptr }
};
ENUMEXPDEF( bool );
