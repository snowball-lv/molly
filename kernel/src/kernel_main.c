#include <console.h>
#include <paging.h>
#include <param.h>
#include <gdt.h>
#include <types.h>
#include <pmm.h>
#include <memmap.h>
#include <molly.h>
#include <idt.h>
#include <syscall.h>
#include <pci.h>
#include <kalloc.h>
#include <proc.h>

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

extern none_t _BSS_START;
extern none_t _BSS_END;
extern none_t _KERNEL_END;

//paging enabled
void kernel_main(MemMap *mm) {

	//zero bss
	uintptr_t bss_start = (uintptr_t)&_BSS_START;
	uintptr_t bss_end	= (uintptr_t)&_BSS_END;
	memset((void *)bss_start, 0, bss_end - bss_start);
	
	console_clear();
	
	kprintfln("running kernel");
	
	//set up gdt
	init_gdt();
	
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
	uintptr_t kend_phys = (uintptr_t)&_KERNEL_END - KERNEL_BASE;
	kprintfln("kernel size: %d kb", kend_phys / 1024);
	pmm_set_blocks(0, kend_phys / PAGE_SIZE);
	
	//add recursive mapping (necessary for kalloc)
	pde_t *last = &init_pd.entries[1023];
	*last = (uintptr_t)&init_pd - KERNEL_OFF;
	*last |= PTE_P | PTE_RW;
	invlpg();
	
	//init kernel heap allocator
	init_kalloc();
	
	//set up idt and interrupt handlers
	init_idt();
	
	//set up syscalls
	init_syscall();
	
	//switch to user mode
	proc_swtch_usr();
	
	kprintfln("booted");
	
	while(1);
}

void user_main() {

	//kprintfln("in user main");
	log("in user main");
	
	while(1);
}




















