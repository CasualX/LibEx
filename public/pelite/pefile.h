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
	PeFile();
	PeFile( void* hmod, void* vbase = nullptr );

	static bool IsValid( void* hmod );
	void Init( void* hmod, void* vbase = nullptr );

	// Accessors
	void*		ImageBase() const;
	void*		VirtualBase() const;
	rva_t		VirtualSize() const;

	// Access to the pe headers
	ImageDosHeader*		DosHeader() const;
	ImageNtHeaders*		NtHeaders() const;
	ImageFileHeader*	FileHeader() const;
	ImageOptionalHeader* OptionalHeader() const;

	// Access section headers
	ImageSectionHeader*	Sections() const;
	ImageSectionHeader*	FindSection( const char* name ) const;
	ImageSectionHeader*	FindSectionFor( rva_t rva ) const;
	
	// Rva/Va to Ptr conversions
	void* RvaToPtr( rva_t rva ) const;
	template< typename T > inline T RvaToPtr( rva_t rva ) const { return (T) RvaToPtr( rva ); }
	template< typename T > inline T VaToPtr( va_t va ) const { return (T) RvaToPtr( VaToRva( va ) ); }

	// Ptr to Rva/Va conversions
	unsigned PtrToRva( const void* ptr ) const;
	va_t PtrToVa( const void* ptr ) const;
	
	// Convert RVA <-> FileOffset
	unsigned RvaToFileOffset( rva_t rva ) const;
	unsigned FileOffsetToRva( unsigned offset ) const;

	// Convert RVA <-> VA
	va_t RvaToVa( rva_t rva ) const;
	rva_t VaToRva( va_t va ) const;

protected:
	void* _ptr;
	void* _vbase;
};



//------------------------------------------------
// Class PeFileRaw
//------------------------------------------------
// To normalize access to an image, just map the file into memory.

class PeFileRaw : public PeFile
{
public:
	PeFileRaw();
	~PeFileRaw();
	bool Init( FILE* file );

private:
	// Not allowed to copy because of memory management
	// However you *ARE* allowed to make a copy of the baseclass!
	PeFileRaw( const PeFileRaw& );
};

}

#endif // !HGUARD_PELITE_PEFILE
