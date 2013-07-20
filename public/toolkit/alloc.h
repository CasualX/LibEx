#ifndef HGUARD_LIBEX_TOOLKIT_ALLOC
#define HGUARD_LIBEX_TOOLKIT_ALLOC
#pragma once

//----------------------------------------------------------------
// Toolkit: Memory stuffs
//----------------------------------------------------------------
// Sometimes you need read/write/execute (RWE) memory to allocate code buffers.
// For security reasons it's not desired to allocate such memory directly from the OS (as this is suspicious for anti-cheat reasons).
// This is a very simple allocator that allows it to allocate from an RWE section directly from the binary.
// Since this requires post processing to add such a section of proficient size, it has a fallback to OS memory.

#include "../libex.h"

namespace toolkit
{

// Initialize the rwe allocator
bool rweInit( void* self );

// Allocate some read/write/execute memory
unsigned char* rweAlloc( unsigned size );

// Aligned alloc, alignment is limited to powers of two!
template< unsigned long align >
inline unsigned char* rweAligned( unsigned size )
{
	// This should all optimize smoothly...
	// Alignment is passed as a power of 2 in the most significant 4 bits of the size parameter.
#define X(x) (align<=(1<<x))?x:
	unsigned long a = X(1) X(2) X(3) X(4) X(5) X(6) X(7) X(8) X(9) X(10) X(11) X(12) X(13) X(14) X(15) 0;
	assert( a>0 && a<16 );
#undef X
	return rweAlloc( size | (a<<28) );
}

// Free earlier allocated memory (null allowed)
void rweFree( void* ptr );

// Debugging, test allocator integrity
bool rweValidate();

}

#endif // !HGUARD_LIBEX_TOOLKIT_ALLOC
