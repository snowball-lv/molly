


section .text

;void user_jump(void *ksp);
global _user_jump
_user_jump:
	push ebp
	mov ebp, esp
	
	mov ax, 0x20 | 0x3
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax
	
	mov eax, [ebp + 8]	;kernel stack pointer
	mov esp, eax
	
	popa
	popf
	
	ret

;void thread_entry();
global _thread_entry
_thread_entry:
	iret
	
;void fork_child_ret();
global _fork_child_ret
_fork_child_ret:
	mov eax, 0
	iret
	
;void switch_context(void **old_sp, void *new_sp);
global _switch_context
_switch_context:

	;save eflags + all 8 general purpose reg
	pushf
	pusha
	
	;save current esp
	mov eax, [esp + 40]
	mov [eax], esp
	
	;load new esp
	mov eax, [esp + 44]
	mov esp, eax
	
	;load new register values
	popa
	popf
	
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	