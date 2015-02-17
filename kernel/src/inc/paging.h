#pragma once

#include <string.h>
#include <stdint.h>
#include <klib.h>

#define PAGE_SIZE		(4096)
#define PAGE_MASK		(0xfffff000)

#define PAGE_PDE(index)		((index) / 1024)
#define PAGE_PTE(index)		((index) % 1024)

void init_paging();

typedef uint32_t pte_t;

typedef struct {
	pte_t entries[1024];
} pt_t;

typedef uint32_t pde_t;

typedef struct {
	pde_t entries[1024];
} pd_t;

void alloc_pages(size_t first, size_t count);
void free_pages(size_t first, size_t count);

void load_PDBR(pd_t *pd);

void map_pde(pd_t *pd, size_t index, void *phys);

void map_pte(pt_t *pt, size_t index, void *phys);

void *virt_to_phys(void *virt);










