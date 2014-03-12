
//----------------------------------------------------------------
// Tools: EnumString tests

#include "enumstring.h"

namespace tools
{
enum test_t
{
	TEST1 = 1,
	TEST2 = 2,
	TEST3,
};
enum fltest_t
{
	FLTEST1 = 1,
	FLTEST2 = 2,
	FLTEST3 = 4,
	FLTEST4 = 8,
};
}
ENUMEXPORT(tools::fltest_t);
ENUMSTRING( tools::test_t, { tools::TEST1, "test1" }, { tools::TEST2, "test2" }, { tools::TEST3, "test3" } );

namespace tools
{
int TestEnumStringToEnum( const CEnumBase& e, const char* str, CEnumBase::enum_t value, bool s )
{
	int fail = 0;
	CEnumBase::enum_t out;
	bool caught = !s;
	// Test ES_FALSE code path
	if ( e.Parse( str, out, ES_FALSE|ES_INT )!=s && ( !s || out==value ) )
		++fail;
	// Test ES_THROW code path
	try { e.Parse( str, out, ES_THROW|ES_INT ); }
	catch ( const std::exception& ) { caught = true; }
	if ( caught==s )
		++fail;
	return fail;
}
int TestEnumStringToString( const CEnumBase& e, const char* str, CEnumBase::enum_t value, bool s )
{
	int fail = 0;
	bool caught = !s;
	char* result;
	// Test ES_FALSE code path
	result = e.Render<64>( value, ES_FALSE|ES_INT );
	if ( result && !strcmp(result,str)!=s )
		++fail;
	// Test ES_THROW code path
	try { result = e.Render<64>( value, ES_THROW|ES_INT ); }
	catch ( const std::exception& ) { caught = true; }
	if ( caught==s )
		++fail;
	return fail;
}
int TestEnumString()
{
	int fail = 0;

	// Test functions for basic enums.
	{
		const CEnumBase& e = EnumStringFactory<test_t>();
		fail += !!strcmp( e.Name(), "tools::test_t" );

		// String->Enum
		// Conversion has to be exact, no extra spaces allowed
		fail += TestEnumStringToEnum( e, "blah", 0, false );
		fail += TestEnumStringToEnum( e, "test1", TEST1, true );
		fail += TestEnumStringToEnum( e, "test1 ", TEST1, false );
		fail += TestEnumStringToEnum( e, " test2", TEST2, false );
		fail += TestEnumStringToEnum( e, "test3", TEST3, true );
		fail += TestEnumStringToEnum( e, "2", TEST2, true );
		fail += TestEnumStringToEnum( e, " 1", TEST1, true ); // strtol 
		fail += TestEnumStringToEnum( e, "3 ", TEST3, false );

		// Enum->String
		fail += TestEnumStringToString( e, "test1", TEST1, true );
		fail += TestEnumStringToString( e, "test2", TEST3, false );
		fail += TestEnumStringToString( e, "8", 8, true );
	}

	// Test functions for flags enums.
	{
		const CEnumBase& e = EnumStringFactory<fltest_t>();
		fail += !!strcmp( e.Name(), "tools::fltest_t" );

		// String->Enum
		fail += TestEnumStringToEnum( e, "blah", 0, false );
		fail += TestEnumStringToEnum( e, "fl1, fl2, fl4", FLTEST1|FLTEST2|FLTEST4, true );
		fail += TestEnumStringToEnum( e, "fl1 ,  fl2 , fl4", FLTEST1|FLTEST2|FLTEST4, false );
		
		// Enum->String
		fail += TestEnumStringToString( e, "fl1,fl3,fl4", FLTEST1|FLTEST3|FLTEST4, true );
		fail += TestEnumStringToString( e, "fl1,fl2,fl3,fl4,0x1B0", 0x1BF, true );
	}

	return fail;
}
}

ENUMFLAGS( tools::fltest_t, { tools::FLTEST1, "fl1" }, { tools::FLTEST2, "fl2" }, { tools::FLTEST3, "fl3" }, { tools::FLTEST4, "fl4" } );

//----------------------------------------------------------------
// Tools: printf tests

#include "printf.h"

namespace tools
{
int TestPrintf()
{
	int fail = 0;

	va_buf<16,char> va( 0 );
	fail += !!strcmp( va, "" );

	// Just to test the overflow case...
	fail += va.printf( "%s%s", "12345678", "12345678" )!=sizeof(va) || !!strcmp( va, "123456781234567" );
	fail += va.printf( "%s%d%s", "abcdefgh", 12, "ijklmnop" )!=sizeof(va) || !!strcmp( va, "abcdefgh12ijklm" );

	// Formatters
	fail += va.format<int>( -58 )!=3 || !!strcmp( va, "-58" );
	fail += va.format<unsigned int>( -1 )!=10 || !!strcmp( va, "4294967295" );
	fail += va.format( 'a' )!=2 || !!strcmp( va, "97" );

	// Helper test
	fail += !!strcmp( va_printf<64>( "Hello %s!", "World" ), "Hello World!" );
	fail += !!strcmp( va_format<32,char>( 2.3 ), "2.3" );

	// Regression test. Explicitly specify char to force cast.
	fail += !!strcmp( va_printf<64,char>( STRDEF("%s"), "explicit!" ), "explicit!" );

	// Wrap test
	char wrap[36];
	cast_vabuf( wrap ).printf( "Wrapper %s!", "test" );

	return fail;
}
}

//----------------------------------------------------------------
// Tools: State tests

#include "state.h"

namespace tools
{
int TestState()
{
	int fail = 0;

	bool once = false;
	int count = 0;
	for ( int i = 0; i<10; ++i )
	{
		RUNONCE( state )
		{
			fail += once;
			once = true;
		}
		RUNEVERY( 2, every )
		{
			++count;
		}
		ONBEGIN( i>5, begin )
		{
			fail += !(i==6);
		}
		ONEND( i<4, end )
		{
			fail += !(i==4);
		}
		ONCHANGE( i>3 && i<7, change )
		{
			fail += !( (i==4) || (i==7) );
		}
	}
	fail += (count!=5);

	return fail;
}
}

//----------------------------------------------------------------
// Tools: Cvar tests

#include "cvar.h"

namespace tools
{

int TestCvarValue()
{
	int fail = 0;
	
	cvar_float val( "fl", 0, 0, 1.0f );
	val.set( "1.5" );
	val.get();
	if ( (val=2.3f)!=2.3f ) ++fail;

	cvar_bounded_int val2( "int", 0, 0, 5, 2, 9 );
	val2.set( "-2" );
	val2.get();
	if ( (val2=20)!=20 ) ++fail;

	cvar_string cv_str( "string", 0, 0, "abcABCdefDEF" );
	cv_str.set( "hello WORLD!" );
	cv_str.get();

	cvar_bool cv_bool( "bool", 0, 0, true );
	cv_bool.set( "0" );
	cv_bool.set( "no" );
	cv_bool.set( "true" );
	cv_bool.get();
	if ( (cv_bool=false)!=false ) ++fail;

	cvar_enum<test_t> cv_etst( "test_t", 0, 0, TEST2 );
	cv_etst.set( "test1" );
	cv_etst.get();
	cv_etst.value();
	if ( (cv_etst=TEST3)!=TEST3 ) ++fail;

	cvar_enum<fltest_t> cv_efl( "fltest_t", 0, 0, enum_cast<fltest_t>(FLTEST1|FLTEST3) );
	cv_efl.set( "fl3, fl4" );
	cv_efl.get();
	if ( (cv_efl=enum_cast<fltest_t>(FLTEST1|FLTEST2))!=(FLTEST1|FLTEST2) ) ++fail;

	cvar_collect sub( "sub", 0, 0 );
	sub.insert( &cv_str );
	sub.insert( &cv_bool );
	sub.insert( &cv_etst );
	sub.insert( &cv_efl );
	cvar_collect root( "root", 0, 0 );
	root.insert( &val );
	root.insert( &val2 );
	root.insert( &sub );

	cvar_completion_t list;
	root.names( "sub.", list );
	list.clear(), root.names( "sub.test", list );

	cvar_string_t fn;
	if ( fn.clear(), cv_efl.fullname(fn), fn!="sub.fltest_t" ) ++fail;
	if ( fn.clear(), val2.fullname(fn), fn!="int" ) ++fail;
	if ( fn.clear(), root.fullname(fn), fn!="root" ) ++fail;

	return fail;
}
int TestCvar()
{
	int fail = 0;

	cvar_arguments args;
	args.parse( "this is 'a comm'nd" );

	fail += TestCvarValue();

	return fail;
}

//----------------------------------------------------------------

#include "enumlogic.h"

bool UnitTests()
{
	int fail = 0;
	fail += TestCvar();
	fail += TestEnumString( );
	fail += TestState();
	fail += TestPrintf();


	return fail==0;
}

}
