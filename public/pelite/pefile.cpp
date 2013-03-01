
#include "pefile.h"
#include "../toolkit/tools.h"
#include <cstdio>
#include <cstdlib>
#include <exception>


namespace pelite
{
using toolkit::make_ptr;
using toolkit::getoffset;


// PeFile

PeFile::PeFile( void* hmod, void* vbase )
{
	if ( !Init( hmod, vbase ) )
	{
		throw std::exception( "Invalid" );
	}
}
bool PeFile::Init( void* hmod, void* vbase )
{
	if ( hmod && (_mzheader=(ImageDosHeader*)hmod)->e_magic==ImageDosHeaderMagic )
	{
		_ntheader = make_ptr<ImageNtHeader*>( hmod, _mzheader->e_lfanew );
		if ( _ntheader->Signature==ImageNtHeaderSignature )
		{
			_sections = make_ptr<ImageSectionHeader*>( &_ntheader->OptionalHeader, _ntheader->FileHeader.SizeOfOptionalHeader );
			_size = _ntheader->OptionalHeader.SizeOfImage;
			_vbase = vbase?vbase:hmod;
			return true;
		}
	}
	return false;
}
bool PeFile::IsMapped() const
{
	return true;
}
ImageSectionHeader* PeFile::SectionTableEnd() const
{
	return SectionTable() + _ntheader->FileHeader.NumberOfSections;
}
ImageSectionHeader* PeFile::FindSection( const char* name ) const
{
	typedef ImageSectionHeader* iterator;
	for ( iterator it = SectionTable(), end = SectionTableEnd(); it<end; ++it )
	{
		// FIXME! if it->Name uses all 8 characters (.textbss) it'll overlap with the next member!
		if ( !strcmp( name, it->Name ) )
			return it;
	}
	return NULL;
}
ImageSectionHeader* PeFile::FindSectionFor( unsigned int rva ) const
{
	typedef ImageSectionHeader* iterator;
	for ( iterator it = SectionTable(), end = SectionTableEnd(); it<end; ++it )
	{
		if ( rva>=it->VirtualAddress && rva<(it->VirtualAddress + it->VirtualSize) )
			return it;
	}
	return NULL;
}
void* PeFile::RvaToPtr( unsigned int rva ) const
{
	return make_ptr<void*>( _mzheader, rva );
}
unsigned int PeFile::PtrToRva( const void* ptr ) const
{
	return getoffset( _mzheader, ptr );
}
unsigned int PeFile::RvaToFileOffset( unsigned int rva ) const
{
	ImageSectionHeader* end = _sections + _ntheader->FileHeader.NumberOfSections;

	if ( rva<static_cast<unsigned int>(getoffset( _mzheader, end )) )
	{
		return rva;
	}

	for ( ImageSectionHeader* hdr = _sections; hdr<end; hdr++ )
	{
		if ( hdr->VirtualAddress<=rva && rva<(hdr->VirtualAddress+hdr->SizeOfRawData) )
		{
			return rva - hdr->VirtualAddress + hdr->PointerToRawData;
		}
	}

	return BADRVA;
}
unsigned int PeFile::FileOffsetToRva( unsigned int offset ) const
{
	ImageSectionHeader* end = _sections + _ntheader->FileHeader.NumberOfSections;

	if ( offset<static_cast<unsigned int>(getoffset( _mzheader, end )) )
	{
		return offset;
	}

	for ( ImageSectionHeader* hdr = _sections; hdr<end; hdr++ )
	{
		if ( hdr->PointerToRawData<=offset && offset<(hdr->PointerToRawData+hdr->SizeOfRawData) )
		{
			return offset - hdr->PointerToRawData + hdr->VirtualAddress;
		}
	}

	return BADRVA;
}





// PeFileRaw

PeFileRaw::PeFileRaw( void* base, unsigned size )
{
	if ( !Init( base, size ) )
	{
		throw std::exception( "Invalid" );
	}
}
bool PeFileRaw::Init( void* ptr, unsigned size )
{
	if ( ptr && (_mzheader=(ImageDosHeader*)ptr)->e_magic==ImageDosHeaderMagic )
	{
		_ntheader = make_ptr<ImageNtHeader*>( ptr, _mzheader->e_lfanew );
		if ( _ntheader->Signature==ImageNtHeaderSignature )
		{
			_sections = make_ptr<ImageSectionHeader*>( &_ntheader->OptionalHeader, _ntheader->FileHeader.SizeOfOptionalHeader );
			_size = size;
			_vbase = (void*) _ntheader->OptionalHeader.ImageBase;
			return true;
		}
	}
	return false;
}
bool PeFileRaw::IsMapped() const
{
	return false;
}
void* PeFileRaw::RvaToPtr( unsigned int rva ) const
{
	unsigned int offset = RvaToFileOffset( rva );
	return ( offset==BADRVA ) ? nullptr : make_ptr<void*>( _mzheader, offset );
}
unsigned int PeFileRaw::PtrToRva( const void* ptr ) const
{
	unsigned int rva = FileOffsetToRva( getoffset( _mzheader, ptr ) );
	return rva;
}





// PeFileRawManaged

PeFileRawManaged::PeFileRawManaged( const char* filename )
{
	if ( !Init( filename ) )
	{
		throw std::exception( "Invalid" );
	}
}
PeFileRawManaged::~PeFileRawManaged()
{
	::free( _mzheader );
}
bool PeFileRawManaged::Init( const char* filename )
{
	if ( FILE* h = ::fopen( filename, "r" ) )
	{
		::fseek( h, 0, SEEK_END );
		unsigned size = ::ftell( h );
		::fseek( h, 0, SEEK_SET );
		char* data = (char*) ::malloc( size );
		::fread( data, 1, size, h );
		::fclose( h );
		PeFileRaw::Init( data, size );
		return true;
	}
	return false;
}

};
