#ifndef HGUARD_LIBEX_LIBEX
#define HGUARD_LIBEX_LIBEX

#include <intrin.h>
#include <cassert>

namespace types
{
typedef long long			llong;

// Unsigned types
typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ullong;
typedef unsigned __int8		byte;
typedef unsigned __int16	word;
typedef unsigned __int32	dword;
typedef unsigned __int64	qword;

// Specific size types
typedef __int8				int8;
typedef __int16				int16;
typedef __int32				int32;
typedef __int64				int64;
typedef unsigned __int8		uint8;
typedef unsigned __int16	uint16;
typedef unsigned __int32	uint32;
typedef unsigned __int64	uint64;

typedef __int8				int8_t;
typedef __int16				int16_t;
typedef __int32				int32_t;
typedef __int64				int64_t;
typedef unsigned __int8		uint8_t;
typedef unsigned __int16	uint16_t;
typedef unsigned __int32	uint32_t;
typedef unsigned __int64	uint64_t;

// Pointer types
typedef intptr_t			int_ptr;
typedef uintptr_t			uint_ptr;

// Make signed/unsigned
template< unsigned int N > struct _make_unsigned;
template<> struct _make_unsigned<1> { typedef unsigned char type; };
template<> struct _make_unsigned<2> { typedef unsigned short type; };
template<> struct _make_unsigned<4> { typedef unsigned int type; };
template<> struct _make_unsigned<8> { typedef unsigned __int64 type; };
template< typename T > struct make_unsigned : public _make_unsigned<sizeof(T)> { };
template< unsigned int N > struct _make_signed;
template<> struct _make_signed<1> { typedef char type; };
template<> struct _make_signed<2> { typedef short type; };
template<> struct _make_signed<4> { typedef int type; };
template<> struct _make_signed<8> { typedef __int64 type; };
template< typename T > struct make_signed : public _make_signed<sizeof(T)> { };

#ifdef _UNICODE
typedef wchar_t tchar;
#else
typedef char tchar;
#endif // _UNICODE
}

//------------------------------------------------
// LibEx macros
//------------------------------------------------
// Don't modify these!!! Toolkit depends on it.

#define LIBEXCC __fastcall
#define LIBEXAPI

//------------------------------------------------
// Some general purpose macros
//------------------------------------------------

#ifndef offsetof
# define offsetof(type,member) ((size_t)&reinterpret_cast<const volatile char&>((((type*)0)->member)))
#endif // !offsetof
#ifndef containerof
#define containerof(ptr,type,member) reinterpret_cast<type*>( ((uintptr_t)(ptr)) - offsetof(type,member) )
#endif // !containerof
#define GETOUTER(TYPE,PTR,MEMBER) containerof(PTR,TYPE,MEMBER)

#ifdef NDEBUG

#ifndef ASSERT
# define ASSERT(e,msg) __assume(e)
#endif // !ASSERT
#ifndef NODEFAULT
# define NODEFAULT default: __assume(0)
#endif // !NODEFAULT
#ifndef ASSUME
# define ASSUME(e) __assume(e)
#endif // !ASSUME

#else

#ifndef ASSERT
# define ASSERT(e,msg) (void)( (!!(e)) || (assert( !msg ),true) )
#endif // !ASSERT
#ifndef NODEFAULT
# define NODEFAULT default: assert(!"Default case reached!")
#endif // !NODEFAULT
#ifndef ASSUME
# define ASSUME(e) assert(e)
#endif // !ASSUME

#endif
#endif // !HGUARD_LIBEX_LIBEX


#ifndef FORCEINLINE
# define FORCEINLINE __forceinline
#endif // !FORCEINLINE
#ifndef INLINE
# define INLINE __forceinline
#endif // !INLINE
#ifndef NOINLINE
# define NOINLINE __declspec(noinline)
#endif // !NOINLINE
#ifdef INTERFACE
# undef INTERFACE
#endif // INTERFACE
# define INTERFACE class __declspec(novtable)
#ifndef NORETURN
# define NORETURN __declspec(noreturn)
#endif // !NORETURN
#ifndef NOTHROW
# define NOTHROW throw()
#endif // !NOTHROW
