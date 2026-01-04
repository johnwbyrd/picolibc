/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * llvm-mos imaginary registers
 *
 * These 32 zero-page locations are used by llvm-mos for code generation.
 * They act as virtual registers on the register-starved 6502.
 *
 * Override __rc_base before including this file to relocate all registers.
 * Individual registers can also be overridden by defining them earlier.
 */

PROVIDE(__rc_base = 0x00);

PROVIDE(__rc0 = __rc_base);
PROVIDE(__rc1 = __rc0 + 1);
PROVIDE(__rc2 = __rc1 + 1);
PROVIDE(__rc3 = __rc2 + 1);
PROVIDE(__rc4 = __rc3 + 1);
PROVIDE(__rc5 = __rc4 + 1);
PROVIDE(__rc6 = __rc5 + 1);
PROVIDE(__rc7 = __rc6 + 1);
PROVIDE(__rc8 = __rc7 + 1);
PROVIDE(__rc9 = __rc8 + 1);
PROVIDE(__rc10 = __rc9 + 1);
PROVIDE(__rc11 = __rc10 + 1);
PROVIDE(__rc12 = __rc11 + 1);
PROVIDE(__rc13 = __rc12 + 1);
PROVIDE(__rc14 = __rc13 + 1);
PROVIDE(__rc15 = __rc14 + 1);
PROVIDE(__rc16 = __rc15 + 1);
PROVIDE(__rc17 = __rc16 + 1);
PROVIDE(__rc18 = __rc17 + 1);
PROVIDE(__rc19 = __rc18 + 1);
PROVIDE(__rc20 = __rc19 + 1);
PROVIDE(__rc21 = __rc20 + 1);
PROVIDE(__rc22 = __rc21 + 1);
PROVIDE(__rc23 = __rc22 + 1);
PROVIDE(__rc24 = __rc23 + 1);
PROVIDE(__rc25 = __rc24 + 1);
PROVIDE(__rc26 = __rc25 + 1);
PROVIDE(__rc27 = __rc26 + 1);
PROVIDE(__rc28 = __rc27 + 1);
PROVIDE(__rc29 = __rc28 + 1);
PROVIDE(__rc30 = __rc29 + 1);
PROVIDE(__rc31 = __rc30 + 1);

/* 6502 interrupt vector location - override for other 6502 variants */
PROVIDE(__vectors_start = 0xFFFA);
PROVIDE(__vectors_length = 6);
