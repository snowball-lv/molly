bits 32

extern _kernel_main

section .start

global start
start:
	push ebp
	mov ebp, esp
	
	call _kernel_main
	
	pop ebp
	ret
	