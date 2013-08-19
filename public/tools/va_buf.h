#ifndef HGUARD_LIBEX_TOOLS_VA_BUF
#define HGUARD_LIBEX_TOOLS_VA_BUF
#pragma once

#include <cstdarg>

namespace tools
{

// Create temporary buffers with formatted data
template< unsigned long L, typename T = char >
class va_buf
{
	typedef typename T buffer_t[L];
	typedef typename const T const_buffer_t[L];
	buffer_t buf;
public:
	// Empty constructor, init buffer to empty string
	inline va_buf()
	{
		((int*)buf)[0] = 0;
	}
	// Formatting constructor
	inline va_buf( const T* fmt, ... )
	{
		va_list va;
		va_start( va, fmt );
		printex( fmt, va );
		va_end( va );	
	}
	// Formatting function
	inline int print( const T* fmt, ... )
	{
		va_list va;
		va_start( va, fmt );
		printex( fmt, va );
		va_end( va );	
	}

	// Need different functions depending on character type, can't I directly specialize based on the provided class template?
	template< typename T > int printex( const T* fmt, va_list va );
	// Char version needs vsnprintf
	template<> inline int printex<char>( const char* fmt, va_list va )
	{
		return ::vsnprintf( buf, sizeof(buf), fmt, va );
	}
#ifdef _MSC_VER
	// Wide version needs _vsnwprintf (Windows only!)
	template<> inline int printex<wchar_t>( const wchar_t* fmt, va_list va )
	{
		return ::_vsnwprintf( buf, sizeof(buf), fmt, va );
	}
#endif // _MSC_VER

	// Make this class behave as if it's a buffer_t
	inline operator buffer_t& () { return buf; }
	inline operator const_buffer_t& () const { return buf; }
	inline T& operator[] ( unsigned int i ) { assert(i>=0 && i<L); return buf[i]; }
	inline const T& operator[] ( unsigned int i ) const { assert(i>=0 && i<L); return buf[i]; }
};

}

#endif // !HGUARD_LIBEX_TOOLS_VA_BUF
