ENTRY(_stage2)
OUTPUT(out/stage2c.exe)
SECTIONS
{
  .text (0x900) : {
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











