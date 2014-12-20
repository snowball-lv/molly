ENTRY(_kernel_main)
OUTPUT(bin/kernel.exe)
SECTIONS
{
	.start (0x100000) : {
		*(.start)
	}
	.text : {
		*(.text)
	}
	.data : {
		*(.data)
	}
	.rdata : {
		*(.rdata*)
	}
	.bss : { 					
		*(.bss)
	}
	/DISCARD/ : {
		*(.eh_frame)
	}
}











