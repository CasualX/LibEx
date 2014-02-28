
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
	// BUG! It's not possible I didn't make a mistake :>

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

const char* cvar_parse_name( const char* cvar, char* name, unsigned size )
{
	unsigned s;
	// Find the next dot
	const char* it = cvar;
	for ( ; *it!='.'; ++it )
	{
		if ( !*it )
		{
			s = it-cvar;
			it = nullptr;
			goto done;
		}
	}
	s = it-cvar;
	++it;
done:
	// Assign it to the given buffer
	s = (s>=size)?(size-1):s;
	__movsb( (unsigned char*)name, (const unsigned char*)cvar, s );
	name[s] = 0;
	// Return the subnodes
	return it;
}

void cvar_partial( const char* str, const char* partial, cvar_completion_t& list )
{
	// TODO! Case insensitive?
	if ( !partial || strstr( str, partial ) )
	{
		list.push_back( str );
	}
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



//------------------------------------------------
// Cvar node
//------------------------------------------------

cvar_node::cvar_node( const char* name, cvar_desc_t desc, unsigned flags ) : _name(name), _desc(desc), _flags(flags), _parent(NULL), _next(NULL)
{
	// In some cases the name may be dynamically created, make a copy instead
	if ( flags&FLCVAR_NAMECOPY )
	{
		unsigned int lname = strlen( name )+sizeof(char);
		char* data = (char*) ::malloc( lname );
		__movsb( (unsigned char*)data, (const unsigned char*)name, lname );
		_name = data;
	}
}
cvar_node::~cvar_node()
{
	// If we still have a parent, remove us first
	if ( _parent )
	{
		_parent->remove( this );
	}
	// Free memory if needed
	if ( _flags&FLCVAR_NAMECOPY )
	{
		free( const_cast<char*>(_name) );
	}
}
int cvar_node::dllid() const
{
	// Only the root is allowed to alloc identifiers.
	static int dllid = -1;
	//if ( dllid<0 && _parent )
	//{
	//	dllid = _parent->allocid();
	//}
	return dllid;
}
cvar_string_t& cvar_node::fullname( cvar_string_t& s ) const
{
	// The root's name isn't normally included (unless you ask the fullname of the root node).
	if ( _parent && _parent->parent() )
	{
		_parent->fullname( s );
		s.push_back( '.' );
	}
	s.append( _name );
	return s;
}


//------------------------------------------------
// Cvar value nodes
//------------------------------------------------

int cvar_value::type() const
{
	return CVAR_VALUE;
}
void cvar_value::set( const char* s )
{
	// The compiler should be smart enough to inline this...
	if ( _flags&FLCVAR_READONLY )
	{
		// Fullname?
		cvar_throw( "Cannot assign \"%s\" to cvar '%s' because it is read-only!", s, _name );
	}
}
bool cvar_value::values( const char* partial, cvar_completion_t& list ) const
{
	return false;
}


cvar_string_t cvar_string::get() const
{
	return _value;
}
void cvar_string::set( const char* s )
{
	cvar_value::set( s );
	cvar_string_t t( s );
	if ( onchange( _value, t ) )
		_value = t;
}
bool cvar_string::onchange( cvar_string_t old, cvar_string_t& s )
{
	return true;
}



cvar_string_t cvar_enumbase::get() const
{
	char* s = _enum.Render<512>( _value );
	return cvar_string_t( s );
}
void cvar_enumbase::set( const char* s )
{
	cvar_value::set( s );
	CEnumBase::enum_t e;
	_enum.Parse( s, e, ES_THROW );
	if ( onchange( _value, e ) )
		_value = e;
}
bool cvar_enumbase::values( const char* partial, cvar_completion_t& list ) const
{
	int e1, e2;
	const char* s;
	for ( int i = 0; s = _enum.Index( i, e1 ); ++i )
	{
		// Only show the first of duplicates as these are usually intended for convenience (but don't encourage this).
		if ( i==0 || e1!=e2 )
		{
			cvar_partial( s, partial, list );
		}
		e2 = e1;
	}
	return true;
}
bool cvar_enumbase::onchange( int old, int& e )
{
	return true;
}



cvar_string_t cvar_color::get() const
{
	cvar_string_t s;
	s.resize( 20 );
	s.resize( _value.format( (char*)s.c_str() ) );
	return s;
}
void cvar_color::set( const char* s )
{
	cvar_value::set( s );

	cvar_color_t c = _value;
	if ( !c.parse( s ) )
		cvar_throw( "Invalid color format!" );

	if ( onchange( _value, c ) )
		_value = c;
}
bool cvar_color::onchange( cvar_color_t old, cvar_color_t& e )
{
	return true;
}

//------------------------------------------------
// Cvar tree nodes
//------------------------------------------------

int cvar_tree::type() const
{
	return CVAR_SUBTREE;
}
bool cvar_tree::names( const char* partial, cvar_completion_t& list ) const
{
	cvar_string_t prefix;
	bool b;
	if ( b = names( partial, list, prefix ) )
	{
		for ( auto it = list.begin(), end = list.end(); it!=end; ++it )
			it->insert( 0, prefix );
	}
	return b;
}


cvar_collect::~cvar_collect()
{
	// Orphan all our children
	while ( _list )
	{
		cvar_node* it = remove( _list );
		
		// If managed we need to clean it up
		if ( it->flags()&FLCVAR_MANAGED )
			delete it;
	}
}
void cvar_collect::insert( cvar_node* node )
{
	// Must not already have a parent!
	// NOTE! The code below should perfectly accept a chain of nodes without problem,
	//       however I want to reduce api complexity as much as possible (should lead to less bugs later on)!
	assert( node && !node->parent() && !node->next() );

#ifndef CVAR_ALLOWDEV
	// Not registering developer cvars
	if ( node->flags()&FLCVAR_DEVONLY ) return;
#endif // CVAR_ALLOWDEV

	// Make it our child
	node->_parent = this;

	// Add at the end
	// NOTE! Should a chain be added, extra care must be taken if added anywhere but the end (future bug-proofing :)
	if ( _list )
	{
		cvar_node* it;
		for ( it = _list; it->next(); it = it->next() );
		it->_next = node;
	}
	else
	{
		_list = node;
	}
}
cvar_node* cvar_collect::remove( cvar_node* node )
{
	// We can only remove our own children obviously
	assert( node && node->parent()==this );

	if ( node==_list )
	{
		_list = _list->_next;
done:
		node->_parent = nullptr;
		node->_next = nullptr;
		return node;
	}
	else
	{
		for ( cvar_node* it = _list; it; it = it->_next )
		{
			if ( it->_next==node )
			{
				it->_next = node->_next;
				goto done;
			}
		}
	}
	// What!? Should never happen!
	// We asserted ourselves as being its parent, but we didn't find it
	assert( false );
	return nullptr;
}
cvar_node* cvar_collect::find( const char* str ) const
{
	// Read out the next subnode name
	char name[64];
	const char* next = cvar_parse_name( str, name );

	// Find the local cvar node referred to
	cvar_node* it = find_local( name );

	if ( next && it )
	{
		// We need to go deeper, so this must be a subtree
		if ( it->type()!=CVAR_SUBTREE )
			return nullptr;

		// Continue looking
		it = static_cast<cvar_tree*>( it )->find( next );
	}

	return it;
}
cvar_node* cvar_collect::nodes() const
{
	return _list;
}
void cvar_collect::erase( int id )
{
	// TODO! Implement me!
	assert( false );
}
bool cvar_collect::names( const char* partial, cvar_completion_t& list, cvar_string_t& prefix ) const
{
	// Read the next name out of partial
	char name[200];
	if ( const char* next = cvar_parse_name( partial, name ) )
	{
		// Find the node referring to
		const cvar_node* it = find_local( name );
		if ( !it || it->type()!=CVAR_SUBTREE )
			return false;

		// We have to go deeper, append the prefix and recurse.
		prefix.append( partial, next );
		return static_cast<const cvar_tree*>(it)->names( next, list, prefix );
	}

	// We want all the cvars starting with partial.
	for ( const cvar_node* it = _list; it; it = it->next() )
	{
		cvar_partial( it->name(), partial, list );
	}
	return true;
}
cvar_node* cvar_collect::find_local( const char* str ) const
{
	for ( cvar_node* it = _list; it; it = it->next() )
	{
		if ( !_stricmp( str, it->name() ) )
			return it;
	}
	return nullptr;
}

//------------------------------------------------
// Cvar command nodes
//------------------------------------------------

int cvar_command::type() const
{
	return CVAR_COMMAND;
}

void cvar_delegate::run( const cvar_arguments& args ) const
{
	if ( _func )
	{
		_func( _user, this, args );
	}
}
bool cvar_delegate::complete( const char* partial, cvar_completion_t& list ) const
{
	bool b = false;
	if ( _complete )
	{
		b = _complete( _user, this, partial, list );
	}
	return b;
}

}
