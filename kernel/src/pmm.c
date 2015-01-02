#include <pmm.h>
#include <types.h>
#include <console.h>
#include <stdlib.h>

#define BLOCK_SIZE 			4096
#define MAX_BLOCKS			(0xffffffff / BLOCK_SIZE)
#define BLOCKS_PER_ENTRY	(sizeof(word) * 8)
#define MAP_SIZE			(MAX_BLOCKS / BLOCKS_PER_ENTRY)
#define MAP_BYTES			(MAP_SIZE * sizeof(word))
#define BLOCKS				(MAP_SIZE * BLOCKS_PER_ENTRY)

#define ENTRY(block)		(block / BLOCKS_PER_ENTRY)
#define POS(block)			(block & BLOCKS_PER_ENTRY)

static word _mem_map[MAP_SIZE];

static void pmm_set(size_t block) {
	_mem_map[ENTRY(block)] |= (1 << POS(block));
}

static void pmm_unset(word block) {
	_mem_map[ENTRY(block)] &= ~(1 << POS(block));
}

static byte pmm_test(word block) {
	return _mem_map[ENTRY(block)] & (1 << POS(block));
}

void initPMM() {
	memset(_mem_map, 0xff, MAP_BYTES);
}

void pmm_free_region(addr_t base, size_t size) {
	
	ASSERT_ALIGN(base, "pmm free base");
	ASSERT_ALIGN(size, "pmm free size");
	
	size_t block = base / BLOCK_SIZE;
	
	printf("freed blocks from %d ", block);
	
	size_t count = size / BLOCK_SIZE;
	while (count) {
		pmm_unset(block++);
		count--;
	}
	
	printfln("to %d ", block);
	
	pmm_set(0);
}

void pmm_alloc_region(addr_t base, size_t size) {

	ASSERT_ALIGN(base, "pmm alloc base");
	ASSERT_ALIGN(size, "pmm alloc size");

	size_t block = base / BLOCK_SIZE;
		
	size_t count = size / BLOCK_SIZE;
	
	printf("allocated blocks from %d ", block);
	
	while (count) {
		pmm_set(block++);
		count--;
	}
	
	printfln("to %d ", block);
}

void *pmm_alloc_block() {

	for (size_t i = 0; i < BLOCKS; i++) {
		if (!pmm_test(i)) {
			pmm_set(i);
			return (void *)(i * BLOCK_SIZE);
		}
	}
	
	printfln("*** out of free blocks!");
	__asm__("hlt");
	
	return 0;
}

void pmm_free_block(void *ptr) {
	size_t block = (addr_t)ptr / BLOCK_SIZE;
	pmm_unset(block);
}










