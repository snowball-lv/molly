#include <pmm.h>
#include <types.h>
#include <console.h>
#include <stdlib.h>

#define BLOCK_SIZE 	4096

static byte 	*_mem_map;
static word 	_blocks;
static word 	_map_size;

word pmm_size() {
	return _map_size;
}

static void pmm_set(word block) {
	_mem_map[block/8] |= (1 << (block % 8));
}

static void pmm_unset(word block) {
	_mem_map[block/8] &= ~(1 << (block % 8));
}

static bool pmm_test(word block) {
	return _mem_map[block/8] & (1 << (block % 8));
}

void initPMM(uword size, byte *map_addr) {

	printfln("memory: %d kb", size);
	
	_blocks = size / 4;
	printfln("blocks: %d", _blocks);
	
	_map_size = (_blocks + 4) / 8;
	printfln("map size: %d", _map_size);
	
	_mem_map = map_addr;
	
	memset(_mem_map, 0xff, _map_size);
}

void pmm_free_region(uword base, uword size) {
	
	word block = base / BLOCK_SIZE;
	
	//skip first unaligned block
	if (base % BLOCK_SIZE)
		block++;
		
	word count = size / BLOCK_SIZE;
	while (count) {
		pmm_unset(block++);
		count--;
	}
	
	pmm_set(0);
}

void pmm_alloc_region(uword base, uword size) {
	printfln("allocating %d, %d", base ,size);
	word block = base / BLOCK_SIZE;
		
	word count = (size + (BLOCK_SIZE - 1)) / BLOCK_SIZE;
	printfln("blocks %d", count);
	while (count) {
		pmm_set(block++);
		count--;
	}
}

byte *pmm_alloc_block() {
	for (word i = 0; i < _blocks; i++) {
		if (!pmm_test(i)) {
			pmm_set(i);
			printfln("allocating block %d", i);
			return (byte *)(i * BLOCK_SIZE);
		}
	}
	printfln("*** out of free blocks!");
	return 0;
}

void pmm_free_block(byte *ptr) {
	word block = (uword)ptr / BLOCK_SIZE;
	printfln("freeing block %d", block);
	pmm_unset(block);
}










