
#include "alloc.h"
#include "../pelite/image.h"

namespace toolkit
{

#ifndef NDEBUG
// Only in Debug builds to make things easy...
// Problems include: Section is not removed by the linker if there are no references and the name and size are fixed.
//#pragma section( ".heap", execute, read, write )
#pragma comment( linker, "/SECTION:.heap,ERW" )
#pragma bss_seg( push, ".heap" )
unsigned char CRweAllocHelper_data[1024*1024*3/2]; // 1.5 MB
#pragma bss_seg( pop )
#endif // !NDEBUG

	
// Read/Write/Execute allocator
// First-fit algorithm with free list by memory address
class CRweAllocHelper
{
public:
	bool Init( unsigned char* self );
	unsigned char* Alloc( unsigned size );
	void Free( void* ptr );
	bool Validate();

protected:
	// Internal list of allocations
	struct link_t
	{
		link_t* next;
		link_t* prev;
		bool taken; // Fixme! use an offset instead & merge with next so it only uses 8 bytes...
		enum { padding = 0xC1C2C3C9 }; // Alignment padding
		enum { minsize = 32 }; // Minimum size of a link_t node + it's data, use padding if needed
	private: inline link_t& lmerge()
		{
			assert( !taken );
			prev->next = next;
			if ( next ) next->prev = prev;
			return *this;
		}
	public: inline link_t& try_lmerge()
		{
			assert( !taken );
			if ( prev && !prev->taken )
				lmerge();
			return *this;
		}
	public: inline link_t& try_rmerge()
		{
			assert( !taken );
			if ( next && !next->taken )
				next->lmerge();
			return *this;
		}
	public: inline link_t& moved( link_t* old )
		{
			// NOTE! Might overlap to the right side! (will never move to the left)
			this->taken = old->taken;
			_WriteBarrier();
			this->prev = old->prev;
			_WriteBarrier();
			this->next = old->next;
			if ( prev ) prev->next = this;
			if ( next ) next->prev = this;
			return *this;
		}
	public: inline link_t& ins_after( link_t* lnk )
		{
			lnk->prev = this;
			lnk->next = next;
			if ( next ) next->prev = lnk;
			next = lnk;
			return *this;
		}
	};
	unsigned char* Claim( link_t* l, unsigned size, unsigned align );

	template< typename T >
	inline T Align( void* ptr, unsigned align )
	{
		return (T) ( ( ( (uintptr_t)ptr - 1 ) / align + 1 ) * align );
	}

private:
	// Chunk of memory for us to use
	unsigned char* base;
	unsigned char* end;

public:
	static CRweAllocHelper g;
};
CRweAllocHelper CRweAllocHelper::g;

bool CRweAllocHelper::Init( unsigned char* hmod )
{
	// Find the latest rwe section
	// Skip the first section because it may be .textbss which is owned by the compiler.
	using namespace pelite;
	ImageNtHeader* nt = (ImageNtHeader*)( hmod + ((ImageDosHeader*)hmod)->e_lfanew );
	for ( auto it = 1+(ImageSectionHeader*)( (char*)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader ),
		end = it+nt->FileHeader.NumberOfSections-1; it<end; ++it )
	{
		// Check for the latest RWE section
		if ( (it->Characteristics&0xF0000000)==0xE0000000 )
		{
			this->base = hmod + it->VirtualAddress;
			this->end = base + it->VirtualSize;
			// First link node is already zero-initialized!
		}
	}
	//base = CRweAllocHelper_data;
	//end = base + sizeof CRweAllocHelper_data;
	// FIXME! Fall back to OS allocated memory.
	//if ( !this->base )
	//{
	//}
	return this->base!=nullptr;
}
NOINLINE unsigned char* CRweAllocHelper::Alloc( unsigned size )
{
	// Initialize ourself
	if ( base || Init( (unsigned char*)&__ImageBase ) )
	{
		// Fetch alignment from arg
		unsigned align = (size>>28);
		if ( !align ) align = 8;
		else align = 1<<align;
		// Fixup size arg (alignment here keeps our link_t structs aligned)
		size = (( (size&0x0FFFFFFF) - 1 )&~3) + 4; 

		// Find a free block
		for ( link_t* it = (link_t*)base; it; it = it->next )
		{
			link_t* lnk = it;
			unsigned link_size;
			if ( !lnk->taken && ( link_size = ( (lnk->next?(char*)lnk->next:(char*)end) - (char*)lnk ) - sizeof(link_t) )>=size )
			{
				return Claim( it, size, align );
			}
		}
	}
	return nullptr;
}
unsigned char* CRweAllocHelper::Claim( link_t* lnk, unsigned size, unsigned align )
{
	unsigned char* abase = Align<unsigned char*>( lnk+1, align );

	// Utilize the space between the link and allocation
	if ( unsigned delta = abase - (unsigned char*)(lnk+1) )
	{
		if ( lnk->prev && !lnk->prev->taken )
		{
			// Just move the link next to the allocation
			// NOTE! to and lnk might overlap (moves only ever to the right)!
			link_t* to = (link_t*)abase - 1;
			to->moved( lnk );
			lnk = to;
		}
		else if ( delta<link_t::minsize )
		{
			// Not enough room for a new link, use padding instead
			__stosd( (unsigned long*)( lnk+1 ), link_t::padding, (unsigned long*)abase - (unsigned long*)( lnk+1 ) );
		}
		else
		{
			// More than enough room to instert a new link
			// Note: cannot merge with left link because we already tested for this!
			link_t* p = (link_t*)abase - 1;
			lnk->ins_after( p );
			lnk = p;
		}
	}
	
	// Split if remainder is large enough
	unsigned rem = (lnk->next?(char*)lnk->next:(char*)end) - (char*)lnk;
	if ( rem>=link_t::minsize )
	{
		link_t* split = (link_t*)( abase + size );
		lnk->ins_after( split );
		split->taken = false;
	}

	// Claim
	lnk->taken = true;
	
	// Integrity check
	assert( Validate() );
	return abase;
}
NOINLINE void CRweAllocHelper::Free( void* ptr )
{
	if ( unsigned* p = (unsigned*)ptr )
	{
		// Skip padding bytes
		do --p; while ( *p==link_t::padding );
		// Find the node this belongs to
		link_t* lnk = (link_t*)(p+1) - 1;
		lnk->taken = false;

		// Check if we can merge with our neighbours
		// NOTE! Must test right merge first (or lnk would become invalid!)
		lnk->try_rmerge();
		lnk->try_lmerge();

		// Integrity check
		assert( Validate() );
	}
}
bool CRweAllocHelper::Validate()
{
	link_t* lnk = (link_t*)base;
	link_t* prev = nullptr;
	for ( ; lnk; prev = lnk, lnk = lnk->next )
	{
		// Asserts:
		// Links must be in memory order
		// Backlinks must be correct
		if ( lnk<=prev || lnk->prev!=prev )
			return false;
	}
	return true;
}


bool rweInit( void* self )
{
	return CRweAllocHelper::g.Init( (unsigned char*)self );
}
unsigned char* rweAlloc( unsigned size )
{
	return CRweAllocHelper::g.Alloc( size );
}
void rweFree( void* ptr )
{
	return CRweAllocHelper::g.Free( ptr );
}
bool rweValidate()
{
	return CRweAllocHelper::g.Validate();
}

}
