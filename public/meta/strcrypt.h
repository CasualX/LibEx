#ifndef HGUARD_LIBEX_META_STRCRYPT
#define HGUARD_LIBEX_META_STRCRYPT
#pragma once

//----------------------------------------------------------------
// Meta: String encryption v3
//----------------------------------------------------------------
// Uses post-processing to encrypt strings...

// Declare a decrypted string, optimized when string encryption is not enabled
#define STRDECRYPT( STR ) XSTRDECRYPT( char, STR, STRCRYPT_PREFIX )
#define WSTRDECRYPT( STR ) XSTRDECRYPT( wchar_t, STR, WSTRCRYPT_PREFIX )
// Creates a strcrypted string for manual use (not optimized when disabled)
#define STRCRYPT( STR ) XSTRCRYPT( char, STRCRYPT_PREFIX STR )
#define WSTRCRYPT( STR ) XSTRCRYPT( wchar_t, WSTRCRYPT_PREFIX STR )
// Pre encrypted
#define PRECRYPT( STR ) XSTRAUTO( char, STR )


// Marks encrypted strings for the post-processing, must be 8 bytes.
#define STRCRYPT_PREFIX "Ca$!qI<4"
#define WSTRCRYPT_PREFIX L"€Ñµï"

// Actual macro implementation
#ifdef STRCRYPT_ENABLE
#define XSTRDECRYPT( TYPE, STR, PRE ) meta::strcrypted_t<TYPE>::buf_t<sizeof(STR)>( XSTRCRYPT(TYPE,PRE STR) )
#else
#define XSTRDECRYPT( TYPE, STR, PRE ) STR
#endif
#define XSTRCRYPT( TYPE, STR ) (*(const meta::strcrypted_t<TYPE>*)STR)
#define XSTRAUTO( TYPE, STR ) meta::strcbuf_t<TYPE,sizeof(STR)-8>( XSTRCRYPT(char,STR) )

namespace meta
{

class strcrypt_t
{
public:
	friend void strencrypt( strcrypt_t* src, unsigned key );

	// Global key for extra annoyingness
	static void setgkey( unsigned g );
	static unsigned _global;
	// Get the key for this string
	inline unsigned getkey() const { return _key^_global; }
	// Are we post-processed (encrypted)
	bool iscrypted() const;

protected:
	void* _decrypt( void* buf, unsigned size ) const;
	const void* _decrypt() const;

private:
	union {
		struct {
			union {
				// Base key
				unsigned int _key;
				// Length of string in dwords
				unsigned char _len;
			};
			unsigned _key2;
		};
		__int64 _prefix;
	};
	// Actual string (extends beyond this declaration)
	char _str[4];
};
template< typename T >
class strcrypted_t : public strcrypt_t
{
public:
	// Decrypt string into a buffer, returns the buffer
	template< unsigned L >
	inline T* decrypt( T (&buf)[L] ) const
	{
		return (T*) _decrypt( (T*)buf, L*sizeof(T) );
	}
	// Returns malloced memory with decrypted string
	template< unsigned U >
	inline const T* decrypt() const
	{
#if STRCRYPT_ENABLE!=0
		static const T* s = nullptr;
		if ( !s )
			s = (const T*) _decrypt();
		return s;
#else
		return this->_str;
#endif
	}
	// Syntactic sugary stuff
	template< unsigned S >
	class buf_t
	{
	public:
		inline buf_t( const strcrypted_t<T>& src ) { src.decrypt( _temp ); }
		inline operator T* () { return _temp; }
		inline operator const T* () const { return _temp; }
	private:
		// Dword aligned buffer given string size in bytes
		T _temp[ ((S/4+1)*4)/sizeof(T) ];
	};
};

// Encrypt a string
void strencrypt( strcrypt_t* src, unsigned key );
// Encrypt all strings in a range
void strencryptall( unsigned* begin, unsigned* end, unsigned basekey, unsigned gkey );

}


// Special version for small strings (<16 chars)
// Just write them dword by dword to a buffer
// FIXME! x64 support to write qword by qword?

#define STRDEF( S ) meta::strdef_t<((sizeof(S)-1)&~3)+4>( S "\0\0\0\0" )
#define STRDECL( S ) meta::strdef_t<((sizeof(#S)-1)&~3)+4> S

namespace meta
{

template< unsigned L >
struct strdef_t
{
	static_assert( L==4 || L==8 || L==12 || L==16,
		"Invalid buffer size! Make sure you use the supplied macro and the string is <16 chars." );
	strdef_t() { }
	strdef_t( const char* s ) { set(s); }
	//char* operator= ( const char* s ) { set(s); return buf; }
	inline operator char* () { return buf; }
	inline operator const char* () const { return buf; }
protected:
	inline void set( const char* s )
	{
		// x64 support write QWORD by QWORD?
		if ( L>0 ) ((int*)buf)[0] = ((const int*)s)[0];
		if ( L>4 ) ((int*)buf)[1] = ((const int*)s)[1];
		if ( L>8 ) ((int*)buf)[2] = ((const int*)s)[2];
		if ( L>12 ) ((int*)buf)[3] = ((const int*)s)[3];
	}
private:
	char buf[L];
};

}

#endif // !HGUARD_LIBEX_META_STRCRYPT
