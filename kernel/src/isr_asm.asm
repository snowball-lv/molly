

extern _default_isr
extern _default_isr_error
extern _default_isr_irq_master
extern _default_isr_irq_slave


%macro INTERVAL 3
	%assign x %1
	%rep %2 - %1 + 1
		%3 x
	%assign x x + 1
	%endrep
%endmacro

%macro WRAPPER 1
	global _isr_wrapper%1
	_isr_wrapper%1:
		pushad
		push %1
		call _default_isr
		add esp, 4
		popad
		iretd
%endmacro

%macro WRAPPER_ERROR 1
	global _isr_wrapper%1
	_isr_wrapper%1:
		push eax
		mov eax, [esp + 4]
		pushad
		push eax
		push %1
		call _default_isr_error
		add esp, 8
		popad
		pop eax
		iretd
%endmacro

%macro WRAPPER_IRQ_MASTER 1
	global _isr_wrapper%1
	_isr_wrapper%1:
		pushad
		push %1
		call _default_isr_irq_master
		add esp, 4
		popad
		;don't send EOI here
		iretd
%endmacro

%macro WRAPPER_IRQ_SLAVE 1
	global _isr_wrapper%1
	_isr_wrapper%1:
		pushad
		push %1
		call _default_isr_irq_slave
		add esp, 4
		popad
		;send master EOI
		mov al, 20h
		out 20h, al
		iretd
%endmacro

%macro WRAPPER_CUSTOM 1
	global _isr_wrapper%1
	_isr_wrapper%1:
		push %1
		call _default_isr
		add esp, 4
		iretd
%endmacro

;WRAPPERS
section .text

INTERVAL 		0, 7, WRAPPER
WRAPPER_ERROR 	8
WRAPPER 		9
INTERVAL 		10, 14, WRAPPER_ERROR
INTERVAL 		15, 16, WRAPPER
WRAPPER_ERROR	17
INTERVAL 		18, 29, WRAPPER
WRAPPER_ERROR	30
WRAPPER			31
INTERVAL 		32, 39, WRAPPER_IRQ_MASTER
INTERVAL 		40, 47, WRAPPER_IRQ_SLAVE
INTERVAL 		48, 255, WRAPPER_CUSTOM









