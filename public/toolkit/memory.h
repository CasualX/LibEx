#ifndef HGUARD_LIBEX_TOOLKIT_MEMORY
#define HGUARD_LIBEX_TOOLKIT_MEMORY
#pragma once

namespace toolkit
{

// Abstraction to write memory (somewhat) safely :)
bool WriteMemory( void* dest, const void* src, unsigned bytes );

// Not sure if a good idea...
// FIXME! Write some documentation for this
bool WriteMemory( void* dest, unsigned mask, ... );

}

#endif // !HGUARD_LIBEX_TOOLKIT_MEMORY
