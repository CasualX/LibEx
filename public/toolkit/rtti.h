#ifndef HGUARD_LIBEX_TOOLKIT_RTTI
#define HGUARD_LIBEX_TOOLKIT_RTTI
#pragma once

//----------------------------------------------------------------
// Toolkit: RTTI structs for 32bit msvc
//----------------------------------------------------------------
// See www.openrce.org/articles/full_view/23 for a more in depth explanation
// See rtti1.gif and rtti2.gif for examples (taken from the above link in case it should go down).

#include "../libex.h"
#include "tools.h"

namespace toolkit
{
using types::dword;




// Declarations
struct RTTICompleteObjectLocator;
struct RTTIClassHierarchyDescriptor;
struct RTTITypeDescriptor;
typedef RTTIClassHierarchyDescriptor* RTTIBaseClassArray;

// Get the CompleteObjectLocator from an instance.
static inline const RTTICompleteObjectLocator* RTTIGet( const void* inst, long offset = 0 )
{
	return reinterpret_cast<const RTTICompleteObjectLocator*>( getvtable( inst, offset )[ -1 ] );
}

struct RTTITypeDescriptor
{
	void** pVMT;
	dword spare;
	char name[1];
};

struct RTTICompleteObjectLocator
{
	dword signature;	// Always zero ?
	dword offset;		// Offset of this vtable in the complete class.
	dword cdOffset;		// Constructor displacement offset.
	RTTITypeDescriptor* pTypeDescriptor; // TypeDescriptor of the complete class.
	RTTIClassHierarchyDescriptor* pClassDescriptor; // Describes inheritance hierarchy.
};

struct RTTIClassHierarchyDescriptor
{
	dword signature;		// Always zero?
	dword attributes;		// Bit 0 set = multiple inheritance, bit 1 set = virtual inheritance.
	dword numBaseClasses;	// Number of classes in pBaseClassArray.
	RTTIBaseClassArray* pBaseClassArray;
};

struct PMD
{
	int mdisp;	// Member displacement.
	int pdisp;	// Vbtable displacement.
	int vdisp;	// Displacement inside vbtable.
};

struct RTTIBaseClassDescriptor
{
	RTTITypeDescriptor* pTypeDescriptor; // Type descriptor of the class.
	dword numContainedBases;	// Number of nested classes following in the Base Class Array.
	PMD disp;					// Pointer-to-member displacement info.
	dword attributes;			// Flags, usually 0.
};

}

#endif // !HGUARD_LIBEX_TOOLKIT_RTTI
