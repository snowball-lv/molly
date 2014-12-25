bits 32

extern _kernel_main

section .start

global start
start:
	push ebp
	mov ebp, esp
	
	mov edx, [ebp + 8] ;pointer to memory map
	
	mov eax, 0x7ffff
	mov esp, eax
	mov ebp, esp
	
	push edx
	call _kernel_main
	
	;spin and sleep forever
	.loop:
	hlt
	jmp .loop