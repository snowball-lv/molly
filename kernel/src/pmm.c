#include <pmm.h>
#include <console.h>
#include <klib.h>

#define BLOCK_SIZE 			(PAGE_SIZE)
#define MAX_BLOCKS			(0xffffffff / BLOCK_SIZE)
#define BLOCKS_PER_ENTRY	(sizeof(int) * 8)
#define MAP_ENTRIES			(MAX_BLOCKS / BLOCKS_PER_ENTRY)
#define MAP_BYTES			(MAP_ENTRIES * sizeof(int))
#define BLOCKS				(MAP_ENTRIES * BLOCKS_PER_ENTRY)

#define ENTRY(block)		((block) / BLOCKS_PER_ENTRY)
#define POS(block)			((block) & BLOCKS_PER_ENTRY)

static int 	_mem_map[MAP_ENTRIES];

static void pmm_set(size_t block) {
	_mem_map[ENTRY(block)] |= (1 << POS(block));
}

static void pmm_unset(size_t block) {
	_mem_map[ENTRY(block)] &= ~(1 << POS(block));
}

static char pmm_test(size_t block) {
	return _mem_map[ENTRY(block)] & (1 << POS(block));
}

void init_pmm() {
	memset(_mem_map, 0xff, MAP_BYTES);
}

void pmm_unset_blocks(size_t first, size_t count) {
	
	for (size_t i = first; i < first + count; i++)
		pmm_unset(i);
	
	pmm_set(0);
}

void pmm_set_blocks(size_t first, size_t count) {

	for (size_t i = first; i < first + count; i++)
		pmm_set(i);
}

void *pmm_alloc_block() {

	for (size_t i = 0; i < BLOCKS; i++) {
		if (!pmm_test(i)) {
			pmm_set(i);
			return (void *)(i * BLOCK_SIZE);
		}
	}
	
	panic("*** out of free blocks!");
	
	return 0;
}

void pmm_free_block(void *ptr) {

	ASSERT_PAGE_ALIGNED((uintptr_t)ptr);
	
	size_t block = (uintptr_t)ptr / BLOCK_SIZE;
	pmm_unset(block);
}





















