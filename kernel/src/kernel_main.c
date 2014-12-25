#include <types.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <stdlib.h>
#include <mem.h>

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
	
	printfln("kernel exited");
	return 0xbabecafe;
}














