#include <pmm.h>
#include <types.h>
#include <console.h>

static byte 	*_mem_map;
static uword 	_blocks;

void pmm_set(word block) {
	_mem_map[block/8] |= (1 << (block % 8));
}

void pmm_unset(word block) {
	_mem_map[block/8] &= ~(1 << (block % 8));
}

bool pmm_test(word block) {
	return _mem_map[block/8] & (1 << (block % 8));
}

word get_free_block() {
	for (word i = 0; i < _blocks; i++)
		if (!pmm_test(i))
			return i;
	return -1;
}

void initPMM(uword size, byte *map_addr) {

}

byte *pmm_alloc_block() {

}

void pmm_free_block() {

}










