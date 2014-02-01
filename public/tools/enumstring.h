#ifndef HGUARD_LIBEX_TOOLS_ENUMSTRING
#define HGUARD_LIBEX_TOOLS_ENUMSTRING
#pragma once

//----------------------------------------------------------------
// Tools: EnumString
//----------------------------------------------------------------
// Provides enum<->string conversions.
// Look this isn't perfect but ugh...
//
// Conversion for 'bool' is defined at the bottom (.h and .cpp)
// And can be used as an example.

#include "../libex.h"
#include "printf.h"

namespace tools
{

class CEnumBase
{
public:
	// For portability?
	typedef int enum_t;
	// Some flags to instruct the functions
	enum {
		R_THROW, // Throw exception if value is not a valid enum.
		R_FALSE, // Return false/nullptr.
		R_INT, // Just write the enum as an integer
		INDEX_NAME = -1, // Get the enum's name from an Index() call
	};
	// A small buffer allocated by the caller in case you cannot return a raw const char*, completely optional to fill out.
	typedef va_buf<32,char> str_t;
	// Search from [str,end( string find matching enum.
	virtual bool String( enum_t& e, const char* str, const char* end ) const;
	// Enum value to string, type is an ES_* value.
	virtual const char* Enum( enum_t e, int type = R_THROW, str_t& buf = str_t() ) const;
	// Get all values, iterate with an index of 0 up until it returns false. INDEX_NAME returns the enum identifier.
	virtual const char* Index( int index, enum_t& e, str_t& buf = str_t() ) const = 0;
	// Return a separator char if we're a flags based enum, 0 otherwise
	virtual char Flags() const = 0;

	// Throws std::exception on failure.
	enum_t Parse( const char* str ) const;
	// Returns false on failure (value is not guaranteed to be left untouched), writes value on success.
	bool Parse( const char* str, enum_t* e ) const;
	// Returns default value on failure.
	enum_t Parse( const char* str, enum_t def ) const;

	// Write out the enum as a string.
	bool Render( enum_t e, char* buf, size_t len, int type = R_THROW ) const;

	template< unsigned L >
	inline bool Render( enum_t e, char (&buf)[L], int type = R_THROW ) const
	{
		return Render( e, buf, L, type );
	}

	template< unsigned L >
	inline char* Render( enum_t e, int type = R_THROW, char (&buf)[L] = va_buf<L,char>() ) const
	{
		return Render( e, buf, sizeof(buf), type ) ? buf : false; 
	}

private:
	// Private implementation
	enum_t _ParseEnum( const char* str ) const;
	bool _ParseEnum( const char* str, enum_t* e ) const;
	enum_t _ParseEnum( const char* str, enum_t def ) const;
	bool _RenderEnum( enum_t e, char* buf, size_t len, int type ) const;
	enum_t _ParseFlags( const char* str ) const;
	bool _ParseFlags( const char* str, enum_t* e ) const;
	enum_t _ParseFlags( const char* str, enum_t def ) const;
	bool _RenderFlags( enum_t e, char* buf, size_t len, int type ) const;
};

// Need this so I can cast int to bool transparently
template< typename T > inline T enum_cast( int e ) { return static_cast<T>(e); }
template<> inline bool enum_cast<bool>( int e ) { return e!=0; }


// Static enums
template< typename C >
struct _enum_t
{
	struct pair_t
	{
		int value;
		const C* str;
	};
	typedef const pair_t* const_iterator;
	const C* id;
	unsigned int flags;
	const_iterator list;
};
typedef _enum_t<char> enum_t;

class ES_enum_char_t : public CEnumBase
{
public:
	ES_enum_char_t( const tools::enum_t& en ) : en(en) { }
	virtual bool String( enum_t& e, const char* str, const char* end ) const;
	virtual const char* Enum( enum_t e, int type, str_t& buf ) const;
	virtual const char* Index( int index, enum_t& e, str_t& buf ) const;
	virtual char Flags() const;
	const tools::enum_t& en;
};

}

// Sharing access with this template
template< typename E > const tools::CEnumBase& EnumString();

// Macros to make declaring static enums easy
#define ENUMSTREX( IDENT, NAME, FLAGS ) extern const tools::enum_t::pair_t IDENT##x []; \
	static const tools::enum_t IDENT = { NAME, FLAGS, IDENT##x }; \
	static const tools::enum_t::pair_t IDENT##x [] =
#define ENUMDECL( ENUM ) extern const tools::enum_t ENUM##_enum;
#define ENUMSTRING( ENUM ) ENUMSTREX( ENUM##_enum, #ENUM, 0 )
#define ENUMFLAGS( ENUM ) ENUMSTREX( ENUM##_enum, #ENUM, 1 )

// Export an enum defined previously to be accessible with EnumString<enum>
// CAN ONLY BE USED in the global namespace!
#define ENUMEXPORT( ENUM ) template<> const tools::CEnumBase& EnumString<ENUM>();
#define ENUMEXPDEF( ENUM ) template<> const tools::CEnumBase& EnumString<ENUM>() { static const tools::ES_enum_char_t es(ENUM##_enum); return es; }

// Enum for bools
ENUMDECL( bool );
ENUMEXPORT( bool );

#endif // !HGUARD_LIBEX_TOOLS_ENUMSTRING
