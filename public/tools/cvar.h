#ifndef HGUARD_LIBEX_TOOLS_CVAR
#define HGUARD_LIBEX_TOOLS_CVAR
#pragma once

//----------------------------------------------------------------
// Tools: Cvar system
//----------------------------------------------------------------
//
// Organizes cvars in a tree structure.
//
// Design rationale:
/*
Cvars (short for console variables) are designed as a simple
mechanism to allow interaction between a program and a human.
Humans expect a textual representation while the program's
native format is its raw binary interpretation (int, float, etc).

This is modelled with a shared interface that accepts and outputs
the textual representation while storing it in whatever format is
most convenient to the program. The cvar's job is thus to convert
the textual representation to its internal format and vica versa.

Because trees are awesome the cvars are stored as such.

There's also command nodes for extra interaction.
*/
//

#include <exception>
#include <string>
#include <vector>

#include "enumstring.h"
#include "../color/argb.h"


//------------------------------------------------
// These defines affect the implementation
//------------------------------------------------

// When undefined, cvars marked FLCVAR_DEVONLY will not be registered at all.
//#define CVAR_DEVELOPER

// When defined, cvar descriptions are integer identifiers in a localization system.
// All it does is store integers instead of strings for descriptions.
//#define CVAR_LOCALIZE

// Calling convention used for some funcs...
#define CVAR_CALL __fastcall

namespace tools
{

//------------------------------------------------
// Forward declarations
//------------------------------------------------

class IEnumString;

class cvar_node;
class cvar_tree;
class cvar_collect;
typedef unsigned int cvar_dllid_t;
#ifdef CVAR_LOCALIZE
typedef unsigned int cvar_desc_t;
#else
typedef const char* cvar_desc_t;
#endif // CVAR_LOCALIZE
typedef std::string cvar_string_t;
typedef std::vector<cvar_string_t> cvar_completion_t;
typedef color::argb cvar_color_t;


//------------------------------------------------
// Helpers
//------------------------------------------------

// Processing cvar arguments
class cvar_arguments
{
public:
	enum { MAX_ARGS = 32 };
	
	cvar_arguments() : _buf(nullptr), _args(nullptr) { }
	cvar_arguments( const char* str ) : _buf(nullptr), _args(nullptr) { parse( str ); }
	cvar_arguments( const cvar_arguments& args ) : _buf(nullptr), _args(nullptr) { parse( args._str ); }
	~cvar_arguments();
	inline cvar_arguments& operator= ( const cvar_arguments& args ) { parse( args._str ); return *this; }
	inline cvar_arguments& operator= ( const char* str ) { parse( str ); return *this; }
		
	// Parse a new string
	void parse( const char* str );

	// Get the number of args
	inline int argc() const { return _argc; }
	// Get a specific arg, ensure that i<argc()
	inline const char* arg( unsigned i ) const { assert( i<_argc ); return _args[i]; }
	// Get the argument string (includes the command itself)
	inline const char* args() const { return _str; }
	// Get the argument string starting from
	inline const char* args( unsigned i ) const { assert( i<_argc ); return _tags[i]; }

	// Get the value from /arg:value (implement me?)
	//const char* find1( const char* arg ) const;

private:
	const char* _str;
	const char** _tags;
	char* _buf;
	char** _args;
	unsigned _argc;
};

// Parse a dot separated name
const char* cvar_parse_name( const char* cvar, char* name, unsigned int size );
template< unsigned int S > const char* cvar_parse_name( const char* cvar, char (&name)[S] ) { return cvar_parse_name( cvar, name, S ); }
// Adds str to list if it is related to partial.
void cvar_partial( const char* str, const char* partial, cvar_completion_t& list );

// Cvar errors
class cvar_error : public std::exception
{
public:
	cvar_error( const char* desc ) : std::exception(desc) { }
};
void cvar_throw( const char* msg, ... );


//------------------------------------------------
// Cvar interface
//------------------------------------------------

// Cvar node types
// FIXME! Throw all this away and get a proper dynamic cast type of thing?
enum cvartype_t
{
	CVAR_UNKNOWN,
	CVAR_SUBTREE,
	CVAR_VALUE,
	CVAR_COMMAND,
};
	
// Some flags
enum flcvar_t
{
	FLCVAR_NORMAL = 0,
	// The node won't be registered in the tree in release builds.
	FLCVAR_DEVONLY = (1<<0),
	// The node will be registered but won't be displayed in cvar listings.
	FLCVAR_HIDDEN = (1<<1),
	// Make a copy of the name & desc when storing it.
	FLCVAR_NAMECOPY = (1<<2),
	// When the node is erased delete it. Use when you don't care about managing the cvar.
	FLCVAR_MANAGED = (1<<3),
	// Contains sensitive information, don't display.
	// Does not prevent you from get()'ing the value, the application must check this flag itself when displaying its value.
	FLCVAR_PASSWORD = (1<<4),
	// This is a read-only cvar, throws an error when attempting to change it.
	FLCVAR_READONLY = (1<<5),
};

// Base cvar node
class cvar_node
{
public:
	// Tree needs access to us
	friend class cvar_collect;

	cvar_node( const char* name, cvar_desc_t desc, unsigned flags );
	virtual ~cvar_node();

	// Get the actual node type
	virtual int type() const = 0;
	// Get the dll identifier for this node
	virtual int dllid() const;
	// Get the full name
	cvar_string_t& fullname( cvar_string_t& name ) const;

	// Get the name of this node
	inline const char* name() const { return _name; }
	// Get the description of this node
	inline cvar_desc_t desc() const { return _desc; }
	// Get the flags for this node
	inline unsigned flags() const { return _flags; }
	// Link back to the parent
	inline cvar_tree* parent() const { return _parent; }
	// Next cvar in line
	inline cvar_node* next() const { return _next; }

protected:
	const char* _name;
	cvar_desc_t _desc;
	unsigned _flags;
	cvar_tree* _parent;
	cvar_node* _next;
};

//------------------------------------------------
// Cvar value nodes
//------------------------------------------------
// Onchange rules:
//  Cannot be defined in the interface of cvar_value because it deals with the underlying types directly.
//  Every class derived from cvar_value should have one.
//  Value has not yet changed, returning 'true' will make the change (either return false or throw an exception if you don't want the change).
//  Given the old value and a modifiable reference to the new value so it can be changed.
//  Direct access should not invoke the onchange callback (by default).

class cvar_value : public cvar_node
{
public:
	cvar_value( const char* name, cvar_desc_t desc, unsigned flags ) : cvar_node(name,desc,flags) { }
		
	// Inherited from cvar_node
	virtual int type() const;

	// Get the value for this variable as a string
	virtual cvar_string_t get() const = 0;
	// Set the value for this variable
	virtual void set( const char* s );

	// Get all the possible values starting with partial, return false if not applicable. nullptr allowed (same as empty string).
	virtual bool values( const char* partial, cvar_completion_t& list ) const;
};
	
// Adds to list if str starts with partial
void cvar_partial( const char* str, const char* partial, cvar_completion_t& list );

// Arithmetic type cvars
// CURRENTLY SUPPORTING: int, float
template< typename T >
class cvar_native : public cvar_value
{
public:
	cvar_native( const char* name, cvar_desc_t desc, unsigned flags, T init );

	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const char* s );

	// Onchange callback, return false to block the change
	virtual bool onchange( T old, T& t );

	// Direct access, does not invoke onchange
	inline const T& value() const { return _value; }
	inline void value( T t ) { _value = t; }
	// Operator convenience
	inline operator const T& () const { return value(); }
	inline cvar_native<T>& operator= ( T t ) { value( t ); return *this; }
	
protected:
	// Formatting helpers...
	int _format( char* out ) const;
	T _parse( const char* in ) const;

protected:
	T _default;
	T _value;
};
typedef cvar_native<int> cvar_int;
typedef cvar_native<float> cvar_float;

// Bounded cvars to be used with cvar_native
template< typename B, typename T >
class cvar_minbound : public B
{
public:
	cvar_minbound( const char* name, cvar_desc_t desc, unsigned flags, T init, T minval ) : B(name,desc,flags,init), _minval(minval) { }
	virtual bool onchange( T old, T& t );
	using B::operator=;
protected:
	T _minval;
};
template< typename B, typename T >
class cvar_maxbound : public B
{
public:
	cvar_maxbound( const char* name, cvar_desc_t desc, unsigned flags, T init, T maxval ) : B(name,desc,flags,init), _maxval(maxval) { }
	virtual bool onchange( T old, T& t );
	using B::operator=;
protected:
	T _maxval;
};
template< typename B, typename T >
class cvar_bounded : public B
{
public:
	cvar_bounded( const char* name, cvar_desc_t desc, unsigned flags, T init, T minval, T maxval ) : B(name,desc,flags,init), _minval(minval), _maxval(maxval) { }
	virtual bool onchange( T old, T& t );
	using B::operator=;
protected:
	T _minval;
	T _maxval;
};
typedef cvar_minbound<cvar_int,int> cvar_minbound_int;
typedef cvar_maxbound<cvar_int,int> cvar_maxbound_int;
typedef cvar_bounded<cvar_int,int> cvar_bounded_int;
typedef cvar_minbound<cvar_float,float> cvar_minbound_float;
typedef cvar_maxbound<cvar_float,float> cvar_maxbound_float;
typedef cvar_bounded<cvar_float,float> cvar_bounded_float;

// String cvars
class cvar_string : public cvar_value
{
public:
	cvar_string( const char* name, cvar_desc_t desc, unsigned flags, const char* init ) : cvar_value(name,desc,flags), _default(init), _value(init) { }

	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const char* s );
	
	// Onchange callback, return false to block the change
	virtual bool onchange( cvar_string_t old, cvar_string_t& str );

	// Direct access, does not invoke onchange
	inline const cvar_string_t& value() const { return _value; }
	inline void value( const char* str ) { _value = str; }
	// Operator convenience
	inline operator const cvar_string_t& () const { return value(); }
	inline cvar_string& operator= ( const char* str ) { value( str ); return *this; }
	inline const cvar_string_t* operator->() const { return &_value; }
	inline cvar_string_t* operator->() { return &_value; }
	inline const cvar_string_t& operator*() const { return _value; }
	inline cvar_string_t& operator*() { return _value; }

protected:
	cvar_string_t _default;
	cvar_string_t _value;
};

// Enum cvars
// No templated version available because of needless duplication
class cvar_enumbase : public cvar_value
{
public:
	cvar_enumbase( const char* name, cvar_desc_t desc, unsigned flags, const IEnumString& en, int init ) : cvar_value(name,desc,flags), _enum(en), _default(init), _value(init) { }
	
	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const char* s );
	
	// Onchange callback, return false to block the change
	virtual bool onchange( int old, int& e );
	
	// Direct access, does not invoke onchange
	inline const int& value() const { return _value; }
	inline void value( int e ) { _value = e; }
	// Operator convenience
	inline operator const int& () const { return value(); }
	inline cvar_enumbase& operator= ( int e ) { value( e ); return *this; }

protected:
	const IEnumString& _enum;
	int _default;
	int _value;
};
template< typename E >
class cvar_enum : public cvar_enumbase
{
public:
	cvar_enum( const char* name, cvar_desc_t desc, unsigned flags, E init ) : cvar_enumbase( name, desc, flags, EnumString<E>::Inst(), init ) { }

	// Direct access, does not invoke onchange
	inline const E& value() const { return *(const E*)&_value; }
	inline void value( E e ) { _value = e; }
	// Operator convenience
	inline operator const E& () const { return value(); }
	inline cvar_enumbase& operator= ( E e ) { value( e ); return *this; }
};
typedef cvar_enum<bool> cvar_bool;

// Color nodes
// SET formats:
//  name
//  [name]
//  [name,alpha]
//  [red,green,blue]
//  [red,green,blue,alpha]
//  r:red
//  g:green
//  b:blue
//  a:alpha
// GET formats:
//  name
//  [name,alpha]
//  [red,green,blue]
//  [red,green,blue,alpha]
class cvar_color : public cvar_value
{
public:
	cvar_color( const char* name, cvar_desc_t desc, unsigned flags, cvar_color_t clr );

	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const char* s );
	
	// Onchange callback, return false to block the change
	virtual bool onchange( cvar_color_t old, cvar_color_t& e );
	
	// Direct access, does not invoke onchange
	inline const cvar_color_t& value() const { return _value; }
	inline void value( cvar_color_t e ) { _value = e; }
	// Operator convenience
	inline operator const cvar_color_t& () const { return value(); }
	inline cvar_color& operator= ( cvar_color_t e ) { value( e ); return *this; }
	
protected:
	cvar_color_t _default;
	cvar_color_t _value;
};


//------------------------------------------------
// Cvar tree nodes
//------------------------------------------------

// Interface for tree nodes
class cvar_tree : public cvar_node
{
public:
	cvar_tree( const char* name, cvar_desc_t desc, unsigned flags );

	virtual int type() const;

	// Insert a node
	virtual void insert( cvar_node* node ) = 0;
	// Remove a node and returns it
	virtual cvar_node* remove( cvar_node* node ) = 0;
	// Find a specific node or subnode, returns NULL if not found
	virtual cvar_node* find( const char* name ) const = 0;
	// Iterate over all its nodes
	virtual cvar_node* nodes() const = 0;

	// Erase all nodes of a group recursively (checks child tree nodes)
	// This removes all nodes and cleans up where necessary
	virtual void erase( int id ) = 0;

	// Get a list of the nodes related to partial, return false if not applicable
	// The list stores only local names, prefix + list[i] = full name
	virtual bool names( const char* partial, cvar_completion_t& list, cvar_string_t& prefix ) const = 0;
	// Lazy thing, prepends prefix to the list items for you.
			bool names( const char* partial, cvar_completion_t& list ) const;
};

class cvar_collect : public cvar_tree
{
public:
	cvar_collect( const char* name, cvar_desc_t desc, unsigned flags );

	virtual ~cvar_collect();
	virtual void insert( cvar_node* node );
	virtual cvar_node* remove( cvar_node* node );
	virtual cvar_node* find( const char* name ) const;
	virtual cvar_node* nodes() const;
	virtual void erase( int id );
	virtual bool names( const char* partial, cvar_completion_t& list, cvar_string_t& prefix ) const;
	using cvar_tree::names;
	
	// Finds a node locally
	cvar_node* find_local( const char* name ) const;

protected:
	cvar_node* _list;
};



//------------------------------------------------
// Cvar command nodes
//------------------------------------------------

class cvar_command : public cvar_node
{
public:
	cvar_command( const char* name, cvar_desc_t desc, unsigned flags );

	// Inherited from cvar_node
	virtual int type() const;

	// Invoke the callback
	virtual void run( const cvar_arguments& args ) const = 0;
	// Custom auto complete
	virtual bool complete( const char* partial, cvar_completion_t& list ) const = 0;

	// Operator
	void operator() ( const cvar_arguments& args ) const;
};

class cvar_delegate : public cvar_command
{
public:
	typedef void (CVAR_CALL* Fn)( void* thisptr, const cvar_command* cvar, const cvar_arguments& args );
	typedef bool (CVAR_CALL* Cb)( void* thisptr, const cvar_command* cvar, const char* partial, cvar_completion_t& list );
	
	cvar_delegate( const char* name, cvar_desc_t desc, unsigned flags, void* ctx, Fn fn = nullptr, Cb cb = nullptr );
	
	// Inherited from cvar_command
	virtual void run( const cvar_arguments& args ) const;
	virtual bool complete( const char* partial, cvar_completion_t& list ) const;

protected:
	void* _user;
	Fn _func;
	Cb _complete;
};





//------------------------------------------------
// Template and inlines!
//------------------------------------------------

template< typename T >
inline cvar_native<T>::cvar_native( const char* name, cvar_desc_t desc, unsigned flags, T init )
	: cvar_value(name,desc,flags), _default(init), _value(init) {
}
template< typename T >
cvar_string_t cvar_native<T>::get() const {
	cvar_string_t s;
	// We directly format into the string buffer, 10 should be large enough :) BUG WARNING
	s.resize( 10 );
	// Format and shrink to fit what we printed
	s.resize( _format( &*s.begin() ) );
	return s;
}
template< typename T >
void cvar_native<T>::set( const char* s ) {
	cvar_value::set( s );
	T t = _parse( s );
	if ( onchange( _value, t ) )
		_value = t;
}
template< typename T >
bool cvar_native<T>::onchange( T old, T& t ) {
	return true;
}
// Implement for each supported type
template<> inline int cvar_native<int>::_format( char* out ) const {
	return sprintf( out, "%d", _value );
}
template<> inline int cvar_native<float>::_format( char* out ) const {
	return sprintf( out, "%f", _value );
}
template<> inline int cvar_native<int>::_parse( const char* in ) const {
	return atoi(in);
}
template<> inline float cvar_native<float>::_parse( const char* in ) const {
	return static_cast<float>(atof(in));
}


template< typename B, typename T >
bool cvar_bounded<B,T>::onchange( T old, T& t ) {
	// Clamp value in range
	if ( t<_minval ) t = _minval;
	if ( t>_maxval ) t = _maxval;
	return true;
}
template< typename B, typename T >
bool cvar_minbound<B,T>::onchange( T old, T& t ) {
	if ( t<_minval ) t = _minval;
	return true;
}
template< typename B, typename T >
bool cvar_maxbound<B,T>::onchange( T old, T& t ) {
	if ( t>_maxval ) t = _maxval;
	return true;
}


inline cvar_tree::cvar_tree( const char* name, cvar_desc_t desc, unsigned flags )
	: cvar_node(name,desc,flags) {
}
inline cvar_collect::cvar_collect( const char* name, cvar_desc_t desc, unsigned flags )
	: cvar_tree(name,desc,flags), _list(nullptr) {
}


inline cvar_command::cvar_command( const char* name, cvar_desc_t desc, unsigned flags )
	: cvar_node(name,desc,flags) {
}
inline void cvar_command::operator() ( const cvar_arguments& args ) const {
	run( args );
}
inline cvar_delegate::cvar_delegate( const char* name, cvar_desc_t desc, unsigned flags, void* ctx, Fn fn, Cb cb )
	: cvar_command(name,desc,flags), _user(ctx), _func(fn), _complete(cb) {
}
	

}

#endif // !HGUARD_LIBEX_TOOLS_CVAR
