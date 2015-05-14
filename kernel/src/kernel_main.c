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
#include <kbd.h>
#include <acpi.h>

//not used
//leave for linking with molly's libc
int main(int argc, char **argv) {
	return 0;
}

extern none_t _BSS_START;
extern none_t _BSS_END;
extern none_t _KERNEL_END;

#define _4MB		(4 * 1024 * 1024)
#define PDE_SHIFT 	(22)

__attribute__((aligned(PAGE_SIZE)))
pd_t boot_pd = {
	.entries = {
		[0]
			= (0) | PTE_P | PTE_RW | PTE_PS,
		[KERNEL_BASE >> PDE_SHIFT]
			= (0) | PTE_P | PTE_RW | PTE_PS
	}
};

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

		uint32_t base = (uint32_t)e->base;
		uint32_t size = (uint32_t)e->size;

		kprintfln(	"%d: %d -> %d",
					e->type,
					base / 1024,
					(base + size) / 1024);

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

	//remove lower mapping
	boot_pd.entries[0] = 0;

	//add recursive mapping (necessary for kalloc)
	pde_t *last = &boot_pd.entries[1023];
	*last = (uintptr_t)&boot_pd - KERNEL_OFF;
	*last |= PTE_P | PTE_RW;

	//allocate kernel PDEs (necessary for new processes)
	for (int i = 768 + 1; i < 1023; i++) {
		pde_t *pde = &boot_pd.entries[i];
		*pde = (uintptr_t)pmm_alloc_block();
		*pde |= PTE_P | PTE_RW;
	}

	reloadPDBR();

	/* deal with ACPI later

	//map acpi memory
	for (size_t i = 0; i < mm->size; i++) {

		MemMapEntry *e = &mm->entries[i];

		if (e->type != MMAP_RECLAIM)
			continue;

		//assert the memory regions are 4k aligned
		ASSERT_PAGE_ALIGNED(e->base);
		ASSERT_PAGE_ALIGNED(e->size);

		char *ptr = (char *)(uintptr_t)e->base;
		char *end = ptr + e->size;
		for (; ptr < end; ptr += PAGE_SIZE) {
			map_page(ptr, ptr, PTE_P | PTE_RW);
		}
	}
	*/

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

	//set up vfs
	init_vfs();

	//init and mount device fs/manager
	init_devfs();

	//register vga and console devices
	init_console();

	//set up acpi
	//init_acpi();

	//set up keyboard and kbd device
	init_kbd();

	//enable interrupts
	enable_ints();

	//start first user process
	run_init();

	//init_pci();

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
