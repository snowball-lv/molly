#include <klib.h>
#include <console.h>
#include <pmm.h>
#include <memmap.h>
#include <paging.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <types.h>
#include <kalloc.h>
#include <proc.h>
#include <scheduler.h>
#include <thread.h>
#include <pci.h>

extern none_t _KERNEL_END;

extern none_t _BSS_START;
extern none_t _BSS_END;

void kmain(MemMap *mm) {

	//zero bss
	uintptr_t bss_start = (uintptr_t)&_BSS_START;
	uintptr_t bss_end	= (uintptr_t)&_BSS_END;
	memset((void *)bss_start, 0, bss_end - bss_start);
	
	//clear the screen
	console_clear();
	
	//assert memory alignment is natural
	ASSERT_ALIGNMENT(PAGE_SIZE, MEM_ALIGNMENT);
	
	//assert the kernel size is 4k aligned
	ASSERT_PAGE_ALIGNED((uintptr_t)&_KERNEL_END);
	
	//set up gdt
	init_gdt();
	
	//set up idt and interrupt handlers
	init_idt();
	
	//init physical memory
	init_pmm();
	
	//free available blocks from memory map
	for (size_t i = 0; i < mm->size; i++) {
	
		MemMapEntry *e = &mm->entries[i];
		
		if (e->type != MMAP_AVAILABLE)
			continue;
			
		//assert the memory regions are 4k aligned
		ASSERT_PAGE_ALIGNED(e->base);
		ASSERT_PAGE_ALIGNED(e->size);
			
		size_t first = e->base / PAGE_SIZE;
		size_t count = e->size / PAGE_SIZE;
		
		pmm_unset_blocks(first, count);
	}
	
	//alloc 1M + kernel (esp at 0x7ffff)
	pmm_set_blocks(0, (uintptr_t)&_KERNEL_END / PAGE_SIZE);
	
	//init default process, thread and page directory
	init_null_proc();
	
	//init virtual memory and enable paging
	init_paging();
	
	//init kernel heap allocator
	init_kernel_allocator();
	
	//init process and thread switching
	init_scheduler();
	
	//set up pic
	init_pic();
	
	//set up pit
	init_pit();
	
	//enable interrupts
	enable_ints();
	
	//init_pci();
	
	//boot complete
	kprintfln("booting complete...");
	
	//never return, wait for interrupts
	while(1)
		halt();
}





















