
#include "state.h"
#include "enumlogic.h"
#include "enumstring.h"
#include "cvar.h"

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
ENUMSTRING( test_t )
{
	{ TEST1, "test1" },
	{ TEST2, "test2" },
	{ TEST3, "test3" },
	{ 0, nullptr },
};
ENUMFLAGS( fltest_t )
{
	{ FLTEST1, "fl1" },
	{ FLTEST2, "fl2" },
	{ FLTEST3, "fl3" },
	{ FLTEST4, "fl4" },
	{ 0, nullptr },
};
}
ENUMEXPORT( tools::test_t );
ENUMEXPDEF( tools::test_t );
ENUMEXPORT( tools::fltest_t );
ENUMEXPDEF( tools::fltest_t );
namespace tools
{

int TestState()
{
	int fail = 0;

	bool once = false;
	for ( int i = 0; i<10; i++ )
	{
		RUNONCE( state )
		{
			if ( once )
				++fail;
			once = true;
		}
	}

	return fail;
}

int TestEnumString()
{
	int fail = 0;

	char buf[512];
	int out;

	{
	const CEnumBase& e = EnumString<test_t>();
	if ( !e.Render( TEST2, buf, e.R_FALSE ) ) ++fail;
	if ( e.Parse( "blah", &out ) ) ++fail;
	if ( !e.Parse( "test2", &out ) || out!=TEST2 ) ++fail;
	// Most efficient
	//if ( !( EnumString<test_t>::Parse( "TEST3", TEST1 )==TEST1 ) ) ++fail;
	}

	{
	const CEnumBase& e = EnumString<fltest_t>();
	if ( !e.Render( FLTEST1|FLTEST3|FLTEST4, buf, e.R_FALSE ) ) ++fail;
	if ( e.Render( 0xFF, buf, e.R_FALSE ) ) ++fail;
	if ( !( e.Parse( "fl1, fl2, fl4", &out ) && out==(FLTEST1|FLTEST2|FLTEST4) ) ) ++fail;
	if ( e.Parse( "fl1 ,  fl2 , fl4", &out ) ) ++fail;

	e.Render<64>( FLTEST3|FLTEST2 );
	}

	return fail;
}

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

bool UnitTests()
{
	int fail = 0;
	fail += TestCvar();
	fail += TestEnumString( );
	fail += TestState();


	return fail==0;
}

}
