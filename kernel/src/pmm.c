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

#define ENTRY(block)		((block)/ BLOCKS_PER_ENTRY)
#define POS(block)			((block) & BLOCKS_PER_ENTRY)

static word 	_mem_map[MAP_SIZE];

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

void pmm_unset_blocks(size_t first, size_t count) {
	
	printfln("unsetting blocks: [%d, %d)", first, first + count);
	
	for (size_t i = first; i < first + count; i++)
		pmm_unset(i);
	
	pmm_set(0);
}

void pmm_set_blocks(size_t first, size_t count) {

	printfln("setting blocks: [%d, %d)", first, first + count);

	for (size_t i = first; i < first + count; i++)
		pmm_set(i);
}

void *pmm_alloc_block() {

	for (size_t i = 0; i < BLOCKS; i++) {
		if (!pmm_test(i)) {
			pmm_set(i);
			printfln("alloc block: %d", i);
			return (void *)(i * BLOCK_SIZE);
		}
	}
	
	printfln("*** out of free blocks!");
	__asm__("hlt");
	
	return 0;
}

void pmm_free_block(void *ptr) {
	size_t block = (addr_t)ptr / BLOCK_SIZE;
	printfln("free block: %d", block);
	pmm_unset(block);
}





















