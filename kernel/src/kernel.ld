OUTPUT(bin/kernel.exe)
SECTIONS
{
	__KERNEL_BASE = 0xc0100000;
	
	.start (__KERNEL_BASE) : {
		*(.start)
	}
	.text : ALIGN(0x1000) {
		*(.text)
	}
	.rdata : ALIGN(0x1000) {
		*(.rdata)
	}
	.data : ALIGN(0x1000) {
		*(.data)
	}
	
	.ramfs : ALIGN(0x1000) {	
		__RAMFS_START = .;
		"../ramfs/ramfs_img.o"
	}
	
	.bss : ALIGN(0x1000) { 	
		__BSS_START = .;				
		*(.bss)
	}
	
	. = ALIGN(0x1000);
	
	__BSS_END 		= .;
	__KERNEL_END 	= .;
	__HEAP_START 	= .;
	
	/DISCARD/ : {
		*(.eh_frame)
	}
}
















