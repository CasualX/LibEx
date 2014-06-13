#ifndef HGUARD_LIBEX_TOOLKIT_SCANNER
#define HGUARD_LIBEX_TOOLKIT_SCANNER
#pragma once

//----------------------------------------------------------------
// Toolkit: Scanner
//----------------------------------------------------------------
// Deals with pattern scanning for signatures.
//
// Due to the dependency on PeFile it's Windows only for now.
//

#include "../libex.h"

namespace pelite { class PeFile; }

namespace toolkit
{


//----------------------------------------------------------------
// Class: Scanner
//----------------------------------------------------------------
// Handles scanning in modules & executable files
class Scanner
{
public:
	// Some compile settings
	enum
	{
		MAX_STORE = 8,
		QS_BUF_LEN = 15,
		MAX_DEPTH = 8,
	};

	
	// Constructing
	Scanner() { }

	// Only scans in the specified section, references allowed throughout the whole module.
	explicit Scanner( const pelite::PeFile& bin, const char* section = nullptr );
	void Init( const pelite::PeFile& bin, const char* sec = nullptr );

	// Manually specify the scan range.
	// begin/end are actual scan ranges, low/high are memory limits when following references (default to scan range).
	explicit Scanner( void* begin, void* end, void* low = nullptr, void* high = nullptr );
	void Init( void* begin, void* end, void* low = nullptr, void* high = nullptr );




	// Maintains the state while scanning
	struct State
	{
		// Pattern
		const char* pattern;
		// Optimization
		unsigned char qslen;
		unsigned char qsbuf[QS_BUF_LEN];
		// Result
		void* ptr;
		void* store[MAX_STORE];
		// Performance
		unsigned hits;
	};




	// Main scanner
	// Will throw an exception if the pattern is malformed. This is for your own good as the matcher doesn't do ANY checks on the pattern.
	//
	// Use case 'quick&easy':
	//  Invoke Scan with your pattern, access store in scan[index].
	//  Returns nullptr if not found or there are multiple matches.
	//
	// Use case 'adept':
	//  Scanner scan( ... );
	//  void* ptr;
	//  for ( scan.Setup( "pattern" ); ptr = scan.Next(); ; ) { use ptr and scan[index] }
	//
	// Note about the format of patterns:
	// Pattern uses human readable hex digits and several control characters, use spaces to make it easier to read:
	//  *   : Add pointer to this location to the store array.
	//  ?   : Ignore this byte (a single ? means 2 hex digits).
	//  ''  : Do a byte scan (text) on everything between quotes.
	//  j   : Absolute jump to the pointer stored here and continue scanning (4 or 8 byte pointers). Match will fail if pointer lands outside the bounds.
	//  rx  : Relative jump, x is either 1, 2, 4 or 8 denoting the size of the relative offset. Match will fail if pointer lands outside the bounds.
	//
	void* Scan( const char* pattern );
	void* operator() ( const char* pattern );
	// Init the state for matching, throws on malformed patterns
	void Setup( const char* pattern );
	// Find the next match, only touches state if a match is found
	void* Next();
	// Matches pattern at current position
	bool Match( unsigned char* pos );

	// Access the store array
	void* operator[] ( unsigned i ) const;
	// Access the latest match position (only valid after Next() returns a non-null value)
	void* Position() const;




	// Throws an std::exception for malformed patterns.
	static void Validate( const char* pattern );


private:
	const pelite::PeFile* _bin;
	// Scan range
	void*	_begin;
	void*	_end;
	// Follow references in this range
	void*	_low;
	void*	_high;
	// Scanner state
	State	state;
};




inline Scanner::Scanner( const pelite::PeFile& bin, const char* section ) { Init( bin, section ); }
inline Scanner::Scanner( void* begin, void* end, void* low, void* high ) { Init( begin, end, low, high ); }
inline void* Scanner::operator() ( const char* pattern ) { return Scan( pattern ); }
inline void* Scanner::operator[] ( unsigned i ) const { assert( i<MAX_STORE ); return state.store[i]; }
inline void* Scanner::Position() const { return state.ptr; }

}

#endif // !HGUARD_LIBEX_TOOLKIT_SCANNER
