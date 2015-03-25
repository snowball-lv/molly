bits 32

extern _kernel_main
extern _kmain
extern _init_pd

%define CR0_PG 			(1 << 31)
%define CR0_WP 			(1 << 16)
%define KERNEL_OFF		(0xc0000000)

section .start

global start
start:
	push ebp
	mov ebp, esp
	
	;enable FPU
	;FNINIT
	
	;map kernel to higher half
	
	;enable 4mb pages
	mov eax, cr4
	or eax, 0x10
	mov cr4, eax
	
	;load initial page dir
	mov eax, _init_pd - KERNEL_OFF
	mov cr3, eax
	
	;enable paging
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	
	;save pointer to memory map
	mov edx, [ebp + 8]
	
	;set new stack
	mov eax, 0x7ffff + KERNEL_OFF
	mov esp, eax
	mov ebp, esp
	
	;push memory map
	push edx
	
	;call kernel indirectly, never return
	mov eax, _kernel_main
	call eax
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	