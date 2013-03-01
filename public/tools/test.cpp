
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
	const IEnumString& e = EnumString<test_t>::Inst();
	if ( !e.Render( TEST2, buf, sizeof(buf), 1 ) ) ++fail;
	if ( e.Parse( "blah", &out ) ) ++fail;
	if ( !e.Parse( "test2", &out ) || out!=TEST2 ) ++fail;
	// Most efficient
	if ( !( EnumString<test_t>::Parse( "TEST3", TEST1 )==TEST1 ) ) ++fail;
	}

	{
	const IEnumString& e = EnumString<fltest_t>::Inst();
	if ( !e.Render( FLTEST1|FLTEST3|FLTEST4, buf, sizeof(buf), 1 ) ) ++fail;
	if ( e.Render( 0xFF, buf, sizeof(buf), 1 ) ) ++fail;
	if ( !( e.Parse( "fl1, fl2, fl4", &out ) && out==(FLTEST1|FLTEST2|FLTEST4) ) ) ++fail;
	if ( e.Parse( "fl1 ,  fl2 , fl4", &out ) ) ++fail;
	}

	return fail;
}

int TestCvar()
{
	int fail = 0;

	cvar_arguments args;
	args.parse( "this is 'a comm'nd" );


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
