
#include "filesdir.h"

namespace pelite
{
// This is a huge mess :)
// Just forget about this and use the filesystem abstractions...

//----------------------------------------------------------------
// Crypto using modified XXTEA
//----------------------------------------------------------------
void FilesSetupKey( const unsigned int* ekey, unsigned int off, unsigned int* outkey )
{
#define PRNG( val ) ((val*1103515245)+12345)
	unsigned int mask = PRNG(off);
	outkey[0] = ekey[0]^mask;
	//mask = PRNG(mask);
	outkey[1] = ekey[1]^mask;
	//mask = PRNG(mask);
	outkey[2] = ekey[2]^mask;
	//mask = PRNG(mask);
	outkey[3] = ekey[3]^mask;
#undef PRNG
}
#define MX ((((z>>5)^(y<<2)) + ((y>>3)^(z<<4))) ^ ((sum^y) + (key[(p&3)^e] ^ z)))
//#define DELTA ~0x9e3779b7
#define DELTA 0x927C9716
void FilesEncryptBlock( void* out, const void* src, const unsigned int* key )
{
	unsigned int y, z, sum;
	unsigned int p, rounds, e;
	// Copy over the block
	unsigned int n = ImageFilesBlockSize / 4;
	unsigned int* v = (unsigned int*)out;
	if ( out!=src )
		__movsd( (unsigned long*)out, (const unsigned long*)src, n );
	// Encoding part
	rounds = 6 + 52/n;
	sum = 0;
	z = v[n-1];
	do
	{
		sum += DELTA;
		e = (sum >> 2) & 3;
		for ( p = 0; p<(n-1); ++p )
		{
			y = v[p+1]; 
			z = ( v[p] += MX );
		}
		y = v[0];
		z = ( v[n-1] += MX );
	}
	while ( --rounds );
}
void FilesDecryptBlock( void* out, const void* src, const unsigned int* key )
{
	unsigned int y, z, sum;
	unsigned int p, rounds, e;
	// Copy over the block
	unsigned int n = ImageFilesBlockSize / 4;
	unsigned int* v = (unsigned int*)out;
	if ( out!=src )
		__movsd( (unsigned long*)out, (const unsigned long*)src, n );
	// Decoding part
	rounds = 6 + 52/n;
	sum = rounds*DELTA;
	y = v[0];
	do
	{
		e = (sum >> 2) & 3;
		for ( p = n-1; p>0; --p )
		{
			z = v[p-1];
			y = ( v[p] -= MX );
		}
		z = v[n-1];
		y = ( v[0] -= MX );

		sum -= DELTA;
	}
	while ( --rounds );
}
#undef DELTA
#undef MX
void FilesDir::Decrypt( void* out, const void* data, unsigned int size )
{
	enum { BLOCK = ImageFilesBlockSize };
	
	struct data_t
	{
		unsigned int key[4];
		unsigned int buf[BLOCK/4];
	} d;
	unsigned int* key = d.key;
	unsigned int* buf = d.buf;

	const unsigned int* iv = hdr->InitVec;
	
	// Get offset to determine alignment
	unsigned int off = static_cast<unsigned int>( (char*)data - (char*)hdr );
	unsigned int begin = off&~(BLOCK-1);

	// Initialize the key for CTR Mode of Operation
	FilesSetupKey( iv, begin, key );
	
	// Decrypt first block if misaligned
	FilesDecryptBlock( buf, (char*)hdr + begin, key );
	unsigned int it = BLOCK-(off-begin);
	if ( it>size ) it = size;
	memcpy( out, (char*)buf+(off-begin), it );

	// Decrypt loop
	if ( it<size )
	{
		out = (char*)out + it;
		data = (char*)data + it;
		while ( (it+BLOCK)<=size )
		{
			// Setup key and decrypt
			FilesSetupKey( iv, static_cast<unsigned int>( (char*)data - (char*)hdr ), key );
			FilesDecryptBlock( out, data, key );

			// Next
			it += BLOCK;
			out = (char*)out + BLOCK;
			data = (char*)data + BLOCK;
		}

		// Decrypt remainder
		int rem = size-it;
		if ( rem>0 )
		{
			FilesSetupKey( iv, static_cast<unsigned int>( (char*)data - (char*)hdr ), key );
			FilesDecryptBlock( buf, data, key );
			memcpy( out, buf, rem );
		}
	}
	// Erase sensitive information
	__stosd( (unsigned long*)&d, 0, sizeof(d)/4 );
}
void FilesDir::Encrypt( ImageFilesHeader* hdr )
{
	if ( hdr->BlockSize!=ImageFilesBlockSize || (hdr->TotalSize%hdr->BlockSize) )
		__debugbreak();

	// Crypt the files stuff
	unsigned int key[4];
	for ( unsigned int it = hdr->BlockSize; it<hdr->TotalSize; it+=hdr->BlockSize )
	{
		FilesSetupKey( hdr->InitVec, it, key );
		FilesEncryptBlock( (char*)hdr + it, (char*)hdr + it, key );
	}
}

bool FilesDir::Init( void* hmod )
{
	if ( !hmod )
		hmod = &__ImageBase;
	ImageNtHeader* nt = (ImageNtHeader*)( (char*)hmod + ((ImageDosHeader*)hmod)->e_lfanew );
	
	// Barely does any checking... lol
	ImageDataDirectory& rsrc = nt->OptionalHeader.DataDirectory[ DataDir_Resource ];
	if ( rsrc.VirtualAddress )
	{
		unsigned int off = ((rsrc.Size-1)&~3)+4;
		hdr = (ImageFilesHeader*)( (char*)hmod + rsrc.VirtualAddress + off );
		return true;
	}

	return false;
}
bool FilesDir::FindDesc( ImageFilesDesc* desc, const char* file, ImageFilesDesc* parent )
{
	ImageFilesDesc* end;
	ImageFilesDesc* it;
	if ( !parent )
	{
		it = (ImageFilesDesc*)( (char*)hdr + hdr->OffsetToRoot );
		end = (ImageFilesDesc*)( (char*)it + hdr->SizeOfRoot );
	}
	else
	{
		it = (ImageFilesDesc*)( (char*)hdr + parent->OffsetToData );
		end = (ImageFilesDesc*)( (char*)it + parent->SizeOfData );
	}
	const unsigned int* iv = hdr->InitVec;

	for ( ; it<end; ++it )
	{
		// Decrypt the desc
		Decrypt( desc, it, sizeof(ImageFilesDesc) );
		// Compare loop...
		for ( unsigned int i = 0; true; ++i )
		{
			char a = desc->Name[i];
			char b = file[i];
			if ( !a )
			{
				// Exact final match found
				if ( !b )
					return true;
				// Match but we have a subdirectory to look for
				else if ( b=='/' || b=='\\' )
				{
					if ( !desc->ContentType )
					{
						return FindDesc( desc, file+i+1, desc );
					}
					// Not a directory
					else return false;
				}
				// No exact match
				else goto nextfile;
			}
			if ( a>='A' && a<='Z' ) a = a-'A'+'a';
			if ( b>='A' && b<='Z' ) b = b-'A'+'a';
			if ( a!=b ) goto nextfile;
		}
nextfile:;
	}
	// Not found
	return false;
}

}
