#ifndef HGUARD_LIBEX_TOOLS_VA_BUF
#define HGUARD_LIBEX_TOOLS_VA_BUF
#pragma once

#include <cstdarg>
#include <cassert>
#include <cstdio>

#pragma warning(push)
#pragma warning(disable:4996)

namespace tools
{

// WARNING!
//
// Do not pass this class directly to a function as a variable arg!
// Instead, make sure to take the address before passing it.
// It will work as expected (and auto-cast to char* as required) everywhere else.
//
// Example, will fail:
// printf( "%s\n", va_printf<64>( "Hello %s!", "World" );
// Do this instead:
// printf( "%s\n", &va_printf<64>( "Hello %s!", "World" );
//

// Helper to select right version
template< typename T > int vsnxprintf( T* buf, size_t count, const T* fmt, va_list va );

template<> inline int vsnxprintf<char>( char* buf, size_t count, const char* fmt, va_list va )
{
	return ::vsnprintf( buf, count, fmt, va );
}
#ifdef _MSC_VER
template<> inline int vsnxprintf<wchar_t>( wchar_t* buf, size_t count, const wchar_t* fmt, va_list va )
{
	return ::_vsnwprintf( buf, count, fmt, va );
}
#endif // _MSC_VER

//------------------------------------------------
// Class va_buf
//------------------------------------------------
// Create temporary buffers with formatted data
template< unsigned long L, typename T >
class va_buf
{
	typedef typename T buffer_t[L];
	typedef typename const T const_buffer_t[L];
	buffer_t buf;
public:
	// Empty constructor
	inline va_buf()
	{
	}
	// Formatting constructor
	inline va_buf( const T* fmt, ... )
	{
		va_list va;
		va_start( va, fmt );
		printex( fmt, va );
	}
	// Formatting function
	inline int print( const T* fmt, ... )
	{
		va_list va;
		va_start( va, fmt );
		return printex( fmt, va );
	}

	int printex( const T* fmt, va_list va )
	{
		int n = vsnxprintf<T>( buf, L-1, fmt, va );
		buf[n] = 0;
		return n;
	}

	// Make this class transparently behave as if it's a character array
	inline operator buffer_t& () { return buf; }
	inline operator const_buffer_t& () const { return buf; }
	inline T& operator[] ( unsigned int i ) { assert(i<L); return buf[i]; }
	inline const T& operator[] ( unsigned int i ) const { assert(i<L); return buf[i]; }
};

//------------------------------------------------
// Function va_printf()
//------------------------------------------------
// Helper with convenient auto type deduction.
template< unsigned long L, typename T >
inline va_buf<L,T> va_printf( const T* fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	va_buf<L,T> buf;
	buf.printex( fmt, va );
	return buf;
}

}

#pragma warning(pop)

#endif // !HGUARD_LIBEX_TOOLS_VA_BUF
