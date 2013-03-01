#ifndef HGUARD_LIBEX_TOOLKIT_CODEGENX86
#define HGUARD_LIBEX_TOOLKIT_CODEGENX86
#pragma once

//----------------------------------------------------------------
// Toolkit: Code generation
//----------------------------------------------------------------
// Only for x86

#include "../libex.h"
#include "tools.h"

namespace rtasm
{

using toolkit::getoffset;
using types::byte;
using types::word;
using types::dword;

typedef types::int8 rel8;
typedef types::int32 rel32;


// General purpose registers
enum gpreg_t
{
	GPREG_EAX = 0, GPREG_AL = 0, // 000
	GPREG_ECX = 1, GPREG_CL = 1, // 001
	GPREG_EDX = 2, GPREG_DL = 2, // 010
	GPREG_EBX = 3, GPREG_BL = 3, // 011
	GPREG_ESP = 4, GPREG_AH = 4, // 100
	GPREG_EBP = 5, GPREG_CH = 5, // 101
	GPREG_ESI = 6, GPREG_DH = 6, // 110
	GPREG_EDI = 7, GPREG_BH = 7, // 111
	GPREG_MASK = 7,
};

// Flags
enum eflags_t
{
	EFLAG_CARRY = (1<<0),
	EFLAG_PARITY = (1<<2),
	EFLAG_ADJUST = (1<<4),
	EFLAG_ZERO = (1<<6),
	EFLAG_SIGN = (1<<7),
	EFLAG_TRAP = (1<<8),
	EFLAG_INTERRUPT = (1<<9),
	EFLAG_DIRECTION = (1<<10),
	EFLAG_OVERFLOW = (1<<11),
};

// Conditionals
enum cond_t
{
	COND_OVERFLOW = 0x0,

	COND_NOTOVERFLOW = 0x1,

	COND_BELOW = 0x2,
	COND_CARRY = 0x2,
	COND_NOTABOVEOREQUAL = 0x2,

	COND_ABOVEOREQUAL = 0x3,
	COND_NOTCARRY = 0x3,
	COND_NOTBELOW = 0x3,

	COND_EQUAL = 0x4,
	COND_ZERO = 0x4,

	COND_NOTEQUAL = 0x5,
	COND_NOTZERO = 0x5,

	COND_BELOWOREQUAL = 0x6,
	COND_NOTABOVE = 0x6,

	COND_ABOVE = 0x7,
	COND_NOTBELOWOREQUAL = 0x7,

	COND_SIGN = 0x8,

	COND_NOTSIGN = 0x9,

	COND_PARITY = 0xA,
	COND_PARITYEVEN = 0xA,

	COND_NOTPARITY = 0xB,
	COND_PARITYODD = 0xB,

	COND_LESS = 0xC,
	COND_NOTGREATEROREQUAL = 0xC,

	COND_GREATEROREQUAL = 0xD,
	COND_NOTLESS = 0xD,

	COND_LESSOREQUAL = 0xE,
	COND_NOTGREATER = 0xE,

	COND_GREATER = 0xF,
	COND_NOTLESSOREQUAL = 0xF,
};

inline int CalcOffset( void* instr, int size, void* dest )
{
	using namespace toolkit;
	return getoffset( make_ptr<void*>( instr, size ), dest );
}



// Simplified class for writing asm
class Writer
{
public:
	Writer() { }
	Writer( void* buf, void* dest ) : it((unsigned char*)buf), diff( (char*)dest - (char*)buf ) { }

protected:
	template< typename T >
	inline void _write( T t )
	{
		*(T*)it = t;
		it += sizeof(T);
	}


public:


	//------------------------------------------------
	// Prefixes
	//------------------------------------------------

	// 16-bit prefix
	Writer& Prefix16();
	// LOCK prefix
	Writer& Lock();
	// REP(N)(Z) prefix
	Writer& Rep( bool nz );
	// Segment override
	Writer& Segment( int sr );


	//------------------------------------------------
	// Easy ModR/M
	//------------------------------------------------

	// ModR/M: reg
	Writer& ModRM_Reg( gpreg_t reg, gpreg_t rm );
	// ModR/M: [reg]
	Writer& ModRM_Disp0( gpreg_t reg, gpreg_t rm );
	// ModR/M: [reg+rel8]
	Writer& ModRM_Disp8( gpreg_t reg, gpreg_t rm, rel8 disp );
	// ModR/M: [reg+rel32]
	Writer& ModRM_Disp32( gpreg_t reg, gpreg_t rm, rel32 disp );
	// ModR/M: [addr]
	Writer& ModRM_Mem( gpreg_t reg, dword addr );


	//------------------------------------------------
	// Mov instruction
	//------------------------------------------------

	// mov reg32, imm32
	Writer& MovRegImm( gpreg_t reg, dword imm );
	// mov mem, eax
	Writer& MovMemEax( dword mem );
	// mov eax, mem
	Writer& MovEaxMem( dword mem );
	// mov reg, modrm
	Writer& MovFromModRM( bool bit8 = false );
	// mov modrm, reg
	Writer& MovToModRM( bool bit8 = false );


	//------------------------------------------------
	// Addition
	//------------------------------------------------

	// add al, imm8
	Writer& AddAlImm8( byte imm );
	// add eax, imm32
	Writer& AddEaxImm32( dword imm );
	// add reg32, imm32
	Writer& AddRegImm32( gpreg_t reg, dword imm );
	// add reg32, imm8
	Writer& AddRegImm8( gpreg_t reg, byte imm );


	//------------------------------------------------
	// Subtraction
	//------------------------------------------------

	// sub reg32, imm32
	Writer& SubRegImm( gpreg_t reg, dword imm );
	// sub reg32, imm8
	Writer& SubRegImm( gpreg_t reg, byte imm );


	//------------------------------------------------
	// Bitwise OR
	//------------------------------------------------

	// or reg8, imm8
	Writer& OrReg8Imm8( gpreg_t dest, byte imm );
	// or reg32, imm32
	Writer& OrReg32Imm32( gpreg_t dest, dword imm );
	// or reg8, reg8
	Writer& OrReg8Reg8( gpreg_t dest, gpreg_t src );
	// or reg32, reg32
	Writer& OrReg32Reg32( gpreg_t dest, gpreg_t src );


	//------------------------------------------------
	// Test
	//------------------------------------------------

	// test reg, reg
	Writer& TestRegReg( gpreg_t a1, gpreg_t a2, bool bit8 = false );


	//------------------------------------------------
	// Stack manipulation
	//------------------------------------------------

	// push reg32
	Writer& PushReg( gpreg_t reg );
	// pop reg32
	Writer& PopReg( gpreg_t reg );
	// push imm32
	Writer& PushImm32( dword imm );
	// push imm8
	Writer& PushImm8( rel8 imm );
	// pushad
	Writer& Pushad();
	// popad
	Writer& Popad();
	// pushfd
	Writer& Pushfd();
	// popfd
	Writer& Popfd();



	//------------------------------------------------
	// Instruction flow
	//------------------------------------------------

	// call rel32
	Writer& CallRel32( rel32 dest );
	// call reg
	Writer& CallReg( gpreg_t reg );
	// call [reg+disp32]
	Writer& CallRegDisp32( gpreg_t reg, rel32 disp );
	// jmp rel8
	Writer& JmpRel8( rel8 dest );
	// jmp rel32
	Writer& JmpRel32( rel32 dest );
	// jcc rel8
	Writer& JccRel8( cond_t cc, rel8 dest );
	// jcc rel32
	Writer& JccRel32( cond_t cc, rel32 dest );
	// jecxz rel8
	Writer& JecxzRel8( rel8 dest );
	// retn
	Writer& Retn();
	// retn x
	Writer& RetnX( word x );


	//------------------------------------------------
	// Misc
	//------------------------------------------------

	// int 3
	Writer& Break();
	// nop
	Writer& Nop();


public:
	unsigned char* it;
	unsigned int diff;
};




// Inlines


//------------------------------------------------
// Prefixes
//------------------------------------------------
inline Writer& Writer::Prefix16()
{
	_write<byte>( 0x66 );
	return *this;
}
inline Writer& Writer::Lock()
{
	_write<byte>( 0xF0 );
	return *this;
}


//------------------------------------------------
// ModR/M
//------------------------------------------------
inline Writer& Writer::ModRM_Reg( gpreg_t reg, gpreg_t rm )
{
	_write<byte>( 0xC0 | (reg<<3) | rm );
	return *this;
}
inline Writer& Writer::ModRM_Disp0( gpreg_t reg, gpreg_t rm )
{
	// Use SIB for EBP/ESP
	assert( rm!=GPREG_ESP && rm!=GPREG_EBP );
	_write<byte>( 0x00 | (reg<<3) | rm );
	return *this;
}
inline Writer& Writer::ModRM_Disp8( gpreg_t reg, gpreg_t rm, rel8 disp )
{
	_write<byte>( 0x40 | (reg<<3) | rm );
	_write<rel8>( disp );
	return *this;
}
inline Writer& Writer::ModRM_Disp32( gpreg_t reg, gpreg_t rm, rel32 disp )
{
	_write<byte>( 0x80 | (reg<<3) | rm );
	_write<rel32>( disp );
	return *this;
}
inline Writer& Writer::ModRM_Mem( gpreg_t reg, dword addr )
{
	_write<byte>( 0x00 | (reg<<3) | 5 );
	_write<dword>( addr );
	return *this;
}


//------------------------------------------------
// Mov instruction
//------------------------------------------------

inline Writer& Writer::MovRegImm( gpreg_t reg, dword imm )
{
	_write<byte>( 0xB8|reg );
	_write<dword>( imm );
	return *this;
}
inline Writer& Writer::MovMemEax( dword mem )
{
	_write<byte>( 0xA3 );
	_write<dword>( mem );
	return *this;
}
inline Writer& Writer::MovEaxMem( dword mem )
{
	_write<byte>( 0xA1 );
	_write<dword>( mem );
	return *this;
}
inline Writer& Writer::MovFromModRM( bool bit8 )
{
	_write<byte>( 0x8A | static_cast<byte>(!bit8) );
	return *this;
}
inline Writer& Writer::MovToModRM( bool bit8 )
{
	_write<byte>( 0x88 | static_cast<byte>(!bit8) );
	return *this;
}


//------------------------------------------------
// Addition
//------------------------------------------------

inline Writer& Writer::AddAlImm8( byte imm )
{
	_write<byte>( 0x04 );
	_write<byte>( imm );
	return *this;
}
inline Writer& Writer::AddEaxImm32( dword imm )
{
	_write<byte>( 0x05 );
	_write<dword>( imm );
	return *this;
}
inline Writer& Writer::AddRegImm32( gpreg_t reg, dword imm )
{
	_write<byte>( 0x81 );
	_write<byte>( 0xC0|reg );
	_write<dword>( imm );
	return *this;
}
inline Writer& Writer::AddRegImm8( gpreg_t reg, byte imm )
{
	_write<byte>( 0x83 );
	_write<byte>( 0xC0|reg );
	_write<byte>( imm );
	return *this;
}

//------------------------------------------------
// Subtraction
//------------------------------------------------

inline Writer& Writer::SubRegImm( gpreg_t reg, dword imm )
{
	_write<byte>( 0x81 );
	_write<byte>( 0xE8|reg );
	_write<dword>( imm );
	return *this;
}
inline Writer& Writer::SubRegImm( gpreg_t reg, byte imm )
{
	_write<byte>( 0x83 );
	_write<byte>( 0xE8|reg );
	_write<byte>( imm );
	return *this;
}

//------------------------------------------------
// Bitwise OR
//------------------------------------------------

inline Writer& Writer::OrReg8Imm8( gpreg_t dest, byte imm )
{
	_write<byte>( 0x0C );
	_write<byte>( imm );
	return *this;
}
inline Writer& Writer::OrReg32Imm32( gpreg_t dest, dword imm )
{
	_write<byte>( 0x0D );
	_write<dword>( imm );
	return *this;
}
inline Writer& Writer::OrReg8Reg8( gpreg_t dest, gpreg_t src )
{
	_write<byte>( 0x0A );
	_write<byte>( 0xC0 | (dest<<3) | src );
	return *this;
}
inline Writer& Writer::OrReg32Reg32( gpreg_t dest, gpreg_t src )
{
	_write<byte>( 0x0B );
	_write<byte>( 0xC0 | (dest<<3) | src );
	return *this;
}

//------------------------------------------------
// Test
//------------------------------------------------

inline Writer& Writer::TestRegReg( gpreg_t a1, gpreg_t a2, bool bit8 )
{
	_write<byte>( 0x84 | static_cast<byte>(!bit8) );
	_write<byte>( 0xC0 | (a1<<3) | a2 );
	return *this;
}

//------------------------------------------------
// Stack manipulation
//------------------------------------------------

inline Writer& Writer::PushReg( gpreg_t reg )
{
	_write<byte>( 0x50|reg );
	return *this;
}
inline Writer& Writer::PopReg( gpreg_t reg )
{
	_write<byte>( 0x58|reg );
	return *this;
}
inline Writer& Writer::PushImm32( dword imm )
{
	_write<byte>( 0x68 );
	_write<dword>( imm );
	return *this;
}
inline Writer& Writer::PushImm8( rel8 imm )
{
	_write<byte>( 0x6A );
	_write<byte>( imm );
	return *this;
}
inline Writer& Writer::Pushad()
{
	_write<byte>( 0x60 );
	return *this;
}
inline Writer& Writer::Popad()
{
	_write<byte>( 0x61 );
	return *this;
}
inline Writer& Writer::Pushfd()
{
	_write<byte>( 0x9C );
	return *this;
}
inline Writer& Writer::Popfd()
{
	_write<byte>( 0x9D );
	return *this;
}


//------------------------------------------------
// Instruction flow
//------------------------------------------------

// call rel32
inline Writer& Writer::CallRel32( rel32 dest )
{
	_write<byte>( 0xE8 );
	_write<rel32>( dest );
	return *this;
}
// call reg
inline Writer& Writer::CallReg( gpreg_t reg )
{
	_write<byte>( 0xFF );
	_write<byte>( 0xD0|reg );
	return *this;
}
// call [reg+disp32]
inline Writer& Writer::CallRegDisp32( gpreg_t reg, rel32 disp )
{
	_write<byte>( 0xFF );
	_write<byte>( 0x90 | reg );
	_write<dword>( disp );
	return *this;
}
// jmp rel8
inline Writer& Writer::JmpRel8( rel8 dest )
{
	_write<byte>( 0xEB );
	_write<byte>( dest );
	return *this;
}
// jmp rel32
inline Writer& Writer::JmpRel32( rel32 dest )
{
	_write<byte>( 0xE9 );
	_write<rel32>( dest );
	return *this;
}
// jcc rel8
inline Writer& Writer::JccRel8( cond_t cc, rel8 dest )
{
	_write<byte>( 0x70|cc );
	_write<rel8>( dest );
	return *this;
}
// jcc rel32
inline Writer& Writer::JccRel32( cond_t cc, rel32 dest )
{
	_write<byte>( 0x0F );
	_write<byte>( 0x80|cc );
	_write<rel32>( dest );
	return *this;
}
// jecxz rel8
inline Writer& Writer::JecxzRel8( rel8 dest )
{
	_write<byte>( 0xE3 );
	_write<rel8>( dest );
	return *this;
}
// retn
inline Writer& Writer::Retn()
{
	_write<byte>( 0xC3 );
	return *this;
}
// retn x
inline Writer& Writer::RetnX( word x )
{
	_write<byte>( 0xC2 );
	_write<word>( x );
	return *this;
}

//------------------------------------------------
// Misc
//------------------------------------------------

inline Writer& Writer::Break()
{
	_write<byte>( 0xCC );
	return *this;
}
inline Writer& Writer::Nop()
{
	_write<byte>( 0x90 );
	return *this;
}

}

#endif // !HGUARD_LIBEX_TOOLKIT_CODEGENX86
