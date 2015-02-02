#include <types.h>
#include <klib.h>
#include <console.h>
#include <pmm.h>
#include <memmap.h>
#include <vmm.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <proc.h>

static void zero_bss();
static void assert_kernel_build();
static void free_available_blocks(MemMap *mm);

void kmain(MemMap *mm) {

	//init bss
	zero_bss();
	
	//clear the screen
	console_clear();
	
	//validate build
	assert_kernel_build();

	//init physical memory
	init_pmm();
	free_available_blocks(mm);
	
	//set up gdt
	init_gdt();
	
	//set up idt and interrupt handlers
	init_idt();
	
	//init vmm and enable paging
	init_vmm();
	
	//set up pic
	init_pic();
	
	//set up pit
	init_pit();
	
	//enable interrupts
	__asm__("sti");
	
	//TODO
	//init_proc();
	
	//boot complete
	kprintfln("booting complete...");
	
	//never return, wait for interrupts
	while(1)
		halt();
}

extern none_t _BSS_START;
extern none_t _BSS_END;

static void zero_bss() {
	addr_t start 	= (addr_t)&_BSS_START;
	addr_t end		= (addr_t)&_BSS_END;
	memset((void *)start, 0, end - start);
}

extern none_t _KERNEL_END;

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

static void free_mm_entry(MemMapEntry *e) {

	if (e->type != MMAP_AVAILABLE)
		return;
	
	//assert the memory regions are 4k aligned
	ASSERT_ALIGN(e->base);
	ASSERT_ALIGN(e->size);
	
	size_t first = e->base / PAGE_SIZE;
	size_t count = e->size / PAGE_SIZE;
	
	pmm_unset_blocks(first, count);
}

static void free_available_blocks(MemMap *mm) {

	//free blocks
	for (size_t i = 0; i < mm->size; i++)
		free_mm_entry(&mm->entries[i]);
	
	//alloc 1M + kernel (esp at 0x7ffff)
	pmm_set_blocks(0, (addr_t)&_KERNEL_END / PAGE_SIZE);
}














