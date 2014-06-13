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
	// Get the stack frame X frames up
	static inline StackFrame<Args,Local>* Get( void* pFramePtr, unsigned int frames )
	{
		// Go to the correct frame
		void* ebp = pFramePtr;
		for ( ; frames; --frames )
		{
			ebp = *(void**)ebp;
			// The stack frames chain doesn't even end with a nullptr, how disappointing :(
			//if ( !ebp )
			//	return nullptr;
		}
		// Give a nicely casted struct back
		return reinterpret_cast<StackFrame<Args,Local>*>( reinterpret_cast<Local*>(ebp)-1 );
	}

	template< typename ArgsB, typename LocalB >
	inline StackFrame<ArgsB,LocalB>* Up( unsigned int frames )
	{
		// Walk up to correct frame
		void* ebp = &pFramePtr;
		for ( ; frames; --frames )
		{
			ebp = *(void**)ebp;
		}
		// Give a nicely casted struct back
		return reinterpret_cast<StackFrame<ArgsB,LocalB>*>( reinterpret_cast<LocalB*>(ebp)-1 );
	}

	Local local;
	void* pFramePtr;
	unsigned char* pRetAddr;
	Args args;
};


class StackFrameIterator
{
public:
	inline explicit StackFrameIterator( void* pFramePtr ) : pFramePtr(pFramePtr) { }
	// Go to the next stack frame
	inline void Next() {
		void* next = *(void**) pFramePtr;
		// Since the stack frame chain does not end with a nullptr, we have to check it heuristically with large deltas...
		// Under rare conditions this may crash!!
		uintptr_t delta = (intptr_t)next - (intptr_t)pFramePtr;
		pFramePtr = ( delta>0x4000 ) ? nullptr : next;
	}
	// Get a stack frame pointer from this iterator
	template< typename Args, typename Local >
	inline StackFrame<Args,Local>* Get() {
		return reinterpret_cast<StackFrame<Args,Local>*>( reinterpret_cast<Local*>(pFramePtr)-1 );
	}

	inline operator bool () const { return pFramePtr!=nullptr; }
	inline StackFrameIterator& operator++ () { Next(); return *this; }
	inline StackFrameIterator operator++ ( int ) { StackFrameIterator it( *this ); it.Next(); return it; }
protected:
	void* pFramePtr;
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
	unsigned char* pRetAddr;
	Args args;
};

}

#endif // !HGUARD_LIBEX_TOOLKIT_STACK
