#ifndef HGUARD_LIBEX_META_DECLPFN
#define HGUARD_LIBEX_META_DECLPFN
#pragma once

//----------------------------------------------------------------
// Meta: Pointer wrapper
//----------------------------------------------------------------
//
// Little thing that comes in handy for creating hidden import tables
// to avoid the annoying typedefs & pointer defs
//

namespace meta
{

template< typename Fn >
struct declpfn
{
	typedef typename Fn type;
	void* pfn;
	declpfn() { }
	declpfn( void* ptr ) : pfn(ptr) { }
	declpfn( const declpfn<Fn>& other ) : pfn(other.pfn) { }
	//inline void operator= ( void* ptr ) { pfn = (Fn) ptr; }
	inline operator Fn () const { return (Fn) pfn; }
};

}

#endif // !HGUARD_LIBEX_META_DECLPFN
