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
//
// OPTIMIZATION TODOs:
// * ENUMSTRING() and ENUMFLAGS() macros will dynamically create their static CEnumDefault instances.
// * CEnumBase::Parse() and CEnumBase::Render() must dynamically check for flags. It would be nice if templated use could autodetect this.
//

#include <exception>

#include "../libex.h"
#include "printf.h"

namespace tools
{
// Instruct the Render() calls to handle errors
enum {
	ES_THROW = 0,	// Throw exception if unknown enum.
	ES_FALSE = (1<<0),	// Return false/nullptr.
	ES_INT = (1<<1),	// Just read/write the enum as an integer. throw if not an integer unless ES_FALSE.
	ES_ELLIPSIS = (1<<2), // Add ellipsis if not fitting buffer, otherwise go to fail scenario.

	// Get the enum's name from an Index() call.
	ES_INDEX_NAME = -1,
};

class CEnumBase
{
public:
	// For portability?
	typedef int enum_t;
	// A small buffer allocated by the caller in case you cannot return a raw const char*, completely optional to fill out.
	typedef va_buf<64,char> temp_t;
	// Search from [str,end( string find matching enum.
	virtual bool String( enum_t& e, const char* str, const char* end ) const;
	// Enum value to string, returns nullptr if not found.
	virtual const char* Enum( enum_t e, temp_t& buf = temp_t() ) const;
	// Get all values, iterate with an index of 0 up until it returns false. INDEX_NAME returns the enum identifier.
	virtual const char* Index( int index, enum_t& e, temp_t& buf = temp_t() ) const = 0;
	// Get the name of this enum.
	inline const char* Name( temp_t& buf = temp_t() ) const { enum_t e; return Index( ES_INDEX_NAME, e, buf ); }
	// Return a separator char if we're a flags based enum, 0 otherwise.
	virtual char Flags() const = 0;

	// Parse a string, param e is guaranteed to be left untouched on failure.
	bool Parse( const char* str, enum_t& e, int type = ES_THROW ) const;
	// Parse a string, throw exception on failure.
	inline enum_t Parse( const char* str ) const { enum_t e; Parse( str, e, ES_THROW ); return e; }

	// Write out the enum as a string. type is an ES_* value.
	char* Render( enum_t e, char* buf, size_t len, int type = ES_THROW ) const;
	// Write out the enum as a string. type is an ES_* value.
	template< unsigned L >
	inline char* Render( enum_t e, int type = ES_THROW, char (&temp)[L] = va_buf<L,char>() ) const {
		return Render( e, temp, sizeof(temp), type );
	}

private:
	// Private implementation.
	bool _ParseEnum( const char* str, enum_t& e, int type ) const;
	char* _RenderEnum( enum_t e, char* buf, size_t len, int type ) const;
	bool _ParseFlags( const char* str, enum_t& e, int type ) const;
	char* _RenderFlags( enum_t e, char* buf, size_t len, int type ) const;
};

// Need this so I can cast int to bool transparently without warnings.
template< typename E > inline E enum_cast( int e ) {
	return static_cast<E>(e);
}
template<> inline bool enum_cast<bool>( int e ) {
	return e!=0;
}

// Ultimate convenience casting string to enum, throws std::exception on failure.
template< typename E >
inline E enum_cast( const char* str, CEnumBase::enum_t value = 0, int type = ES_THROW ) {
	auto& es = EnumStringFactory<E>();
	es.Parse( str, value, type );
	return enum_cast<E>( value );
}
// Ultimate convenience casting enum to string.
template< size_t L, typename E >
inline char* enum_cast( E e, int type = ES_THROW, char (&temp)[L] = va_buf<L,char>() ) {
	auto& es = EnumStringFactory<E>();
	return es.Render( e, temp, sizeof(temp), type );
}



// Default wrapper used by ENUMSTRING() and ENUMFLAGS() macros.
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
		: name(name), pairs(list), count(N), sep(sep) {
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
template< typename E > const tools::CEnumBase& EnumStringFactory();

// Export an enum defined previously to be accessible with EnumStringFactory<enum>
// CAN ONLY BE USED in the global namespace!
#define ENUMEXPORT( ENUM ) template<> const tools::CEnumBase& EnumStringFactory<ENUM>();
// If you already have an instance to return, use this macro.
#define ENUMEXPDEF( ENUM, INST ) template<> const tools::CEnumBase& EnumStringFactory<ENUM>() { return INST; }
// Implement the conversion with the provided default implementation.
// Example: ENUMSTRING( bool, { "true", 1 }, { "false", 0 } );
#define ENUMSTRING( ENUM, ... ) _ENUMSTRING1( ENUM, 0, __LINE__, e, __COUNTER__, __VA_ARGS__ )
// Implement the conversion of a flags based enum with the provided default implementation.
#define ENUMFLAGS( ENUM, ... ) _ENUMSTRING1( ENUM, ',', __LINE__, f, __COUNTER__, __VA_ARGS__ )

// Implementation wrapper, objects need to be global so they are not static constructed...
#define _ENUMSTRING1( ENUM, SEP, UID1, UID2, UID3, ... ) _ENUMSTRING2( ENUM, SEP, UID1, UID2, UID3, __VA_ARGS__ )
#define _ENUMSTRING2( ENUM, SEP, UID1, UID2, UID3, ... ) _ENUMSTRING3( ENUM, SEP, UID1##UID2##UID3, __VA_ARGS__ )
#define _ENUMSTRING3( ENUM, SEP, UID, ... ) OPTGLOBAL const tools::CEnumDefault::pair_t _EnumString_pairs_##UID[] = { __VA_ARGS__ }; \
	OPTGLOBAL const tools::CEnumDefault _EnumString_obj_##UID( #ENUM, SEP, _EnumString_pairs_##UID ); \
	ENUMEXPDEF( ENUM, _EnumString_obj_##UID )

// Enum for bools
ENUMEXPORT( bool );

#endif // !HGUARD_LIBEX_TOOLS_ENUMSTRING
