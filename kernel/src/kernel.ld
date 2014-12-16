ENTRY(_kernel_main)
OUTPUT(bin/kernel.exe)
SECTIONS
{
  .text (0x100000) : {
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











