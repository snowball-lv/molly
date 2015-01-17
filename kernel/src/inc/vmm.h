#pragma once

#include <types.h>

typedef uint32_t PTEntry;

typedef struct {
	PTEntry entries[1024];
} PTable;

typedef uint32_t PDEntry;

typedef struct {
	PDEntry entries[1024];
} PDirectory;

void init_vmm();

void *kmalloc(size_t size);
void kfree(void *ptr);


















