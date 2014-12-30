OUTPUT(out/stage2c.exe)
SECTIONS
{
	.start (0x1500) : {
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
	/DISCARD/ : {
		*(.eh_frame)
	}
}








