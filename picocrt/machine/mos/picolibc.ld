/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Picolibc linker script for MOS 6502 with ZBC semihosting.
 *
 * Memory layout:
 *   0x0000-0x001F  Imaginary registers (__rc0-__rc31)
 *   0x0020-0x00FF  Zero page variables
 *   0x0100-0x01FF  Hardware stack (6502)
 *   0x0200-0xFCDF  Program (code + data + BSS + soft stack)
 *   0xFCE0-0xFCFF  ZBC semihost device (32 bytes)
 *   0xFD00-0xFFFF  Reserved
 */

ENTRY(_start)

/* llvm-mos imaginary registers in zero page - chained definitions */
__rc0 = 0x00;
__rc1 = __rc0 + 1;
PROVIDE(__rc2 = __rc1 + 1);
__rc3 = __rc2 + 1;
PROVIDE(__rc4 = __rc3 + 1);
__rc5 = __rc4 + 1;
PROVIDE(__rc6 = __rc5 + 1);
__rc7 = __rc6 + 1;
PROVIDE(__rc8 = __rc7 + 1);
__rc9 = __rc8 + 1;
PROVIDE(__rc10 = __rc9 + 1);
__rc11 = __rc10 + 1;
PROVIDE(__rc12 = __rc11 + 1);
__rc13 = __rc12 + 1;
PROVIDE(__rc14 = __rc13 + 1);
__rc15 = __rc14 + 1;
PROVIDE(__rc16 = __rc15 + 1);
__rc17 = __rc16 + 1;
PROVIDE(__rc18 = __rc17 + 1);
__rc19 = __rc18 + 1;
PROVIDE(__rc20 = __rc19 + 1);
__rc21 = __rc20 + 1;
PROVIDE(__rc22 = __rc21 + 1);
__rc23 = __rc22 + 1;
PROVIDE(__rc24 = __rc23 + 1);
__rc25 = __rc24 + 1;
PROVIDE(__rc26 = __rc25 + 1);
__rc27 = __rc26 + 1;
PROVIDE(__rc28 = __rc27 + 1);
__rc29 = __rc28 + 1;
PROVIDE(__rc30 = __rc29 + 1);
__rc31 = __rc30 + 1;

MEMORY {
    zp  (rw)  : ORIGIN = __rc31 + 1, LENGTH = 0x100 - (__rc31 + 1)   /* Zero page after registers */
    ram (rwx) : ORIGIN = 0x0200, LENGTH = 0xFAE0   /* Up to 0xFCDF */
}

/* Soft stack at top of RAM, grows down */
__stack = 0xFCDF;

SECTIONS {
    /* Code */
    .text : {
        *(.text.startup .text.startup.*)
        *(.text .text.*)
        *(.gnu.linkonce.t.*)
    } > ram

    /* Read-only data */
    .rodata : {
        *(.rodata .rodata.*)
        *(.gnu.linkonce.r.*)
    } > ram

    /* Constructors */
    .init_array : {
        PROVIDE_HIDDEN(__preinit_array_start = .);
        KEEP(*(.preinit_array))
        PROVIDE_HIDDEN(__preinit_array_end = .);
        PROVIDE_HIDDEN(__init_array_start = .);
        KEEP(*(SORT(.init_array.*)))
        KEEP(*(.init_array))
        PROVIDE_HIDDEN(__init_array_end = .);
    } > ram

    /* Destructors */
    .fini_array : {
        PROVIDE_HIDDEN(__fini_array_start = .);
        KEEP(*(SORT(.fini_array.*)))
        KEEP(*(.fini_array))
        PROVIDE_HIDDEN(__fini_array_end = .);
    } > ram

    /* Initialized data - source in RAM for RAM-only system */
    .data : {
        __data_start = .;
        *(.data .data.*)
        *(.gnu.linkonce.d.*)
        __data_end = .;
    } > ram
    __data_source = __data_start;  /* RAM-only: no copy needed */
    __data_size = __data_end - __data_start;

    /* Uninitialized data */
    .bss (NOLOAD) : {
        __bss_start = .;
        *(.bss .bss.*)
        *(.gnu.linkonce.b.*)
        *(COMMON)
        __bss_end = .;
    } > ram
    __bss_size = __bss_end - __bss_start;

    /* Zero page BSS */
    .zp.bss (NOLOAD) : {
        *(.zp.bss .zp.bss.*)
    } > zp

    /* Zero page initialized data */
    .zp.data : {
        *(.zp.data .zp.data.*)
    } > zp

    /* TLS - not typically used on 6502 but define for compatibility */
    __tls_base = .;
    __tdata_end = .;
    __tls_end = .;

    /* End of used RAM */
    _end = .;
    PROVIDE(end = .);

    /* Discard unwanted sections */
    /DISCARD/ : {
        *(.eh_frame .eh_frame.*)
        *(.note .note.*)
        *(.comment)
    }
}
