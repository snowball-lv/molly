#include <alloc.h>
#include <types.h>
#include <console.h>
#include <pmm.h>
#include <vmm.h>

extern void _HEAP_START;

static addr_t _program_break = (addr_t)&_HEAP_START;

static void *sbrk(word increment) {
	addr_t tmp = _program_break;
	addr_t new_break = _program_break + increment;
	
	if (increment > 0) {
		//alloc
	
		PDirectory *dir = vmm_get_dir();
	
		size_t i = _program_break;
		for (; i < new_break; i += PMM_BLOCK_SIZE) {
			
			PDEntry *de = &dir->entries[PD_INDEX(i)];
		}
	}
	
	return (void *)tmp;
}






















