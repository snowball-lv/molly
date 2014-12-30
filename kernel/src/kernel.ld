ENTRY(_kernel_main)
OUTPUT(bin/kernel.exe)
SECTIONS
{
	.start (0x100000) : {
		*(.start)
	}
	.bss : { 					
		*(.bss)
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
	.end : { 					
		*(.end)
	}
	/DISCARD/ : {
		*(.eh_frame)
	}
}











