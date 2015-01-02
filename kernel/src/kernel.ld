ENTRY(_kernel_main)
OUTPUT(bin/kernel.exe)
SECTIONS
{
	__KERNEL_BASE = 0x100000;
	
	.start (__KERNEL_BASE) : {
		*(.start)
	}
	.text : ALIGN(0x1000) {
		*(.text)
	}
	.rdata : ALIGN(0x1000) {
		*(.rdata*)
	}
	.data : ALIGN(0x1000) {
		*(.data)
	}
	.bss : ALIGN(0x1000) { 					
		*(.bss)
	}
	
	. = ALIGN(0x1000);
	__KERNEL_END = .;
	__HEAP_START = .;
	
	/DISCARD/ : {
		*(.eh_frame)
	}
}
















