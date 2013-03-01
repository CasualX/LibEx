
#include "lde.h"
#include "tools.h"
#include "../libex.h"
//#ifdef WIN32
//#include <Windows.h>
//#endif // WIN32

namespace toolkit
{

using namespace types;
using namespace toolkit;


// Copied from somewhere...
NOINLINE unsigned Lde( const unsigned char* in )
{
	const unsigned char* addr = in;

	// Skip prefixes F0h, F2h, F3h, 66h, 67h, D8h-DFh, 2Eh, 36h, 3Eh, 26h, 64h and 65h
	int operandSize = 4;
	int fpu = 0;
	while(*addr == 0xF0 ||
		*addr == 0xF2 ||
		*addr == 0xF3 ||
		(*addr & 0xFC) == 0x64 ||
		(*addr & 0xF8) == 0xD8 ||
		(*addr & 0x7E) == 0x62)
	{
		if(*addr == 0x66)
		{
			operandSize = 2;
		}
		else if((*addr & 0xF8) == 0xD8)
		{
			fpu = *addr++;
			break;
		}
		addr++;
	}

	// Skip two-byte opcode byte
	bool twoByte = false;
	if(*addr == 0x0F)
	{
		twoByte = true;
		addr++;
	} 

	// Skip opcode byte
	unsigned char opcode = *addr++;

	// Skip mod R/M byte
	unsigned char modRM = 0xFF;
	if(fpu)
	{
		if((opcode & 0xC0) != 0xC0)
		{
			modRM = opcode;
		}
	}
	else if(!twoByte)
	{
		if((opcode & 0xC4) == 0x00 ||
			(opcode & 0xF4) == 0x60 && ((opcode & 0x0A) == 0x02 || (opcode & 0x09) == 0x9) ||
			(opcode & 0xF0) == 0x80 ||
			(opcode & 0xF8) == 0xC0 && (opcode & 0x0E) != 0x02 ||
			(opcode & 0xFC) == 0xD0 ||
			(opcode & 0xF6) == 0xF6)
		{
			modRM = *addr++;
		}
	}
	else
	{
		if((opcode & 0xF0) == 0x00 && (opcode & 0x0F) >= 0x04 && (opcode & 0x0D) != 0x0D ||
			(opcode & 0xF0) == 0x30 ||
			opcode == 0x77 ||
			(opcode & 0xF0) == 0x80 ||
			(opcode & 0xF0) == 0xA0 && (opcode & 0x07) <= 0x02 ||
			(opcode & 0xF8) == 0xC8)
		{
			// No mod R/M byte
		}
		else
		{
			modRM = *addr++;
		}
	} 

	// Skip SIB
	if((modRM & 0x07) == 0x04 &&
		(modRM & 0xC0) != 0xC0)
	{
		addr += 1;   // SIB
	}

	// Skip displacement
	if((modRM & 0xC5) == 0x05) addr += 4;   // Dword displacement, no base
	if((modRM & 0xC0) == 0x40) addr += 1;   // Byte displacement
	if((modRM & 0xC0) == 0x80) addr += 4;   // Dword displacement

	// Skip immediate
	if(fpu)
	{
		// Can't have immediate operand
	}
	else if(!twoByte)
	{
		if((opcode & 0xC7) == 0x04 ||
			(opcode & 0xFE) == 0x6A ||   // PUSH/POP/IMUL
			(opcode & 0xF0) == 0x70 ||   // Jcc
			opcode == 0x80 ||
			opcode == 0x83 ||
			(opcode & 0xFD) == 0xA0 ||   // MOV
			opcode == 0xA8 ||            // TEST
			(opcode & 0xF8) == 0xB0 ||   // MOV
			(opcode & 0xFE) == 0xC0 ||   // RCL
			opcode == 0xC6 ||            // MOV
			opcode == 0xCD ||            // INT
			(opcode & 0xFE) == 0xD4 ||   // AAD/AAM
			(opcode & 0xF8) == 0xE0 ||   // LOOP/JCXZ
			opcode == 0xEB ||
			opcode == 0xF6 && (modRM & 0x30) == 0x00)   // TEST
		{
			addr += 1;
		}
		else if((opcode & 0xF7) == 0xC2)
		{
			addr += 2;   // RET
		}
		else if((opcode & 0xFC) == 0x80 ||
			(opcode & 0xC7) == 0x05 ||
			(opcode & 0xF8) == 0xB8 ||
			(opcode & 0xFE) == 0xE8 ||      // CALL/Jcc
			(opcode & 0xFE) == 0x68 ||
			(opcode & 0xFC) == 0xA0 ||
			(opcode & 0xEE) == 0xA8 ||
			opcode == 0xC7 ||
			opcode == 0xF7 && (modRM & 0x30) == 0x00)
		{
			addr += operandSize;
		}
	}
	else
	{
		if(opcode == 0xBA ||            // BT
			opcode == 0x0F ||            // 3DNow!
			(opcode & 0xFC) == 0x70 ||   // PSLLW
			(opcode & 0xF7) == 0xA4 ||   // SHLD
			opcode == 0xC2 ||
			opcode == 0xC4 ||
			opcode == 0xC5 ||
			opcode == 0xC6)
		{
			addr += 1;
		}
		else if((opcode & 0xF0) == 0x80)
		{
			addr += operandSize;   // Jcc -i
		}
	}

	return getoffset( in, addr );
}





NOINLINE unsigned LdeCopy( const unsigned char* func, unsigned char* dest, unsigned bytes )
{
	unsigned i, len;
	for ( i = 0; i<bytes; i+=len )
	{
		unsigned char opcode = func[i];
		// Fixup jmp/call instructions
		if ( opcode==0xE9 || opcode==0xE8 )
		{
			dest[0] = opcode;
			const unsigned char* target = (func+i+5) + (unsigned&)func[i+1];
			((unsigned&)dest[1]) = (dest+5) - target;
			len = 5;
			dest += 5;
		}
		//// Fixup jmp short
		//else if ( opcode==0xEB )
		//{
		//	dest[0] = 0xE9;
		//	const unsigned char* target = func + i + (char&)func[i+1] + 2;
		//	((unsigned&)dest[1]) = ( dest - target ) + 5;
		//	len = 2;
		//	dest += 5;
		//}
		// Find the length of the instruction
		else if ( len = Lde( func+i ) )
		{
			__movsb( dest, func+i, len );
			dest += len;
		}
		// Unknown instruction, fail
		else return 0;
		assert( len>0 );
	}

	// Add jmp to original code
	dest[0] = 0xE9;
	((unsigned&)dest[1]) = ((func + i) - dest) - 5;
	i += 5;

	// Just to make sure
	//::FlushInstructionCache( (HANDLE)-1, dest-i, i );
	return i;
}

//int LdeCopy( const byte* src, byte* dest, uint bytes )
//{
//	uint i;
//	int len;
//	for ( i = 0; i<bytes; i += len, src += len )
//	{
//		// Binary search for special case opcodes
//		byte opcode = *src;
//		if ( opcode==0xE3 )
//		{
//			// jecxz doesn't have a 32bit counter part so *shrug*
//			// I could decompose it into sub ecx, 1 and jz but oh well
//			return -1;
//		}
//		if ( opcode<0xE3 )
//		{
//			if ( opcode<0x70 )
//			{
//				if ( opcode==0x0F )
//					goto jcc_rel32;
//				goto def;
//			}
//			if ( opcode<0x80 )
//				goto jcc_rel8;
//			goto def;
//		}
//		if ( opcode==0xE8 || opcode==0xE9 )
//			goto jmpcall_rel32;
//		if ( opcode!=0xEB )
//			goto def;
//
////jmp_rel8:
//		{
//			// Absolute ptr
//			const byte* p = (src+2)+getmember<char>( src, 1 );
//			// Write rel32 version
//			*dest = 0xE9;
//			write<dword>( dest+1, getoffset( dest+5, p ) );
//			// Move on
//			dest += 5;
//			len = 2;
//			continue;
//		}
//jcc_rel32:
//		if ( (src[1]&0xF0)==0x80 )
//		{
//			dest[0] = src[0];
//			dest[1] = src[1];
//			// Absolute ptr
//			const byte* p = (src+6)+getmember<dword>( src, 2 );
//			// New relative ptr
//			write<dword>( dest+2, getoffset( dest+6, p ) );
//			// Move on
//			dest += 6;
//			len = 6;
//			continue;
//		}
//def:
//		{
//			len = Lde( (void*)src );
//			// Unknown or unsupported opcode
//			if ( len<=0 )
//				return len;
//			// Copy bytes
//			for ( int i = 0; i<len; i++ )
//				*dest++ = src[i];
//			continue;
//		}
//jcc_rel8:
//		{
//			// Absolute ptr
//			const byte* p = (src+2)+getmember<char>( src, 1 );
//			// Write rel32 version
//			dest[0] = 0x0F;
//			dest[1] = 0x80|(src[0]&0xF);
//			write<dword>( dest+2, getoffset( dest+6, p ) );
//			// Move on
//			dest += 6;
//			len = 2;
//			continue;
//		}
//jmpcall_rel32:
//		{
//			*dest = *src;
//			// Absolute ptr
//			const byte* p = (src+5)+getmember<dword>( src, 0x1 );
//			// New relative ptr
//			write<dword>( dest+1, getoffset( dest+5, p ) );
//			// Move on
//			dest += 5;
//			len = 5;
//			continue;
//		}
//	}
//	return i;
//}


}
