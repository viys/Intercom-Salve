/* Copyright (C) 2018 RDA Technologies Limited and/or its affiliates("RDA").
 * All rights reserved.
 *
 * This software is supplied "AS IS" without any warranties.
 * RDA assumes no responsibility or liability for the use of the software,
 * conveys no license or title under any patent, copyright, or mask work
 * right to the product. RDA reserves the right to make changes in the
 * software without notification.  RDA also make no representation or
 * warranty that such application will be suitable for the specified use
 * without further testing or modification.
 */
 
OUTPUT_FORMAT("elf32-littlearm", "elf32-littlearm", "elf32-littlearm")
OUTPUT_ARCH(arm)
ENTRY(_boot_app_entry)
EXTERN(_boot_app_entry)

MEMORY {
    flash (rwx): ORIGIN = 0x60020000, LENGTH = 0x10000
    sram (rwx): ORIGIN = 0x838000, LENGTH = 0x8000
}

SECTIONS {

    /* ============================= */
    /* entry in flash                */
    /* ============================= */
    /* Revered 128 bytes for header */
    . = ORIGIN(flash) + 0x80;

    ._boot_app_api . :
    {
        KEEP(*(._boot_app_api))
    } > flash
	
	__flash_location = .;

    .entry __flash_location : AT(ADDR(.entry))
    {                                               
        __entry_start = .;                       
        *(._boot_app_entry)
        . = ALIGN(0x10);
        __entry_end = .;                         
    }                                               
    __flash_location += SIZEOF(.entry);	
	
    . = ORIGIN(sram);

    .sram_data . : AT(__flash_location)
    {
        __sram_data_start = .;
        *(.data .data.* .sramdata .gnu.linkonce.d.*) *(.sramdata)
        . = ALIGN(0x10);
        __sram_data_end = .;
        __sram_data_load_start = LOADADDR(.sram_data);
    }
    __flash_location += SIZEOF(.sram_data);

    .sram_bss . (NOLOAD) : AT(ADDR(.sram_bss))
    {
        __sram_bss_start = .;
        *(.bss .bss.* .srambss COMMON .scommon .sbss .sbss.* .sbss2 .sbss2.* .gnu.linkonce.b.*) *(.srambss)
        . = ALIGN(0x10);
        __sram_bss_end = .;
    }
    __boot_app_sram_end = .;
	
    ASSERT(__boot_app_sram_end <= ORIGIN(sram) + LENGTH(sram), "SRAM overflow")

    .flash_text __flash_location : AT(ADDR(.flash_text))
    {
        __flash_text_start = .;
        *(.text .text.* .sramboottext .sramtext .gnu.linkonce.t.* .glue_7 .glue_7t .vfp11_veneer .v4_bx .rdata .rodata .rodata.* .gnu.linkonce.r.*)
        . = ALIGN(0x10);
        __flash_text_end = .;
    }
	
    /DISCARD/ : { *(.iplt*) }
    /DISCARD/ : { *(.igot*) }
    /DISCARD/ : { *(.rel*) }
    /DISCARD/ : { *(.dynstr*) }
    /DISCARD/ : { *(.dynamic*) }
    /DISCARD/ : { *(.plt*) }
    /DISCARD/ : { *(.interp*) }
    /DISCARD/ : { *(.gnu*) }
}











