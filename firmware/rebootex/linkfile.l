OUTPUT_FORMAT("elf32-littlemips")
OUTPUT_ARCH(mips)

ENTRY(rebootex_entry)

SECTIONS
{
  . = 0x88F10000;
  .text.start : {
    *(.text.start)
  }
  .text : {
    *(.text)
  }
  .rodata : {
    *(.rodata)
  }
  .data : {
    *(.data)
  }
  .bss : {
    *(.bss)
  }
}
