

section .text

;void out8(u16 port, u8 value);
global _out8
_out8:
	push ebp
	mov ebp, esp
	mov dx, [ebp + 8]
	mov al, [ebp + 12]
	out dx, al
	pop ebp
	ret

;u8 in8(u16 port);
global _in8
_in8:
	push ebp
	mov ebp, esp
	mov dx, [ebp + 8]
	in al, dx
	pop ebp
	ret		

;u16 in16(u16 port);
global _in16
_in16:
	push ebp
	mov ebp, esp
	mov dx, [ebp + 8]
	in ax, dx
	pop ebp
	ret
	
	
	
	
	
	
	
	