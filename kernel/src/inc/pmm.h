#pragma once

#include <types.h>

void initPMM(uword size, byte *map_addr);
void pmm_free_region(uword base, uword size);
byte *pmm_alloc_block();
void pmm_free_block(byte *ptr);








