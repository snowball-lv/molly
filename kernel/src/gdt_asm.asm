

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
	
;void tss_flush();
global _tss_flush
_tss_flush:

	mov ax, 0x28 | 0x3
	ltr ax

	ret
	
%define KERNEL_OFF		(0xc0000000)
extern _user_main

;void user_mode();
global _user_mode
_user_mode:
	push ebp
	mov ebp, esp

	mov ax, 0x20 | 0x3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov eax, esp
	push 0x20 | 0x3
	push eax
	pushf
	push 0x18 | 0x3
	push .mark
	
	iret
	.mark:
	
	pop ebp
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	