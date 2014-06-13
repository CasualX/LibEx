
#include "pefile.h"
#include <cstdio>
#include <cstdlib>
#include <exception>


namespace pelite
{

// PeFile

PeFile::PeFile() : _ptr(nullptr), _vbase(nullptr)
{
}
PeFile::PeFile( void* hmod, void* vbase )
{
	Init( hmod, vbase );
}
bool PeFile::IsValid( void* hmod )
{
	if ( hmod )
	{
		ImageDosHeader* dos = (ImageDosHeader*) hmod;
		ImageNtHeaders* nt = (ImageNtHeaders*)( (char*)hmod + dos->e_lfanew );
		return dos->e_magic==ImageDosHeaderMagic && nt->Signature==ImageNtHeaderSignature;
	}
	return false;
}
void PeFile::Init( void* hmod, void* vbase )
{
	_ptr = hmod;
	_vbase = vbase?vbase:hmod;
}
void* PeFile::ImageBase() const
{
	return _ptr;
}
void* PeFile::VirtualBase() const
{
	return _vbase;
}
rva_t PeFile::VirtualSize() const
{
	return NtHeaders()->OptionalHeader.SizeOfImage;
}
ImageDosHeader* PeFile::DosHeader() const
{
	return (ImageDosHeader*)_ptr;
}
ImageNtHeaders* PeFile::NtHeaders() const
{
	ImageDosHeader* dos = DosHeader();
	return (ImageNtHeaders*)( (char*)dos + dos->e_lfanew );
}
ImageFileHeader* PeFile::FileHeader() const
{
	return & NtHeaders()->FileHeader;
}
ImageOptionalHeader* PeFile::OptionalHeader() const
{
	return & NtHeaders()->OptionalHeader;
}
ImageSectionHeader* PeFile::Sections() const
{
	ImageNtHeaders* nt = NtHeaders();
	return (ImageSectionHeader*)( (char*)&nt->OptionalHeader + nt->FileHeader.SizeOfOptionalHeader );
}
inline bool SectionCompare( ImageSectionHeader* sec, const char* name )
{
	for ( unsigned int i = 0; i<8; ++i )
	{
		if ( name[i]!=sec->Name[i] )
			return false;
		if ( !name[i] )
			break;
	}
	return true;
}
ImageSectionHeader* PeFile::FindSection( const char* name ) const
{
	for ( auto it = Sections(), end = it + FileHeader()->NumberOfSections; it<end; ++it )
	{
		if ( SectionCompare( it, name ) )
		{
			return it;
		}
	}
	return nullptr;
}
ImageSectionHeader* PeFile::FindSectionFor( rva_t rva ) const
{
	for ( auto it = Sections(), end = it + FileHeader()->NumberOfSections; it<end; ++it )
	{
		if ( rva>=it->VirtualAddress && rva<(it->VirtualAddress + it->VirtualSize) )
		{
			return it;
		}
	}
	return nullptr;
}
void* PeFile::RvaToPtr( rva_t rva ) const
{
	return (char*)_ptr + rva;
}
rva_t PeFile::PtrToRva( const void* ptr ) const
{
	return static_cast<rva_t>( (char*)ptr - (char*)_ptr );
}
unsigned PeFile::RvaToFileOffset( rva_t rva ) const
{
	ImageNtHeaders* nt = NtHeaders();

	if ( rva<nt->OptionalHeader.SizeOfHeaders )
	{
		return rva;
	}

	for ( auto it = Sections(), end = it + nt->FileHeader.NumberOfSections; it<end; ++it )
	{
		if ( it->VirtualAddress<=rva && rva<(it->VirtualAddress+it->SizeOfRawData) )
		{
			return rva - it->VirtualAddress + it->PointerToRawData;
		}
	}

	return BADRVA;
}
rva_t PeFile::FileOffsetToRva( unsigned offset ) const
{
	ImageNtHeaders* nt = NtHeaders();

	if ( offset<nt->OptionalHeader.SizeOfHeaders )
	{
		return offset;
	}

	for ( auto it = Sections(), end = it + nt->FileHeader.NumberOfSections; it<end; ++it )
	{
		if ( it->PointerToRawData<=offset && offset<(it->PointerToRawData+it->SizeOfRawData) )
		{
			return offset - it->PointerToRawData + it->VirtualAddress;
		}
	}

	return BADRVA;
}
va_t PeFile::RvaToVa( rva_t rva ) const
{
	return (char*)_vbase + rva;
}
rva_t PeFile::VaToRva( va_t va ) const
{
	return static_cast<rva_t>( (char*)va - (char*)_vbase );
}





// PeFileRaw

PeFileRaw::~PeFileRaw()
{
	::VirtualFree( _ptr, 0, MEM_FREE );
}
bool PeFileRaw::Init( FILE* file )
{
	// First read the pe headers
	ImageDosHeader dos;
	if ( ::fseek( file, 0, SEEK_SET ) || ::fread( &dos, sizeof(dos), 1, file )!=1 )
		return false;
	ImageNtHeaders nt;
	if ( ::fseek( file, dos.e_lfanew, SEEK_SET ) || ::fread( &nt, sizeof(nt), 1, file )!=1 )
		return false;

	// Allocate memory to map the image
	_ptr = ::VirtualAlloc( NULL, nt.OptionalHeader.SizeOfImage, MEM_COMMIT|MEM_RESERVE, PAGE_READWRITE );
	_vbase = (void*)nt.OptionalHeader.ImageBase;
	assert( _ptr );

	// Copy the headers
	if ( ::fseek( file, 0, SEEK_SET ) || ::fread( _ptr, nt.OptionalHeader.SizeOfHeaders, 1, file )!=1 )
		return false;

	// Copy the sections
	auto s = Sections();
	for ( auto it = s, end = it + nt.FileHeader.NumberOfSections; it<end; ++it )
	{
		if ( it->SizeOfRawData )
		{
			if ( ::fseek( file, it->PointerToRawData, SEEK_SET ) || ::fread( (char*)_ptr + it->VirtualAddress, it->SizeOfRawData, 1, file )!=1 )
				return false;
		}
	}

	return true;
}

}
