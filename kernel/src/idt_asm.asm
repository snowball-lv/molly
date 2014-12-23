

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
	
;void wrapper();
extern _handler
global _wrapper
_wrapper:
	pushad
    call _handler
    popad
    iret
	
	
	
	
	
	
	
	
	
	
	
	