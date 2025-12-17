/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Initialize soft stack pointer from __stack symbol.
 * This code runs before any C code executes.
 *
 * llvm-mos generates a reference to __do_init_stack whenever code
 * references __rc0 or __rc1, which are the soft stack pointer registers.
 * The linker pulls in this object to satisfy that reference.
 */

.global __do_init_stack

/* Initialize soft stack pointer from __stack symbol, then jump to _start.
 * Placed in .init.100 section to run early in startup sequence.
 * The reset vector points here, so after initializing the stack we must
 * explicitly jump to _start (we can't fall through because other code
 * may be placed between .init and .text sections).
 */
.section .init.100,"ax",@progbits
__do_init_stack:
	lda #mos16lo(__stack)
	sta __rc0
	lda #mos16hi(__stack)
	sta __rc1
	jmp _start
