
#include "filecstd.h"

namespace filesystem
{


bool filecstd::open( hpath file, int mode )
{
	char szMode[8];
	char* pm = szMode;
	
	if ( (mode&3)==MODE_READ ) *pm++ = 'r';
	else if ( (mode&3)==MODE_WRITE ) *pm++ = 'w';
	else if ( (mode&3)==MODE_APPEND ) *pm++ = 'a';

	if ( mode&MODE_UPDATE ) *pm++ = '+';
	if ( mode&MODE_TEXT ) *pm++ = 't';
	if ( mode&MODE_BINARY ) *pm++ = 'b';

	*pm = '\0';
	handle = ::fopen( file->c_str(), szMode );
	return handle != nullptr;
}
void filecstd::close()
{
	if ( handle )
		::fclose( handle );
	handle = nullptr;
}
int filecstd::status() const
{
	int s = S_BAD;
	if ( handle )
	{
		if ( ::ferror(handle) )
			s = S_FAIL;
		else if ( ::feof(handle) )
			s = S_EOF;
		else
			s = S_GOOD;
	}
	return s;
}
bool filecstd::info( info_t& fi ) const
{
	// Defaults
	fi.size = 0;
	fi.attributes = 0;
	fi.address.buffer[0] = 0;

	// Get file size
	pos_t pos = tell();
	const_cast<filecstd*>(this)->seek( 0, END );
	fi.size = tell();
	const_cast<filecstd*>(this)->seek( pos, BEGIN );

	// Rest is not available...
	return true;
}
bool filecstd::seek( pos_t offset, int origin )
{
	// If the high order dword is non-zero, only support SEEK_BEGIN
	if ( offset>>32 )
	{
		if ( origin!=SEEK_SET )
			return false;
		fpos_t pos = static_cast<fpos_t>( offset );
		return ::fsetpos( handle, &pos )==0;
	}
	// Otherwise we support all
	return ::fseek( handle, static_cast<long>(offset&0xFFFFFFFF), origin )==0;
}
file::pos_t filecstd::tell() const
{
	fpos_t pos = 0;
	::fgetpos( handle, &pos );
	return static_cast<pos_t>(pos);
}
file::size_t filecstd::read( void* buf, size_t size, unsigned int term ) const
{
	if ( term!=-1 )
	{
		char* s = ::fgets( (char*)buf, size, handle );
		if ( !s )
		{
			((char*)buf)[0] = 0;
			return 0;
		}
		return strlen( s );
	}
	return ::fread( buf, 1, size, handle );
}
file::size_t filecstd::write( const void* src, size_t size )
{
	return ::fwrite( src, size, 1, handle );
}
file::size_t filecstd::vprintf( const char* fmt, va_list va )
{
	return ::vfprintf( handle, fmt, va );
}
void filecstd::flush()
{
	::fflush( handle );
}


}
