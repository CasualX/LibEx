#ifndef HGUARD_LIBEX_PELITE_RESX
#define HGUARD_LIBEX_PELITE_RESX
#pragma once

//----------------------------------------------------------------
// PeLite: Resource Walking
//----------------------------------------------------------------

#include "pefile.h"

namespace pelite
{

class CResourceWalker
{
public:
	CResourceWalker( const char* Type, const char* File, const char* Lang );

	bool Bind( const PeFile& bin );
	char* Read( dword& size );

private:

	bool Compare( ImageResourceDirectoryStringU* str, const char* name );
	bool Compare( ImageResourceDirectoryEntry* it, const char* name );
	ImageResourceDirectoryEntry* Lookup( ImageResourceDirectory* dir, const char* name );
	char* Found( ImageResourceDataEntry* entry, dword& size );

	template< typename T >
	inline T GetAddress( dword offset )
	{
		return bin->RvaToPtr<T>( DataDir->VirtualAddress + offset );
	}

private:
	const char* Type;
	const char* File;
	const char* Lang;
	const PeFile* bin;
	ImageDataDirectory* DataDir;
};

}

#endif // !HGUARD_LIBEX_PELITE_RESX
