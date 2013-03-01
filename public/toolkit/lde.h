#ifndef HGUARD_LIBEX_TOOLKIT_LDE
#define HGUARD_LIBEX_TOOLKIT_LDE
#pragma once

//----------------------------------------------------------------
// Toolkit: Length Disassembling
//----------------------------------------------------------------

namespace toolkit
{

// Finds the length of the instruction pointed at
// Returns zero if invalid/unknown instruction.
unsigned Lde( const unsigned char* addr );

	
// Copies (at least 'bytes' bytes) code while fixing relative addressing (jmp & call instructions)
// Appends jmp src+total_bytes_copied (continue where it left off), because this is a helper for detours etc.
// Returns total number of bytes written, zero if an invalid instruction is found.
unsigned LdeCopy( const unsigned char* src, unsigned char* dest, unsigned bytes );

}

#endif // !HGUARD_LIBEX_TOOLKIT_LDE
