

section .text


;void lidt(IDTR *idtr);
global _lidt
_lidt:
	push ebp
	mov ebp, esp
	mov eax, [ebp + 8]
	lidt [eax]
	pop ebp
	ret
	

extern _default_isr
global _default_isr_wrapper
_default_isr_wrapper:
	pusha
	call _default_isr
	popa
	iret
	
	
	
	
	
	
	
	
	
	