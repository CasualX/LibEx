#ifndef HGUARD_LIBEX_PELITE_PEFILE
#define HGUARD_LIBEX_PELITE_PEFILE
#pragma once

//----------------------------------------------------------------
// PeLite: PE file
//----------------------------------------------------------------
// Provides toplevel abstraction for pe files.

#include "image.h"


// Avoid tedious type work
#define PELITE_DECLARE_RAW( TYPE, EXPR ) \
	inline TYPE* Raw() const { return EXPR; } \
	inline TYPE* operator->() const { return EXPR; } \
	inline TYPE& operator*() const { return *EXPR; }

namespace pelite
{
	
// Invalid RVAs
enum { BADRVA = -1 };
typedef unsigned int rva_t;
	



//------------------------------------------------
// Class PeFile
//------------------------------------------------
// Wraps a mapped image in memory.

class PeFile
{
public:
	// Constructing
	explicit PeFile( void* hModule, void* vbase = nullptr );
	explicit PeFile();
	virtual ~PeFile() { }
	
	bool Init( void* hmod, void* vbase = nullptr );

	// Is this a mapped image
	virtual bool IsMapped() const;

	// Accessors
	void*		RawBase() const;
	unsigned	RawSize() const;
	void*		VirtualBase() const;
	unsigned	VirtualSize() const;

	// Quick access to the pe headers
	ImageDosHeader*		DosHeader() const;
	ImageNtHeader*		NtHeader() const;

	// Access section headers
	ImageSectionHeader*	SectionTable() const;
	ImageSectionHeader*	SectionTableEnd() const;
	ImageSectionHeader*	FindSection( const char* name ) const;
	ImageSectionHeader*	FindSectionFor( unsigned rva ) const;
	
	// Rva/Va to Ptr conversions
	virtual void* RvaToPtr( unsigned rva ) const;
	template< typename T > T RvaToPtr( unsigned rva ) const;
	template< typename T > T VaToPtr( const void* va ) const;

	// Ptr to Rva/Va conversions
	virtual unsigned PtrToRva( const void* ptr ) const;
	void* PtrToVa( const void* ptr ) const;
	
	// Convert RVA <-> FileOffset
	unsigned RvaToFileOffset( unsigned rva ) const;
	unsigned FileOffsetToRva( unsigned offset ) const;

	// Convert RVA <-> VA
	void* RvaToVa( unsigned rva ) const;
	unsigned VaToRva( const void* va ) const;

protected:
	ImageDosHeader*		_mzheader;
	ImageNtHeader*		_ntheader;
	ImageSectionHeader*	_sections;
	void*		_vbase;
	unsigned	_size;
};



//------------------------------------------------
// Class PeFileRaw
//------------------------------------------------
// Wraps an unmapped image but doesn't manage the buffer

class PeFileRaw : public PeFile
{
public:
	// Wrap a buffer already loaded in memory
	explicit PeFileRaw( void* base, unsigned size );
	explicit PeFileRaw();
	
	bool Init( void* ptr, unsigned size = 0 );

	// Inherited from PeFile
	virtual bool IsMapped() const;
	virtual void* RvaToPtr( unsigned rva ) const;
	virtual unsigned PtrToRva( const void* ptr ) const;
	using PeFile::RvaToPtr;

protected:
};


//------------------------------------------------
// Class PeFileRawManaged
//------------------------------------------------
// Convenience wrapper that manages the file contents buffer itself

class PeFileRawManaged : public PeFileRaw
{
public:
	// Manage the file ourself
	explicit PeFileRawManaged( const char* filename );
	explicit PeFileRawManaged();
	bool Init( const char* filename );

	// Clean up
	virtual ~PeFileRawManaged();
};






// Inlines
inline PeFile::PeFile() : _mzheader(nullptr) { }
inline void* PeFile::RawBase() const { return _mzheader; }
inline unsigned PeFile::RawSize() const { return _size; }
inline void* PeFile::VirtualBase() const { return _vbase; }
inline size_t PeFile::VirtualSize() const { return _ntheader->OptionalHeader.SizeOfImage; }
inline ImageDosHeader* PeFile::DosHeader() const { return _mzheader; }
inline ImageNtHeader* PeFile::NtHeader() const { return _ntheader; }
inline ImageSectionHeader* PeFile::SectionTable() const { return _sections; }
template< typename T > inline T PeFile::RvaToPtr( unsigned rva ) const { return (T) RvaToPtr( rva ); }
template< typename T > inline T PeFile::VaToPtr( const void* va ) const { return (T) RvaToPtr( VaToRva( va ) ); }
inline void* PeFile::PtrToVa( const void* ptr ) const { return RvaToVa( PtrToRva( ptr ) ); }
inline void* PeFile::RvaToVa( unsigned rva ) const { return reinterpret_cast<void*>( (char*)_vbase + rva ); }
inline unsigned PeFile::VaToRva( const void* va ) const { return static_cast<unsigned>( (char*)va - (char*)_vbase ); }
inline PeFileRaw::PeFileRaw() { }
inline PeFileRawManaged::PeFileRawManaged() { }

}

#endif // !HGUARD_PELITE_PEFILE
