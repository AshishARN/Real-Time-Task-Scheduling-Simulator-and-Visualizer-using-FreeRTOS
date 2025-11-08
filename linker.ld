ENTRY(_reset_handler)

MEMORY
{
  FLASH (rx) : ORIGIN = 0x00000000, LENGTH = 256K
  RAM (rwx)  : ORIGIN = 0x20000000, LENGTH = 64K
}

/* Define a symbol for the end of the .data section in Flash */
_sidata = LOADADDR(.data);

SECTIONS
{
  .text :
  {
    KEEP(*(.vector_table))
    *(.text*)
  } > FLASH

.data :
{
  . = ALIGN(4);
  _sdata = .;        /* Create a symbol for the start of .data in RAM */
  *(.data*)
  . = ALIGN(4);
  _edata = .;        /* Create a symbol for the end of .data in RAM */
} > RAM AT> FLASH

  /* This is the new, explicit BSS section */
  .bss :
  {
    . = ALIGN(4);
    __bss_start__ = .; /* Create a symbol for the start of BSS */
    *(.bss*)
    *(COMMON)
    . = ALIGN(4);
    __bss_end__ = .;   /* Create a symbol for the end of BSS */
  } > RAM

  _stack_top = ORIGIN(RAM) + LENGTH(RAM);
}