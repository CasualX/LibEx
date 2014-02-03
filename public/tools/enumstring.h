#ifndef HGUARD_LIBEX_TOOLS_ENUMSTRING
#define HGUARD_LIBEX_TOOLS_ENUMSTRING
#pragma once

//----------------------------------------------------------------
// Tools: EnumString
//----------------------------------------------------------------
// Provides enum<->string conversions.
// Look this isn't perfect but ugh...
//
// WARNING! Assumes the enum is equivalent of an int.
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
	typedef va_buf<32,char> temp_t;
	// Search from [str,end( string find matching enum.
	virtual bool String( enum_t& e, const char* str, const char* end ) const;
	// Enum value to string, returns nullptr if not found.
	virtual const char* Enum( enum_t e, temp_t& buf = temp_t() ) const;
	// Get all values, iterate with an index of 0 up until it returns false. INDEX_NAME returns the enum identifier.
	virtual const char* Index( int index, enum_t& e, temp_t& buf = temp_t() ) const = 0;
	// Get the name of this enum.
	inline const char* Name() const { enum_t e; return Index( INDEX_NAME, e ); }
	// Return a separator char if we're a flags based enum, 0 otherwise
	virtual char Flags() const = 0;

	// Throws std::exception on failure.
	enum_t Parse( const char* str ) const;
	// Returns false on failure (value is not guaranteed to be left untouched), writes value on success.
	bool Parse( const char* str, enum_t* e ) const;
	// Returns default value on failure.
	enum_t Parse( const char* str, enum_t def ) const;

	// Write out the enum as a string. type is an R_* value.
	bool Render( enum_t e, char* buf, size_t len, int type = R_THROW ) const;

	template< unsigned L >
	inline bool Render( enum_t e, char (&buf)[L], int type = R_THROW ) const
	{
		return Render( e, buf, L, type );
	}

	template< unsigned L >
	inline char* Render( enum_t e, int type = R_THROW, char (&buf)[L] = va_buf<L,char>() ) const
	{
		return Render( e, buf, sizeof(buf), type ) ? static_cast<char*>(buf) : nullptr;
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



// Wrapper for convenience
class CEnumDefault : public CEnumBase
{
public:
	struct pair_t
	{
		enum_t e;
		const char* s;
	};

	template< size_t N >
	inline CEnumDefault( const char* name, char sep, const pair_t (&list)[N] )
		: name(name), pairs(list), count(N), sep(sep)
	{
	}

	virtual const char* Index( int index, enum_t& e, temp_t& buf = temp_t() ) const;
	virtual char Flags() const;

protected:
	const char* name;
	const pair_t* pairs;
	size_t count;
	char sep;
};

}

// Sharing access with this template
template< typename E > const tools::CEnumBase& EnumString();

// Export an enum defined previously to be accessible with EnumString<enum>
// CAN ONLY BE USED in the global namespace!
#define ENUMEXPORT( ENUM ) template<> const tools::CEnumBase& EnumString<ENUM>();
// If you already have an instance to return, use this macro.
#define ENUMEXPDEF( ENUM, INST ) template<> const tools::CEnumBase& EnumString<ENUM>() { return INST; }
// Implement the conversion with the provided default implementation.
// Example: ENUMSTRING( bool, { "true", 1 }, { "false", 0 } );
#define ENUMSTRING( ENUM, ... ) template<> const tools::CEnumBase& EnumString<ENUM>() { static const tools::CEnumDefault::pair_t pairs[] = { __VA_ARGS__ }; static const tools::CEnumDefault es( #ENUM, 0, pairs ); return es; }
// Implement the conversion of a flags based enum with the provided default implementation.
#define ENUMFLAGS( ENUM, ... ) template<> const tools::CEnumBase& EnumString<ENUM>() { static const tools::CEnumDefault::pair_t pairs[] = { __VA_ARGS__ }; static const tools::CEnumDefault es( #ENUM, ',', pairs ); return es; }

// Enum for bools
ENUMEXPORT( bool );

#endif // !HGUARD_LIBEX_TOOLS_ENUMSTRING
