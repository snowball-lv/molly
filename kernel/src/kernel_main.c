#include <types.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <stdlib.h>
#include <pmm.h>
#include <vmm.h>
#include <kbd.h>

#define MMAP_AVAILABLE 	0x1
#define MMAP_RESERVED	0x2
#define MMAP_RECLAIM	0x3
#define MMAP_NVS		0x4

#define ATTR __attribute__((packed))

typedef struct {
	u64 base;
	u64 size;
	u32 type;
	u32 reserved;
} ATTR MemMapEntry;

#undef ATTR

typedef struct {
	uword size;
	MemMapEntry *entries;
} MemMap;
	
extern none_t _KERNEL_BASE;
extern none_t _BSS_START;
extern none_t _BSS_END;
extern none_t _KERNEL_END;
	
static void printMemMap(MemMap *mm);
	
static void initBSS() {
	addr_t start 	= (addr_t)&_BSS_START;
	addr_t end		= (addr_t)&_BSS_END;
	memset((void *)start, 0, end - start);
}
	
word kernel_main(MemMap *mm) {

	//zero init bss
	initBSS();
	
	//assert the kernel size is 4k aligned
	ASSERT_ALIGN(&_KERNEL_END, "kernel end");

	//clear the screen
	clear();
	
	printfln("executing kernel...");
	
	initGDT();
	initIDT();
	initPIC();
	initPIT();
	
	//enable interrupts
	__asm__("sti");
	
	printMemMap(mm);
	
	initPMM();
	
	for (size_t i = 0; i < mm->size; i++) {
		MemMapEntry *e = &mm->entries[i];
		if (e->type == MMAP_AVAILABLE) {
		
			//assert the memory regions are 4k aligned
			ASSERT_ALIGN(e->base, "mm entry base");
			ASSERT_ALIGN(e->size, "mm entry size");
			
			size_t first = e->base / PAGE_SIZE;
			size_t count = e->size / PAGE_SIZE;
			
			pmm_unset_blocks(first, count);
		}
	}
	
	//alloc 1M + kernel (esp at 0x7ffff)
	pmm_set_blocks(0, (addr_t)&_KERNEL_END / PAGE_SIZE);
	
	initVMM();
	initKBD();
	
	//printfln("dynamic word 1: %d", *w1);
	//printfln("dynamic word 2: %d", *w2);
	
	printfln("kernel exited");
	return 0xbabecafe;
}


static void printMemMap(MemMap *mm) {

	printfln("memory entries: %d", mm->size);
	for (uword i = 0; i < mm->size; i++) {
		MemMapEntry *e = &mm->entries[i];
		printf("%d: ", i);
		
		switch (e->type) {
		case MMAP_AVAILABLE:
			printf(NAME(MMAP_AVAILABLE));
			break;
		case MMAP_RESERVED:
			printf(NAME(MMAP_RESERVED));
			break;
		case MMAP_RECLAIM:
			printf(NAME(MMAP_RECLAIM));
			break;
		case MMAP_NVS:
			printf(NAME(MMAP_NVS));
			break;
		default:
			printf("unknown");
			break;
		}
		
		printf(", %d,", e->base);
		printfln(" %d", e->base + e->size);
	}
	
}












