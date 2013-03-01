#ifndef HGUARD_LIBEX_PELITE_BASERELOCS
#define HGUARD_LIBEX_PELITE_BASERELOCS
#pragma once

//----------------------------------------------------------------
// PeLite: Base relocations
//----------------------------------------------------------------

#include "pefile.h"
#include "image.h"

namespace pelite
{
//
// Iterators
// Non-standard iterator interface because of abstractions...
// Very limited in functionality, can only iterate once through all the relocations.
// Use like this: for ( BaseRelocs::iterator it( bin ); it; ++it )
//
	
class BaseRelocIterator
{
public:
	BaseRelocIterator( const PeFile& bin );
	void Next();

	BaseRelocIterator& operator++();
	operator bool () const;

	unsigned Address() const;
	unsigned Type() const;

private:
	ImageDataDirectory& relocs;
	ImageBaseRelocation* outer_it;
	ImageBaseRelocation* outer_end;
	ImageBaseRelocBlock* inner_it;
	ImageBaseRelocBlock* inner_end;
};



class BaseRelocs
{
public:
	BaseRelocs( const PeFile& pe );
	const PeFile& GetPeFile() const;

	static bool Exists( const PeFile& pe );

	// Gets the preferred load address this binary was linked with
	unsigned PreferredLoadAddress() const;

	// Iterate over the ImageBaseRelocation structures
	// Non-standard, see comments above
	typedef BaseRelocIterator iterator;

	// Raw access if you need to mess with it yourself
	ImageDataDirectory& DataDir() const;
	ImageBaseRelocation* Raw() const;
	unsigned RawSize() const;

private:
	const PeFile& mPeFile;
	//ImageBaseRelocation* mRelocs; // Should I cache this?
};





// Inlines

inline const PeFile& BaseRelocs::GetPeFile() const { return mPeFile; }

inline BaseRelocIterator& BaseRelocIterator::operator++() { Next(); return *this; }
inline BaseRelocIterator::operator bool () const { return outer_it<outer_end; }
inline unsigned BaseRelocIterator::Address() const { return outer_it->VirtualAddress + inner_it->Offset; }
inline unsigned BaseRelocIterator::Type() const { return inner_it->Type; }

}

#endif // !HGUARD_LIBEX_PELITE_BASERELOCS
