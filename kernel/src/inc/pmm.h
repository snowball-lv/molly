#pragma once

#include <types.h>

void initPMM(uword size, byte *map_addr);
void pmm_free_region(uword base, uword size);
void pmm_alloc_region(uword base, uword size);
void *pmm_alloc_block();
void pmm_free_block(void *ptr);
word pmm_size();





