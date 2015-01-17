

section .text

;void vmm_load_PDBR(PDirectory *dir);
global _vmm_load_PDBR
_vmm_load_PDBR:
	push ebp
	mov ebp, esp
	
	mov eax, [ebp + 8]
	mov cr3, eax
	
	pop ebp
	ret
	
;void vmm_enable_paging();
global _vmm_enable_paging
_vmm_enable_paging:
	mov eax, cr0
	or eax, 0x80000000
	mov cr0, eax
	ret
	
		
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	