
#include "resx.h"

namespace pelite
{

CResourceWalker::CResourceWalker( const char* Type, const char* File, const char* Lang ) :
	Type(Type), File(File), Lang(Lang)
{
}

bool CResourceWalker::Bind( HMODULE hModule )
{
	if ( this->hModule = hModule )
	{
		ImageDosHeader* dos = (ImageDosHeader*) hModule;
		ImageNtHeader* nt = (ImageNtHeader*)( (uintptr_t)hModule + dos->e_lfanew );
		DataDir = &nt->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_RESOURCE];
		return DataDir->VirtualAddress!=0;
	}
	return false;
}


bool CResourceWalker::Compare( ImageResourceDirectoryStringU* str, const char* name )
{
	unsigned int i;
	for ( i = 0; i<str->Length; ++i )
	{
		// FIXME! Proper uppercase please...
		wchar_t c = str->NameString[i];
		if ( c>='a' && c<='z' ) c = c - 'a' + 'A';
		wchar_t d = name[i]; // FIXME! UTF-8 please...
		if ( d>='a' && d<='z' ) d = d - 'a' + 'A';

		if ( c!=d )
		{
			return false;
		}
	}
	return name[i]==0;
}
bool CResourceWalker::Compare( ImageResourceDirectoryEntry* it, const char* name )
{
	// Optional, will match the first entry in the directory
	if ( name==nullptr )
	{
		return true;
	}
	// Name is a string (FIXME! Detect wchar_t!)
	else if ( (uintptr_t)name >= 0x10000 )
	{
		return it->NameIsString && Compare( GetAddress<ImageResourceDirectoryStringU*>( it->NameOffset ), name );
	}
	// Name is a resource id
	else
	{
		return !it->NameIsString && ((unsigned short)name)==it->Id;
	}
}

ImageResourceDirectoryEntry* CResourceWalker::Lookup( ImageResourceDirectory* dir, const char* name )
{
	for ( auto
		it = (ImageResourceDirectoryEntry*)( dir + 1 ),
		end = it + dir->NumberOfIdEntries + dir->NumberOfNamedEntries;
		it<end; ++it )
	{
		if ( Compare( it, name ) )
		{
			return it;
		}
	}
	return nullptr;
}

char* CResourceWalker::Read( dword& size )
{
	// First expect a 'resource type' directory
	auto root = GetAddress<ImageResourceDirectory*>( 0 );
	if ( auto typeEntry = Lookup( root, Type ) )
	{
		// Now expect the actual file entry
		assert( typeEntry->DataIsDirectory );
		auto typeDir = GetAddress<ImageResourceDirectory*>( typeEntry->OffsetToDirectory );
		if ( auto fileEntry = Lookup( typeDir, File ) )
		{
			if ( fileEntry->DataIsDirectory )
			{
				// If it is a 'language directory' (wtf?) look up the language
				auto fileDir = GetAddress<ImageResourceDirectory*>( fileEntry->OffsetToDirectory );
				if ( auto langEntry = Lookup( fileDir, Lang ) )
				{
					// Ok this better be the actual resource
					assert( !langEntry->DataIsDirectory );
					return Found( GetAddress<ImageResourceDataEntry*>( langEntry->OffsetToData ), size );
				}
			}
			else
			{
				return Found( GetAddress<ImageResourceDataEntry*>( fileEntry->OffsetToData ), size ); 
			}
		}
	}
	return nullptr;
}

char* CResourceWalker::Found( ImageResourceDataEntry* entry, dword& size )
{
	size = entry->Size;
	// GOD WHY IS THIS AN RVA SUDDENLY?!?
	return (char*)( (uintptr_t)hModule + entry->OffsetToData );
	//return GetAddress<char*>( entry->OffsetToData );
}

}
