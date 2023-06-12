
ENTRY(APP_Entry)
MEMORY {
    flash (rx): ORIGIN = 0x60000000 +0x1E0000, LENGTH = 0x100000
    ram (rwx): ORIGIN = 0x80000000 +0xF00000, LENGTH = 0x100000
}
SECTIONS {
    . = ORIGIN(flash) + 0x80;
    /* From flash to current position is revered for header,  128 bytes. */
    __boot_start = .;
    .boot_app_entry . : 
    {
        *(.APP_Entry)
    } > flash
    
    .app_text ALIGN(0x20) : 
    {
      *(.text .text .text.* .gnu.linkonce.t.* .rdata .rodata .rodata.* .gnu.linkonce.r.* .glue_7 .glue_7t .vfp11_veneer .v4_bx)
      . = ALIGN(0x20);
    } > flash
	  
    _app_data_flash_start = .;

    . = ORIGIN(ram);
    .app_data ALIGN(0x20) : AT(_app_data_flash_start)
    {
        _app_data_ram_start = .;
        *(.data .data.* .gnu.linkonce.d.*);
        _app_data_ram_end = .;
        . = ALIGN(0x20);
    }

    _app_data_flash_end = _app_data_flash_start + (_app_data_ram_end - _app_data_ram_start);

    .app_bss ALIGN(0x20) : 
    {
        _app_bss_start = .;
        *(.bss .bss.* COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .gnu.linkonce.b.*);
        _app_bss_end = .;
        . = ALIGN(0x20);
    }
    _app_ram_end = .;
	
    ASSERT(_app_data_flash_end <= ORIGIN(flash) + LENGTH(flash), "FLASH overflow")
    ASSERT(_app_ram_end <= ORIGIN(ram) + LENGTH(ram), "RAM overflow")    
}
