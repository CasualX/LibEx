#ifndef HGUARD_LIBEX_COLOR_ARGB
#define HGUARD_LIBEX_COLOR_ARGB
#pragma once

//------------------------------------------------
// Class: color::argb
//------------------------------------------------
// Purpose:
//  Represents a color with a red, green, blue and alpha component, packed in 4 bytes ( 0xAARRGGBB ).

#include "names.h"

namespace color
{	
class hsv;

class argb
{
public:
	// Constructors
	argb() : _raw(0) {}
	argb( const argb& c ) : _raw(c._raw) {}
	// Args are [0, 255]
	argb( int r, int g, int b, int a = 255 ) : _raw( ((a&0xFF)<<24) + ((r&0xFF)<<16) + ((g&0xFF)<<8) + (b&0xFF) ) {}
	argb( name c ) : _raw( static_cast<unsigned int>(c)|(0xFF<<24) ) {}
	argb( unsigned int c ) : _raw(c) {}
	argb( const hsv& c );
	// Accessors
	inline void alpha( unsigned char a )	{ _raw = (_raw&0x00FFFFFF)|(a<<24); }
	inline void red( unsigned char r )		{ _raw = (_raw&0xFF00FFFF)|(r<<16); }
	inline void green( unsigned char g )	{ _raw = (_raw&0xFFFF00FF)|(g<<8); }
	inline void blue( unsigned char b )		{ _raw = (_raw&0xFFFFFF00)|(b<<0); }
	inline unsigned char alpha() const		{ return (_raw>>24)&0xFF; }
	inline unsigned char red() const		{ return (_raw>>16)&0xFF; }
	inline unsigned char green() const		{ return (_raw>>8)&0xFF; }
	inline unsigned char blue() const		{ return (_raw>>0)&0xFF; }
	// Formatting
	bool parse( const char* str );
	bool parse( const wchar_t* str );
	void format( char* buf ) const;
	void format( wchar_t* buf ) const;
	// Get a color_name from this color
	inline color::name name() const { return static_cast<color::name>( _raw&0x00FFFFFF ); }
	// Get the raw value of this color as an int
	inline unsigned int raw() const { return _raw; }
	// Mix two colors evenly
	void mix( argb c );
	// Mix two colors, pct is [0, 1] and specifies how much to take from the other color
	void mix( argb c, float pct );
	// Operators
	inline bool operator== ( argb c ) const { return _raw==c._raw; }
	inline bool operator!= ( argb c ) const { return _raw!=c._raw; }
private:
	unsigned int _raw;
};

}

#endif // HGUARD_LIBEX_COLOR_ARGB
