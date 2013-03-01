#ifndef HGUARD_LIBEX_FILESYSTEM_FILE
#define HGUARD_LIBEX_FILESYSTEM_FILE
#pragma once

#include <cassert>
#include <cstdlib>

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

INTERFACE file
{
public:
	enum origin_t
	{
		begin = 0,
		current = 1,
		end = 2,
	};
	enum status_t
	{
		S_BAD = (1<<0),
		S_GOOD = (1<<1),
		S_EOF = (1<<2),
		S_FAIL = (1<<3),
	};
	
	typedef int pos_t;
	typedef unsigned upos_t;

	virtual ~file() {}

	virtual bool open( hpath file, int mode ) = 0;
	virtual void close() = 0;

	// Shared functions
	virtual int status() const = 0;
			bool eof() const;
			bool good() const;

	virtual bool info( file_info& fi ) const = 0;

	virtual bool seek( pos_t offset, int origin ) = 0;
			void rewind();
	virtual pos_t tell() const = 0;

	// Read functions
	virtual unsigned int read( void* buf, unsigned int size, unsigned int term = -1 ) const = 0;
			char* dump( unsigned int& size ) const;
	
	// Write functions
	virtual unsigned int write( const void* src, unsigned int size ) = 0;
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
	seek( 0, 0 );
}
inline char* file::dump( unsigned int& size ) const
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
		char* data = (char*) ::malloc( size );
		read( data, size );
		return data;
	}
	return NULL;
}

}

#endif // !HGUARD_LIBEX_FILESYSTEM_FILE
