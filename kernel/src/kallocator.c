#include <kallocator.h>

#include <types.h>
#include <stdint.h>
#include <console.h>
#include <paging.h>

#define PAGE_ALIGN_RIGHT(v)	\
	(ALIGN_RIGHT((v), PAGE_SIZE))

#define ALIGN_RIGHT(v, a)	\
	(((v) + (a) - 1) & ~((a) - 1))

#define MEM_ALIGN_RIGHT(v)	\
	(ALIGN_RIGHT((v), MEM_ALIGNMENT))

extern none_t _HEAP_START;

static uintptr_t _heap_base;
static uintptr_t _heap_top;

void init_kernel_allocator() {
	
	_heap_base 	= (uintptr_t)&_HEAP_START;
	_heap_top 	= _heap_base;
}

static void *kmalloc_custom(size_t size, size_t alignment) {
	
	kprintfln("kmalloc: %d, %d", size, alignment);
	
	if (size == 0)
		return 0;
		
	uintptr_t brk = _heap_top;
	
	size_t first = PAGE_ALIGN_RIGHT(brk) / PAGE_SIZE;
	
	kprintfln("first: %d", first);
	
	uintptr_t mem = ALIGN_RIGHT(brk, alignment);
	uintptr_t new_brk = mem + size;
	new_brk = MEM_ALIGN_RIGHT(new_brk);
	
	size_t old_page = PAGE_ALIGN_RIGHT(brk) / PAGE_SIZE;
	size_t new_page = PAGE_ALIGN_RIGHT(new_brk) / PAGE_SIZE;
	size_t count = new_page - old_page;
	
	kprintfln("count: %d", count);
	
	alloc_pages(first, count);
	
	_heap_top = new_brk;
	
	return (void *)mem;
}

void *kmalloc_page() {
	return kmalloc_custom(PAGE_SIZE, PAGE_SIZE);
}

void *kmalloc(size_t size) {
	return kmalloc_custom(size, MEM_ALIGNMENT);
}

void kfree(void *ptr) {
	
}
















