#ifndef HGUARD_LIBEX_COLOR_NAMES
#define HGUARD_LIBEX_COLOR_NAMES

//----------------------------------------------------------------
// Color: HTML Color names
//----------------------------------------------------------------
// Because I can

namespace color
{
enum name
{
	Error				= 1,

	// Red color names
	IndianRed			= 0xCD5C5C,
	LightCoral			= 0xF08080,
	Salmon				= 0xFA8072,
	DarkSalmon			= 0xE9967A,
	LightSalmon			= 0xFFA07A,
	Crimson				= 0xDC143C,
	Red					= 0xFF0000,
	FireBrick			= 0xB22222,
	DarkRed				= 0x8B0000,

	// Pink color names
	Pink				= 0xFFC0CB,
	LightPink			= 0xFFB6C1,
	HotPink				= 0xFF69B4,
	DeepPink			= 0xFF1493,
	MediumVioletRed		= 0xC71585,
	PaleVioletRed		= 0xDB7093,

	// Orange color names
	Coral				= 0xFF7F50,
	Tomato				= 0xFF6347,
	OrangeRed			= 0xFF4500,
	DarkOrange			= 0xFF8C00,
	Orange				= 0xFFA500,

	// Yellow color names
	Gold				= 0xFFD700,
	Yellow				= 0xFFFF00,
	LightYellow			= 0xFFFFE0,
	LemonChiffon		= 0xFFFACD,
	LightGoldenrodYellow = 0xFAFAD2,
	PapayaWhip			= 0xFFEFD5,
	Moccasin			= 0xFFE4B5,
	PeachPuff			= 0xFFDAB9,
	PaleGoldenrod		= 0xEEE8AA,
	Khaki				= 0xF0E68C,
	DarkKhaki			= 0xBDB76B,

	// Purple color names
	Lavender			= 0xE6E6FA,
	Thistle				= 0xD8BFD8,
	Plum				= 0xDDA0DD,
	Violet				= 0xEE82EE,
	Orchid				= 0xDA70D6,
	Fuchsia				= 0xFF00FF,
	Magenta				= 0xFF00FF,
	MediumOrchid		= 0xBA55D3,
	MediumPurple		= 0x9370DB,
	Amethyst			= 0x9966CC,
	BlueViolet			= 0x8A2BE2,
	DarkViolet			= 0x9400D3,
	DarkOrchid			= 0x9932CC,
	DarkMagenta			= 0x8B008B,
	Purple				= 0x800080,
	Indigo				= 0x4B0082,

	// Green color names
	GreenYellow			= 0xADFF2F,
	Chartreuse			= 0x7FFF00,
	LawnGreen			= 0x7CFC00,
	Lime				= 0x00FF00,
	LimeGreen			= 0x32CD32,
	PaleGreen			= 0x98FB98,
	LightGreen			= 0x90EE90,
	MediumSpringGreen	= 0x00FA9A,
	SpringGreen			= 0x00FF7F,
	MediumSeaGreen		= 0x3CB371,
	SeaGreen			= 0x2E8B57,
	ForestGreen			= 0x228B22,
	Green				= 0x008000,
	DarkGreen			= 0x006400,
	YellowGreen			= 0x9ACD32,
	OliveDrab			= 0x6B8E23,
	Olive				= 0x808000,
	DarkOliveGreen		= 0x556B2F,
	MediumAquamarine	= 0x66CDAA,
	DarkSeaGreen		= 0x8FBC8F,
	LightSeaGreen		= 0x20B2AA,
	DarkCyan			= 0x008B8B,
	Teal				= 0x008080,

	// Blue color names
	Aqua				= 0x00FFFF,
	Cyan				= 0x00FFFF,
	LightCyan			= 0xE0FFFF,
	PaleTurquoise		= 0xAFEEEE,
	Aquamarine			= 0x7FFFD4,
	Turquoise			= 0x40E0D0,
	MediumTurquoise		= 0x48D1CC,
	DarkTurquoise		= 0x00CED1,
	CadetBlue			= 0x5F9EA0,
	SteelBlue			= 0x4682B4,
	LightSteelBlue		= 0xB0C4DE,
	PowderBlue			= 0xB0E0E6,
	LightBlue			= 0xADD8E6,
	SkyBlue				= 0x87CEEB,
	LightSkyBlue		= 0x87CEFA,
	DeepSkyBlue			= 0x00BFFF,
	DodgerBlue			= 0x1E90FF,
	CornflowerBlue		= 0x6495ED,
	SlateBlue			= 0x6A5ACD,
	DarkSlateBlue		= 0x483D8B,
	MediumSlateBlue		= 0x7B68EE,
	RoyalBlue			= 0x4169E1,
	Blue				= 0x0000FF,
	MediumBlue			= 0x0000CD,
	DarkBlue			= 0x00008B,
	Navy				= 0x000080,
	MidnightBlue		= 0x191970,

	// Brown color names
	Cornsilk			= 0xFFF8DC,
	BlanchedAlmond		= 0xFFEBCD,
	Bisque				= 0xFFE4C4,
	NavajoWhite			= 0xFFDEAD,
	Wheat				= 0xF5DEB3,
	BurlyBrown			= 0xDEB887,
	Tan					= 0xD2B48C,
	RosyBrown			= 0xBC8F8F,
	SandyBrown			= 0xF4A460,
	Goldenrod			= 0xDAA520,
	DarkGoldenrod		= 0xB8860B,
	Peru				= 0xCD853F,
	Chocolate			= 0xD2691E,
	SaddleBrown			= 0x8B4513,
	Sienna				= 0xA0522D,
	Brown				= 0xA52A2A,
	Maroon				= 0x800000,

	// White color names
	White				= 0xFFFFFF,
	Snow				= 0xFFFAFA,
	HoneyDew			= 0xF0FFF0,
	MintCream			= 0xF5FFFA,
	Azure				= 0xF0FFFF,
	AliceBlue			= 0xF0F8FF,
	GhostWhite			= 0xF8F8FF,
	WhiteSmoke			= 0xF5F5F5,
	Seashell			= 0xFFF5EE,
	Beige				= 0xF5F5DC,
	OldLace				= 0xFDF5E6,
	FloralWhite			= 0xFFFAF0,
	Ivory				= 0xFFFFF0,
	AntiqueWhite		= 0xFAEBD7,
	Linen				= 0xFAF0E6,
	LavenderBlush		= 0xFFF0F5,
	MistyRose			= 0xFFE4E1,

	// Grey color names
	Gainsboro			= 0xDCDCDC,
	LightGrey			= 0xD3D3D3,
	Silver				= 0xC0C0C0,
	DarkGrey			= 0xA9A9A9,
	Grey				= 0x808080,
	DimGrey				= 0x696969,
	LightSlateGrey		= 0x778899,
	SlateGrey			= 0x708090,
	DarkSlateGrey		= 0x2F4F4F,
	Black				= 0x000000,
};

name Lookup( const char* str, const char* end = nullptr );

}

#endif // HGUARD_LIBEX_COLOR_NAMES
