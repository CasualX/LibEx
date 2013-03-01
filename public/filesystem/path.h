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

class path
{
public:

	enum
	{
		max_length = 260,
		slash = '\\',
	};
	
	//------------------------------------------------
	// Initialization
	//------------------------------------------------

	// Constructors
	path();
	path( const char* str );
	path( const wchar_t* str );
	path( const path& rhs );
	// Assign
	void assign( const char* str );
	void assign( const wchar_t* str );
	void assign( const path& rhs );
	path& operator= ( const char* str );
	path& operator= ( const wchar_t* str );
	path& operator= ( const path& rhs );
	// Adds files and dirs
	void append( const char* str );
	void append( const wchar_t* str );
	void append( const path& rhs );
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
	void make_absolute( const path& base );
	// Makes relative, error we aren't a subdir of dir
	bool make_relative( const path& base );
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
	const char* relative_path( const path& base ) const;
	char* relative_path();
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
	// Are we absolute
	bool is_absolute() const;
	bool is_relative() const;
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

// Const handle accepts string literals
class hpath
{
public:
	inline hpath() { }
	inline hpath( const hpath& rhs ) { h = rhs.h; }
	inline hpath( const path& p ) { h = &p; }
	inline hpath( const path* p ) { h = p; }
	inline hpath( const char* s ) { str = s; }

	inline hpath& operator= ( const hpath& rhs ) { h = rhs.h; }
	inline hpath& operator= ( const path& p ) { h = &p; }
	inline hpath& operator= ( const path* p ) { h = p; }
	inline hpath& operator= ( const char* s ) { str = s; }

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




// Inlines

inline path::path() { }
inline path::path( const char* str ) { assign( str ); }
inline path::path( const wchar_t* str ) { assign( str ); }
inline path::path( const path& rhs ) { assign( rhs ); }
inline void path::assign( const char* str ) { _copy( buffer, str ); }
inline void path::assign( const wchar_t* str ) { _copy( buffer, str ); }
inline void path::assign( const path& rhs ) { assign( rhs.buffer ); }
inline path& path::operator= ( const char* str ) { assign( str ); return *this; }
inline path& path::operator= ( const wchar_t* str ) { assign( str ); return *this; }
inline path& path::operator= ( const path& rhs ) { assign( rhs ); return *this; }

inline void path::append( const path& rhs ) { append( rhs.buffer ); }
inline path& path::operator/= ( const char* str ) { append( str ); return *this; }
inline path& path::operator/= ( const wchar_t* str ) { append( str ); return *this; }
inline path& path::operator/= ( const path& rhs ) { append( rhs.buffer ); return *this; }

inline void path::clear() { *(unsigned int*)buffer = 0; }

inline char* path::ext() { return const_cast<char*>( const_cast<const path*>(this)->ext() ); }
inline char* path::stem() { return const_cast<char*>( const_cast<const path*>(this)->stem() ); }
inline char* path::filename() { return const_cast<char*>( const_cast<const path*>(this)->filename() ); }
//inline char* path::relative_path() { return const_cast<char*>( const_cast<const path*>(this)->relative_path() ); }
inline char* path::root_path() { return const_cast<char*>( const_cast<const path*>(this)->root_path() ); }
inline const char* path::c_str() const { return buffer; }
inline char* path::c_str() { return buffer; }

inline bool path::empty() const { return buffer[0]==0; }
inline bool path::is_absolute() const { return buffer[1]==':'; }
inline bool path::is_relative() const { return buffer[1]!=':'; }

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
