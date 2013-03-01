
#include "cvar.h"
#include <cstdlib>
#include <cstdarg>

namespace tools
{

//------------------------------------------------
// Helpers
//------------------------------------------------
	
cvar_arguments::~cvar_arguments()
{
	if ( _buf ) free( _buf );
	if ( _args ) free( _args );
}
void cvar_arguments::parse( const char* str )
{
	if ( _buf ) { free( _buf ); _buf = NULL; }
	if ( _args ) { free( _args ); _args = NULL; }
	_argc = 0;
	_str = str;

	// Lol hacked C code
	// FIXME! Make this code readable

	int len = strlen( _str ) + 1;
	_buf = (char*) malloc( len );
	memcpy( _buf, _str, len );
	_args = (char**) malloc( 2 * MAX_ARGS * sizeof(char**) );
	_tags = (const char**) _args + MAX_ARGS;

	int it;
	char c;
	for ( it = 0; c = _str[it]; ++it )
	{
		if ( c!=' ' && c!='\t' && c!='\n' )
			goto found_something;
	}
	return;

found_something:
	int begin = 0;
	do
	{
		c = _str[it];
		if ( !c || c==' ' || c=='\t' )
		{
			if ( _argc<MAX_ARGS )
			{
				_args[_argc] = _buf + begin;
				_tags[_argc] = _str + begin;
				_argc++;
			}
			if ( !c ) break;
			_buf[it] = 0;
			do { ++it; } while ( ( c = _str[it] ) && ( c==' ' || c=='\t' ) && c!='\"' && c!='\'' );
			begin = it;
		}
		if ( c=='\"' || c=='\'' )
		{
			_buf[it] = 0;
			if ( _argc<MAX_ARGS )
			{
				_args[_argc] = _buf + it + 1;
				_tags[_argc] = _str + it;
				_argc++;
			}
			char d = c;
			do { ++it; } while ( ( c = _str[it] ) && c!=d );
			if ( !c ) break;
			_buf[it] = 0;
			do { ++it; } while ( ( c = _str[it] ) && ( c==' ' || c=='\t' ) );
			begin = it;
		}
		else ++it;
	}
	while ( c );
}

void cvar_throw( const char* msg, ... )
{
	// Yeah yeah this isn't how you should do exceptions but it's convenient...
	char buf[2048];
	va_list va;
	va_start( va, msg );
	vsnprintf( buf, sizeof(buf), msg, va );
	va_end( va );
	throw cvar_error( msg );
}


int cvar_value::type() const
{
	return CVAR_VALUE;
}

}
