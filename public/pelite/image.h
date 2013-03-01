#ifndef HGUARD_PELITE_IMAGE
#define HGUARD_PELITE_IMAGE

//----------------------------------------------------------------
// PeLite: Image structs
//----------------------------------------------------------------
// Structs for the internal representation of PE headers
// Mainly copy&paste from winnt.h
//
// FIXME! Make this 64bit compatible!
//

#include "../libex.h"


// Ensure the compiler packs the following structs correctly
#ifdef _MSC_VER
#pragma pack(push,4)
#endif // _MSC_VER

extern "C" struct _IMAGE_DOS_HEADER __ImageBase;

namespace pelite
{
using types::byte;
using types::word;
using types::dword;
using types::qword;
using types::int32;
using types::uint_ptr;



struct Image32
{
	typedef dword ptr;
};
struct Image64
{
	typedef qword ptr;
};




//------------------------------------------------
// Dos Header
//------------------------------------------------

static const word ImageDosHeaderMagic = 0x5A4D;
struct ImageDosHeader
{
	word	e_magic;		// Magic number
	word	e_cblp;			// Bytes on last page of file
	word	e_cp;			// Pages in file
	word	e_crlc;			// Relocations
	word	e_cparhdr;		// Size of header in paragraphs
	word	e_minalloc;		// Minimum extra paragraphs needed
	word	e_maxalloc;		// Maximum extra paragraphs needed
	word	e_ss;			// Initial (relative) SS value
	word	e_sp;			// Initial SP value
	word	e_csum;			// Checksum
	word	e_ip;			// Initial IP value
	word	e_cs;			// Initial (relative) CS value
	word	e_lfarlc;		// File address of relocation table
	word	e_ovno;			// Overlay number
	word	e_res[4];		// Reserved words
	word	e_oemid;		// OEM identifier (for e_oeminfo)
	word	e_oeminfo;		// OEM information; e_oemid specific
	word	e_res2[10];		// Reserved words
	long	e_lfanew;		// File address of new exe header
};






//------------------------------------------------
// Nt Header
//------------------------------------------------


struct ImageFileHeader
{
	word    Machine;
	word    NumberOfSections;
	dword   TimeDateStamp;
	dword   PointerToSymbolTable;
	dword   NumberOfSymbols;
	word    SizeOfOptionalHeader;
	word    Characteristics;

	enum ImageFileChars
	{
		Char_RelocsStripped			= 0x0001,  // Relocation info stripped from file.
		Char_ExecutableFile			= 0x0002,  // File is executable  (i.e. no unresolved externel references).
		Char_LineNumsStripped		= 0x0004,  // Line nunbers stripped from file.
		Char_LocalSymsStripped		= 0x0008,  // Local symbols stripped from file.
		Char_AggresiveWSTrim		= 0x0010,  // Agressively trim working set
		Char_LargeAddressAware		= 0x0020,  // App can handle >2gb addresses
		Char_BytesReversedLo		= 0x0080,  // Bytes of machine word are reversed.
		Char_32BitMachine			= 0x0100,  // 32 bit word machine.
		Char_DebugStripped			= 0x0200,  // Debugging info stripped from file in .DBG file
		Char_RemovableRunFromSwap	= 0x0400,  // If Image is on removable media, copy and run from the swap file.
		Char_NetRunFromSwap			= 0x0800,  // If Image is on Net, copy and run from the swap file.
		Char_System					= 0x1000,  // System File.
		Char_DLL					= 0x2000,  // File is a DLL.
		Char_UPSystemOnly			= 0x4000,  // File should only be run on a UP machine
		Char_BytesReversedHi		= 0x8000,  // Bytes of machine word are reversed.
	};
	enum ImageFileMachine
	{
		Machine_Unknown			= 0,
		Machine_I386			= 0x014C,  // Intel 386.
		Machine_R3000			= 0x0162,  // MIPS little-endian, 0x160 big-endian
		Machine_R4000			= 0x0166,  // MIPS little-endian
		Machine_R10000			= 0x0168,  // MIPS little-endian
		Machine_WCEMIPSV2		= 0x0169,  // MIPS little-endian WCE v2
		Machine_ALPHA			= 0x0184,  // Alpha_AXP
		Machine_SH3				= 0x01A2,  // SH3 little-endian
		Machine_SH3DSP			= 0x01A3,
		Machine_SH3E			= 0x01A4,  // SH3E little-endian
		Machine_SH4				= 0x01A6,  // SH4 little-endian
		Machine_SH5				= 0x01A8,  // SH5
		Machine_ARM				= 0x01C0,  // ARM Little-Endian
		Machine_THUMB			= 0x01C2,
		Machine_AM33			= 0x01D3,
		Machine_PowerPC			= 0x01F0,  // IBM PowerPC Little-Endian
		Machine_PowerPCFP		= 0x01F1,
		Machine_IA64			= 0x0200,  // Intel 64
		Machine_MIPS16			= 0x0266,  // MIPS
		Machine_ALPHA64			= 0x0284,  // ALPHA64
		Machine_MIPSFPU			= 0x0366,  // MIPS
		Machine_MIPSFPU16		= 0x0466,  // MIPS
		Machine_AXP64			= Machine_ALPHA64,
		Machine_TRICORE			= 0x0520,  // Infineon
		Machine_CEF				= 0x0CEF,
		Machine_EBC				= 0x0EBC,  // EFI Byte Code
		Machine_AMD64			= 0x8664,  // AMD64 (K8)
		Machine_M32R			= 0x9041,  // M32R little-endian
		Machine_CEE				= 0xC0EE,
	};
};



struct ImageDataDirectory
{
	dword   VirtualAddress;
	dword   Size;
};
enum ImageDataDirectories
{
	DataDir_Export			= 0,	// Export Directory
	DataDir_Import			= 1,	// Import Directory
	DataDir_Resource		= 2,	// Resource Directory
	DataDir_Exception		= 3,	// Exception Directory
	DataDir_Security		= 4,	// Security Directory
	DataDir_BaseReloc		= 5,	// Base Relocation Table
	DataDir_Debug			= 6,	// Debug Directory
	DataDir_Architecture	= 7,	// Architecture Specific Data
	DataDir_GlobalPtr		= 8,	// RVA of GP
	DataDir_TLS				= 9,	// TLS Directory
	DataDir_LoadConfig		= 10,	// Load Configuration Directory
	DataDir_BoundImport		= 11,	// Bound Import Directory in headers
	DataDir_IAT				= 12,	// Import Address Table
	DataDir_DelayImport		= 13,	// Delay Load Import Descriptors
	DataDir_COMDescriptor	= 14,	// COM Runtime descriptor

	DataDir_Count = 16,
};
struct ImageOptionalHeader
{
	// Standard fields.
	word    Magic;
	byte    MajorLinkerVersion;
	byte    MinorLinkerVersion;
	dword   SizeOfCode;
	dword   SizeOfInitializedData;
	dword   SizeOfUninitializedData;
	dword   AddressOfEntryPoint;
	dword   BaseOfCode;
	dword   BaseOfData;
	// NT additional fields.
	dword   ImageBase;
	dword   SectionAlignment;
	dword   FileAlignment;
	word    MajorOperatingSystemVersion;
	word    MinorOperatingSystemVersion;
	word    MajorImageVersion;
	word    MinorImageVersion;
	word    MajorSubsystemVersion;
	word    MinorSubsystemVersion;
	dword	Win32VersionValue;
	dword   SizeOfImage;
	dword   SizeOfHeaders;
	dword   CheckSum;
	word    Subsystem;
	word    DllCharacteristics;
	dword   SizeOfStackReserve;
	dword   SizeOfStackCommit;
	dword   SizeOfHeapReserve;
	dword   SizeOfHeapCommit;
	dword   LoaderFlags;
	dword   NumberOfRvaAndSizes;
	ImageDataDirectory DataDirectory[DataDir_Count];
};



static const dword ImageNtHeaderSignature = 0x00004550;
struct ImageNtHeader
{
	dword Signature;
	ImageFileHeader FileHeader;
	ImageOptionalHeader OptionalHeader;
};




//------------------------------------------------
// Section header
//------------------------------------------------

struct ImageSectionHeader
{
	char    Name[8];
	//union
	//{
	//	dword   PhysicalAddress;
	//	dword   VirtualSize;
	//} Misc;
	dword   VirtualSize;
	dword   VirtualAddress;
	dword   SizeOfRawData;
	dword   PointerToRawData;
	dword   PointerToRelocations;
	dword   PointerToLinenumbers;
	word    NumberOfRelocations;
	word    NumberOfLinenumbers;
	dword   Characteristics;
};

enum ImageSectionChar
{
	ImageScnCntCode			= 0x00000020,
	ImageScnCntInitData		= 0x00000040,
	ImageScnCntUninitData	= 0x00000080,

	ImageScnMemDiscardable	= 0x02000000,
	ImageScnMemNotCached	= 0x04000000,
	ImageScnMemNotPaged		= 0x08000000,
	ImageScnMemShared		= 0x10000000,
	ImageScnMemExecute		= 0x20000000,
	ImageScnMemRead			= 0x40000000,
	ImageScnMemWrite		= 0x80000000,
};






//------------------------------------------------
// Import Directory
//------------------------------------------------

struct ImageImportDescriptor
{
	// RVA to original unbound IAT (PIMAGE_THUNK_DATA)
	// 0 for terminating null import descriptor
	dword	OriginalFirstThunk;
	// 0 if not bound,
	// -1 if bound, and real date\time stamp in IMAGE_DIRECTORY_ENTRY_BOUND_IMPORT (new BIND)
	// O.W. date/time stamp of DLL bound to (Old BIND)
	dword	TimeDateStamp;			
	// -1 if no forwarders
	dword	ForwarderChain;
	dword	Name;
	// RVA to IAT (if bound this IAT has actual addresses)
	dword	FirstThunk;
};

struct ImageImportThunk
{
	union
	{
		dword Function;
		int32 Ordinal;
		dword AddressOfData; // RVA to ImageImportByName
	};
};

struct ImageImportByName
{
	word Hint;
	char Name[2];
};

// Custom import dir
struct ImageXImportHeader
{
	dword Null; // Matches OriginalFirstThunk of ImageImportDescriptor, pretending there are no imports
	dword Magic; // Magic number to decode the module & imports
	dword Count; // Number of ImageXImportModules
};
struct ImageXImportModule
{
	// bit0: If set, imported module should already be loaded and the 'Name' field is a hash of the module name.
	// bit1: If set, importing by ordinal. Table is filled with ordinals (as opposed to string hashes).
	dword Flags;
	dword Name;
	dword Table; // IAT initially filled with identifiers (hashes/ordinals)
	dword Count; // Number of entries in the table
};



//------------------------------------------------
// Export Directory
//------------------------------------------------

struct ImageExportDirectory
{
	dword   Characteristics;
	dword   TimeDateStamp;
	word    MajorVersion;
	word    MinorVersion;
	dword   Name;
	dword   Base;
	dword   NumberOfFunctions;
	dword   NumberOfNames;
	dword   AddressOfFunctions;     // RVA from base of image
	dword   AddressOfNames;         // RVA from base of image
	dword   AddressOfNameOrdinals;  // RVA from base of image
};



//------------------------------------------------
// Base Relocations
//------------------------------------------------


struct ImageBaseRelocation
{
	dword	VirtualAddress;
	dword	SizeOfBlock;
};
struct ImageBaseRelocBlock
{
	word	Offset : 12;
	word	Type : 4;
};

// Based relocation types.

enum ImageRelocType
{
	ImageRelBasedAbsolute = 0,
	ImageRelBasedHigh = 1,
	ImageRelBasedLow = 2,
	ImageRelBasedHighLow = 3,
	ImageRelBasedHighAdj = 4,
	ImageRelBasedMipsJmpAddr = 5,
	ImageRelBasedMipsJmpAddr16 = 9,
	ImageRelBasedIA64Imm64 = 9,
	ImageRelBasedDir64 = 10,

	ImageRelBasedTotal = 16,
};



//------------------------------------------------
// Debug Directory
//------------------------------------------------

struct ImageDebugDirectory
{
	dword	Characteristics;
	dword	TimeDateStamp;
	word	MajorVersion;
	word	MinorVersion;
	dword	Type;
	dword	SizeOfData;
	dword	AddressOfRawData;
	dword	PointerToRawData;
};

enum ImageDebugType
{
	ImageDebugTypeUnknown = 0,
	ImageDebugTypeCOFF = 1,
	ImageDebugTypeCodeView = 2,
	ImageDebugTypeFPO = 3,
	ImageDebugTypeMisc = 4,
	ImageDebugTypeException = 5,
	ImageDebugTypeFixup = 6,
	ImageDebugTypeOmapToSrc = 7,
	ImageDebugTypeOmapFromSrc = 8,
	ImageDebugTypeBorland = 9,
	ImageDebugTypeReserved10 = 10,
	ImageDebugTypeCLSID = 11,
};



//------------------------------------------------
// TLS Directory
//------------------------------------------------

struct ImageTlsDirectory
{
	dword	StartAddressOfRawData;
	dword	EndAddressOfRawData;
	dword	AddressOfIndex;             // PDWORD
	dword	AddressOfCallBacks;         // PIMAGE_TLS_CALLBACK *
	dword	SizeOfZeroFill;
	dword	Characteristics;
};

typedef void (__stdcall *ImageTlsCallbackFn)( void* DllHandle, dword Reason, void* Reserved );



//------------------------------------------------
// Files Directory
//------------------------------------------------
// Custom data that can replace .rsrc
// Offsets are from the files header.

enum { ImageFilesBlockSize = 32, };

struct ImageFilesHeader
{
	// Size of decryption blocks
	unsigned int BlockSize;
	// Total size of this files directory
	unsigned int TotalSize;
	// Root directory
	unsigned int OffsetToRoot;
	unsigned int SizeOfRoot;
	// Initializing vector for decryption
	unsigned int InitVec[4];
};
struct ImageFilesDesc
{
	// 0 = subdirectory, anything else = specifies encoding/content
	unsigned int ContentType;
	// Offset from section
	unsigned int OffsetToData;
	// Size of content
	unsigned int SizeOfData;
	// Filename
	char Name[20];
};

}


// Restore the packing format
#ifdef _MSC_VER
#pragma pack(pop)
#endif // _MSC_VER


#endif // !HGUARD_PELITE_IMAGE
