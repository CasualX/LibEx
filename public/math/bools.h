#ifndef HGUARD_LIBEX_MATH_BOOLS
#define HGUARD_LIBEX_MATH_BOOLS
#pragma once

//----------------------------------------------------------------
// Class: bools
//----------------------------------------------------------------
// Wraps the result of vector compares.

#include "defs.h"

namespace math
{

template< dim D >
class bools {
public:
	// Underlying type for this boolean holder.
	typedef unsigned int type;
	// Mask that covers all the bits.
	static const type mask = (1<<D)-1;

	// Constructor. No bits outside mask should be set (not asserted!, see .hpp)
	explicit bools( type bits );

	// Any compares were true.
	bool any() const;
	// Any compares of the non-masked components were true.
	bool any( type mask ) const;
	// All compares were true.
	bool all() const;
	// All compares of the non-masked components were true.
	bool all( type mask ) const;

	// Convenience, same as any().
	// FIXME! I am not convinced this is a good idea, as either any() or all() are good candidates.
	//        Hitting this operator means you probably forgot to wrap in any() or all().
	//operator bool () const;

private:
	type bits;
};

// Any compares were true.
template< dim D > bool any( const bools<D>& result );
// Any compares of the non-masked components were true.
template< dim D > bool any( const bools<D>& result, typename bools<D>::type mask );
// All compares were true.
template< dim D > bool all( const bools<D>& result );
// All compares of the non-masked components were true.
template< dim D > bool all( const bools<D>& result, typename bools<D>::type mask );

}

#endif // !HGUARD_LIBEX_MATH_BOOLS
