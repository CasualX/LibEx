
#include "memory.h"
#include "../libex.h"
#include <cstdarg>

#ifdef WIN32
#include <Windows.h>
#endif

namespace toolkit
{
	
// Not sure if this is a good idea...
bool WriteMemory( void* dest, unsigned mask, ... )
{
	unsigned char temp[8*4];
	unsigned char* p = temp;
	unsigned total = 0;
	va_list va;
	va_start( va, mask );
	for ( ; mask; mask >>= 4 )
	{
		// Examine each nibble of the mask
		unsigned n = mask&7;
		total += n;
		// Extract a parameter
		unsigned d = va_arg( va, unsigned );
		// Write bytes to buffer
		switch ( n )
		{
		case 4: *p++ = d&0xFF; d>>=8;
		case 3: *p++ = d&0xFF; d>>=8;
		case 2: *p++ = d&0xFF; d>>=8;
		case 1: *p++ = d&0xFF;
			break;
		}
	}
	va_end( va );

	// Write bytes to memory
	return WriteMemory( dest, temp, total );
}
NOINLINE bool WriteMemory( void* dest, const void* src, unsigned bytes )
{
#ifdef WIN32
	DWORD old;
	// Make sure we can write here
	if ( ::VirtualProtect( dest, bytes, PAGE_EXECUTE_READWRITE, &old ) )
	{
		__movsb( (unsigned char*)dest, (const unsigned char*)src, bytes );
		// Make sure to flush modifications to code
		if ( old==PAGE_EXECUTE_READ || old==PAGE_EXECUTE_READWRITE )
			::FlushInstructionCache( (HANDLE)-1, dest, bytes );
		// Restore protection, don't care if this fails
		::VirtualProtect( dest, bytes, old, &old );
		return true;
	}
#else
# error Implement me for this operating system!
#endif // WIN32
	return false;
}

}
