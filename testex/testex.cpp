// testex.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <intrin.h>

#include "../public/hash/test.cpp"
//#include "../public/toolkit/test.cpp"
#include "../public/meta/test.cpp"
#include "../public/tools/test.cpp"
#include "../public/math_sse/test.cpp"

int _tmain( int argc, char* argv[] )
{
	if ( !tools::UnitTests() )
		printf( "Module <tools> failed!\n" );
	if ( !meta::UnitTest() )
		printf( "Module <meta> failed!\n" );
	if ( !math_sse::UnitTest() )
		printf( "Module <math_sse> failed!\n" );
	//if ( !toolkit::UnitTests() )
	//	printf( "Module <toolkit> failed!\n" );
	if ( !hash::test() )
		printf( "Module <hash> failed!\n" );
	return 0;
}
