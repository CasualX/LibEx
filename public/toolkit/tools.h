#ifndef HGUARD_LIBEX_TOOLKIT_TOOLS
#define HGUARD_LIBEX_TOOLKIT_TOOLS
#pragma once

//--------------------------------------------------------------------------------
// Toolkit: Tools
//--------------------------------------------------------------------------------
// Very basic memory tools

#include "../libex.h"


// Evil Macro Versions
#define GETMEMBER( TYPE, PTR, OFF ) ( *(TYPE*)( ((uintptr_t)PTR) + OFF ) )
#define DEREF( TYPE, PTR ) ( *(TYPE*)PTR )
#define MAKEPTR( TYPE, PTR, OFF ) ( (TYPE)( ((uintptr_t)PTR) + OFF ) )
#define GETOFFSET( BASE, PTR ) ( ((uintptr_t)(PTR)) - ((uintptr_t)(BASE)) )
#define WRITEMEM( PTR, TYPE, VAL ) ( (*(TYPE*)=VAL), ((TYPE*)PTR)+1 )


namespace toolkit
{
	
// Where do I go?
inline unsigned char parsehex_unsafe( char h )
{
	static const unsigned char lookup[] = {
		0, 1, 2, 3, 4, 5, 6,7,8,9,0,0,0,0,0,0,
		0,10,11,12,13,14,15,0,0,0,0,0,0,0,0,0,
		0, 0, 0, 0, 0, 0, 0,0,0,0,0,0,0,0,0,0,
		0,10,11,12,13,14,15,0
	};
	assert( (static_cast<unsigned char>(h-'0'))<56 );
	return lookup[static_cast<unsigned char>(h-'0')];
}
inline unsigned char parsehex( char h )
{
	unsigned char i = h-'0';
	unsigned char c = 0;
	if ( static_cast<unsigned char>(h-'0')<56 ) c = parsehex_unsafe(h);
	return c;
}
inline bool ishex( char h )
{
	return parsehex( h ) || h=='0';
}


// getmember<T>( ptr, offset )
// Returns a reference as a type of your choice from an offset.

template< typename T >
inline T& getmember( void* ptr, long offset )
{
	return *reinterpret_cast<T*>( (uintptr_t)ptr + offset );
}
template< typename T >
inline const T& getmember( const void* ptr, long offset )
{
	return *reinterpret_cast<const T*>( (uintptr_t)ptr + offset );
}



// deref<T>( ptr )
// Dereferences the pointer with a type of your choice. The same as getmember with an offset of 0.

template< typename T >
inline T& deref( void* ptr )
{
	return *reinterpret_cast<T*>( ptr );
}
template< typename T >
inline const T& deref( const void* ptr )
{
	return *reinterpret_cast<const T*>( ptr );
}



// make_ptr<T>( ptr, offset )
// Returns (ptr+offset) as a type of your choice.

template< typename T >
inline T make_ptr( void* ptr, long offset )
{
	return (T)( (uintptr_t)ptr + offset );
}
template< typename T >
inline const T make_ptr( const void* ptr, long offset )
{
	return (const T)( (uintptr_t)ptr + offset );
}
inline void* make_ptr( void* ptr, long offset )
{
	return (void*)( (uintptr_t)ptr + offset );
}
inline const void* make_ptr( const void* ptr, long offset )
{
	return (const void*)( (uintptr_t)ptr + offset );
}



// getoffset( base, ptr )
// Returns (ptr-base).

inline long getoffset( const void* base, const void* ptr )
{
	return (uintptr_t)ptr - (uintptr_t)base;
}



// getvtable( inst, offset )
// Returns the vtable as an array of void pointers.

inline void**& getvtable( const void* inst, long offset = 0 )
{
	return *reinterpret_cast<void***>( (uintptr_t)inst + offset );
}



// getvfunc<T>( inst, index, offset )
// Returns the vfunc from the instance casted to a (function) type of your choice.

template< typename Fn >
inline Fn getvfunc( const void* inst, long index, long offset = 0 )
{
	return (*reinterpret_cast<Fn**>( (uintptr_t)inst + offset ))[ index ];
}




// write<T>( ptr, data )
// Writes data to the pointer and returns an incremented pointer by sizeof<T>.

template< typename T >
inline T* write( void* ptr, const T& data )
{
	*(T*)ptr = data;
	return reinterpret_cast<T*>( (uintptr_t)ptr + sizeof(T) );
}



// class writer
// Writes data to memory.

class writer
{
public:
	writer() { }
	writer( void* buf ) : base(buf), cur(buf) { }
	template< typename T >
	writer& write( const T& data )
	{
		*reinterpret_cast<T*&>( cur )++ = data;
		return *this;
	}
	template< typename T >
	writer& write( const T* data, unsigned int num )
	{
		while ( num )
		{
			*reinterpret_cast<T*&>( cur )++ = *data++;
			--num;
		}
		return *this;
	}
	inline unsigned int bytes() const
	{
		return (uintptr_t)cur - (uintptr_t)base;
	}
	inline void reset( void* buf )
	{
		base = buf;
		cur = buf;
	}
private:
	void* base;
	void* cur;
};

}

#endif // !HGUARD_LIBEX_TOOLKIT_TOOLS
