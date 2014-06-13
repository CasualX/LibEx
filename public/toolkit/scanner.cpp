
#include <exception>
#include <cstdarg>
#include <cstdio>

#include "scanner.h"
#include "tools.h"
#include "../pelite/pefile.h"
#include "../meta/strcrypt.h"

namespace toolkit
{
using types::byte;

/*
	void* OldFindPattern( void* begin, void* end, const char* pat, const char* mask, size_t len )
	{
		// Adjust the end pointer
		end = reinterpret_cast<void*>( (size_t)end - len );
		// Brute-force it
		for ( char* p = (char*)begin; p<end; ++p )
		{
			for ( size_t i = 0; i<len; ++i )
			{
				if ( (p[i]&mask[i])!=pat[i] ) goto not_found;
			}
			return p;
not_found:	;
		}
		return nullptr;
	}
	void* FindPattern( void* begin, void* end, const char* pat, const char* mask, size_t cnt )
	{
		int offs;
		// In case mask starts with with 0 characters, do some optimization work
		for ( offs = 0; !mask[offs]; ++offs );
		pat += offs;
		mask += offs;
		byte len = strlen(mask);

		// Initialize the jump table for quicksearch
		byte jumps[256];
		for ( size_t i = 0; i<256; ++i ) jumps[i] = len;
		len -= 1;
		for ( size_t i = 0; i<len; ++i ) jumps[ static_cast<byte>( pat[i] ) ] = len-i;

		// Adjust the end pointer
		end = reinterpret_cast<void*>( (size_t)end - cnt );

		// Loop trough the memory
		for ( char* p = (char*)begin; p<end; p += jumps[ static_cast<byte>( *(p+len) ) ] )
		{
			if ( *p!=*pat ) continue;
			// Check the rest
			for ( size_t i = 1; i<cnt; ++i )
			{
				if ( (p[i]&mask[i])!=pat[i] ) goto mismatch;
			}
			// Found a match
			return p - offs;
mismatch:	;
		}

		// No match found
		return nullptr;
	}
*/














void Scanner::Init( const pelite::PeFile& bin, const char* secname )
{
	_bin = &bin;
	_low = bin.ImageBase();
	_high = make_ptr( _low, bin.VirtualSize() );
	_begin = _low;
	_end = _high;
	if ( secname )
	{
		// Shorten scan times by only scanning where needed.
		using namespace pelite;
		if ( ImageSectionHeader* sec = bin.FindSection( secname ) )
		{
			_begin = bin.RvaToPtr( sec->VirtualAddress );
			_end = make_ptr( _begin, sec->SizeOfRawData );
		}
	}
}
void Scanner::Init( void* begin, void* end, void* low, void* high )
{
	_bin = nullptr;
	_begin = begin;
	_low = low?low:begin;
	_end = end;
	_high = high?high:end;
}








NOINLINE void* Scanner::Scan( const char* pattern )
{
	// Setup the state
	Setup( pattern );
	// Find a match
	void* ptr;
	if ( ptr = Next() )
	{
		// Only allow 1 match
		if ( Next() )
			ptr = nullptr;
		// Rematch to update the state
		else
			Match( (unsigned char*)ptr );
	}
	return ptr;
}
NOINLINE void Scanner::Setup( const char* pattern )
{
	state.pattern = pattern;
	state.ptr = make_ptr<void*>( _begin, -1 );
	state.hits = 0;
	// One time, one day, you'll love me for this.
	Validate( pattern );
	// Optimize the query with binary scans
	state.qslen = 0;
	for ( const char* it = state.pattern; state.qslen<QS_BUF_LEN; ++it )
	{
		char c = *it;
		byte b;

		if ( ( b = parsehex(c) ) || c=='0' )
		{
			// Get the 2nd (low) nibble
			byte lo = parsehex_unsafe( *++it );
			// Compose and store
			state.qsbuf[state.qslen++] = (b<<4)|lo;
		}
		else if ( c!=' ' && c!='*' )
		{
			// Cannot quicksearch beyond these
			break;
		}
	}
}
void* Scanner::Next()
{
	// Strategy:
	//  Cannot optimize the search, just brute-force it.
	if ( state.qslen==0 )
	{
		for ( byte* ptr = ((byte*)state.ptr)+1; ptr<_end; ++ptr )
		{
			if ( Match( ptr ) )
			{
				return ptr;
			}
		}
	}
	// Strategy:
	//  Raw pattern is too small for full blown quicksearch.
	//  Can still do a small optimization though.
	else if ( state.qslen<=2 )
	{
		byte c = state.qsbuf[0];
		for ( byte* ptr = ((byte*)state.ptr)+1; ptr<_end; ++ptr )
		{
			if ( ptr[0]==c && Match( ptr ) )
			{
				return ptr;
			}
		}
	}
	// Strategy:
	//  Full blown quicksearch for raw pattern.
	//  Most likely completely uneccessary but oh well...
	else
	{
		byte len = state.qslen;

		// Initialize the jump table for quicksearch
		byte jumps[256];
		__stosb( jumps, len, 256 );
		len -= 1;
		for ( unsigned i = 0; i<len; ++i ) jumps[ state.qsbuf[i] ] = len-i;

		// Adjust the end pointer
		byte* end = (byte*)_end - len;

		// Loop trough the memory
		for ( byte* p = ((byte*)state.ptr)+1; p<end; p += jumps[ *(p+len) ] )
		{
			if ( *p!=state.qsbuf[0] ) continue;
			// Check the rest
			for ( unsigned i = 1; i<len; ++i )
			{
				if ( p[i]!=state.qsbuf[i] )
					goto mismatch;
			}
			// Found a match, check full pattern
			if ( Match( p ) )
			{
				return p;
			}
mismatch:;
		}
	}
	// Not found
	return nullptr;
}
bool Scanner::Match( unsigned char* start )
{
	++state.hits;
	unsigned char* pos = start;
	// Assumes everything is fine.
	byte* stack[MAX_DEPTH];
	unsigned stacki = 0;
	bool recurse = false;
	unsigned sti = 0;
	for ( const char* it = state.pattern; *it; ++it )
	{
		char c = *it;
		switch ( c )
		{
		// Save this position
		case '*':
			state.store[sti++] = pos;
			break;
		// Ignore this value
		case '?':
			++pos;
			break;
		// Recurse on next jump
		case '+':
			recurse = true;
			break;
		// Return from recursion
		case '-':
			pos = stack[--stacki];
			break;
		// Absolute jump
		case 'j': case 'J':
			if ( recurse )
			{
				stack[stacki++] = pos+sizeof(void*);
				recurse = false;
			}
			pos = deref<byte*>( pos );
			// Let the pefile correct the scan pointer
			if ( _bin )
			{
				pos = _bin->VaToPtr<byte*>( pos );
			}
			break;
		// Relative jump
		case 'r': case 'R':
			// Read offset size
			c = *++it - '0';
			// Store in case of recursion
			if ( recurse )
			{
				stack[stacki++] = pos+c;
				recurse = false;
			}
			// Read offset and sign extend it
			int offset;
			if		( c==1 ) offset = deref<char>( pos );
			else if ( c==2 ) offset = deref<short>( pos );
			else if ( c==4 ) offset = deref<int>( pos );
			// FIXME! qword pointers on 64bit systems!
			pos = pos + offset + c;
			// FIXME! Let the pefile correct the scan pointer?
			break;
		// Read a hex digit
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9': case 'A': case 'B':
		case 'C': case 'D': case 'E': case 'F':
		case 'a': case 'b': case 'c': case 'd':
		case 'e': case 'f':
			{
				// Doesn't care about errors
				byte c = (parsehex_unsafe(it[0])<<4)|parsehex_unsafe(it[1]);
				if ( *pos!=c )
					return false;
				++it, ++pos;
			}
			break;
		// Raw comparison
		case '\'': case '\"':
			for ( char e = *it++; *it!=e; ++it, ++pos )
			{
				byte b = static_cast<byte>( *it );
				// Overkill...
				//if ( b=='\\' )
				//{
				//	char d = *++it;
				//	if ( d=='\\' ) b = '\\';
				//	else if ( d=='0' ) b = '\0';
				//	else if ( d=='n' ) b = '\n';
				//	else if ( d=='t' ) b = '\t';
				//	else if ( d=='\'' ) b = '\''; 
				//	else if ( d=='\"' ) b = '\"';
				//}
				if ( *pos!=b )
					return false;
			}
			break;
		// Ignore any other characters
		default:
			break;
		}
		// Make sure the scan pointer is still within the bounds
		// FIXME! This check disallows matching at the edge of the scan range!
		if ( pos<_low || pos>=_high )
			return false;
	}
	
	state.ptr = start;
	return true;
}







void Scanner_Format( char* buf, unsigned len, const char* fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	::vsnprintf( buf, len, fmt, va );
	va_end( va );
}
void Scanner::Validate( const char* pattern )
{
	enum
	{
		SUCCESS = 0,
		UNEXPECTED_EOS,			// Expecting more input
		INVALID_RECURSION,		// Found a '+' not immediately followed by a jump
		INVALID_SIZEOF,			// Sizeof for relative jump is not either '1', '2' or '4'
		UNPAIRED_HEXDIGIT,		// A hex digit was not followed by another one
		UNKNOWN_CHARACTER,		// Only accepts spaces to make the pattern more friendly to read. All other characters are reserved for future use
		STACK_ERROR,			// Too much or invalid use of '+' and '-'
		UNKNOWN_ESCAPE,			// Only accepts escape sequences \\ \0 \n \t \' and \"
		STORE_OVERFLOW,			// Too many stores
	};
	int reason;
	const char* it;

#define fail(r) do { reason = r; goto fail; } while ( false )
	{
	int rec = 0;
	int store = 0;
	// This validator has some very strict rules
	for ( it = pattern; *it; ++it )
	{
		switch ( *it )
		{
		// Recursive sign must be followed by a jump
		case '+':
			{
				char n = *(it+1);
				rec++;
				if ( rec>MAX_DEPTH ) fail( STACK_ERROR );
				if ( !n ) fail( UNEXPECTED_EOS );
				if ( n=='j' || n=='J' || n=='r' || n=='R' ) break;
				fail( INVALID_RECURSION );
			}
			break;
		// Relative jump must be followed by a 1, 2, 4 or 8 (in a 64 bit system)
		case 'r': case 'R':
			{
				char n = *++it;
				if ( !n ) fail( UNEXPECTED_EOS );
				if ( n=='1' || n=='2' || n=='4' ) break;
				fail( INVALID_SIZEOF );
			}
			break;
		// Hex digits must be paired
		case '0': case '1': case '2': case '3':
		case '4': case '5': case '6': case '7':
		case '8': case '9': case 'A': case 'B':
		case 'C': case 'D': case 'E': case 'F':
		case 'a': case 'b': case 'c': case 'd':
		case 'e': case 'f':
			{
				char n = *++it;
				if ( !n ) fail( UNEXPECTED_EOS );
				if ( !( parsehex(n) || n=='0' ) ) fail( UNPAIRED_HEXDIGIT );
			}
			break;
		// Check recursion
		case '-':
			if ( --rec<0 ) fail( STACK_ERROR );
			break;
		// Check raw comparison
		case '\'': case '\"':
			{
				char e = *it;
				if ( !(*++it ) ) fail( UNEXPECTED_EOS );
				for ( ; *it!=e; it++ )
				{
					char c = *it;
					if ( !c ) fail( UNEXPECTED_EOS );
					//if ( c=='\\' )
					//{
					//	if ( !(*++it ) ) fail( UNEXPECTED_EOS );
					//	char d = *it;
					//	if ( !( d=='\\' || d=='0' || d=='n' || d=='t' || d=='\'' || d=='\"' ) ) fail( UNKNOWN_ESCAPE );
					//}
				}
			}
			break;
		// Only allow 8 of these
		case '*':
			if ( ++store>MAX_STORE ) fail( STORE_OVERFLOW );
			break;
		// Other allowed characters:
		case ' ': case '?': case 'j': case 'J':
			break;
		// The rest is not allowed
		default:
			fail( UNKNOWN_CHARACTER );
		}
	}
	if ( rec ) fail( STACK_ERROR );
	}
#undef fail

	// SUCCESS!
	return;

	// FAIL!:(
fail:
	{
	//const char* str;
	//switch ( reason )
	//{
	//default:					{ STRCRYPT( str_err, "Success" ); str = str_err; break; }
	//case UNEXPECTED_EOS:		{ STRCRYPT( str_err, "Expected more input" ); str = str_err; break; }
	//case INVALID_RECURSION:		{ STRCRYPT( str_err, "Invalid recursion" ); str = str_err; break; }
	//case INVALID_SIZEOF:		{ STRCRYPT( str_err, "Invalid sizeof" ); str = str_err; break; }
	//case UNPAIRED_HEXDIGIT:		{ STRCRYPT( str_err, "Unpaired hexdigit" ); str = str_err; break; }
	//case UNKNOWN_CHARACTER:		{ STRCRYPT( str_err, "Unknown character" ); str = str_err; break; }
	//case STACK_ERROR:			{ STRCRYPT( str_err, "Stack error" ); str = str_err; break; }
	//case UNKNOWN_ESCAPE:		{ STRCRYPT( str_err, "Unknown escape" ); str = str_err; break; }
	//case STORE_OVERFLOW:		{ STRCRYPT( str_err, "Store overflow" ); str = str_err; break; }
	//}
		char buf[512];
		Scanner_Format( buf, sizeof(buf), STRDECRYPT("Malformed (%d) @%d in <%s>"), reason, it-pattern, pattern );
		throw std::exception( buf );
	}
}

}
