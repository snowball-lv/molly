bits 16
org 0x7c00 + 62 ;boot sector address + fat16 boot code offset

%define BYTES_PER_SECTOR	0x7c00 + 11 ;word
%define ROOT_ENTRIES		0x7c00 + 17 ;word
%define NUMBER_OF_FATS		0x7c00 + 16 ;byte
%define SECTORS_PER_FAT		0x7c00 + 22 ;word
%define RESERVED_SECTORS	0x7c00 + 14 ;word
%define HIDDEN_SECTORS		0x7c00 + 28 ;dword
%define SECTORS_PER_CLUSTER	0x7c00 + 13 ;byte

jmp start

BOOT_DRIVE		db 0
ROOT_SECTORS	dw 0
ROOT_START		dw 0
DATA			dw 0

;caller-saved: ax, cx, dx
;callee-saved: bx, si, di, bp, sp

println: ;void (char *msg)
	push bp
	mov bp, sp
	
	push si
	mov si, [bp + 4]
	
	mov ah,	0eh
	.start:
		lodsb 
		cmp al, 0
		jz .printDone
		int 10h
		jmp .start
	.printDone:
	
	mov al, 13
	int 10h
	mov al, 10
	int 10h
	
	pop si
	
	pop bp
	ret

nextCluster: ;word (word cluster)
	push bp
	mov bp, sp
	
	push si
	mov si, [bp + 4]
	add si, si
	
	push fs
	mov ax, 0x7e0
	mov fs, ax
	mov ax, [fs:si]
	pop fs
	
	pop si
	
	cmp ax, 0xfff8
	jae .last
	jmp .end
	
	.last:
	mov ax, 0
	
	.end:
	pop bp
	ret
	
readDisk: ;void ()
	push si
	
	mov si, DAP
	mov ah, 0x42
	mov dl, [BOOT_DRIVE]
	int 0x13
	jc .error
	
	jmp .end
	
	.error:
		push err1
		call println
		add [err0], byte ah
		push err0
		call println
		cli
		hlt
	
	.end:
	
	pop si
	ret

ok 		db "ok", 0
err0 	db "0", 0	;int13 error
err1 	db "1", 0	;read disk error

;disk address packet structure
align 4
DAP:
DAP_SIZE		db 16
DAP_0			db 0
DAP_SECTORS		dw 1
DAP_OFFSET		dw 0x0
DAP_SEGMENT		dw 0x7e0
DAP_START		dd 0
DAP_HIGH		dd 0

MARKER		db "STAGE2  BIN", 0
CLUSTER		dw 0
FILE_SIZE	dd 0
	
start:
	;save boot drive number
	mov [BOOT_DRIVE], dl
	
	;set up segments
	mov ax, 0
	mov es, ax
	mov ds, ax
	mov fs, ax
	mov gs, ax
	;set up stack
	mov ax, 0x7000
	mov ss, ax
	mov ax, 0xffff
	mov sp, ax
	mov bp, ax
	
	;get root sector count
	mov ax, 32
	mul word [ROOT_ENTRIES]
	add ax, 16
	mov dx, 0
	div word [BYTES_PER_SECTOR]
	mov [ROOT_SECTORS], ax
	
	;get start of root
	mov ax, 0
	mov al, [NUMBER_OF_FATS]
	mul word [SECTORS_PER_FAT] 
	add ax, [RESERVED_SECTORS]
	mov [ROOT_START], ax
	
	;get start of data
	mov ax, [ROOT_START]
	add ax, [ROOT_SECTORS]
	mov [DATA], ax
	
	;test int 0x13 extensions
	mov ah, 0x41
	mov bx, 0x55aa
	mov dl, [BOOT_DRIVE]
	int 0x13
	;check for error
	mov eax, err0
	jc .error
	
	;read root
	mov ax, [ROOT_SECTORS]
	mov [DAP_SECTORS], ax
	mov ax, [ROOT_START]
	mov [DAP_START], ax
	call readDisk
	
	;search for the marker
	mov cx, [ROOT_ENTRIES]
	mov di, 0
	mov ax, 0x7e0
	mov es, ax
	.loop:
		push cx
		push di
		
		mov cx, 11
		mov si, MARKER
		cld
		repe cmpsb
		je .found
		
		pop di
		add di, 32
		pop cx
	loop .loop
	mov ax, 0
	mov es, ax
	jmp .error
	
	;marker found
	.found:
	pop di
	add sp, 2
	mov ax, [es:di + 26]
	mov [CLUSTER], ax
	mov eax, [es:di + 28]
	mov [FILE_SIZE], eax
	mov ax, 0
	mov es, ax
	
	;read fat
	mov ax, [SECTORS_PER_FAT]
	mov [DAP_SECTORS], ax
	mov ax, [RESERVED_SECTORS]
	mov [DAP_START], ax
	call readDisk
	
	;read stage 2
	mov [DAP_SEGMENT], word 0x0050
	mov ax, [CLUSTER]
	.cluster:
		
		push ax
		
		;read cluster
		sub ax, 2
		mov cx, 0
		mov cl, [SECTORS_PER_CLUSTER]
		mul cx
		add ax, [DATA]
		mov [DAP_START], ax
		
		mov ax, 0
		mov al, [SECTORS_PER_CLUSTER]
		mov [DAP_SECTORS], ax
		
		call readDisk
		
		mov ax, [BYTES_PER_SECTOR]
		mov cx, 0
		mov cl, [SECTORS_PER_CLUSTER]
		mul cx
		add [DAP_OFFSET], ax
	
		pop ax
		
		push ax
		call nextCluster
		add sp, 2
		
		cmp ax, 0
		je .end
		
		jmp .cluster
		
	.end:
	
	push ok
	call println
	add sp, 2
	
	mov dl, [BOOT_DRIVE]
	jmp 0x0500
	
	cli
	hlt
	
	.error:
	
	push eax
	call println
	cli
	hlt

times 448 - ($ - $$) db 0
;0xaa55








