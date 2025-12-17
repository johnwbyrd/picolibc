/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Indirect call trampoline for llvm-mos.
 *
 * The 6502 doesn't have an indirect JSR instruction, so llvm-mos generates
 * `jsr __call_indir` when it needs to call through a function pointer.
 * The function pointer is placed in __rc18/__rc19 before the call.
 *
 * This trampoline performs `jmp (__rc18)`, which jumps to the target.
 * When the target executes RTS, it returns to the original caller
 * (skipping this stub), because we used JMP not JSR.
 */

.text
.global __call_indir
__call_indir:
	jmp (__rc18)
