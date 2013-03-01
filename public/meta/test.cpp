
#include "strcrypt.h"
#include "declpfn.h"
#include "encptr.h"
#include "rndheader.h"
#include "../pelite/image.h"

namespace meta
{


template< typename T > int tstrcmp( const T* s1, const T* s2 );
template<> int tstrcmp<char>( const char* s1, const char* s2 ) { return strcmp( s1, s2 ); }
template<> int tstrcmp<wchar_t>( const wchar_t* s1, const wchar_t* s2 ) { return wcscmp( s1, s2 ); }

template< typename T, unsigned L >
int TestStrcryptInstance( const T (&str)[L] )
{
	const T* pre = (sizeof(T)==1)?(const T*)STRCRYPT_PREFIX:(const T*)WSTRCRYPT_PREFIX;
	T buf[128];
	T final[128];
	((__int64*)buf)[0] = *(const __int64*)pre;
	memcpy( buf+8/sizeof(T), str, (L+1)*sizeof(T) );

	unsigned key = static_cast<unsigned>(__rdtsc());
	strcrypt_t::_global = L;
	strencrypt( (strcrypt_t*)buf, key );
	((strcrypted_t<T>*)buf)->decrypt( final );

	return tstrcmp( str, final )!=0;
}
int TestStrcrypt()
{
	int fail = 0;
	
	fail += TestStrcryptInstance( "abcdef" );
	fail += TestStrcryptInstance( "I'm a very large string or so I think!" );
	fail += TestStrcryptInstance( L"wide strings work too!" );
	fail += TestStrcryptInstance( L"More Wide Strings To Test YaY!" );
	strcrypt_t::_global = 0;

	fail += strcmp( STRDECRYPT("This is a string!"), "This is a string!" )!=0;
	fail += wcscmp( WSTRDECRYPT(L"Wide string!"), L"Wide string!" )!=0;

	for ( int i = 0; i<5; ++i )
	{
		_ReadWriteBarrier();
		fail += strcmp( "Testing full auto :>", STRCRYPT("Testing full auto :>").decrypt<0xe117402e>() )!=0;
		_ReadWriteBarrier();
		fail += strcmp( "More testing because crazy", STRDECRYPT("More testing because crazy") )!=0;
		_ReadWriteBarrier();
	}

	return fail;
}
int TestEncPtr( bool rec = true )
{
	int fail = 0;
	if ( rec )
	{
		fail += maskfnxref<827,int (*)( bool ),TestEncPtr>()( false );
		fail += maskxref<9212>("Hello there!")[0]!='H';
	}
	return fail;
}
int TestRndHeader()
{
	int fail = 0;
	fail += (RANDOMINT==RANDOMINT);
	fail += (RANDOMINT==RANDOMINT);
	fail += (RANDOMINT==RANDOMINT);
	fail += (RANDOMFLOAT==RANDOMFLOAT);
	fail += (RANDOMFLOAT==RANDOMFLOAT);
	fail += (RANDOMFLOAT==RANDOMFLOAT);
	_ReadWriteBarrier();
	printf( "%f,%x,%x,%x,%x,%x,%x,%x,%x\n", RANDOMFLOAT, RANDOMINT, RANDOMINT, RANDOMINT, RANDOMINT, RANDOMINT, RANDOMINT, RANDOMINT, RANDOMINT );
	return fail;
}

NOINLINE bool UnitTest()
{
	int fail = 0;
	fail += TestStrcrypt();
	fail += TestEncPtr();
	fail += TestRndHeader();


	return fail==0;
}

}
