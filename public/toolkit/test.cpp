
#include "alloc.h"
#include "detours.h"
#include "vmthooks.h"
#include "stack.h"
#include "scanner.h"
#include "../pelite/image.h"

namespace toolkit
{


int TestAlloc()
{
	int fail = 0;
	unsigned char* p1 = rweAligned<16>( 63 );
	if ( !rweValidate() ) ++fail;
	unsigned char* p2 = rweAlloc( 23 );
	if ( !rweValidate() ) ++fail;
	unsigned char* p3 = rweAlloc( 22 );
	if ( !rweValidate() ) ++fail;
	rweFree( p2 ); p2 = nullptr;
	if ( !rweValidate() ) ++fail;
	unsigned char* p4 = rweAligned<1024>( 4093 );
	if ( !rweValidate() ) ++fail;
	rweFree( p4 ); p4 = nullptr;
	if ( !rweValidate() ) ++fail;
	rweFree( p1 ); p1 = nullptr;
	if ( !rweValidate() ) ++fail;
	rweFree( p3 ); p3 = nullptr;
	if ( !rweValidate() ) ++fail;
	return fail;
}
template< typename Detour >
int TestDetour()
{
	struct _t {
		static NOINLINE int __stdcall TestHook( int x ) { return x-1; }
		static NOINLINE int __stdcall TestFunc( int x ) { return x+1; }
	};
	int fail = 0;
	typedef int (__stdcall* Fn)( int );

	// Create the detour
	codeptr_t ptr = (codeptr_t)_t::TestHook;
	if ( *ptr==0xE9 ) ptr = ptr + 5 + *(unsigned*)(ptr+1);
	Detour det( ptr, _t::TestFunc );

	// Call original (unhooked)
	if ( det.Call<Fn>()( 4 )!=3 )
		++fail;
	// Call the original (hooked)
	if ( det.Function<Fn>()( 5 )!=6 )
		++fail;
	// Call original directly (hooked)
	if ( _t::TestHook( 8 )!=9 )
		++fail;
	// Call the original (unhooked)
	det.Unhook();
	if ( det.Call<Fn>()( 6 )!=5 )
		++fail;
	// Call original directly (unhooked)
	if ( _t::TestHook( 7 )!=6 )
		++fail;

	return fail;
}
static __declspec(naked) NOINLINE int __stdcall WiretapMe( int x )
{
__asm
{
	push ebp
	mov ebp, esp
	mov eax, dword ptr [ebp+8]
	add eax, eax
	sub eax, 1
	pop ebp
	retn 4
}
}
template< typename Wiretap >
int TestWiretap()
{
	struct _t
	{
		static void LIBEXCC TapIt( BaseWiretap& tap, Wiretap::Context& ctx )
		{
			StackFrame<int,int>& stack = *stack.Get( (void*)ctx.ebp, 0 );
			++ctx.eax;
		}
	};
	int fail = 0;

	// Create the wiretap
	codeptr_t ptr = (codeptr_t)WiretapMe;
	if ( *ptr==0xE9 ) ptr = (ptr+5)+(unsigned&)ptr[1];
	Wiretap tap( ptr+6, &_t::TapIt );

	// Let's see if it works
	if ( WiretapMe( 5 )!=0xB )
		++fail;
	// Try unhooked
	tap.Unhook();
	if ( WiretapMe( 5 )!=9 )
		++fail;

	return fail;
}
int TestScanner()
{
	int fail = 0;

	// General test
	const char* hay = "on\x12oar\x12\x03\xC1\xC2\xC3ninja";
	const char* pattern = "12+r1*'ninja'-C1C2";
	Scanner scan( (void*)hay, (void*)(hay+strlen(hay)+1), nullptr, nullptr );
	if ( scan( pattern )!=(hay+6) || scan[0]!=(hay+11) )
		++fail;

	// Quicksearch test
	scan.Init( (void*)&__ImageBase );
	if ( scan( "3132 2b7231 2a276e696e6a6127" )!=pattern )
		++fail;

	return fail;
}



bool UnitTests()
{
	int fail = 0;

	fail += TestAlloc();
	fail += TestDetour<DetourJmp>();
	fail += TestWiretap<WiretapJmp>();
	fail += TestScanner();

	return fail==0;
}

}
