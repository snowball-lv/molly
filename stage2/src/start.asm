bits 32

extern _stage2

section .start

global start
start:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp + 8]
	push eax
	
	call _stage2
	
	add esp, 4
	
	pop ebp
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	
	