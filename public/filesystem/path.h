#ifndef HGUARD_LIBEX_FILESYSTEM_PATH
#define HGUARD_LIBEX_FILESYSTEM_PATH
#pragma once

//----------------------------------------------------------------
// Filename Paths
//----------------------------------------------------------------
// Somewhat based on boost::filesystem::path but uses plain C array as buffer.
// Encodes wchar_t (utf16le) to char in utf8.
//
// #define FILESYSTEM_USE_EXCEPTIONS to throw on unfixable errors.
// Defaults to truncating the paths if they become too large.
//

namespace filesystem
{

class path;
	
// Const handle accepts string literals
class hpath
{
public:
	inline hpath() { }
	inline hpath( const hpath& rhs ) { h = rhs.h; }
	inline hpath( const path& p ) { h = &p; }
	inline hpath( const path* p ) { h = p; }
	inline hpath( const char* s ) { str = s; }

	inline hpath& operator= ( const hpath& rhs ) { h = rhs.h; return *this; }
	inline hpath& operator= ( const path& p ) { h = &p; return *this; }
	inline hpath& operator= ( const path* p ) { h = p; return *this; }
	inline hpath& operator= ( const char* s ) { str = s; return *this; }

	inline const path* operator-> () const { return h; }
	inline const path& operator* () const { return *h; }
	inline const char& operator[] ( unsigned int i ) const { return str[i]; }
	inline operator const char* () { return str; }

private:
	union {
		const path* h;
		const char* str;
	};
};


class path
{
public:

	enum
	{
		max_length = 260,
		slash = '\\',
	};

	typedef const path& handle;
	
	//------------------------------------------------
	// Initialization
	//------------------------------------------------

	// Constructors
	path();
	path( const char* str );
	path( const wchar_t* str );
	path( const path& rhs );
	static const path& make( const char* str );
	static const path make( const wchar_t* str );
	// Assign
	path& assign( const char* str );
	path& assign( const wchar_t* str );
	path& assign( const path& rhs );
	path& operator= ( const char* str );
	path& operator= ( const wchar_t* str );
	path& operator= ( const path& rhs );
	// Adds files and dirs
	path& append( const char* str );
	path& append( const wchar_t* str );
	path& append( const path& rhs );
	path& operator/= ( const char* str );
	path& operator/= ( const wchar_t* str );
	path& operator/= ( const path& rhs );
	
	//------------------------------------------------
	// Modifiers
	//------------------------------------------------

	// Reset
	void clear();
	// Remove redundant slashes & converts to native format and verifies the contents to be a valid path name
	bool normalize();
	// Expand environment vars, return value is false if one or more failed to expand
	// In which case it'll complete the job but the env var remains in % signs
	// WARNING! This function has a lot of edge cases you should be aware of! (probably best to avoid using it...)
	bool expand( char sign = '%' );
	// Removes the filename
	void remove_filename();
	// Replaces the filename
	void replace_filename( const char* new_name );
	void replace_filename( const wchar_t* new_name );
	// Replaces the extension
	void replace_ext( const char* new_ext );
	void replace_ext( const wchar_t* new_ext );
	void replace_stem( const char* new_stem );
	void replace_stem( const wchar_t* new_stem );
	// Makes absolute
	void make_absolute( hpath base );
	// Makes relative, error we aren't a subdir of dir
	bool make_relative( hpath base );
	// Ensures this path is a directory by adding a trailing slash
	void make_dir();
	// Goes to parent path, will also strip filename
	void parent_dir();
	
	//------------------------------------------------
	// Decomposition
	//------------------------------------------------

	// Gets the final extension (includes the .)
	const char* ext() const;
	char* ext();
	// Gets all extensions in case of multiple exts
	const char* stem() const;
	char* stem();
	// Gets the filename
	const char* filename() const;
	char* filename();
	// Gets the relative path (NULL if such doesn't exist)
	// FIXME! Use .. notation on partial match? Requires a buffer to be passed though...
	const char* relative_path( hpath base ) const;
	char* relative_path( hpath base );
	// Gets the path without the root, must be an absolute path
	const char* root_path() const;
	char* root_path();
	// Get the full thing
	const char* c_str() const;
	char* c_str();
	// Extracts as utf16le
	//void extract( wchar_t* buf, unsigned int size );

	//------------------------------------------------
	// Query
	//------------------------------------------------

	// Do we have anything
	bool empty() const;
	// Are we absolute: start with a drive letter
	bool is_absolute() const;
	// Are we relative: !is_absolute && !is_win_unc && !is_nt_devname
	bool is_relative() const;
	// Windows UNC paths: begins with \\ or \\?\ -
	bool is_win_unc() const;
	// Windows NT Device Namespace: begins with \\.\ -
	bool is_nt_devname() const;
	// Are we a directory (checks for ending slash)
	bool is_directory() const;
	
	//------------------------------------------------
	// Iterating
	//------------------------------------------------



	static bool _unit_test();
protected:
	// Copy internal strings, assumes to is a ptr in the buffer
	char* _copy( char* to, const char* src );
	char* _copy( char* to, const wchar_t* src );
	// End of string (ptr to internal buffer)
	char* _end();

	// Error handling
	bool _testlen( const char* base, unsigned int chars );
	void _errlen();
	void _errenc( wchar_t c );

public:
	char buffer[max_length];
};


inline const path& make_path( const char* str ) { return *reinterpret_cast<const path*>( str ); }



// Inlines

inline path::path() { }
inline path::path( const char* str ) { assign( str ); }
inline path::path( const wchar_t* str ) { assign( str ); }
inline path::path( const path& rhs ) { assign( rhs ); }
inline const path& path::make( const char* str ) { return *reinterpret_cast<const path*>(str); }
inline const path path::make( const wchar_t* str ) { return path( str ); }
inline path& path::assign( const char* str ) { _copy( buffer, str ); return *this; }
inline path& path::assign( const wchar_t* str ) { _copy( buffer, str ); return *this; }
inline path& path::assign( const path& rhs ) { return assign( rhs.buffer ); }
inline path& path::operator= ( const char* str ) { return assign( str ); }
inline path& path::operator= ( const wchar_t* str ) { return assign( str ); }
inline path& path::operator= ( const path& rhs ) { return assign( rhs ); }

inline path& path::append( const path& rhs ) { return append( rhs.buffer ); }
inline path& path::operator/= ( const char* str ) { return append( str ); }
inline path& path::operator/= ( const wchar_t* str ) { return append( str ); }
inline path& path::operator/= ( const path& rhs ) { return append( rhs.buffer ); }

inline void path::clear() { *(unsigned int*)buffer = 0; }

inline char* path::ext() { return const_cast<char*>( const_cast<const path*>(this)->ext() ); }
inline char* path::stem() { return const_cast<char*>( const_cast<const path*>(this)->stem() ); }
inline char* path::filename() { return const_cast<char*>( const_cast<const path*>(this)->filename() ); }
inline char* path::relative_path( hpath base ) { return const_cast<char*>( const_cast<const path*>(this)->relative_path(base) ); }
inline char* path::root_path() { return const_cast<char*>( const_cast<const path*>(this)->root_path() ); }
inline const char* path::c_str() const { return buffer; }
inline char* path::c_str() { return buffer; }

inline bool path::_testlen( const char* base, unsigned int chars )
{
	// Tests if we have room for additional characters starting from base
	if ( static_cast<unsigned int>(base-buffer)>=(static_cast<unsigned int>(max_length)-chars-1) )
	{
		_errlen();
		return false;
	}
	else
	{
		return true;
	}
}
}

#endif // !HGUARD_LIBEX_FILESYSTEM_PATH
