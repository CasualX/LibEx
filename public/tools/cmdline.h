#pragma once

#include "..\hash\hash.h"

namespace tools
{

class CCmdLine
{
public:
	CCmdLine( const char* str );
	CCmdLine( int argc, char* argv );

	static inline const char* GetParam( const char* s )
	{
		while ( *s && *s!=':' ) ++s;
		return s + ( *s==':' );
	}

	//char* Find( const char* id );
	inline const char* Find( unsigned int id ) const
	{
		// Should not be inlined :)
		for ( const_iterator i = begin(), e = end(); i!=e; ++i )
		{
			const char* p = *i;
			if ( *p++=='/' )
			{
				// Hash until a colon
				typedef hash::hash_t hash_t;
				hash_t::type h = hash_t::init;
				for ( ; *p && *p!=':'; ++p )
					h = hash_t::algo( h, static_cast<unsigned char>(*p) );
				// Check for a match
				if ( id==h )
				{
					// Skip colon
					return p + (*p==':');
				}
			}
		}
		return nullptr;
	}
	template< unsigned int S >
	inline const char* Find( const char (&id)[S] ) const
	{
		return Find( HASH(id) );
	}

	// Helpers
	//bool Float( unsigned int id, float& out );
	//bool Int( unsigned int id, float& out );

	// Iteration
	typedef char** iterator;
	inline iterator begin() { return _argv; }
	inline iterator end() { return _argv+_argc; }
	typedef const char* const* const_iterator;
	inline const_iterator begin() const { return _argv; }
	inline const_iterator end() const { return _argv+_argc; }

protected:
	void Parse( const char* str );

private:
	int _argc;
	char** _argv;
};

}
