


section .text

;void user_jump(void *ksp);
global _user_jump
_user_jump:
	push ebp
	mov ebp, esp
	
	mov ax, 0x20 | 0x3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov eax, [ebp + 8]	;kernel stack pointer
	mov esp, eax
	
	iret
	