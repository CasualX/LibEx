#ifndef HGUARD_LIBEX_PELITE_FILESDIR
#define HGUARD_LIBEX_PELITE_FILESDIR
#pragma once

//----------------------------------------------------------------
// PeLite: Hidden encrypted files directory
//----------------------------------------------------------------

#include "image.h"
#include "pefile.h"

namespace pelite
{

class FilesDir
{
public:
	bool Init( void* hmod = nullptr );

	// Crypto
	void Decrypt( void* out, const void* data, unsigned size );
	static void Encrypt( ImageFilesHeader* hdr );

	// Fetch a file descriptor
	bool FindDesc( ImageFilesDesc* desc, const char* file, ImageFilesDesc* parent = nullptr );

	// Fetch file contents
	template< typename T >
	inline T GetData( const ImageFilesDesc* desc )
	{
		assert( desc->OffsetToData<hdr->TotalSize );
		return (T)( (char*)hdr + desc->OffsetToData );
	}
private:
	ImageFilesHeader* hdr;
};

}

#endif // !HGUARD_LIBEX_PELITE_FILESDIR
