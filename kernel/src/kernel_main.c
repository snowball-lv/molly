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
#include <pic.h>
#include <pit.h>

//not used
//leave for linking with molly's libc
int main(int argc, char **argv) {
	return 0;
}

extern none_t _BSS_START;
extern none_t _BSS_END;
extern none_t _KERNEL_END;
extern pd_t init_pd;

#define _4MB		(4 * 1024 * 1024)

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
	//pmm_set_blocks(0, kend_phys / PAGE_SIZE);
	
	//alloc 4mb
	pmm_set_blocks(0, _4MB / PAGE_SIZE);
	
	//add recursive mapping (necessary for kalloc)
	pde_t *last = &init_pd.entries[1023];
	*last = (uintptr_t)&init_pd - KERNEL_OFF;
	*last |= PTE_P | PTE_RW;
	
	//allocate kernel PDEs
	for (int i = 768 + 1; i < 1023; i++) {
		pde_t *pde = &init_pd.entries[i];
		*pde = (uintptr_t)pmm_alloc_block();
		*pde |= PTE_P | PTE_RW;
	}
	
	reloadPDBR();
	
	//init kernel heap allocator
	init_kalloc();
	
	//set up idt and interrupt handlers
	init_idt();
	
	//set up pic
	init_pic();
	
	//set up pit
	init_pit();
	
	//set up syscalls
	init_syscall();
	
	enable_ints();
	
	char *args[] = {
		"init.exe",
		"-s",
		0
	};
	
	exec(args);
	
	//init_pci();
	
	//switch to user mode
	//user_mode();
	
	kprintfln("booted");
	
	while(1);
}

int thread() {
	log("in a new thread");
	yield();
	while(1);
	return 1337;
}

void user_main() {

	//kprintfln("in user main");
	log("in user main");
	mkt(thread);
	yield();
	log("back in user-main");
	
	if (fork() != 0) {
		log("in parent proc");
		yieldp();
	} else {
		log("in child proc");
	}
	
	log("after fork");
	
	while(1);
}




















