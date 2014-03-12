#ifndef HGUARD_LIBEX_FILESYSTEM_FILE
#define HGUARD_LIBEX_FILESYSTEM_FILE
#pragma once

#include <cassert>
#include <cstdlib>
#include <cstdarg>

#include "path.h"
#include "../libex.h"

namespace filesystem
{

struct file_info
{
	unsigned long long size;
	unsigned int attributes;
	path address;
};

enum mode_t
{
	MODE_READ = 1,
	MODE_WRITE = 2,
	MODE_APPEND = 3,
	MODE_UPDATE = (1<<2),

	MODE_TEXT = (1<<3),
	MODE_BINARY = (1<<4),
};

INTERFACE file
{
public:
	enum origin_t
	{
		BEGIN = 0,
		CURRENT = 1,
		END = 2,
	};
	enum status_t
	{
		S_BAD = (1<<0),
		S_GOOD = (1<<1),
		S_EOF = (1<<2),
		S_FAIL = (1<<3),
	};
	
	typedef long long pos_t;
	typedef unsigned long long upos_t;
	typedef size_t size_t;
	typedef file_info info_t;
	typedef mode_t mode_t;

	// FIXME! WTF I want to call close() here but it won't let me!!!
	virtual ~file() {}

	virtual bool open( hpath file, int mode ) = 0;
	virtual void close() = 0;

	// Shared functions
	virtual int status() const = 0;
			bool eof() const;
			bool good() const;

	virtual bool info( info_t& fi ) const = 0;

	virtual bool seek( pos_t offset, int origin ) = 0;
			void rewind();
	virtual pos_t tell() const = 0;

	// Read functions
	virtual size_t read( void* buf, size_t size, unsigned int term = -1 ) const = 0;
			char* dump( size_t& size ) const;
	
	// Write functions
	virtual size_t write( const void* src, size_t size ) = 0;
	virtual size_t vprintf( const char* fmt, va_list va = nullptr ) = 0;
			size_t printf( const char* fmt, ... );
	virtual void flush() = 0;
};

class cache
{
public:
	cache();
	~cache();

	enum { RDBUF_SIZE = 2048, };

	// Base addr for our data buffer
	unsigned long long base;
	// Current seek pos in data
	unsigned int offset;
	// Buffered data
	unsigned char* data;
};

template< class base >
class textfile : public base
{
public:

};





//
// Inlines
//
inline bool file::eof() const
{
	return (status()&S_EOF)!=0;
}
inline bool file::good() const
{
	return (status()&S_GOOD)!=0;
}
inline void file::rewind()
{
	seek( 0, BEGIN );
}
inline char* file::dump( size_t& size ) const
{
	// Read a complete file & dump to allocated memory, arbitrary 10MB limit
	// Has to be inline so the proper memory alloc is used, don't forget to ::free() when you're done
	// Must be at the beginning of the file (which is normally the case), seek offset will be at the end when done
	file_info fi;
	if ( this->info( fi ) )
	{
		assert( fi.size<(10*1024*1024) );
		assert( this->tell()==0 );
		size = static_cast<unsigned int>( fi.size );
		char* data = (char*) ::malloc( size )+1;
		size_t r = read( data, size );
		data[r] = 0;
		return data;
	}
	return nullptr;
}
inline file::size_t file::printf( const char* fmt, ... )
{
	va_list va;
	va_start( va, fmt );
	return vprintf( fmt, va );
}

}

#endif // !HGUARD_LIBEX_FILESYSTEM_FILE
