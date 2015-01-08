#include <vmm.h>
#include <types.h>
#include <pmm.h>
#include <console.h>
#include <stdlib.h>
#include <isr.h>

#define PAGE_INDEX(addr) 	((addr) / PAGE_SIZE)

typedef u32 PTEntry;

typedef struct {
	PTEntry entries[1024];
} PTable;

typedef u32 PDEntry;

typedef struct {
	PDEntry entries[1024];
} PDirectory;

static PDirectory 	*_kernel_pd;

#define PT_INDEX(x)	(((x) >> 12) & 0x3ff)
#define PD_INDEX(x)	(((x) >> 22) & 0x3ff)

//vmm_asm.asm
void vmm_load_PDBR(PDirectory *pd);
void vmm_enable_paging();

static void pf_isr(word id, u32 error) {
	printfln("*** page fault!");
	halt();
}

#define PAGE_MASK			0xfffff000
#define PRESENT				0b01
#define READ_WRITE			0b10

#define PTABLE_ADDR(pde)		((pde) & PAGE_MASK)
#define PAGE_ADDR(pte)			((pte) & PAGE_MASK)

static PTEntry *get_pte(PTable *pt, addr_t addr) {
	return &pt->entries[PT_INDEX(addr)];
}

static PTable *get_pt(PDEntry *pde) {
	return (PTable *)PTABLE_ADDR(*pde);
}

static PDEntry *get_pde(addr_t addr) {
	return &_kernel_pd->entries[PD_INDEX(addr)];
}

static void map_ptable(PDEntry *pde) {
	PTable *pt = pmm_alloc_block();
	*pde |= PRESENT;
	*pde |= READ_WRITE;
	*pde |= (addr_t)pt;
}

static void map_page(PTEntry *pte) {
	void *page = pmm_alloc_block();
	*pte |= PRESENT;
	*pte |= READ_WRITE;
	*pte |= (addr_t)page;
}

static void map_pages(size_t first, size_t count) {

	printfln("mapping: [%d, %d)", first, first + count);
	
	for (size_t i = first; i < first + count; i++) {
	
		addr_t addr = i * PAGE_SIZE;
		PDEntry *pde = get_pde(addr);
		
		if (*pde == 0) {
			printfln("mapping ptable!");
			map_ptable(pde);
		}
		
		PTable *pt = get_pt(pde);
		PTEntry *pte = get_pte(pt, addr);
		
		if (*pte == 0) {
			printfln("mapping page!");
			map_page(pte);
			printfln("page mapped!");
		}
	}
}

static void unmap_page(PTEntry *pte) {
	void *page = (void *)PAGE_ADDR(*pte);
	pmm_free_block(page);
	*pte = 0;
}

static word pt_is_empty(PTable *pt) {
	for (size_t i = 0; i < 1024; i++)
		if (pt->entries[i])
			return 0;
	return 1;
}

static void unmap_pages(size_t first, size_t count) {

	printfln("unmapping: [%d, %d)", first, first + count);
	
	for (size_t i = first; i < first + count; i++) {
	
		addr_t addr = i * PAGE_SIZE;
		PDEntry *pde = get_pde(addr);
		
		if (!*pde)
			continue;
		
		PTable *pt = get_pt(pde);
		PTEntry *pte = get_pte(pt, addr);
		
		if (*pte)
			unmap_page(pte);
	}
	
	//delete empty page tables
	for (size_t i = 0; i < 1024; i++) {
	
		PDEntry *pde = &_kernel_pd->entries[i];
		
		if (!*pde)
			continue;
			
		PTable *pt = (PTable *)PTABLE_ADDR(*pde);
		
		if (pt_is_empty(pt)) {
			pmm_free_block(pt);
			*pde = 0;
		}
	}
}

extern none_t 		_KERNEL_END;
static addr_t 		_kernel_break;
static addr_t 		_heap_start;

#define ALIGN(v, a)	((((v) + (a) - 1) / (a)) * (a))

#define PAGE_ALIGN(addr)	ALIGN(addr, PAGE_SIZE)

static void *sbrk(word increment) {
	
	printfln("sbrk: %d", increment);

	addr_t old_break = _kernel_break;
	addr_t new_break = _kernel_break + increment;
	
	if (increment > 0) {
	
		size_t first = PAGE_INDEX(old_break);
		addr_t rem = old_break % PAGE_SIZE;
		size_t count = PAGE_ALIGN(rem + increment) / PAGE_SIZE;
		map_pages(first, count);
		
	} else if (increment < 0) {
	
		size_t first = PAGE_INDEX(PAGE_ALIGN(new_break));
		addr_t rem = new_break % PAGE_SIZE;
		rem = PAGE_SIZE - rem;
		increment = abs(increment) - rem;
		size_t count = PAGE_ALIGN(increment) / PAGE_SIZE;
		unmap_pages(first, count);
		
	}
	
	_kernel_break = new_break;
	return (void *)old_break;
}

void initVMM() {

	//validate struct alignment
	printfln("sizeof(PTEntry): %d", sizeof(PTEntry));
	printfln("sizeof(PDEntry): %d", sizeof(PDEntry));

	//end of kernel data
	_kernel_break = (addr_t)&_KERNEL_END;
	printfln("kernel end %d", _kernel_break);
	
	//set 1 block after kernel for pdirectory
	pmm_set_blocks(PAGE_INDEX(_kernel_break), 1);
	_kernel_pd = (PDirectory *)_kernel_break;
	_kernel_break += PAGE_SIZE;
	memset(_kernel_pd, 0, sizeof(PDirectory));
	
	//page count to identity map
	size_t count = _kernel_break / PAGE_SIZE;
	printfln("identity: [%d, %d)", 0, count);
	
	//identity map kernel pages
	for (size_t i = 0; i < count; i++) {
		addr_t addr = i * PAGE_SIZE;
		
		PDEntry *pde = get_pde(addr);
		
		if (!*pde) {
			printfln("identity mapping ptable!");
			
			//set 1 block after kernel for ptable
			pmm_set_blocks(PAGE_INDEX(_kernel_break), 1);
			PTable *pt = (PTable *)_kernel_break;
			_kernel_break += PAGE_SIZE;
			count++;
			memset(pt, 0, sizeof(PTable));
			
			*pde |= PRESENT;
			*pde |= READ_WRITE;
			*pde |= (addr_t)pt;
		}
			
		PTable *pt = get_pt(pde);
		
		PTEntry *pte = get_pte(pt, addr);
		
		*pte |= PRESENT;
		*pte |= READ_WRITE;
		*pte |= (i * PAGE_SIZE);
	}
	
	//set PF isr
	set_isr(14, pf_isr);
	
	vmm_load_PDBR(_kernel_pd);
	vmm_enable_paging();
	
	_heap_start = _kernel_break;
}

//kalloc & kfree

#define KALLOC_MIN_SIZE 		4
#define KALLOC_ALIGNMENT 		KALLOC_MIN_SIZE
#define KALLOC_ALIGN(addr)		ALIGN(addr, KALLOC_ALIGNMENT)

#define STATUS_ALLOCATED  		1
#define STATUS_FREE		  		2
	
typedef struct Header {
	size_t size;
	byte status;
} Header;

static Header *next(Header *last) {

	if (last == 0) {
		if (_heap_start == _kernel_break)
			return 0;
		else
			return (Header *)_heap_start;
	}
	
	Header *h = (Header *)
		((byte *)last + sizeof(Header) + last->size);
		
	if ((addr_t)h == _kernel_break)
		return 0;
	
	return h;
}

void *kalloc(size_t size) {

	printfln("kalloc");
	printfln("size: %d", size);
	
	size_t as = KALLOC_ALIGN(size);
	printfln("as: %d", as);

	Header *h = 0;
	while ((h = next(h))) {
		
		if (h->status != STATUS_FREE)
			continue;
		
		if (h->size == as) {
			h->status = STATUS_ALLOCATED;
			printfln("best fit kalloc!");
			return h;
		}
		
		size_t diff = h->size - (sizeof(Header) + as);
			
		if (diff >= KALLOC_MIN_SIZE) {
			h->size = diff;
			Header *h2 = next(h);
			h2->size = as;
			h2->status = STATUS_ALLOCATED;
			printfln("split kalloc!");
			return h2;
		}
	}
	
	printfln("sbrk kalloc!");
			
	h = sbrk(sizeof(Header) + as);
	h->size = as;
	h->status = STATUS_ALLOCATED;

	return (byte *)h + sizeof(Header);
}

void kfree(void *ptr) {

	printfln("free");

	Header *h = (Header *)((byte *)ptr - sizeof(Header));
	h->status = STATUS_FREE;
	
	//merge free blocks
	Header *prev = 0;
	while ((h = next(prev))) {
		if (	prev && 
				prev->status == STATUS_FREE &&
				h->status == STATUS_FREE)
		{
			printfln("merging: %d + %d", prev->size, h->size);
			prev->size += sizeof(Header) + h->size;
		} else {
			prev = h;
		}
	}
	
	//release last free block
	Header *last = 0;
	while (next(last) != 0)
		last = next(last);
		
	if (last->status == STATUS_FREE) {
		printfln("releasing last free block!");
		sbrk(-(sizeof(Header) + last->size));
	}
}


















