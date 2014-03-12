#ifndef HGUARD_LIBEX_TOOLS_VA_BUF
#define HGUARD_LIBEX_TOOLS_VA_BUF
#pragma once

#include <cstdarg>
#include <cassert>
#include <cstdio>
//#include <cinttypes> // Not available for older versions...

#pragma warning(push)
#pragma warning(disable:4996)

namespace tools
{

// WARNING!
//
// Do not pass a va_buf instance by value to a function as a variable arg!
// Instead, make sure to take the address before passing it.
// It will work as expected (and auto-cast to char/wchar_t* as required) everywhere else.
//
// Example, will fail:  printf( "%s\n", va_printf<64>( "Hello %s!", "World" );
// Do this instead:     printf( "%s\n", &va_printf<64>( "Hello %s!", "World" );

//----------------------------------------------------------------
// Helper to select right version of vsn(w)printf and _vsc(w)printf

template< typename T > int vsnprintf( T* buf, size_t count, const T* fmt, va_list va );
template< typename T > int vscprintf( const T* fmt, va_list va );

template<> inline int vsnprintf<char>( char* buf, size_t count, const char* fmt, va_list va ) {
	assert( count<0x80000000 );
	// Standard says to return the number of characters written as if the buffer is always large enough.
	// Windows/MSVC of course, return -1 if output is truncated...
	int n = ::vsnprintf( buf, count, fmt, va );
	// This isn't an ideal solution as we're swallowing legitimate -1 returns and still not returning the right number...
	// But in practice this should do the job :)
	if ( /*n<0 ||*/ static_cast<unsigned int>(n)>=count )
	{
		buf[count-1] = '\0';
		n = static_cast<int>(count);
	}
	return n;
}
#ifdef _MSC_VER
template<> inline int vsnprintf<wchar_t>( wchar_t* buf, size_t count, const wchar_t* fmt, va_list va ) {
	assert( count<0x80000000 );
	int n = ::_vsnwprintf( buf, count, fmt, va );
	if ( /*n<0 ||*/ static_cast<unsigned int>(n)>=count )
	{
		buf[count-1] = '\0';
		n = static_cast<int>(count);
	}
	return n;
}
template<> inline int vscprintf<char>( const char* fmt, va_list va ) {
	return ::_vscprintf( fmt, va );
}
template<> inline int vscprintf<wchar_t>( const wchar_t* fmt, va_list va ) {
	return ::_vscwprintf( fmt, va );
}
#endif // _MSC_VER

//----------------------------------------------------------------
// Class va_buf
//
// Create temporary buffers with formatted data.
//
// Some issues:
// * Generates code for pretty much every unique template instance, just accept it.
// * Gets passed by value to a function taking variable arguments (pass the address instead!).
// * Fails with auto-deduced string argument accepting chars or wide chars (explicitly specify char type for the template).

template< unsigned int L, typename T >
class va_buf
{
	typedef typename T buffer_t[L];
	typedef typename const T const_buffer_t[L];
	buffer_t buf;
public:
	// Empty constructor, does not initialize the buffer!
	inline va_buf() {
	}
	// Zero constructor, initializes the buffer to a zero length string
	explicit inline va_buf( int ) {
		buf[0] = 0;
	}
	// Formatting constructor
	explicit inline va_buf( const T* fmt, ... ) {
		va_list va;
		va_start( va, fmt );
		vprintf( fmt, va );
	}

	// Formatting function
	inline int printf( const T* fmt, ... ) {
		va_list va;
		va_start( va, fmt );
		return vprintf( fmt, va );
	}
	inline int vprintf( const T* fmt, va_list va ) {
		return vsnprintf<T>( buf, L, fmt, va );
	}

	// Formats various basic types (integers/floats/pointers)
	inline int format( int i )                 { return printf( "%d", i ); }
	inline int format( unsigned int ui )       { return printf( "%u", ui ); }
	inline int format( long l )                { return printf( "%ld", l ); }
	inline int format( unsigned long ul )      { return printf( "%lu", ul ); }
	inline int format( long long ll )          { return printf( "%lld", ll ); }
	inline int format( unsigned long long ull ){ return printf( "%llu", ull ); }
	inline int format( float f )               { return printf( "%f", f ); }
	inline int format( double d )              { return printf( "%f", d ); }
	inline int format( const void* ptr )       { return printf( "%#p", ptr ); }
	// Use this to enforce a type
	template< typename N >
	inline int format( N n ) {
		// Unary+ to force promotion,
		// All 'unsupported' types will generate infinite recursion...
		return format( +n );
	}

	// Make this class transparently behave as if it's a char array
	inline operator buffer_t& () { return buf; }
	// Ambiguity Errors! The above should be good enough as long as you don't const instances.
	//inline operator const_buffer_t& () const { return buf; }
	//inline T& operator[] ( unsigned int i ) { assert(i<L); return buf[i]; }
	//inline const T& operator[] ( unsigned int i ) const { assert(i<L); return buf[i]; }
};

// Turn regular buffer in va_buf safely.
template< unsigned int L, typename T >
inline va_buf<L,T>& cast_vabuf( T (&raw)[L] ) {
	return (va_buf<L,T>&)raw;
}

//----------------------------------------------------------------
// Function va_printf() va_format()
//
// Helper with convenient auto type deduction.
// Note to self: do not split in 2 functions for char and wchar_t to help with type deduction,
//  instead explicitly specify the char type in the template argument!
//

template< unsigned int L, typename T >
inline va_buf<L,T> va_printf( const T* fmt, ... ) {
	va_buf<L,T> buf;
	va_list va;
	va_start( va, fmt );
	buf.vprintf( fmt, va );
	return buf;
}
template< unsigned int L, typename T, typename N >
inline va_buf<L,T> va_format( N n ) {
	va_buf<L,T> buf;
	buf.format<N>( n );
	return buf;
}

}

#pragma warning(pop)

#endif // !HGUARD_LIBEX_TOOLS_VA_BUF
