/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * llvm-mos imaginary register declarations for assembly files.
 *
 * The .zeropage directive tells the assembler these symbols are in
 * zero page, enabling the more efficient zero-page addressing modes.
 */

.zeropage __rc0,  __rc1,  __rc2,  __rc3,  __rc4,  __rc5,  __rc6,  __rc7
.zeropage __rc8,  __rc9,  __rc10, __rc11, __rc12, __rc13, __rc14, __rc15
.zeropage __rc16, __rc17, __rc18, __rc19, __rc20, __rc21, __rc22, __rc23
.zeropage __rc24, __rc25, __rc26, __rc27, __rc28, __rc29, __rc30, __rc31
