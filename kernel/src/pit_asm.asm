

section .text


extern _pit_isr
global _pit_isr_wrapper
_pit_isr_wrapper:
	pushad
	call _pit_isr
	popad
	;send EOI
	mov al, 0x20
	out 0x20, al
	iretd
	