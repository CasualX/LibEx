
#include "enumstring.h"
#include "../meta/strcrypt.h"

#include <exception>

namespace tools
{

template< typename C >
static inline bool _isspace( C c )
{
	types::make_unsigned<C>::type k = c-1;
	if ( k>=32 )
		return false;
	unsigned int mask = (1<<(' '-1)) | (1<<('\t'-1)) | (1<<('\n'-1)) | (1<<('\r'-1));
	return ((1<<k)&mask)!=0;
}



static inline bool CEnumBase_strncmp( const char *s1, const char *s2, size_t n )
{
	for ( ; n > 0; s1++, s2++, --n )
	{
		if ( *s1!=*s2 )
			return false;
		else if ( *s1=='\0' )
			return true;
	}
	return true;
}
bool CEnumBase::String( enum_t& e, const char* str, const char* end ) const
{
	// Make sure to only overwrite e if we have a match
	enum_t t;
	int i = 0;
	while ( const char* s = Index( i++, t ) )
	{
		if ( CEnumBase_strncmp( str, s, end-str ) )
		{
			e = t;
			return true;
		}
	}
	return false;
}
const char* CEnumBase::Enum( enum_t e, temp_t& buffer ) const
{
	int i = 0;
	enum_t val;
	while ( const char* s = Index( i++, val, buffer ) )
	{
		if ( e==val )
		{
			return s;
		}
	}
	return false;
}
NOINLINE bool CEnumBase::Parse( const char* str, enum_t& e, int type ) const
{
	return Flags() ? _ParseFlags( str, e, type ) : _ParseEnum( str, e, type );
}
NOINLINE char* CEnumBase::Render( enum_t e, char* buffer, size_t len, int type ) const
{
	// Fallback to normal enum if no flags are set
	return ( Flags() && e ) ? _RenderFlags( e, buffer, len, type ) : _RenderEnum( e, buffer, len, type );
}
bool CEnumBase::_ParseEnum( const char* str, enum_t& e, int type ) const
{
	// Lookup string
	if ( String( e, str, str+strlen(str) ) )
	{
		return true;
	}
	// Try to parse as integer
	if ( type&ES_INT )
	{
		char* end;
		long i = strtol( str, &end, 10 );
		if ( *end=='\0' )
		{
			e = static_cast<enum_t>(i);
			return true;
		}
	}
	// Return false on failure
	if ( type&ES_FALSE )
	{
		return false;
	}
	// Throw exception on failure (recommended!)
	else
	{
		const char* name = Name();
		size_t c = strlen(str);
		throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: unknown \"%.*s\" of type %s!"), c, str, name ) );
	}
}
char* CEnumBase::_RenderEnum( enum_t e, char* buffer, size_t len, int type ) const
{
	const char* s;
	temp_t temp;
	if (( s = Enum( e, temp ) ))
	{
addstr:
		// Assume this always fits...
		// FIXME! Support ES_ELLIPSIS here?
		assert( len>strlen(s) );
		strcpy( buffer, s );
		return buffer;
	}
	// Write as integer instead
	else if ( type&ES_INT )
	{
		s = ( temp.format( e ), temp );
		goto addstr;
	}
	// Return false on failure
	else if ( type&ES_FALSE )
	{
		return false;
	}
	// Throw exception on failure (recommended!)
	else
	{
		const char* name = Name( temp );
		throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: %d unknown for %s!"), e, name ) );
	}
}

//char CEnumBase::_Separator( const char* s )
//{
//	// Find the first suitable separator and stick to it
//	char sep = ',';
//	for ( const char* t = s; *t; ++t )
//	{
//		if ( *t==',' || *t=='|' || *t==' ' )
//		{
//			sep = *t;
//			break;
//		}
//	}
//	return sep;
//}
bool CEnumBase::_ParseFlags( const char* s, enum_t& out, int type ) const
{
	char sep = Flags();

	// Flags enum
	enum_t e, t = 0;
	const char* n;
	for ( bool b = true; b; s = n+1 )
	{
		// Find next separator
		for ( n = s; *n && *n!=sep; ++n );
		b = *n!=0;

		// Strip whitespace
		while ( _isspace( *s ) ) ++s;

		// Lookup identifier
		if ( String( e, s, n ) )
		{
			t |= e;
		}
		else
		{
			// Try to parse as integer
			if ( type&ES_INT )
			{
				char* end;
				long i = strtol( s, &end, 10 );
				if ( end==n )
				{
					e = static_cast<enum_t>(i);
					t |= e;
					continue;
				}
			}
			// Return false on failure
			if ( type&ES_FALSE )
			{
				return false;
			}
			// Throw exception on failure (recommended!)
			else
			{
				const char* name = Name();
				throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: unknown \"%.*s\" of type %s!"), n-s, s, name ) );
			}
		}
	}
	out = t;
	return true;
}
char* CEnumBase::_RenderFlags( enum_t e, char* buffer, size_t len, int type ) const
{
	char sep = Flags();
	char* it = buffer;
	const char* s;
	enum_t x;
	temp_t temp;

	// Iterate through all valid flags
	for ( int i = 0; s = Index( i, x, temp ); ++i )
	{
		// Found a match
		if ( e&x )
		{
addstr:
			// Remove match
			e &= ~x;
			// Append flag name
			size_t u = strlen(s);
			if ( u>len ) u = len;
			__movsb( (unsigned char*)it, (const unsigned char*)s, u );
			it += u;
			len -= u;
			if ( len==0 )
				goto overflow;

			if ( e )
			{
				// If we still have values, add separator
				*it++ = sep;
				if ( !--len )
					goto overflow;
			}
			else
			{
				// Break out early if no more bits are set.
				break;
			}
		}
	}

	// If we still have bits set, raise error
	if ( e )
	{
		if ( type&ES_INT )
		{
			x = e;
			s = ( temp.printf( STRDEF("0x%X"), x ), temp );
			goto addstr;
		}
		// Return false on failure
		else if ( type&ES_FALSE )
		{
			return false;
		}
		// Throw exception on failure (recommended!)
		else
		{
			const char* name = Name( temp );
			throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: 0x%X unknown for %s!"), x, name ) );
		}
	}

	// Null terminator...
	*it = 0;
	return buffer;

overflow:
	assert( len==0 );

	// Buffer overflow, add ellipsis.
	if ( type&ES_ELLIPSIS )
	{
		*(unsigned int*)(it-4) = *(const unsigned int*)"...\0";
	}
	else
	{
		*(it-1) = '\0';
	}
	return buffer;
}


const char* CEnumDefault::Index( int index, enum_t& e, temp_t& buffer ) const
{
	if ( index<0 )
	{
		return name;
	}
	else if ( static_cast<unsigned int>(index)<count )
	{
		const pair_t& p = pairs[index];
		e = p.e;
		return p.s;
	}
	return nullptr;
}
char CEnumDefault::Flags() const
{
	return sep;
}

}
ENUMSTRING( bool, { true, "true" }, { true, "yes" }, { true, "1\0" "0" }, { false, "false" }, { false, "no" }, { false, "1\0" "0"+2 } );
