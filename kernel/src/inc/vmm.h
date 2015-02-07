#pragma once

#include <string.h>
#include <stdint.h>

void init_vmm();

typedef uint32_t pte_t;

typedef struct {
	pte_t entries[1024];
} pt_t;

typedef uint32_t pde_t;

typedef struct {
	pde_t entries[1024];
} pd_t;

pte_t *vmm_get_pte(pt_t *pt, uintptr_t addr);
pde_t *vmm_get_pde(pd_t *pd, uintptr_t addr);

#define VMM_PRESENT			(0b01)
#define VMM_READ_WRITE		(0b10)

#define VMM_PAGE_MASK		(0xfffff000)

void *kmalloc_page();
void *kmalloc(size_t size);


















