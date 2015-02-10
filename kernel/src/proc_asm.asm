

section .text

;void switch_context(void **old_sp, void *new_sp);
global _switch_context
_switch_context:

	;save all 8 general purpose reg
	pusha
	
	;save current esp
	mov eax, [esp + 36]
	mov [eax], esp
	
	;load new esp
	mov eax, [esp + 40]
	mov esp, eax
	
	;load new register values
	popa
	
	ret
	
	
	
	
	
	