


section .text

;int mutex_test(mutex_t *m);
global _mutex_test
_mutex_test:
	push ebp
	mov ebp, esp
	
	;lock state
	mov eax, 0
	
	;save mutex state address
	mov ecx, [ebp + 8]
	
	lock xchg eax, [ecx]

	pop ebp
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	