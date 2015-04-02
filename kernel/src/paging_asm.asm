

section .text

;void load_PDBR(pd_t *pd);
global _load_PDBR
_load_PDBR:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp + 8]
	mov cr3, eax
	
	pop ebp
	ret
	
;void vmm_enable_paging();
global _enable_paging
_enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret
	
;void invlpg();
global _invlpg
_invlpg:
	mov eax, cr3
	mov cr3, eax
	ret
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	