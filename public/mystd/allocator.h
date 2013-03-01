#ifndef HGUARD_LIBEX_MYSTD_ALLOCATOR
#define HGUARD_LIBEX_MYSTD_ALLOCATOR
#pragma once

//----------------------------------------------------------------
// MyStd: Allocator
//----------------------------------------------------------------

#include "../libex.h"


#define MYSTD_CONSTRUCT( TYPE, VAR, ... ) new (VAR) TYPE( __VA_ARGS__ )
#define MYSTD_DESTRUCT( TYPE, OBJ ) OBJ->~TYPE()

namespace mystd
{


// Wrapper for destructing objects
template< typename T >
inline void destruct( T* obj )
{
	obj->~T();
}

// Wrapper for constructing objects
template< typename T >
inline T* construct( void* data )
{
	return new (data) T();
}
template< typename T >
inline T* construct( void* data, T&& ref )
{
	// Sigh... Just use MYSTD_CONSTRUCT instead
	return new (data) T( ((T&&)ref) );
}



INTERFACE allocator
{
public:
	virtual ~allocator() { }
	virtual void* alloc( unsigned long size ) = 0;
	virtual void* realloc( void* data, unsigned long size ) = 0;
	virtual void free( void* data ) = 0;
	virtual void stats() = 0;

	// Construct a new object
	template< typename T >
	T* make()
	{
		void* obj = alloc( sizeof(T) );
		if ( obj )
		{
			// Make sure to clean up when an exception is throw during construction
			try { construct<T>( obj ); }
			catch (...) { free( obj ); throw; }
		}
		return obj;
	}
	template< typename T >
	inline void kill( T* obj )
	{
		if ( obj )
		{
			destruct<T>( obj );
			free( obj );
		}
	}
};

}

#endif // !HGUARD_LIBEX_MYSTD_ALLOCATOR
