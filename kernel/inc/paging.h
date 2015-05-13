#pragma once

#include <string.h>
#include <stdint.h>
#include <klib.h>

#define PTE_P		(1 << 0)
#define PTE_RW		(1 << 1)
#define PTE_U		(1 << 2)
#define PTE_PS		(1 << 7)
#define PAGE_SIZE		(4096)

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

void map_page(void *virt, void *phys, int flags);

//paging_asm.asm
void load_PDBR(pd_t *pd);
void enable_paging();
void invlpg(void *page);
void reloadPDBR();

pd_t *create_pd();
void *vtp(void *virt);















