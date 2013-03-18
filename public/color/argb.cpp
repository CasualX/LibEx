
#include "argb.h"
#include "names.h"

namespace color
{

bool argb::parse( const char* s )
{
	if ( !s )
		return false;

	if ( s[0]=='[' )
	{
		// Find number of args
		unsigned n = 1;
		for ( const char* t = ++s; *t; ++t ) { if ( *t==',' ) ++n; }

		// "[NAME]"
		// "[NAME,ALPHA]"
		if ( n<=2 )
		{
			const char* e;
			for ( e = s; *e && ( *e==',' || *e==']' ); ++e );
			color::name c = Lookup( s, e ); // Leave ourselves unmodified on failure
			if ( c==Error )
				return false;
			*this = c;

			if ( n==2 )
			{
				alpha( atoi( e+1 ) );
			}
		}
		// "[RED,GREEN,BLUE]"
		// "[RED,GREEN,BLUE,ALPHA]"
		else if ( n<=4 )
		{
			int red, green, blue, alpha = 255;
			red = atoi( s+1 );
			for ( ; *s!=','; ++s );
			green = atoi( s+1 );
			for ( ; *s!=','; ++s );
			blue = atoi( s+1 );
			if ( n==4 )
			{
				for ( ; *s!=','; ++s );
				alpha = atoi( s+1 );
			}
			*this = argb( red, green, blue, alpha );
		}
		// Invalid number of args
		else
		{
			return false;
		}
	}
	// Assign individual channels
	else if ( s[1]==':' )
	{
		int i = atoi( s+2 );
		char c = s[0];
		if		( c=='r' || c=='R' ) red( i );
		else if ( c=='g' || c=='G' ) green( i );
		else if ( c=='b' || c=='B' ) blue( i );
		else if ( c=='a' || c=='A' ) alpha( i );
		else return false;
	}
	// "NAME"
	else
	{
		// Leave ourselves unmodified on failure
		color::name c = Lookup( s );
		if ( c==Error )
			return false;
		*this = c;
	}

	return true;
}
int argb::format( char* buf ) const
{
	// Hide alpha if not needed
	int a = alpha();
	return sprintf( buf, (a==255)?"[%d,%d,%d]":"[%d,%d,%d,%d]",
		red(), green(), blue(), a );
}

}
