#include "names.h"
#include "../hash/hash.h"

namespace color
{
NOINLINE name Lookup( const char* str, const char* end )
{
	typedef hash::hash_t hash_t;
	struct entry_t
	{
		hash_t::type id;
		name color;
	};
	enum {
		key1=0x9B72A81C,
		key2=0x8F92BC,
		base=__COUNTER__,
	};
#define ENC(n) ((n^key1)-key2)
#define DEC(n) ((n+key2)^key1)
#define PUT(id,clr) { ENC(id), static_cast<color::name>(ENC(clr)) }
	static const entry_t db[] =
	{
		PUT(0x00f66133,0x32CD32),PUT(0x03356e46,0xFF4500),PUT(0x03cf6ca0,0x66CDAA),PUT(0x05ae31d3,0xDEB887),
		PUT(0x0730c375,0x8FBC8F),PUT(0x07f14046,0xF0FFF0),PUT(0x0a2ab9dc,0xF0FFFF),PUT(0x0a4698be,0x008000),
		PUT(0x0a52b645,0xFAF0E6),PUT(0x0a6d0774,0x3CB371),PUT(0x0a76ca49,0xF5F5DC),PUT(0x0a7a8f62,0x000000),
		PUT(0x0a7df1a3,0xA52A2A),PUT(0x0a8c5296,0xFF7F50),PUT(0x0ab531de,0xFFFFF0),PUT(0x0acfb11c,0x808000),
		PUT(0x0b15bf25,0xF0E68C),PUT(0x0b66c342,0xFFFFFF),PUT(0x0b66f7ea,0xF5DEB3),PUT(0x0b8761fe,0xD2B48C),
		PUT(0x0b878b76,0xFF0000),PUT(0x102a38ea,0xFF69B4),PUT(0x155a9557,0xDAA520),PUT(0x15710813,0xCD5C5C),
		PUT(0x21cc1817,0x20B2AA),PUT(0x231e29fd,0x7FFFD4),PUT(0x298a358a,0x7CFC00),PUT(0x348e1ca7,0x00008B),
		PUT(0x348e6b4c,0x008B8B),PUT(0x3490a910,0xA9A9A9),PUT(0x34d83ca7,0xFFB6C1),PUT(0x34ddd2c5,0xADD8E6),
		PUT(0x34de006e,0xE0FFFF),PUT(0x34e07032,0xD3D3D3),PUT(0x37e6103a,0x9ACD32),PUT(0x387ed792,0x00FA9A),
		PUT(0x3d06832d,0xFF1493),PUT(0x3ed6bde7,0xDCDCDC),PUT(0x3f857490,0xFF00FF),PUT(0x46ba4e8b,0xB8860B),
		PUT(0x4c12af5d,0x7FFF00),PUT(0x4cc76e3f,0x48D1CC),PUT(0x5333b9f2,0x4169E1),PUT(0x56527b8c,0xFFF5EE),
		PUT(0x574c5c95,0x800000),PUT(0x57b25629,0x2E8B57),PUT(0x59cd66c9,0x7B68EE),PUT(0x5a2e495c,0xFFE4C4),
		PUT(0x5f6b7a69,0x6B8E23),PUT(0x5fa84fdb,0x556B2F),PUT(0x5fad6687,0x4B0082),PUT(0x5ff4a8f8,0x8B4513),
		PUT(0x61307ecc,0x8B008B),PUT(0x61eee1a6,0x0000CD),PUT(0x622f6594,0xFFE4E1),PUT(0x65bfa63e,0xDA70D6),
		PUT(0x65c0d635,0xFFA500),PUT(0x67ca1bd6,0x9370DB),PUT(0x67cb78ae,0x00BFFF),PUT(0x688e1169,0xFF6347),
		PUT(0x6e33504b,0xE9967A),PUT(0x6fd0454b,0x800080),PUT(0x70ff9c03,0xF5FFFA),PUT(0x730587fe,0xD8BFD8),
		PUT(0x745f96d4,0x9400D3),PUT(0x7621a988,0xEE82EE),PUT(0x793433a7,0x191970),PUT(0x7962195f,0x2F4F4F),
		PUT(0x796508a8,0x483D8B),PUT(0x7c6dc8c1,0x00FFFF),PUT(0x7c6ebecc,0x808080),PUT(0x7c6ec8e5,0xFFD700),
		PUT(0x7c6f1ce8,0x00FF00),PUT(0x7c70547b,0x0000FF),PUT(0x7c712610,0x00FFFF),PUT(0x7c727805,0x000080),
		PUT(0x7c73b179,0x008080),PUT(0x7c75bed7,0xCD853F),PUT(0x7c75dca1,0xDDA0DD),PUT(0x7c75f259,0xFFC0CB),
		PUT(0x7c798200,0xFFFAFA),PUT(0x80f94117,0xFA8072),PUT(0x8170dee2,0xF4A460),PUT(0x81812012,0xC0C0C0),
		PUT(0x8181e9db,0xA0522D),PUT(0x831edd3f,0xFFDEAD),PUT(0x84306826,0x98FB98),PUT(0x8591f1de,0x00CED1),
		PUT(0x859d2c01,0xFFFF00),PUT(0x86fb4f46,0xFF00FF),PUT(0x88b80969,0xFF8C00),PUT(0x88bb5a62,0x9932CC),
		PUT(0x8c6eb963,0xBA55D3),PUT(0x8c750f03,0xFFFACD),PUT(0x9011e8f1,0xFAEBD7),PUT(0x9131faf4,0xBC8F8F),
		PUT(0x922556ec,0x5F9EA0),PUT(0x94fc49aa,0x8B0000),PUT(0x96354d7b,0xFFEFD5),PUT(0x972dd1e6,0xFFF0F5),
		PUT(0x97cf902d,0xFAFAD2),PUT(0x99c72ecf,0xEEE8AA),PUT(0x9b1e9efd,0xF5F5F5),PUT(0x9e8deb66,0xC71585),
		PUT(0x9f7e5226,0xE6E6FA),PUT(0xa38ebf1f,0xFFDAB9),PUT(0xa405332f,0x00FF7F),PUT(0xa4a8db74,0x6A5ACD),
		PUT(0xa4af1403,0x708090),PUT(0xa649c8ac,0x696969),PUT(0xa87d8705,0xF8F8FF),PUT(0xa9fbf87f,0xFFFFE0),
		PUT(0xae98ef52,0xFFE4B5),PUT(0xb1b96518,0xFFFAF0),PUT(0xb4b9561a,0x87CEEB),PUT(0xb8a8b5ae,0x9966CC),
		PUT(0xb932d96e,0xB0C4DE),PUT(0xba7d28f6,0x8A2BE2),PUT(0xbc166808,0xFFF8DC),PUT(0xbfcb5bba,0xADFF2F),
		PUT(0xc0bcade9,0xFFA07A),PUT(0xc5d8d579,0xBDB76B),PUT(0xc6a5cae2,0x006400),PUT(0xc8495cb1,0xD2691E),
		PUT(0xc8518490,0x4682B4),PUT(0xce16688e,0x6495ED),PUT(0xd0abdc68,0xF08080),PUT(0xd0ee7580,0x90EE90),
		PUT(0xd1d54182,0x40E0D0),PUT(0xd5678864,0x87CEFA),PUT(0xd8671d2c,0xB22222),PUT(0xd89aa2fd,0x778899),
		PUT(0xdafa03eb,0xFFEBCD),PUT(0xdb1d7a19,0xF0F8FF),PUT(0xe8c8a527,0x228B22),PUT(0xea7c4c82,0xDC143C),
		PUT(0xec83131a,0xAFEEEE),PUT(0xf01cc744,0x1E90FF),PUT(0xf4beede0,0xB0E0E6),PUT(0xfb1fc5c3,0xDB7093),
		PUT(0xfcbb7629,0xFDF5E6),
	};
#undef PUT
	if ( !end ) end = str + strlen(str);
	// Lowercase hash
	hash_t::type h = hash_t::init;
	for ( const char* it = str; it<end; ++it )
	{
		unsigned char c = *it;
		if ( c>='A' && c<='Z' )
			c = c-'A'+'a';
		h = hash_t::algo( h, c );
	}
	// Quick lookup in table
	typedef const entry_t* iterator;
	for ( iterator it = db, end = it+sizeof(db)/sizeof(db[0]); it!=end; ++it )
	{
		if ( DEC(it->id)==h )
		{
			name n = static_cast<name>(DEC(it->color));
			return n;
		}
	}
	// Special value indicates not found
	return Error;
#undef ENC
#undef DEC
}
}