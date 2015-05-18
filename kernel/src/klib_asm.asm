

section .text

;uint32_t read_ebp()
global _read_ebp
_read_ebp:
	mov eax, ebp
	ret

;uint32_t read_esp()
global _read_esp
_read_esp:
	mov eax, esp
	ret

;uint32_t read_eip()
global _read_eip
_read_eip:
	pop eax
	jmp eax
	
;uint32_t read_cr2();
global _read_cr2
_read_cr2:
	mov eax, cr2
	ret
	
;uint32_t read_ret();
global _read_ret
_read_ret:
	mov eax, [ebp + 4]
	ret
	
;uint32_t read_flags();
global _read_flags
_read_flags:
	pushf
	mov eax, [esp]
	add esp, 4
	ret
	
;int cas(int *ptr, int oldval, int newval);
global _cas
_cas:
	push ebp
	mov ebp, esp

	mov ecx, [ebp + 8]	; *ptr
	mov eax, [ebp + 12]	; oldval
	mov edx, [ebp + 16]	; newval

	lock cmpxchg [ecx], edx

	jz .true
	.false:
		mov eax, 0
		jmp .end
	.true:
		mov eax, 1
	.end:

	pop ebp
	ret
	
	
	
	
	
	