#include <proc.h>

#include <paging.h>
#include <pmm.h>
#include <types.h>
#include <kalloc.h>
#include <console.h>
#include <molly.h>
#include <param.h>
#include <gdt.h>
#include <idt.h>

#define PDE_SHIFT 		(22)

__attribute__((aligned(PAGE_SIZE)))
pd_t init_pd = {
	.entries = {
		[0]
			= (0) | PTE_P | PTE_RW | PTE_PS | PTE_U,
		[KERNEL_BASE >> PDE_SHIFT]
			= (0) | PTE_P | PTE_RW | PTE_PS
	}
};

#define MAX_PROCS	(16)

static proc_t procs[MAX_PROCS];
static int cp_num = 0;

proc_t *get_proc(size_t num) {
	return &procs[num];
}

proc_t *cproc() {
	return &procs[cp_num];
}

extern pd_t init_pd;
void user_main();

//proc_asm.asm
void user_jump(void *ksp);

#define _4MB		(4 * 1024 * 1024)
#define PUSH(v)		(t->ksp--, *t->ksp = (int)(v))
#define UPUSH(v)	(t->usp--, *t->usp = (int)(v))

extern none_t _RAMFS_START;

#define ROUND_UP(v)	((char *)(((uintptr_t)(v) + 7) & ~(7)))

static char		*init_start;
static size_t 	init_size;

static int find_init() {

	char *ptr = (char *)&_RAMFS_START;
	size_t count = *(size_t *)ptr;
	ptr += 4;
	
	for (size_t i = 0; i < count; i++) {
	
		ptr = ROUND_UP(ptr);
		char *name = ptr;
		ptr += strlen(name) + 1;
		
		ptr = ROUND_UP(ptr);
		init_size = *(size_t *)ptr;
		ptr += 4;
		
		ptr = ROUND_UP(ptr);
		init_start = ptr;
		ptr += init_size;
		
		if (!strcmp("init.exe", name))
			return 1;
	}
	
	return 0;
}

typedef int8_t 	BYTE;
typedef int16_t WORD;
typedef int32_t DWORD;

#define PACKED __attribute__((packed))

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

#define IMAGE_SIZEOF_SHORT_NAME		8

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

typedef struct {
	DWORD VirtualAddress;
	DWORD Size;
} PACKED IMAGE_DATA_DIRECTORY;

static intptr_t init_brk = 0;
static entry_f	init_entry;

static void load_image() {

	if (!find_init()) {
		panic("init not found");
	}
	
	//kprintfln("start: %s", init_start);
	intptr_t sigoff = *(int32_t *)(init_start + 0x3c);
	//kprintfln("sigoff: %x", sigoff);
	char *sig = init_start + sigoff;
	//kprintfln("sig: %s", sig);
	
	IMAGE_FILE_HEADER *h = (IMAGE_FILE_HEADER *)(sig + 4);
	//kprintfln("sections: %d", h->NumberOfSections);
	
	char *ptr = (char *)h;
	ptr += sizeof(IMAGE_FILE_HEADER);
	
	IMAGE_OPTIONAL_HEADER *opt = (IMAGE_OPTIONAL_HEADER *)ptr;
	
	//kprintfln("NumberOfRvaAndSizes: %d", opt->NumberOfRvaAndSizes);
	//IMAGE_DATA_DIRECTORY *brt = (char *)opt + 136;
	//kprintfln("brt addr: %x", brt->VirtualAddress);
	//kprintfln("brt size: %d", brt->Size);
	
	intptr_t entry = opt->ImageBase + opt->AddressOfEntryPoint;
	init_entry = (entry_f)entry;
	//kprintfln("entry: %x", entry);
	//kprintfln("magic: %x", opt->Magic);
	//kprintfln("align: %x", opt->SectionAlignment);
	//kprintfln("base: %x", opt->ImageBase);
	
	ptr = (char *)opt;
	ptr += h->SizeOfOptionalHeader;
	
	for (int i = 0; i < h->NumberOfSections; i++) {
	
		IMAGE_SECTION_HEADER *sh = (IMAGE_SECTION_HEADER *)ptr;
		
		/*
		kprintfln(
			"name: %s, size: %x, addr: %d",
			sh->Name,
			sh->Misc.VirtualSize,
			sh->VirtualAddress);
		*/
		
		ptr += sizeof(IMAGE_SECTION_HEADER);
	
		char *vbase = (char *)opt->ImageBase + sh->VirtualAddress;
		char *pbase = init_start + sh->PointerToRawData;
	
		for (int off = 0; off < sh->Misc.VirtualSize; off += PAGE_SIZE) {
			char *dst = vbase + off;
			char *src = pbase + off;
			void *page = pmm_alloc_block();
			map_page(dst, page, PTE_P | PTE_RW | PTE_U);
			memcpy(dst, src, PAGE_SIZE);
			
			intptr_t end = (intptr_t)dst + PAGE_SIZE;
			if (end > init_brk)
				init_brk = end;
		}
		
	}
}

void run_init() {

	//remove lower mapping
	init_pd.entries[0] = 0;
	reloadPDBR();
	
	//load init.exe from ram disk
	load_image();

	for (int i = 0; i < MAX_PROCS; i++)
		procs[i].state = S_FREE;
		
	proc_t *p = &procs[0];
	
	p->state	= S_USED;
	p->pd 		= &init_pd;
	p->brk 		= (void *)init_brk;
	p->ct_num 	= 0;
	
	for (int i = 0; i < MAX_THREADS; i++)
		p->threads[i].state = S_FREE;
		
	thread_t *t = &p->threads[0];
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop 	= t->kstack + 1024;
	t->ksp 		= t->ktop;
	t->entry 	= init_entry;
	
	UPUSH(0);	//argv
	UPUSH(0);	//argc
	UPUSH(0);	//ret
	
	PUSH(0x20 | 0x3);		//ss
	PUSH(t->usp);			//esp
	PUSH(read_flags());		//eflags
	PUSH(0x18 | 0x3);		//cs
	PUSH(t->entry);			//eip
	
	set_tss(t->ktop);
	
	user_jump(t->ksp);
}

void user_mode() {

	for (int i = 0; i < MAX_PROCS; i++)
		procs[i].state = S_FREE;

	proc_t *p = &procs[0];
	
	p->state	= S_USED;
	p->pd 		= &init_pd;
	p->brk 		= (void *)_4MB;
	p->ct_num 	= 0;

	for (int i = 0; i < MAX_THREADS; i++)
		p->threads[i].state = S_FREE;
	
	thread_t *t = &p->threads[0];
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop 	= t->kstack + 1024;
	t->ksp 		= t->ktop;
	t->entry 	= 0;
	
	PUSH(0x20 | 0x3);				//ss
	PUSH(t->usp);					//esp
	PUSH(read_flags());				//eflags
	PUSH(0x18 | 0x3);				//cs
	PUSH(user_main - KERNEL_OFF);	//eip
	
	set_tss(t->ktop);
	
	user_jump(t->ksp);
}

//proc_asm.asm
void thread_entry();

void set_up_thread(thread_t *t, entry_f entry) {

	t->entry 	= entry;
	t->state 	= S_USED;
	t->ustack 	= sbrk(4096);
	t->usp 		= t->ustack + 1024;
	t->kstack	= kmalloc(4096);
	t->ktop		= t->kstack + 1024;
	t->ksp 		= t->ktop;
	
	PUSH(0x20 | 0x3);				//ss
	PUSH(t->usp);					//esp
	PUSH(read_flags());				//eflags
	PUSH(0x18 | 0x3);				//cs
	PUSH(t->entry - KERNEL_OFF);	//eip
	
	PUSH(thread_entry);
	PUSH(read_flags());
	for (int i = 0; i < 8; i++)
		PUSH(0);
}

pd_t *clone_pd();
void load_PDBR(pd_t *pd);
void *vtp(void *virt);
void fork_child_ret();

int proc_clone(trapframe_t *tf) {

	proc_t *np = &procs[1];
	proc_t *cp = cproc();
	
	np->state 	= S_USED;
	np->pd 		= clone_pd();
	np->brk 	= cp->brk;
	np->ct_num	= 0;
	
	for (int i = 0; i < MAX_THREADS; i++)
		np->threads[i].state = S_FREE;
		
	thread_t *t = &np->threads[np->ct_num];
	thread_t *ct = &cp->threads[cp->ct_num];
	
	t->state 	= S_USED;
	t->ustack 	= ct->ustack;
	t->usp 		= ct->usp;
	t->kstack 	= kmalloc(4096);
	t->ktop		= t->kstack + 1024;
	t->ksp		= t->ktop;
	t->entry	= 0;
	
	PUSH(0x20 | 0x3);		//ss
	PUSH(tf->uesp);			//esp
	PUSH(tf->eflags);		//eflags
	PUSH(0x18 | 0x3);		//cs
	PUSH(tf->eip);			//eip
	
	PUSH(fork_child_ret);
	PUSH(read_flags());
	for (int i = 0; i < 8; i++)
		PUSH(0);
	
	return 1;
}


















