#ifndef HGUARD_LIBEX_TOOLS_ENUMLOGIC
#define HGUARD_LIBEX_TOOLS_ENUMLOGIC
#pragma once

//----------------------------------------------------------------
// Tools: Enum logic
//----------------------------------------------------------------
// Defines the operators to work on enums as integers.
// Allows (partial) typesafe enum operations.

#define ENUMLOGIC( E ) \
	static_assert( sizeof(int)>=sizeof(E), "ENUMLOGIC(" #E ") Does not fit in an int!" ); \
	inline E  operator|  ( E  a, E b )   { return E( int(a) | int(b) ); } \
	inline E  operator&  ( E  a, E b )   { return E( int(a) & int(b) ); } \
	inline E  operator^  ( E  a, E b )   { return E( int(a) ^ int(b) ); } \
	inline E  operator<< ( E  a, int b ) { return E( int(a) << b ); } \
	inline E  operator>> ( E  a, int b ) { return E( int(a) >> b ); } \
	inline E& operator|= ( E& a, E b )   { return a = a | b; } \
	inline E& operator&= ( E& a, E b )   { return a = a & b; } \
	inline E& operator^= ( E& a, E b )   { return a = a ^ b; } \
	inline E& operator<<=( E& a, int b ) { return a = a << b; } \
	inline E& operator>>=( E& a, int b ) { return a = a >> b; } \
	inline E  operator~  ( E a )         { return E( ~int(a) ); }

#define ENUMITERATE( E ) \
	inline E& operator++( E& a      ) { return a = E( int(a) + 1 ); } \
	inline E& operator--( E& a      ) { return a = E( int(a) - 1 ); } \
	inline E  operator++( E& a, int ) { E t = a; ++a; return t; } \
	inline E  operator--( E& a, int ) { E t = a; --a; return t; }

#endif // !HGUARD_LIBEX_TOOLS_ENUMLOGIC
