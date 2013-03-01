
#include "path.h"
#include "../libex.h"
#ifdef FILESYSTEM_USE_EXCEPTIONS
#include <exception>
#endif // FILESYSTEM_USE_EXCEPTIONS

namespace filesystem
{

NOINLINE bool path::normalize()
{
	// Unfinished sort of...
	// (done) * Collapse all \ and / into a single \
	// (done) * Lowercase the whole thing (for easy path comparison later)
	// (todo) * Resolve .. into parent directory if possible

	char* in = buffer;
	char* out = buffer;
	while ( *in )
	{
		char c = *in;
		switch ( c )
		{
		case '/':
			c = slash;
		case slash:
			while ( *++in=='/' || *in==slash );
			*out++ = c;
			break;

		default:
			if ( c>='A' && c<='Z' ) c = c - 'A' + 'a';
			*out++ = c;
			in++;
			break;
		}
	}
	return true;
}
NOINLINE void path::append( const char* str )
{
	char* it = _end();
	// Check if we have a slash, if not add one
	if ( !empty() && *(it-1)!=slash )
		*it++ = slash;
	// Check if str comes with a slash prefixed, if so skip it
	if ( *str==slash )
		str++;
	_copy( it, str );
}
NOINLINE void path::append( const wchar_t* str )
{
	char* it = _end();
	// Check if we have a slash, if not add one
	if ( *(it-1)!=slash )
		*it++ = slash;
	// Check if str comes with a slash prefixed, if so skip it
	if ( *str==slash )
		str++;
	_copy( it, str );
}
NOINLINE void path::remove_filename()
{
	char* it = const_cast<char*>( filename() );
	*it = 0;
}
NOINLINE void path::replace_filename( const char* new_name )
{
	char* it = const_cast<char*>( filename() );
	_copy( it, new_name );
}
NOINLINE void path::replace_filename( const wchar_t* new_name )
{
	char* it = const_cast<char*>( filename() );
	_copy( it, new_name );
}
NOINLINE void path::replace_ext( const char* new_ext )
{
	assert( new_ext[0]==0 || new_ext[0]=='.' );

	char* it = const_cast<char*>( ext() );
	_copy( it, new_ext );
}
NOINLINE void path::replace_ext( const wchar_t* new_ext )
{
	assert( new_ext[0]==0 || new_ext[0]=='.' );

	char* it = const_cast<char*>( ext() );
	_copy( it, new_ext );
}
NOINLINE void path::replace_stem( const char* new_stem )
{
	assert( new_stem[0]==0 || new_stem[0]=='.' );

	char* it = const_cast<char*>( stem() );
	_copy( it, new_stem );
}
NOINLINE void path::replace_stem( const wchar_t* new_stem )
{
	assert( new_stem[0]==0 || new_stem[0]=='.' );

	char* it = const_cast<char*>( stem() );
	_copy( it, new_stem );
}
NOINLINE void path::make_absolute( const path& dir )
{
	// Must be a valid directory (ending in a slash)
	assert( dir.is_directory() );

	if ( is_relative() )
	{
		// Easiest is just to rebuild it...
		path temp( dir );
		temp /= buffer;
		*this = temp;
	}
}
NOINLINE bool path::make_relative( const path& dir )
{
	// Must be a valid directory (ending in a slash)
	assert( dir.is_directory() );

	if ( const char* str = relative_path( dir ) )
	{
		_copy( buffer, str );
		return true;
	}

	return false;
}
NOINLINE void path::parent_dir()
{
	char* parent = 0;
	char* fname = 0;
	for ( char* it = buffer; *it; ++it )
	{
		if ( *it==slash )
		{
			parent = fname;
			fname = it+1;
		}
	}
	if ( parent )
		*parent = 0;
}
void path::make_dir()
{
	char* p = _end();

	if ( *(p-1)!=slash )
	{
		if ( !_testlen( p, 1 ) )
			return;

		p[0] = slash;
		p[1] = 0;
	}
}

const char* path::ext() const
{
	const char* dot = 0;
	const char* it;
	for ( it = buffer; *it; ++it )
	{
		if ( *it==slash )
			dot = 0;
		else if ( *it=='.' )
			dot = it;
	}
	return dot?dot:it;
}
const char* path::stem() const
{
	const char* dot = 0;
	const char* it;
	for ( it = buffer; *it; ++it )
	{
		if ( *it==slash )
			dot = 0;
		else if ( *it=='.' && !dot )
			dot = it;
	}
	return dot?dot:it;
}
const char* path::filename() const
{
	const char* fname = buffer;
	for ( const char* it = buffer; *it; ++it )
	{
		if ( *it==slash )
			fname = it+1;
	}
	return fname;
}
const char* path::relative_path( const path& dir ) const
{
	assert( dir.is_directory() );

	// Compare until 0 or inequal.
	unsigned int i;
	for ( i = 0; dir.buffer[i] && dir.buffer[i]==buffer[i]; ++i );

	if ( dir.buffer[i] )
		return 0;
	else
		return buffer+i;
}
const char* path::root_path() const
{
	assert( is_absolute() );

	// Find first slash (skip if we start with a slash)
	for ( const char* it = buffer+1; *it; ++it )
	{
		if ( *it==slash )
			return it+1;
	}
	// Shouldn't happen :/
	return buffer;
}


bool path::is_directory() const
{
	if ( !empty() && const_cast<path*>(this)->_end()[-1]==slash )
		return true;
	return false;
}


NOINLINE char* path::_copy( char* to, const char* from )
{
	char c;
	while ( _testlen( to, 1 ) && ( c = *from++ ) )
	{
		// Convert slashes
		if ( c=='/' || c==slash )
		{
			// Skip redundant slashes
			do c = *from++;
			while ( c=='/' || c==slash );
			--from;
			// Replace with a single slash
			c = slash;
		}
		// Lowercase for comparisons
		else if ( c>='A' && c<='Z' ) c = c-'A'+'a';
		*to++ = c;
	}
	*to = 0;
	return to;
}
NOINLINE char* path::_copy( char* to, const wchar_t* src )
{
	// Convert utf16le (wchar_t) to utf8 (char)
	wchar_t c;
	while ( _testlen( to, 3 ) && ( c = *src++ ) )
	{
		// Convert slashes
		if ( c=='/' || c==slash )
		{
			// Skip redundant slashes
			do c = *src++;
			while ( c=='/' || c==slash );
			--src;
			// Replace with a single slash
			//c = slash;
			*to++ = slash;
			continue;
		}
		// Lower case for comparisons
		else if ( c>='A' && c<='Z' ) c = c-'A'+'a';
		// Encode
		if ( c<(1<<7) )
		{
			*to++ = static_cast<char>( c );
		}
		else if ( c<(1<<11) )
		{
			*to++ = 0xC0|((c>>6)&0x1F);
			*to++ = 0x80|((c)&0x3F);
		}
		else
		{
			*to++ = 0xE0|((c>>12)&0x0F);
			*to++ = 0x80|((c>>6)&0x3F);
			*to++ = 0x80|((c)&0x3F);
		}
	}
	*to = 0;
	return to;
}
char* path::_end()
{
	char* it = buffer;
	while ( *it ) ++it;
	return it;
}
void path::_errlen()
{
#ifdef FILESYSTEM_USE_EXCEPTIONS
	throw std::exception( "path too long!" );
#endif // FILESYSTEM_USE_EXCEPTIONS
}
void path::_errenc( wchar_t c )
{
#ifdef FILESYSTEM_USE_EXCEPTIONS
	throw std::exception( "invalid encoding!" );
#endif // FILESYSTEM_USE_EXCEPTIONS
}


#ifdef _DEBUG
#if 0
bool path::_unit_test()
{
	path p;
	::GetCurrentDirectoryA( path::max_length, p.buffer );
	p.normalize();
	p.make_dir();
	p.parent_dir();

	path base( "C:/Users/" );
	p.relative_path( base );
	p.make_relative( base );
	p.make_absolute( base );

	p /= "file.txt";

	const wchar_t* wsrc = L"C:/\u0234usèrs/.\u1ABCadm/bï.ts/userdata.dat.zip";
	path uni( wsrc );
	uni.ext();
	uni.stem();
	uni.filename();

	uni.replace_ext( ".rar" );
	uni.replace_stem( ".bin" );
	uni.replace_filename( "etc.txt" );
	uni.remove_filename();

	path buf;
	::WideCharToMultiByte( CP_UTF8, 0, wsrc, -1, buf.buffer, buf.max_length, NULL, NULL );

	return true;
}
#endif
#endif // _DEBUG

}
