
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
	int i = 0;
	while ( const char* s = Index( i++, e ) )
	{
		if ( CEnumBase_strncmp( str, s, end-str ) )
		{
			return true;
		}
	}
	return false;
}
const char* CEnumBase::Enum( enum_t e, temp_t& buf ) const
{
	int i = 0;
	enum_t val;
	while ( const char* s = Index( i++, val, buf ) )
	{
		if ( e==val )
		{
			return s;
		}
	}
	return false;
}
CEnumBase::enum_t CEnumBase::Parse( const char* str ) const
{
	return Flags() ? _ParseFlags( str ) : _ParseEnum( str );
}
bool CEnumBase::Parse( const char* str, enum_t* e ) const
{
	return Flags() ? _ParseFlags( str, e ) : _ParseEnum( str, e );
}
CEnumBase::enum_t CEnumBase::Parse( const char* str, enum_t def ) const
{
	return Flags() ? _ParseFlags( str, def ) : _ParseEnum( str, def );
}
bool CEnumBase::Render( enum_t e, char* buf, size_t len, int type ) const
{
	// Fallback to normal enum if no flags are set
	return ( Flags() && e ) ? _RenderFlags( e, buf, len, type ) : _RenderEnum( e, buf, len, type );
}
CEnumBase::enum_t CEnumBase::_ParseEnum( const char* str ) const
{
	enum_t e = 0;
	size_t c = strlen(str);
	if ( !String( e, str, str+c ) )
	{
		const char* name = Index( INDEX_NAME, e );
		throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: missing \"%*s\" of type %s!"), c, str, name ) );
	}
	return e;
}
bool CEnumBase::_ParseEnum( const char* str, enum_t* e ) const
{
	return String( *e, str, str+strlen(str) );
}
CEnumBase::enum_t CEnumBase::_ParseEnum( const char* str, enum_t def ) const
{
	enum_t e = 0;
	return String( e, str, str+strlen(str) ) ? e : def;
}
bool CEnumBase::_RenderEnum( enum_t e, char* buf, size_t len, int type ) const
{
	const char* s;
	temp_t temp;
	if (( s = Enum( e, temp ) ))
	{
addstr:
		// Assume this always fits...
		assert( len>strlen(s) );
		strcpy( buf, s );
		return true;
	}
	else if ( type==R_THROW )
	{
		enum_t x;
		const char* name = Index( INDEX_NAME, x, temp );
		throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: %d not found for %s!"), e, name ) );
	}
	else if ( type==R_FALSE )
	{
		return false;
	}
	else// if ( type==R_INT )
	{
		temp.print( STRDEF("%d"), e );
		goto addstr;
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
CEnumBase::enum_t CEnumBase::_ParseFlags( const char* s ) const
{
	char sep = Flags();

	// Flags enum
	enum_t e, out = 0;
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
			out |= e;
		}
		else
		{
			const char* name = Index( INDEX_NAME, e );
			throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: missing \"%*s\" of type %s!"), n-s, s, name ) );
		}
	}
	return out;
}
bool CEnumBase::_ParseFlags( const char* s, enum_t* out ) const
{
	char sep = Flags();

	// Flags enum
	enum_t e;
	*out = 0;
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
			*out |= e;
		}
		else
		{
			return false;
		}
	}
	return true;
}
CEnumBase::enum_t CEnumBase::_ParseFlags( const char* s, int def ) const
{
	char sep = Flags();

	// Flags enum
	enum_t e, out = 0;
	const char* n;
	for ( bool b = true; b; s = n+1 )
	{
		// Find next separator
		for ( n = s; *n && *n!=sep; ++n );
		b = *n!=0;

		// Strip whitespace
		while ( _isspace( *s ) ) ++s;

		// Lookup identifier
		if ( !String( e, s, n ) )
		{
			e = def;
		}
		out |= e;
	}
	return out;
}
bool CEnumBase::_RenderFlags( enum_t e, char* buf, size_t len, int type ) const
{
	char sep = Flags();

	// Need unsigned for standards compliance overflow...
	typedef unsigned int uenum_t;
	for ( uenum_t x = 1; x; x = x<<1 )
	{
		// Test if this flag/bit is set
		if ( static_cast<uenum_t>(e)&x )
		{
			// Remove bit so we can test if we converted everything
			e = static_cast<enum_t>(static_cast<uenum_t>(e)&~x);

			temp_t temp;
			const char* s;
			if (( s = Enum( x, temp ) ))
			{
addstr:
				// Copy enum to end of buffer
				size_t u = strlen(s);
				if ( u>len ) u = len;
				__movsb( (unsigned char*)buf, (const unsigned char*)s, u );
				buf += u;
				len -= u;
				if ( len==0 ) break;

				// If we still have values, add separator
				if ( e && len>0 )
				{
					*buf++ = sep;
					if ( !--len ) break;
				}
				else break;
			}
			else if ( type==R_THROW )
			{
				const char* name = Index( INDEX_NAME, e, temp );
				throw std::exception( va_printf<128,char>( STRDECRYPT("EnumString: 0x%X not found for %s!"), x, name ) );
			}
			else if ( type==R_FALSE )
			{
				return false;
			}
			else //if ( type==R_INT )
			{
				s = ( temp.print( STRDEF("0x%X"), x ), temp );
				goto addstr;
			}
		}
	}

	// Buffer overflow, add ellipsis.
	if ( len==0 )
	{
		*(unsigned int*)(buf-4) = *(const unsigned int*)"...\0";
	}
	else
	{
		// Null terminator...
		*buf = 0;
	}
	return true;
}


const char* CEnumDefault::Index( int index, enum_t& e, temp_t& buf ) const
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
ENUMSTRING( bool, { true, "true" }, { true, "yes" }, { true, "1" }, { false, "false" }, { false, "no" }, { false, "0" } );
