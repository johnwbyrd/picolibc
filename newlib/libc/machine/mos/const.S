/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Fixed constants used by the llvm-mos compiler.
 */

.section .rodata,"a",@progbits

/*
 * Location that when used with BIT will set V (overflow flag).
 * Compensates for the lack of a SEV instruction on 6502.
 */
.global __set_v
__set_v:
  .byte 0xFF
