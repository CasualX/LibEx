#ifndef HGUARD_LIBEX_TOOLS_YALLOC
#define HGUARD_LIBEX_TOOLS_YALLOC
#pragma once

//
// Yalloc
//
// A super simple allocator useful for parsing into read-only structures.
// See vgui/keyvalues.h
// See vgui/localize.h
//
// Todo! Dynamically increase blocksize on each alloc.
//

namespace tools
{

template< size_t BLOCKSIZE >
class Yalloc
{
	struct block_t
	{
		block_t* old;
		char* next;
		char data[BLOCKSIZE];
	};
public:
	Yalloc() : block(NULL) { }

	void* alloc( size_t size )
	{
		// Keep everything 4 byte aligned (FIXME! 16 byte alignment plz...)
		size = ((size-1)&~3)+4;
		// Requested size is too big
		if ( size>BLOCKSIZE/3 )
			return _elarge( size );
		// Check if we have enough room
		if ( !_check( size ) )
			_inc();
		// Alloc memory
		void* p = block->next;
		block->next += size;
		return p;
	}
	template< typename T >
	T* alloc()
	{
		return reinterpret_cast<T*>( alloc( sizeof(T) ) );
	}
	void release()
	{
		// Loop through the linked blocks and free them
		while ( block )
		{
			block_t* p = block->old;
			::free( block );
			block = p;
		}
	}

protected:
	bool _check( size_t size )
	{
		if ( !block )
			return false;
		// Do we have enough room for the requested size in the current block
		size_t rem = ( block->data + sizeof(block->data) ) - block->next;
		return rem > size;
	}
	void* _inc()
	{
		// Alloc a new block
		block_t* p = (block_t*) ::malloc( sizeof(block_t) );
		// Set the backpointer to the previous block
		p->old = block;
		block = p;
		// Return and set the next free memory
		p->next = p->data;
		return p;
	}
	void* _elarge( size_t size )
	{
		// FIXME! Untested code!
		// When a size too large is requested, it gets its own dedictated block.
		block_t* p = (block_t*) ::malloc( size+sizeof(void*)*2 );
		p->next = 0;
		// Insert before the current block
		if ( block )
		{
			p->old = block->old;
			block->old = p;
		}
		else
		{
			p->old = block;
			block = p;
		}
		// Return begin of block
		return p->data;
	}

private:
	block_t* block;
};

}

#endif // !HGUARD_LIBEX_TOOLS_YALLOC
