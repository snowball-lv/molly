#pragma once

#include <string.h>

void init_pmm();

void pmm_unset_blocks(size_t first, size_t count);

void pmm_set_blocks(size_t first, size_t count);

void *pmm_alloc_block();

void *pmm_alloc_page();

void pmm_free_block(void *ptr);

void pmm_info();

