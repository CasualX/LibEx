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


//------------------------------------------------
// These defines affect the implementation
//------------------------------------------------

// When undefined, cvars marked FLCVAR_DEVONLY will not be registered at all.
//#define CVAR_DEVELOPER

// When defined, cvar descriptions are integer identifiers in a localization system.
// All it does is store integers instead of strings for descriptions.
//#define CVAR_LOCALIZE

namespace tools
{

//------------------------------------------------
// Forward declarations
//------------------------------------------------

class IEnumString;

class cvar_node;
class cvar_tree;
typedef unsigned int cvar_dllid_t;
#ifdef CVAR_LOCALIZE
typedef unsigned int cvar_desc_t;
#else
typedef const char* cvar_desc_t;
#endif // CVAR_LOCALIZE
typedef std::string cvar_string_t;
typedef std::vector<cvar_string_t> cvar_completion_list_t;


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
	cvar_node( const char* name, cvar_desc_t desc, unsigned flags );
	virtual ~cvar_node();

	// Get the actual node type
	virtual int type() const = 0;
	// Get the dll identifier for this node
	virtual int dllid() const;
	// Get the full name
	void fullname( char* buf, unsigned len ) const;
	template< unsigned L > inline void fullname( char (&buf)[L] ) const { fullname( buf, L ); }

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

public:
	// Internal use only
	virtual bool erased();
	inline void _setp( cvar_tree* parent ) { _parent = parent; }
	inline void _setn( cvar_node* next ) { _next = next; }

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

class cvar_value : public cvar_node
{
public:
	cvar_value( const char* name, cvar_desc_t desc, unsigned flags ) : cvar_node(name,desc,flags) { }
		
	// Inherited from cvar_node
	virtual int type() const;

	// Get the value for this variable as a string
	virtual cvar_string_t get() const = 0;
	// Set the value for this variable
	virtual void set( const cvar_string_t& s );
	// Restore default value
	virtual void restore() = 0;
	// Get all the possible values starting with partial, return false if not applicable. nullptr allowed (same as empty string).
	virtual bool values( const char* partial, cvar_completion_list_t& list ) const;
};
	
// Adds to list if str starts with partial
void cvar_partial( const char* str, const char* partial, cvar_completion_list_t& list );

// Arithmetic type cvars
template< typename T >
class cvar_native : public cvar_value
{
public:
	cvar_native( const char* name, cvar_desc_t desc, unsigned flags, T init ) : cvar_value(name,desc,flags), _default(init), _value(init) { }

	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const cvar_string_t& s );
	virtual void restore();

	// Onchange callback, return false to block the change
	virtual bool onchange( T old, T& t );

	// Direct access, does not invoke onchange
	inline const T& value() const { return _value; }
	inline void value( T t ) { _value = t; }

	// Operator convenience
	inline operator const T& () const { return value(); }
	inline cvar_native<T>& operator= ( T t ) { value( t ); return *this; }

protected:
	T _default;
	T _value;
};
typedef cvar_native<int> cvar_int;
typedef cvar_native<float> cvar_float;

// Bounded cvars
template< typename B, typename T >
class cvar_minbound : public B
{
public:
	cvar_minbound( const char* name, cvar_desc_t desc, unsigned flags, T init, T minval ) : B(name,desc,flags,init), _minval(minval) { }
	virtual bool onchange( T old, T& t );
protected:
	T _minval;
};
template< typename B, typename T >
class cvar_maxbound : public B
{
public:
	cvar_maxbound( const char* name, cvar_desc_t desc, unsigned flags, T init, T maxval ) : B(name,desc,flags,init), _maxval(maxval) { }
	virtual bool onchange( T old, T& t );
protected:
	T _maxval;
};
template< typename B, typename T >
class cvar_bounded : public B
{
public:
	cvar_bounded( const char* str, cvar_desc_t desc, unsigned flags, T init, T minval, T maxval ) : B(name,desc,flags,init), _minval(minval), _maxval(maxval) { }
	virtual bool onchange( T old, T& t );
protected:
	T _minval;
	T _maxval;
};

// String cvars
class cvar_string : public cvar_value
{
public:
	cvar_string( const char* name, cvar_desc_t desc, unsigned flags, const char* init ) : cvar_value(name,desc,flags), _value(init) { }

	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const cvar_string_t& s );
	virtual void restore();
	
	// Onchange callback, return false to block the change
	virtual bool onchange( cvar_string_t old, cvar_string_t& str );

	// Direct access, does not invoke onchange
	inline const cvar_string_t& value() const { return _value; }
	inline void value( const char* str ) { _value = str; }

	// Operator convenience
	inline operator const cvar_string_t& () const { return value(); }
	inline cvar_string& operator= ( const char* str ) { value( str ); return *this; }

protected:
	cvar_string_t _value;
};

// Enum cvars
class cvar_enum : public cvar_value
{
public:
	cvar_enum( const char* name, cvar_desc_t desc, unsigned flags, const IEnumString& en, int init ) : cvar_value(name,desc,flags), _enum(en), _value(init) { }
	
	// Inherited from cvar_value
	virtual cvar_string_t get() const;
	virtual void set( const cvar_string_t& s );
	virtual void restore();
	
	// Onchange callback, return false to block the change
	virtual bool onchange( int old, int& e );
	
	// Direct access, does not invoke onchange
	inline const int& value() const { return _value; }
	inline void value( int e ) { _value = e; }

	// Operator convenience
	inline operator const int& () const { return value(); }
	inline cvar_enum& operator= ( int e ) { value( e ); return *this; }

protected:
	const IEnumString& _enum;
	int _value;
};
class cvar_bool : public cvar_enum
{
public:
	cvar_bool( const char* name, cvar_desc_t desc, unsigned flags, bool init ) : cvar_enum( name, desc, flags, EnumString<bool>::Inst(), init ) { }
};

// Color nodes
class cvar_color : public cvar_value
{
public:
};



}

#endif // !HGUARD_LIBEX_TOOLS_CVAR
