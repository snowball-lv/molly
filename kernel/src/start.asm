bits 32

extern _kernel_main

section .start

global start
start:
	mov eax, 0x7ffff
	mov esp, eax
	mov ebp, esp
	
	call _kernel_main
	
	;spin and sleep forever
	.loop:
	hlt
	jmp .loop
	