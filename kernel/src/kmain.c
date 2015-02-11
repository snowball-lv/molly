#include <klib.h>
#include <console.h>
#include <pmm.h>
#include <memmap.h>
#include <vmm.h>
#include <gdt.h>
#include <idt.h>
#include <pic.h>
#include <pit.h>
#include <types.h>
#include <proc.h>
#include <sync.h>

static void zero_bss();
static void free_available_blocks(MemMap *mm);

extern none_t _KERNEL_END;

void kmain(MemMap *mm) {

	//init bss
	zero_bss();
	
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
	free_available_blocks(mm);
	
	//init default process
	init_kproc();
	
	//init vmm and enable paging
	init_vmm();
	
	//set up pic
	init_pic();
	
	//set up pit
	init_pit();
	
	//enable interrupts
	enable_ints();
	
	//create_process();
	
	//boot complete
	kprintfln("booting complete...");
	
	//never return, wait for interrupts
	while(1)
		halt();
}

extern none_t _BSS_START;
extern none_t _BSS_END;

static void zero_bss() {
	uintptr_t start = (uintptr_t)&_BSS_START;
	uintptr_t end	= (uintptr_t)&_BSS_END;
	memset((void *)start, 0, end - start);
}

static void free_mm_entry(MemMapEntry *e) {

	if (e->type != MMAP_AVAILABLE)
		return;
	
	//assert the memory regions are 4k aligned
	ASSERT_PAGE_ALIGNED(e->base);
	ASSERT_PAGE_ALIGNED(e->size);
	
	size_t first = e->base / PAGE_SIZE;
	size_t count = e->size / PAGE_SIZE;
	
	pmm_unset_blocks(first, count);
}

static void free_available_blocks(MemMap *mm) {

	//free blocks
	for (size_t i = 0; i < mm->size; i++)
		free_mm_entry(&mm->entries[i]);
	
	//alloc 1M + kernel (esp at 0x7ffff)
	pmm_set_blocks(0, (uintptr_t)&_KERNEL_END / PAGE_SIZE);
}















