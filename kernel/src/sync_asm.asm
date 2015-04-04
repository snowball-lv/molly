

section .text

;int spinlock_xchg(int *state, int value);
global _spinlock_xchg
_spinlock_xchg:
	push ebp
	mov ebp, esp

	mov ecx, [ebp + 8]	;*state
	mov eax, [ebp + 12]	;value
	
	lock xchg eax, [ecx]
	
	pop ebp
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	