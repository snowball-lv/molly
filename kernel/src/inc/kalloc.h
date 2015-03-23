#pragma once

#include <string.h>
#include <stdint.h>

#define MEM_ALIGNMENT	(sizeof(intmax_t))

void init_kernel_allocator();

void *kmalloc(size_t size);
void *kmalloc_page();

void kfree(void *ptr);