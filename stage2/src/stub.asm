bits 16
org 0x0500

jmp start

GDT_START:
	;null descriptor
	dd 0
	dd 0
	
	;pmode code descriptor
	dw 0xffff
	dw 0
	db 0
	db 10011010b
	db 11001111b
	db 0
	
	;pmode data descriptor
	dw 0xffff
	dw 0
	db 0
	db 10010010b
	db 11001111b
	db 0
	
	;real code descriptor
	dw 0xffff
	dw 0
	db 0
	db 10011010b
	db 0xf
	db 0
	
	;real data descriptor
	dw 0xffff
	dw 0
	db 0
	db 10010010b
	db 0xf
	db 0
	
GDT_END:

GDT:
	dw GDT_END - GDT_START - 1
	dd GDT_START

BOOT_DRIVE	db 0

testA20:
    push ds
    push es
    push di
    push si
	
    mov ax, 0
    mov es, ax
	mov ax, 0xffff
    mov ds, ax
    mov di, 0x0500
    mov si, 0x0510
    mov al, byte [es:di]
    push ax
    mov al, byte [ds:si]
    push ax
    mov byte [es:di], 0x00
    mov byte [ds:si], 0xff
    cmp byte [es:di], 0xff
    pop ax
    mov byte [ds:si], al
    pop ax
    mov byte [es:di], al
    mov ax, 0
    je .disabled
    mov ax, 1
	
	.disabled:
    pop si
    pop di
    pop es
    pop ds
    ret

waitA20In:
	in      al, 0x64
	test    al, 2
	jnz     waitA20In
	ret
	
waitA20Out:
	in      al, 0x64
	test    al, 1
	jz      waitA20Out
	ret
	
start:
	;save boot drive number
	mov [BOOT_DRIVE], dl
	
	;set up segments
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ax, 0x7000
	mov ss, ax
	mov sp, 0xffff
	mov bp, sp
	
	;get memory map
	mov ebx, 0
	mov di, MEM_MAP
	.loop:
		mov eax, 0xe820
		mov ecx, 24
		mov edx, 0x534d4150
		int 15h
		jc .error
		add [MEM_MAP_ENTRIES], dword 1
		add di, 24
		cmp ebx, 0
		je .ok
		jmp .loop
	.error:
		cli
		hlt
	.ok:
	
	;set 80x25 video mode
	mov ah, 0x0
	mov al, 0x3
	int 0x10
	
	;install gdt
	cli
	lgdt [GDT]
	
	;enable a20
	
	;test
	call testA20
	cmp ax, 1
	je .enabledA20
	
	;BIOS method
	sti
	mov ax, 0x2401
	int 0x15
	cli
	
	;test
	call testA20
	cmp ax, 1
	je .enabledA20
	
	;keyboard controller method
	
	;disable keyboard
	call waitA20In
	mov al, 0xad
	out 0x64, al
	
	;read output port
	call waitA20In
	mov al, 0xd0
	out 0x64, al
	
	;write keyboard controller command byte
	call waitA20Out
	in al, 0x60
	push eax
	
	;write output port (demand?)
	call waitA20In
	mov al, 0xd1
	out 0x64, al
	
	;actually write to the port?
	call waitA20In
	pop eax
	or al, 2
	out 0x60, al
	
	;enable keyboard
	call waitA20In
	mov al, 0xae
	out 0x64, al
	
	;wait for the last op to finish
	call waitA20In
	
	;test
	call testA20
	cmp ax, 1
	je .enabledA20
	
	;fast A20
	in al, 0x92
	test al, 2
	jnz .after
	or al, 2
	and al, 0xfe
	out 0x92, al
	.after:
	
	;test
	call testA20
	cmp ax, 1
	je .enabledA20
	
	hlt
	
	.enabledA20:
	
	;enable pmode
	mov eax, cr0
	or eax, 1
	mov cr0, eax

	;jump to pmode
	jmp 0x8:pmode
	
bits 32

pmode:
	;set up segmetns
	mov ax, 0x10
	mov ds, ax
	mov ss, ax
	mov es, ax
	mov esp, 0x7ffff
	mov ebp, esp
	
	mov eax, 0
	mov al, [BOOT_DRIVE]
	push eax
	
	call STAGE2C
	
	add esp, 4
	
	hlt

;disk address packet structure
align 4
DAP:
DAP_SIZE		db 16
DAP_0			db 0
DAP_SECTORS		dw 1
DAP_OFFSET		dw 0x0
DAP_SEGMENT		dw 0x0
DAP_START		dd 0
DAP_HIGH		dd 0
	
times 512 - ($ - $$) db 0

;0x700
;int (*)(u8 drive, u32 sector, u16 seg, u16 off)
read:
	push ebp
	mov ebp, esp
	push esi
	
	mov edx, [ebp + 8]
	mov eax, [ebp + 12]
	mov [DAP_START], eax
	mov eax, [ebp + 16]
	mov [DAP_SEGMENT], ax
	mov eax, [ebp + 20]
	mov [DAP_OFFSET], ax
	
	;load 16bit data selectos
	mov ax, 0x20
	mov ds, ax
	mov es, ax
	mov ss, ax
	
	;load 16bit code selector
	jmp 0x18:.pmode16
	bits 16
	.pmode16:
	
	;disable pmode
	mov eax, cr0
	and al, 0xfe
	mov cr0, eax
	
	;load code segment
	jmp 0:.real
	.real:
	
	;load data segments
	mov ax, 0
	mov ds, ax
	mov es, ax
	mov ss, ax
	
	sti
	
	;error
	mov ecx, 0
	
	;read the disk
	mov ah, 0x42
	mov si, DAP
	int 0x13
	jc .error
	jmp .end
	.error:
	mov ecx, 1
	.end:
	
	cli
	
	;enable pmode
	mov eax, cr0
	or al, 1
	mov cr0, eax
	
	;load code selector
	jmp 0x8:.pmode
	bits 32
	.pmode:
	
	;load data selectors
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov ss, ax
	
	;set error
	mov eax, ecx
	
	pop esi
	mov esp, ebp
	pop ebp
	ret
	
times 1024 - ($ - $$) db 0
;memory map
MEM_MAP_ENTRIES dd 0
MEM_MAP:

times 4096 - ($ - $$) db 0
STAGE2C:
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	
	