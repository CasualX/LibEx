#ifndef HGUARD_LIBEX_TOOLS_STATE
#define HGUARD_LIBEX_TOOLS_STATE
#pragma once

//----------------------------------------------------------------
// State
//----------------------------------------------------------------
// These macros make my life a bit easier.
// ONBEGIN will run when <expr> becomes true.
// ONEND will run when <expr> becomes false.
// ONCHANGE will run when <expr> changes.
// RUNONCE will run only once.
// RUNEVERY will run every <count> times the code is executed.
//
// Examples:
//
// ONBEGIN( <expr>, statevar )
// {
//     // Code will only be run the first time <expr> becomes true.
//     // This code will then run again after <expr> becomes false and true again.
// }
//
// ONCHANGE( <expr>, statevar )
// {
//     if ( statevar )
//     {
//         // Run when <expr> becomes true
//     }
//     else
//     {
//         // Run when <expr> becomes false
//     }
// }
//

#define ONBEGIN( EXPR, VAR ) static bool VAR = false; ONBEGINX( EXPR, VAR )
#define ONBEGINX( EXPR, VAR ) if ( tools::OnBeginTest( VAR, static_cast<bool>(EXPR) ) )

#define ONEND( EXPR, VAR ) static bool VAR = false; ONENDX( EXPR, VAR )
#define ONENDX( EXPR, VAR ) if ( tools::OnEndTest( VAR, static_cast<bool>(EXPR) ) )

#define ONCHANGE( EXPR, VAR ) static bool VAR = false; ONCHANGEX( EXPR, VAR )
#define ONCHANGEX( EXPR, VAR ) if ( tools::OnChangeTest( VAR, EXPR ) )

#define RUNONCE( VAR ) static bool VAR = false; RUNONCEX( VAR )
#define RUNONCEX( VAR ) if ( tools::RunOnceTest( VAR ) )

#define RUNEVERY( COUNT, VAR ) static int VAR = 0; RUNEVERYX( COUNT, VAR )
#define RUNEVERYX( COUNT, VAR ) if ( tools::RunEveryTest( VAR, COUNT ) )

namespace tools
{

inline bool OnBeginTest( bool& s, bool b )
{
	bool r = !s && b;
	s = b;
	return r;
}

inline bool OnEndTest( bool& s, bool b )
{
	bool r = s && !b;
	s = b;
	return r;
}

template< typename T >
inline bool OnChangeTest( T& s, T b )
{
	bool r = ( s != b );
	s = b;
	return r;
}

inline bool RunOnceTest( bool& b )
{
	bool r = !b;
	b = true;
	return r;
}

inline bool RunEveryTest( int& i, int c )
{
	++i;
	bool r = (i>=c);
	if ( r ) i = 0;
	return r;
}

}

#endif // !HGUARD_LIBEX_TOOLS_STATE
