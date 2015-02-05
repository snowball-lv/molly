
%macro INTERVAL 3
	%assign x %1
	%rep %2
		%3 x
		%assign x x + 1
	%endrep
%endmacro
	
%macro ISR_NOERRCODE 1
	global _isr_wrapper_%1
	_isr_wrapper_%1:
		push 0
		push %1
		jmp isr_common
%endmacro
	
%macro ISR_ERRCODE 1
	global _isr_wrapper_%1
	_isr_wrapper_%1:
		push %1
		jmp isr_common
%endmacro

section .text
	
INTERVAL 		0, 8, ISR_NOERRCODE
ISR_ERRCODE		8
ISR_NOERRCODE 	9
INTERVAL 		10, 5, ISR_ERRCODE
INTERVAL 		15, 17, ISR_NOERRCODE
;irqs
INTERVAL		32, 16, ISR_NOERRCODE
;unused
INTERVAL		48, 208, ISR_NOERRCODE
	
extern _isr_handler
	
isr_common:
	pushad
	push esp
	call _isr_handler
	add esp, 4 ;esp
	popad
	add esp, 8 ;number and error code
	iretd
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	