#include <pe.h>
#include <stdint.h>
#include <pmm.h>
#include <console.h>
#include <paging.h>

typedef int8_t 	BYTE;
typedef int16_t WORD;
typedef int32_t DWORD;

#define PACKED 						__attribute__((packed))
#define IMAGE_SIZEOF_SHORT_NAME		8

typedef struct {
	WORD  Machine;
	WORD  NumberOfSections;
	DWORD TimeDateStamp;
	DWORD PointerToSymbolTable;
	DWORD NumberOfSymbols;
	WORD  SizeOfOptionalHeader;
	WORD  Characteristics;
} PACKED IMAGE_FILE_HEADER;

typedef struct {
	WORD                 Magic;
	BYTE                 MajorLinkerVersion;
	BYTE                 MinorLinkerVersion;
	DWORD                SizeOfCode;
	DWORD                SizeOfInitializedData;
	DWORD                SizeOfUninitializedData;
	DWORD                AddressOfEntryPoint;
	DWORD                BaseOfCode;
	DWORD                BaseOfData;
	DWORD                ImageBase;
	DWORD                SectionAlignment;
	DWORD                FileAlignment;
	WORD                 MajorOperatingSystemVersion;
	WORD                 MinorOperatingSystemVersion;
	WORD                 MajorImageVersion;
	WORD                 MinorImageVersion;
	WORD                 MajorSubsystemVersion;
	WORD                 MinorSubsystemVersion;
	DWORD                Win32VersionValue;
	DWORD                SizeOfImage;
	DWORD                SizeOfHeaders;
	DWORD                CheckSum;
	WORD                 Subsystem;
	WORD                 DllCharacteristics;
	DWORD                SizeOfStackReserve;
	DWORD                SizeOfStackCommit;
	DWORD                SizeOfHeapReserve;
	DWORD                SizeOfHeapCommit;
	DWORD                LoaderFlags;
	DWORD                NumberOfRvaAndSizes;
  //IMAGE_DATA_DIRECTORY DataDirectory[IMAGE_NUMBEROF_DIRECTORY_ENTRIES];
} PACKED IMAGE_OPTIONAL_HEADER;

typedef struct {
	BYTE  Name[IMAGE_SIZEOF_SHORT_NAME];
	union {
		DWORD PhysicalAddress;
		DWORD VirtualSize;
	} Misc;
	DWORD VirtualAddress;
	DWORD SizeOfRawData;
	DWORD PointerToRawData;
	DWORD PointerToRelocations;
	DWORD PointerToLinenumbers;
	WORD  NumberOfRelocations;
	WORD  NumberOfLinenumbers;
	DWORD Characteristics;
} PACKED IMAGE_SECTION_HEADER;

int load_image(char *bin, entry_f *e, uintptr_t *brk) {
	
	int32_t sigoff = *(int32_t *)(bin + 0x3c);
	char *sig = bin + sigoff;
	
	IMAGE_FILE_HEADER *h = (IMAGE_FILE_HEADER *)(sig + 4);
	
	char *ptr = (char *)h;
	ptr += sizeof(IMAGE_FILE_HEADER);
	
	IMAGE_OPTIONAL_HEADER *opt = (IMAGE_OPTIONAL_HEADER *)ptr;
	
	intptr_t entry = opt->ImageBase + opt->AddressOfEntryPoint;
	*e = (entry_f)entry;
	
	ptr = (char *)opt;
	ptr += h->SizeOfOptionalHeader;
	
	for (int i = 0; i < h->NumberOfSections; i++) {
	
		IMAGE_SECTION_HEADER *sh = (IMAGE_SECTION_HEADER *)ptr;
		
		ptr += sizeof(IMAGE_SECTION_HEADER);
	
		char *vbase = (char *)opt->ImageBase + sh->VirtualAddress;
		char *pbase = bin + sh->PointerToRawData;
	
		for (int off = 0; off < sh->Misc.VirtualSize; off += PAGE_SIZE) {
		
			char *dst = vbase + off;
			char *src = pbase + off;
			void *page = pmm_alloc_block();
			
			map_page(dst, page, PTE_P | PTE_RW | PTE_U);
			memcpy(dst, src, PAGE_SIZE);
			
			uintptr_t end = (uintptr_t)dst + PAGE_SIZE;
			if (end > (uintptr_t)*brk)
				*brk = end;
		}
		
	}

	return 0;
}






















