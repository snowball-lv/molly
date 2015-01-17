

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
	
%macro INTERVAL 3
	%assign x %1
	%rep %2
		%3 x
		%assign x x + 1
	%endrep
%endmacro
	
%macro ISR_WRAPPER 1
	global _isr_wrapper_%1
	_isr_wrapper_%1:
		iretd
%endmacro
	
INTERVAL 0, 32, ISR_WRAPPER
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	