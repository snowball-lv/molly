#include <alloc.h>
#include <types.h>
#include <console.h>
#include <pmm.h>
#include <vmm.h>

extern void _HEAP_START;

static addr_t _program_break = (addr_t)&_HEAP_START;

static void alloc_region(addr_t from, addr_t to) {
	size_t first = PAGE_INDEX(from);
	size_t last = PAGE_INDEX(to);
	
}

static void free_region(addr_t from, addr_t to) {

}

static void *sbrk(word increment) {
	addr_t old = _program_break;
	addr_t new_break = _program_break + increment;
	
	if (increment > 0)
		alloc_region(old, new_break);
	else if (increment < 0)
		free_region(new_break, old);
	
	return (void *)old;
}






















