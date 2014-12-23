

section .text

;void lgdt(GDTR *gdtr);
global _lgdt
_lgdt:
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]
	lgdt [eax]
	jmp 0x8:.mark
	.mark:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov ss, ax
	mov fs, ax
	mov gs, ax
	pop ebp
	ret
	
	
	
	
	
	
	
	
	
	