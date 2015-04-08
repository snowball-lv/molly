#include <kalloc.h>

#include <types.h>
#include <stdint.h>
#include <console.h>
#include <paging.h>
#include <pmm.h>
#include <param.h>

//extern none_t _HEAP_START;

static void *kheap_base;
static void *kheap_top;

#define _4MB		(4 * 1024 * 1024)

void init_kalloc() {

	kheap_base 	= (void *)(KERNEL_OFF + _4MB);
	kheap_top 	= kheap_base;
}

void *kmalloc_page() {

	char *cbrk = (void *)kheap_top;
	void *phys = pmm_alloc_block();
	map_page(cbrk, phys, PTE_P | PTE_RW);
	
	kheap_top = cbrk + PAGE_SIZE;
	
	return cbrk;
}

#define ROUND_UP(v, a)	(((v) + (a) - 1) & ~((a) - 1))

void *kmalloc(size_t size) {

	if (size == 0)
		return 0;

	char *cbrk = (void *)kheap_top;
	void *ret = cbrk;
	char *nbrk = cbrk + size;
	nbrk = (char *)ROUND_UP((uintptr_t)nbrk, PAGE_SIZE);
	
	for (;cbrk < nbrk; cbrk += PAGE_SIZE) {
		void *phys = pmm_alloc_block();
		map_page(cbrk, phys, PTE_P | PTE_RW);
	}
		
	kheap_top = cbrk;
	
	return ret;
}

void kfree(void *ptr) {
	kprintfln("kfree: %x", ptr);
}
















