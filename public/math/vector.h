#ifndef HGUARD_LIBEX_MATH_VECTOR
#define HGUARD_LIBEX_MATH_VECTOR
#pragma once

#include "defs.h"
#include "bools.h"

namespace math
{

// Simple vector class with a type T and dimensions D
// Specifies the minimum API required for vector specializations.

enum { X, Y, Z, W };

template< typename T, dim D >
class vector {
public:
	typedef typename T type;
	static const dim dims = D;

	typedef typename vector<T,D> value;
	typedef typename vector<T,D>& ref;
	typedef typename const vector<T,D>& const_ref;
	typedef typename bools<D> bools;

	//----------------------------------------------------------------
	// Construction
	//----------------------------------------------------------------

	// Does not initialize its components.
	vector();
	// Initializes all components with the same value.
	explicit vector( T t );
	// Copy constructor.
	vector( const_ref vec );

	//----------------------------------------------------------------
	// Assignment
	//----------------------------------------------------------------
	
	// Copy a vector.
	ref assign( const_ref vec );
	// Assign all components the same value.
	ref dup( T t );
	// Assign all components their default initializer value (zero).
	ref zero();
	// Load from memory array.
	// FIXME! Should this enforce alignment/size?
	ref load( const T (&mem)[D] );
	// Set individual component
	template< dim I >
	ref set( T t );
	// Shuffle can't be implemented generically here... Maybe with variadic templates?

	//----------------------------------------------------------------
	// Extraction
	//----------------------------------------------------------------

	// Get a component
	template< dim I >
	T get() const;
	// Get any component (that is cheapest to retrieve)
	T extract() const;
	// Store to memory array.
	void store( T (&mem)[D] ) const;

	//----------------------------------------------------------------
	// Basic operations
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// Basic math
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// Rounding stuff
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// Length and distance operations
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// Vector operations
	//----------------------------------------------------------------

	//----------------------------------------------------------------
	// Comparison
	//----------------------------------------------------------------

protected:
	template< typename Op >
	bools _cmp( const_ref vec ) const;
public:

	// Equal
	bools cmpeq( const_ref vec ) const;
	// Less than
	bools cmplt( const_ref vec ) const;
	// Less than or equal
	bools cmple( const_ref vec ) const;
	// Greater than
	bools cmpgt( const_ref vec ) const;
	// Greater than or equal
	bools cmpge( const_ref vec ) const;
	// Is finite
	bools cmpord() const;

	// Not equal
	bools cmpneq( const_ref vec ) const;
	// Not less than
	bools cmpnlt( const_ref vec ) const;
	// Not less than or equal
	bools cmpnle( const_ref vec ) const;
	// Not greater than
	bools cmpngt( const_ref vec ) const;
	// Not greater than or equal
	bools cmpnge( const_ref vec ) const;
	// Is not finite
	bools cmpunord() const;

	//----------------------------------------------------------------
	// Operator convenience
	//----------------------------------------------------------------

private:
	T data[D];
};


template< typename T, dim D >
T dot( const vector<T,D>& left, const vector<T,D>& right );

template< typename T, dim D >
vector<T,D> cross( const vector<T,D>& left, const vector<T,D>& right );

template< typename T, dim D >
vector<T,D> clamp( const vector<T,D>& vec, const vector<T,D>& min, const vector<T,D>& max );

template< typename T, dim D >
vector<T,D> lerp( const vector<T,D>& left, const vector<T,D>& right, T scale );

template< typename T, dim D >
vector<T,D> slerp( const vector<T,D>& left, const vector<T,D>& right, T scale );

}

#endif // !HGUARD_LIBEX_MATH_VECTOR
