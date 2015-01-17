bits 32

extern _kernel_main
extern _kmain

section .start

global start
start:
	push ebp
	mov ebp, esp
	
	;save pointer to memory map
	mov edx, [ebp + 8] 
	
	;set stack
	mov eax, 0x7ffff
	mov esp, eax
	mov ebp, esp
	
	;enable FPU
	FNINIT
	
	;push memory map
	push edx
	
	;call kernel, never return
	call _kmain
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	