
#include "detours.h"
#include "tools.h"
#include "alloc.h"
#include "lde.h"

namespace toolkit
{

	
bool BaseDetour::Init( void* func, void* hook, unsigned bytes )
{
	assert( bytes<=8 );

	if ( _stub = rweAligned<16>(32) )
	{
		if ( unsigned len = LdeCopy( (const unsigned char*)func, (unsigned char*)_stub, bytes ) )
		{
			assert( len<=32 );
			_func = func;
			_hook = hook;
			_bytes = bytes;
			// LdeCopy needs to relocate the code, so the original bytes might be lost...
			_orig = *(unsigned __int64*) func;
			return true;
		}
		else
		{
			// Cleanup and fail
			rweFree( _stub );
			_stub = nullptr;
		}
	}
	return false;
}
NOINLINE void BaseDetour::Destruct()
{
	if ( _stub )
	{
		Unhook();
		rweFree( _stub );
		_stub = nullptr;
	}
}
void BaseDetour::Unhook()
{
	WriteMemory( _func, &_orig, _bytes );
}
bool BaseDetour::Hooked() const
{
	return (*(unsigned __int64*)_func)==_orig;
}
DetourJmp::DetourJmp( void* func, void* hook )
{
	if ( Init( func, hook, 5 ) ) Rehook();
	//else throw std::exception();
}
DetourJmp::~DetourJmp()
{
	Destruct();
}
void DetourJmp::Rehook()
{
	// Jmp (0xE9) and 4 bytes relative offset
	WriteMemory( _func, 1+(4<<4), 0xE9, ((char*)_hook - (char*)_func) - 5 );
}









void BaseWiretap::Destruct()
{
	if ( _stub )
	{
		Unhook();
		rweFree( _stub );
		_stub = nullptr;
	}
}
bool BaseWiretap::Init( void* place, HookFn hook, unsigned bytes )
{
	assert( bytes<=8 );

	if ( _stub = rweAligned<16>(48) )
	{
		codeptr_t stub = (codeptr_t) _stub;
		stub[0] = 0x60;		// pushad
		stub[1] = 0x9C;		// pushfd
		stub[2] = 0x8B;		// mov edx, esp
		stub[3] = 0xD4;
		stub[4] = 0xB9;		// mov ecx, this
		(unsigned&)stub[5] = (unsigned)this;
		stub[9] = 0xE8;		// call hook
		(unsigned&)stub[10] = (codeptr_t)hook - (stub+10+4);
		stub[14] = 0x9D;	// popfd
		stub[15] = 0x61;	// popad

		if ( unsigned len = LdeCopy( (codeptr_t)place, stub+16, bytes ) )
		{
			assert( len<=48 );
			_place = place;
			_bytes = bytes;
			_orig = *(unsigned __int64*) place;
			return true;
		}
		else
		{
			rweFree( _stub );
			_stub = nullptr;
		}
	}
	return false;
}
void BaseWiretap::Unhook()
{
	WriteMemory( _place, &_orig, _bytes );
}
bool BaseWiretap::Hooked() const
{
	return (*(unsigned __int64*)_place)==_orig;
}
WiretapJmp::WiretapJmp( void* place, HookFn hook )
{
	if ( Init( place, hook, 5 ) ) Rehook();
	//else throw std::exception();
}
WiretapJmp::~WiretapJmp()
{
	Destruct();
}
void WiretapJmp::Rehook()
{
	WriteMemory( _place, 1+(4<<4), 0xE9, (codeptr_t)_stub - ((codeptr_t)_place+5) );
}

}
