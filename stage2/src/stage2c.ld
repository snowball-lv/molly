OUTPUT(out/stage2c.exe)
SECTIONS
{
	.start (0x900) : {
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








