#pragma once

#include <types.h>

void initPMM();

void pmm_free_region(addr_t base, size_t size);
void pmm_alloc_region(addr_t base, size_t size);
void *pmm_alloc_block();
void pmm_free_block(void *ptr);





