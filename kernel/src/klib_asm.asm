

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
	
	
	
	
	
	
	
	
	
	
	
	
	
	