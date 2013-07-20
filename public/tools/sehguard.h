#ifndef HGUARD_LIBEX_TOOLS_SEHGUARD
#define HGUARD_LIBEX_TOOLS_SEHGUARD
#pragma once

//----------------------------------------------------------------
// Handle SEH exceptions as C++EH exceptions
//----------------------------------------------------------------
// Using this requires /EHa (C/C++ -> Code Generation -> Enable C++ Exceptions) and #define USE_TOOLS_SEHGUARD

#include <stdexcept>

struct _EXCEPTION_POINTERS;

namespace tools
{

// Guard will catch SE exceptions and translate to C++EH
class SehGuard
{
public:
	static void SehTranslator( unsigned int, _EXCEPTION_POINTERS* );
#ifdef USE_TOOLS_SEHGUARD
	inline explicit SehGuard( bool s = true )
	{
		_prev = _set_se_translator( &SehTranslator );
		// Compiler should optimize this out if you use a constant value as param
		if (!s) _set_se_translator( _prev );
	}
	~SehGuard()
	{
		_set_se_translator( _prev );
	}
private:
	_se_translator_function _prev;
#endif // USE_TOOLS_SEHGUARD
};

// C++ exception thrown when a SE exception is encountered
class SehException : public std::runtime_error
{
public:
	SehException( unsigned int code, _EXCEPTION_POINTERS* err ) : std::runtime_error(""), _code(code), _err(err) { }
	virtual const char* what() const;
private:
	unsigned int _code;
	_EXCEPTION_POINTERS* _err;
	mutable char _buf[32];
};

}

#endif // !HGUARD_LIBEX_TOOLS_SEHGUARD
