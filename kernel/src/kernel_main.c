#include <types.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <stdlib.h>
#include <pmm.h>

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

static byte _kernel_end 
	__attribute__((section(".end"))) = 0;

word kernel_main(MemMap *mm) {

	clear();
	printfln("executing kernel...");
	
	initGDT();
	initIDT();
	initPIC();
	initPIT();
	
	__asm__("sti");
	
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
	
	u64 max_mem = 0;
	for (uword i = 0; i < mm->size; i++) {
		MemMapEntry *e = &mm->entries[i];
		if (e->type == MMAP_AVAILABLE) {
			u64 max = e->base + e->size;
			if (max_mem == 0 || max > max_mem)
				max_mem = max;
		}
	}
	
	initPMM(max_mem / 1024, (byte *)&_kernel_end);
	
	for (uword i = 0; i < mm->size; i++) {
		MemMapEntry *e = &mm->entries[i];
		if (e->type == MMAP_AVAILABLE)
			pmm_free_region(e->base, e->size);
	}
	
	uword end = (uword)&_kernel_end;
	pmm_alloc_region(0x100000, end - 0x100000);
	pmm_alloc_region(end, pmm_size());
	
	printfln("kernel exited");
	return 0xbabecafe;
}














