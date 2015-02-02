#include <proc.h>
#include <types.h>
#include <klib.h>
#include <vmm.h>

typedef struct proc_t proc_t;

struct proc_t {
	addr_t	esp;
	proc_t 	*next;
};

static proc_t kernel_proc;
static proc_t *current;

void init_proc() {
	
	kernel_proc.next = 0;
	current = &kernel_proc;
}

//proc_asm.asm
void switch_context(addr_t *old_sp, addr_t *new_sp);

void reschedule() {
	
	proc_t *next = current->next;
	if (next == 0)
		next = &kernel_proc;
		
	if (next == current)
		return;
		
	switch_context(&current->esp, &next->esp);
}




















