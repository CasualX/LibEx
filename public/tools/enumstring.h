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

namespace tools
{
// Public interface to access the conversion
INTERFACE IEnumString
{
public:
	// Throws std::exception on failure
	virtual int Parse( const char* str ) const = 0;
	// Returns false on failure (value is not guarantied to be left untouched), writes value on success
	virtual bool Parse( const char* str, int* value ) const = 0;
	// Returns default value on failure
	virtual int Parse( const char* str, int def ) const = 0;

	// Type values; 0 = Throw exception, 1 = return false, 2 = write integer
	virtual bool Render( int value, char* buf, unsigned len, int type ) const = 0;
	template< unsigned L > inline bool Render( int value, char (&buf)[L], int type ) { return Render( value, buf, L, type ); }

	// Get all values, iterate with an index of 0 until it returns false
	// const IEnumString& es = ...;
	// int value;
	// char buf[256];
	// for ( int i = 0; es.Lookup( i, value, buf ); ++i ) ...;
	virtual bool Lookup( int index, int& value, char* buf, unsigned len ) const = 0;
	template< unsigned L > inline bool Lookup( int index, int& value, char (&buf)[L] ) const { return Lookup( index, value, buf, L ); }
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

	inline const pair_t* lookup( const C* name, C sep ) const
	{
		for ( const_iterator it = list; it->str; ++it )
		{
			unsigned int i;
			for ( i = 0; it->str[i]; ++i )
			{
				if ( name[i]!=it->str[i] )
					goto next;
			}
			// Match of EOS or separator
			if ( !name[i] || name[i]==sep )
				return it;
next:;
		}
		return nullptr;
	}
	inline const pair_t* lookup( int value ) const
	{
		for ( const_iterator it = list; it->str; ++it )
		{
			if ( it->value==value )
				return it;
		}
		return nullptr;
	}
};
typedef _enum_t<char> enum_t;

class ES_enum_char_t : public IEnumString
{
public:
	ES_enum_char_t( const enum_t& en ) : en(en) { }
	virtual int Parse( const char* str ) const;
	virtual bool Parse( const char* str, int* value ) const;
	virtual int Parse( const char* str, int def ) const;
	virtual bool Render( int value, char* buf, unsigned len, int type ) const;
	virtual bool Lookup( int index, int& value, char* buf, unsigned len ) const;
	const enum_t& en;
};

}

// Sharing access with this template
template< typename E > struct EnumString;

// Macros to make declaring static enums easy
#define ENUMSTREX( IDENT, NAME, FLAGS ) extern const tools::enum_t::pair_t IDENT##x []; \
	static const tools::enum_t IDENT = { NAME, FLAGS, IDENT##x }; \
	static const tools::enum_t::pair_t IDENT##x [] =
#define ENUMDECL( ENUM ) extern const tools::enum_t ENUM##_enum;
#define ENUMSTRING( ENUM ) ENUMSTREX( ENUM##_enum, #ENUM, 0 )
#define ENUMFLAGS( ENUM ) ENUMSTREX( ENUM##_enum, #ENUM, 1 )

// Export an enum defined previously to be accessible with EnumString<enum>
// CAN ONLY BE USED in the global namespace!
#define ENUMEXPORT( ENUM ) template<> struct EnumString<ENUM> { \
	static inline const tools::IEnumString& Inst() { return _es; } \
	static inline ENUM Parse( const char* str ) { return tools::enum_cast<ENUM>( _es.Parse(str) ); } \
	static inline bool Parse( const char* str, ENUM* value ) { return _es.Parse( str, (int*)value ); } \
	static inline ENUM Parse( const char* str, ENUM def ) { return tools::enum_cast<ENUM>( _es.Parse( str, def ) ); } \
	static inline bool Render( ENUM value, char* buf, unsigned len, int type = 0 ) { return _es.Render( value, buf, len, type ); } \
	template< unsigned L > static inline bool Render( ENUM value, char (&buf)[L], int type = 0 ) { return _es.Render( value, buf, L, type ); } \
	static inline bool Lookup( int index, ENUM& value, char* buf, unsigned len ) { return _es.Lookup( index, (int&)value, buf, len ); } \
	template< unsigned L > static inline bool Lookup( int index, ENUM& value, char (&buf)[L] ) { return _es.Lookup( index, (int&)value, buf, L ); }\
private:static const tools::ES_enum_char_t _es; \
}
#define ENUMEXPDEF( ENUM ) const tools::ES_enum_char_t EnumString<ENUM>::_es(ENUM##_enum)

// Enum for bools
ENUMDECL( bool );
ENUMEXPORT( bool );

#endif // !HGUARD_LIBEX_TOOLS_ENUMSTRING
