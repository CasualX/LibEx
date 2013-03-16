#ifndef HGUARD_LIBEX_TOOLKIT_STACK
#define HGUARD_LIBEX_TOOLKIT_STACK
#pragma once

//----------------------------------------------------------------
// Toolkit: Fun with the stack
//----------------------------------------------------------------
// Some neat stuff allowing easy access to the runtime stack

#include <intrin.h>

namespace toolkit
{


// Get the stack frame ptr
// Must be a macro to enforce inlining at all times
#define GetCurrentStackFrame() ( (void*)( ((char*)_AddressOfReturnAddress()) - sizeof(void*) ) )




//------------------------------------------------
// StackFrame
//------------------------------------------------
// Can't be used to spy on stack frames in the debugger but oh well...
template< typename Args, typename Local >
struct StackFrame
{
	static inline StackFrame<Args,Local>* Get( void* pFramePtr )
	{
		return reinterpret_cast<StackFrame<Args,Local>*>( reinterpret_cast<Local*>(pFramePtr)-1 );
	}

	Local local;
	void* pFramePtr;
	void* pRetAddr;
	Args args;
};




//------------------------------------------------
// StackFrameDbg
//------------------------------------------------
// Use with the debugger to find the stack frame you may be interested in
// Compile in Debug mode, breakpoint after Get() and check the args.
// If they're garbage, click the pFramePtr in the debugger to check the next stack frame until you found what you're looking for.
template< typename Args >
struct StackFrameDbg
{
	static inline StackFrameDbg<Args>* Get( void* pFramePtr )
	{
		return reinterpret_cast<StackFrameDbg<Args>*>( pFramePtr );
	}

	StackFrameDbg<Args>* pFramePtr;
	void* pRetAddr;
	Args args;
};

}

#endif // !HGUARD_LIBEX_TOOLKIT_STACK