
section .text

%define I_SYSCALL	(0x80)

;int syscall_do(int f, ...);
global _syscall_do
_syscall_do:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp + 8]
	int I_SYSCALL
	
	pop ebp
	ret
	