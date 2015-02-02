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

typedef uint32_t pte_t;

typedef struct {
	pte_t entries[1024];
} pt_t;

typedef uint32_t pde_t;

typedef struct {
	pde_t entries[1024];
} pd_t;
















