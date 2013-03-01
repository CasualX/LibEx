#ifndef HGUARD_LIBEX_FILESYSTEM_FILESDIR
#define HGUARD_LIBEX_FILESYSTEM_FILESDIR
#pragma once

#include "path.h"
#include "file.h"
#include "../pelite/filesdir.h"

namespace filesystem
{

class fdir_file : public file
{
public:
	fdir_file( void* hmod = nullptr, unsigned int key = 0 );
	
	virtual bool open( hpath file, int mode );
	virtual void close();
	virtual int status() const;
	virtual bool info( file_info& fi ) const;
	virtual bool seek( pos_t offset, int origin );
	virtual pos_t tell() const;
	virtual unsigned int read( void* buf, unsigned int size, unsigned int term = -1 ) const;
	virtual unsigned int write( const void* src, unsigned int size );
	virtual void flush();

private:
	pelite::ImageFilesHeader* hdr;
	unsigned int ekey;
	unsigned int off;
	unsigned int state;
	pelite::ImageFilesDesc desc;
	path name;
};


}

#endif // !HGUARD_LIBEX_FILESYSTEM_FILESDIR
