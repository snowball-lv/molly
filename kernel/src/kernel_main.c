#include <types.h>
#include <console.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <stdlib.h>
#include <pmm.h>
#include <vmm.h>

#define MMAP_AVAILABLE 	0x1
#define MMAP_RESERVED	0x2
#define MMAP_RECLAIM	0x3
#define MMAP_NVS		0x4

#define ATTR __attribute__((packed))

typedef struct {
	uint64_t base;
	uint64_t size;
	uint32_t type;
	uint32_t reserved;
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
	
static void init_bss() {
	addr_t start 	= (addr_t)&_BSS_START;
	addr_t end		= (addr_t)&_BSS_END;
	memset((void *)start, 0, end - start);
}

static void free_available_blocks(MemMap *mm) {

	//free blocks
	for (size_t i = 0; i < mm->size; i++) {
	
		MemMapEntry *e = &mm->entries[i];
		
		if (e->type == MMAP_AVAILABLE) {
		
			//assert the memory regions are 4k aligned
			ASSERT_ALIGN(e->base);
			ASSERT_ALIGN(e->size);
			
			size_t first = e->base / PAGE_SIZE;
			size_t count = e->size / PAGE_SIZE;
			
			pmm_unset_blocks(first, count);
		}
	}
	
	//alloc 1M + kernel (esp at 0x7ffff)
	pmm_set_blocks(0, (addr_t)&_KERNEL_END / PAGE_SIZE);
}

static void assert_kernel_build() {

	//assert type sizes and kernel size
	ASSERT_SIZE(int8_t, 	1);
	ASSERT_SIZE(int16_t, 	2);
	ASSERT_SIZE(int32_t, 	4);
	ASSERT_SIZE(int64_t, 	8);
	
	ASSERT_SIZE(uint8_t, 	1);
	ASSERT_SIZE(uint16_t, 	2);
	ASSERT_SIZE(uint32_t, 	4);
	ASSERT_SIZE(uint64_t, 	8);
	
	//assert the kernel size is 4k aligned
	ASSERT_ALIGN(&_KERNEL_END);
}
	
word kernel_main(MemMap *mm) {

	//zero init bss
	init_bss();
	
	//validate build
	assert_kernel_build();

	//clear the screen
	clear();
	
	printfln("executing kernel...");
	
	init_gdt();
	init_idt();
	init_pic();
	init_pit();
	
	//enable interrupts
	__asm__("sti");
	
	//init and map physical memory
	init_pmm();
	free_available_blocks(mm);
	
	//enable paging and memory allocation
	__asm__("cli");
	init_vmm();
	__asm__("sti");
	
	init_proc();
	
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










