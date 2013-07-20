#ifndef HGUARD_LIBEX_FILESYSTEM_WINFILE
#define HGUARD_LIBEX_FILESYSTEM_WINFILE
#pragma once

#include <cstdio>
#include "file.h"

namespace filesystem
{

class filecstd : public file
{
public:
	filecstd() : handle(nullptr) { }
	
	virtual bool open( hpath file, int mode );
	virtual void close();
	virtual int status() const;
	virtual bool info( info_t& fi ) const;
	virtual bool seek( pos_t offset, int origin );
	virtual pos_t tell() const;
	virtual size_t read( void* buf, size_t size, unsigned int term = -1 ) const;
	virtual size_t write( const void* src, size_t size );
	virtual size_t vprintf( const char* fmt, va_list va = nullptr );
	virtual void flush();

private:
	FILE* handle;
};

}

#endif // !HGUARD_LIBEX_FILESYSTEM_WINFILE
