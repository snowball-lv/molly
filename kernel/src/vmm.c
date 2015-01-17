#include <vmm.h>
#include <types.h>
#include <pmm.h>
#include <console.h>
#include <stdlib.h>
#include <isr.h>

//vmm_asm.asm
void vmm_load_PDBR(PDirectory *pd);
void vmm_enable_paging();

static void pf_isr(word id, uint32_t error) {
	printfln("*** page fault!");
	stop();
}

#define PAGE_MASK			0xfffff000
#define PRESENT				0b01
#define READ_WRITE			0b10

#define PT_INDEX(x)	(((x) >> 12) & 0x3ff)
#define PD_INDEX(x)	(((x) >> 22) & 0x3ff)

static PTEntry *get_pte(PTable *pt, addr_t addr) {
	return &pt->entries[PT_INDEX(addr)];
}

static PDEntry *get_pde(PDirectory *pd, addr_t addr) {
	return &pd->entries[PD_INDEX(addr)];
}

static void map_pages(size_t first, size_t count) {

	printfln("mapping: [%d, %d)", first, first + count);
	
	PDirectory *pd = (PDirectory *)0xfffff000;
	
	for (size_t i = first; i < first + count; i++) {
	
		addr_t addr = i * PAGE_SIZE;
		PDEntry *pde = get_pde(pd, addr);
		
		if (*pde == 0) {
			printfln("mapping ptable!");
			*pde |= PRESENT;
			*pde |= READ_WRITE;
			*pde |= (addr_t)pmm_alloc_block();
		}
		
		PTable *pt = (PTable *)
			((byte *)0xffc00000 + 4096 * PD_INDEX(addr));
			
		PTEntry *pte = get_pte(pt, addr);
		
		if (*pte == 0) {
			printfln("mapping page!");
			*pte |= PRESENT;
			*pte |= READ_WRITE;
			*pte |= (addr_t)pmm_alloc_block();
		}
	}
}

static word pt_is_empty(PTable *pt) {
	for (size_t i = 0; i < 1024; i++)
		if (pt->entries[i] != 0)
			return 0;
	return 1;
}

static void unmap_pages(size_t first, size_t count) {

	printfln("unmapping: [%d, %d)", first, first + count);
	
	PDirectory *pd = (PDirectory *)0xfffff000;
	
	//delete empty pages
	for (size_t i = first; i < first + count; i++) {
	
		addr_t addr = i * PAGE_SIZE;
		PDEntry *pde = get_pde(pd, addr);
		
		if (*pde == 0)
			continue;
		
		PTable *pt = (PTable *)
			((byte *)0xffc00000 + 4096 * PD_INDEX(addr));
			
		PTEntry *pte = get_pte(pt, addr);
		
		if (*pte != 0) {
			pmm_free_block((void *)(*pte & PAGE_MASK));
			*pte = 0;
		}
	}
	
	//delete empty page tables
	for (size_t i = 0; i < 1024; i++) {
	
		PDEntry *pde = &pd->entries[i];
		
		if (*pde == 0)
			continue;
			
		PTable *pt = (PTable *)
			((byte *)0xffc00000 + 4096 * i);
		
		if (pt_is_empty(pt)) {
			pmm_free_block(pt);
			*pde = 0;
		}
	}
}

extern none_t 		_KERNEL_END;
static addr_t 		_kernel_break;
static addr_t 		_heap_start;

#define ALIGN(v, a)			((((v) + (a) - 1) / (a)) * (a))
#define PAGE_ALIGN(addr)	(ALIGN((addr), PAGE_SIZE))
#define PAGE_INDEX(addr) 	((addr) / PAGE_SIZE)
#define ABS(value)			(((value) >= 0)? (value) : -(value))

static void *sbrk(word increment) {
	
	printfln("sbrk: %d", increment);

	addr_t old_break = _kernel_break;
	addr_t new_break = _kernel_break + increment;
	
	if (increment > 0) {
	
		size_t first = PAGE_INDEX(PAGE_ALIGN(old_break));
		size_t last = PAGE_INDEX(PAGE_ALIGN(new_break));
		size_t count = last - first;
		map_pages(first, count);
		
	} else if (increment < 0) {
	
		size_t first = PAGE_INDEX(PAGE_ALIGN(new_break));
		size_t last = PAGE_INDEX(PAGE_ALIGN(old_break));
		size_t count = last - first;
		unmap_pages(first, count);
		
	}
	
	_kernel_break = new_break;
	return (void *)old_break;
}

void init_vmm() {
	
	ASSERT_SIZE(PTEntry, 4);
	ASSERT_SIZE(PDEntry, 4);

	//end of kernel data
	_kernel_break = (addr_t)&_KERNEL_END;
	printfln("kernel end %d", _kernel_break);
	
	//aloc block for kernel directory
	PDirectory *pd = (PDirectory *)pmm_alloc_block();
	memset(pd, 0, sizeof(PDirectory));
	
	//recursive map last pde
	PDEntry *last = &pd->entries[1023];
	*last |= PRESENT;
	*last |= READ_WRITE;
	*last |= (addr_t)pd;
	
	//page count to identity map
	size_t count = _kernel_break / PAGE_SIZE;
	printfln("identity: [%d, %d)", 0, count);
	
	//identity map kernel pages
	for (size_t i = 0; i < count; i++) {
		addr_t addr = i * PAGE_SIZE;
		
		PDEntry *pde = get_pde(pd, addr);
		
		if (*pde == 0) {
			printfln("mapping ptable!");
			PTable *pt = (PTable *)pmm_alloc_block();
			memset(pt, 0, sizeof(PTable));
			*pde |= PRESENT;
			*pde |= READ_WRITE;
			*pde |= (addr_t)pt;
		}
			
		PTable *pt = (PTable *)(*pde & PAGE_MASK);
		PTEntry *pte = get_pte(pt, addr);
		
		*pte |= PRESENT;
		*pte |= READ_WRITE;
		*pte |= (i * PAGE_SIZE);
	}
	
	//set PF isr
	set_isr(14, pf_isr);
	
	vmm_load_PDBR(pd);
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

void *kmalloc(size_t size) {

	printfln("kmalloc");
	printfln("size: %d", size);
	
	size_t as = KALLOC_ALIGN(size);
	printfln("as: %d", as);

	Header *h = 0;
	while ((h = next(h))) {
		
		if (h->status != STATUS_FREE)
			continue;
		
		if (h->size == as) {
			h->status = STATUS_ALLOCATED;
			printfln("best fit kmalloc!");
			return h;
		}
		
		size_t diff = h->size - (sizeof(Header) + as);
			
		if (diff >= KALLOC_MIN_SIZE) {
			h->size = diff;
			Header *h2 = next(h);
			h2->size = as;
			h2->status = STATUS_ALLOCATED;
			printfln("split kmalloc!");
			return h2;
		}
	}
	
	printfln("sbrk kmalloc!");
			
	h = sbrk(sizeof(Header) + as);
	h->size = as;
	h->status = STATUS_ALLOCATED;

	return (byte *)h + sizeof(Header);
}

void kfree(void *ptr) {

	printfln("kfree");

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
















