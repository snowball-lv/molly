#include <proc.h>
#include <types.h>
#include <stdlib.h>
#include <klib.h>
#include <vmm.h>

typedef struct proc_t proc_t;

struct proc_t {
	pid_t 		id;
	uint32_t	ebp;
	uint32_t	esp;
	uint32_t	eip;
	proc_t		*next;
};

static proc_t *current_proc;
static proc_t *proc_list;

static pid_t next_pid = 1;

void init_proc() {
	
}

pid_t fork() {
	__asm__("cli");
	
	proc_t *parent = current_proc;
	
	uint32_t eip = read_eip();
	//new proc will start here
	
	if (current_proc == parent) {
		//same proc
		
		proc_t *child = kmalloc(sizeof(proc_t));
		
		child->id 	= next_pid++;
		child->ebp 	= read_ebp();
		child->esp 	= read_esp();
		child->eip 	= eip;
		child->next = 0;
		
		proc_t *ptr = proc_list;
		while (ptr->next)
			ptr = ptr->next;
			
		ptr->next = child;
		
		__asm__("sti");
		
		return child->id;
	}
	
	//new proc
	return 0;
}




















