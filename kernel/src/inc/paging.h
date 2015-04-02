#pragma once

#include <string.h>
#include <stdint.h>
#include <klib.h>

#define PTE_P		(1 << 0)
#define PTE_RW		(1 << 1)
#define PTE_U		(1 << 2)
#define PTE_PS		(1 << 7)

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

_Static_assert(sizeof(pte_t) 	== 4, "pte_t size not 6");
_Static_assert(sizeof(pde_t)	== 4, "pde_t size not 8");

void alloc_pages(size_t first, size_t count);
void free_pages(size_t first, size_t count);

void load_PDBR(pd_t *pd);

void map_pde(pd_t *pd, size_t index, void *phys);

void map_pte(pt_t *pt, size_t index, void *phys);

void *virt_to_phys(void *virt);

//v 2

void invlpg();

void map_page(void *virt, void *phys, int flags);















