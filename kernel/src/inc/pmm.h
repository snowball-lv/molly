#pragma once

#include <types.h>

void init_pmm();

void pmm_unset_blocks(size_t first, size_t count);

void pmm_set_blocks(size_t first, size_t count);

void *pmm_alloc_block();

void pmm_free_block(void *ptr);



