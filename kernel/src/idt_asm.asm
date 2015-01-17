

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
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	