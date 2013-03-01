#ifndef HGUARD_LIBEX_TOOLKIT_DETOURS
#define HGUARD_LIBEX_TOOLKIT_DETOURS
#pragma once

//----------------------------------------------------------------
// Toolkit: Detours
//----------------------------------------------------------------
// Bunch of classes to allow easy modifying of the target code in order to hook functions.

#include "../libex.h"

namespace toolkit
{
typedef unsigned char* codeptr_t;



//------------------------------------------------
// Class: Detour
//------------------------------------------------
// Base version, doesn't specify the patch method,
// Derive a class and implement a 'Rehook()' function to patch the code.
class BaseDetour
{
	BaseDetour( const BaseDetour& );
	BaseDetour& operator= ( const BaseDetour& );

public:
	BaseDetour() : _stub(nullptr) { }

	// Detour Constructor
	// Note: Does NOT patch anything (as this class doesn't specify the method).
	bool Init( void* func, void* hook, unsigned bytes );
	// Shutdown and free resources
	void Destruct();

	// Hook management
	void Unhook();
	bool Hooked() const;

	// Call the original function ( without unhooking )
	template< typename Fn >
	inline Fn Call() const { return (Fn)_stub; }

	// Original function
	template< typename Fn >
	inline Fn Function() const { return (Fn)_func; }

protected:
	void*	_func;
	void*	_hook;
	void*	_stub;
	unsigned _bytes;
	unsigned __int64 _orig;
};
// Hook using the standard jmp method
class DetourJmp : public BaseDetour
{
public:
	DetourJmp( void* func, void* hook );
	~DetourJmp();
	void Rehook();
};




//----------------------------------------------------------------
// Class: Wiretap
//----------------------------------------------------------------
// Run your hook code in the middle of a function.
// It'll continue execution after your hook returns.
//
// The function will get redirected to a small stub which does this:
// stub:
//   pushad			; Create a context record
//   pushfd			;
//   mov ecx, this	; Pointer to the wiretap object
//   mov edx, esp	; Pointer to the context on the stack
//   call your_hook
//   popfd			; Restore context
//   popad			;
//   ; Overwritten bytes go here
//   jmp continue
//
class BaseWiretap
{
	BaseWiretap( const BaseWiretap& );
	BaseWiretap& operator= ( const BaseWiretap& );

public:
	BaseWiretap() { }
	
	// Context that gets created upon dispatching to your hook.
	// Modifying this context will modify the ACTUAL registers when the function continues.
	struct Context
	{
		// Result of pushfd
		types::dword eflags;
		// Result of pushad
		types::dword edi, esi, ebp, esp, ebx, edx, ecx, eax;
	};
	// Callback function prototype
	typedef void (LIBEXCC *HookFn)( BaseWiretap& self, Context& ctx );
	
	// Wiretap Constructor
	// Note: Does NOT patch anything (as this class doesn't specify the method).
	bool Init( void* place, HookFn hook, unsigned bytes );
	// Shutdown and free resources
	void Destruct();
	
	// Hook management
	void Unhook();
	bool Hooked() const;

	// Note: Cannot call original function! Use detours instead.
	// Wiretaps are just to tap executing code anywhere, not to hook whole functions.
	
protected:
	void*	_place;
	void*	_stub;
	unsigned _bytes;
	unsigned __int64 _orig;
};
class WiretapJmp : public BaseWiretap
{
public:
	WiretapJmp( void* place, HookFn hook );
	~WiretapJmp();
	void Rehook();
};

}

#endif // !HGUARD_LIBEX_TOOLKIT_DETOURS
