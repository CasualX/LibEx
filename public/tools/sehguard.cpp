
#include "sehguard.h"
#include <Windows.h>

namespace tools
{

void SehGuard::SehTranslator( unsigned int code, _EXCEPTION_POINTERS* err )
{
	throw SehException( code, err );
}
const char* SehException::what() const
{
	SehException* self = const_cast<SehException*>(this);
	sprintf( self->buf, "SEH 0x%08X @ 0x%08X", _code, _err->ExceptionRecord->ExceptionAddress );
	return buf;
}

}
