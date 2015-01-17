

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
	
	