/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * setjmp/longjmp for MOS 6502.
 *
 * jmp_buf layout (19 bytes):
 *   [0-1]   Return address (from hardware stack)
 *   [2]     Hardware stack pointer
 *   [3-4]   Soft stack pointer (__rc0/__rc1)
 *   [5-18]  Callee-saved registers (__rc18-__rc31)
 *
 * The jmp_buf pointer is passed in __rc2/__rc3.
 * Return value is in A (low byte) and X (high byte).
 */

.include "imag.inc"

.section .text.setjmp
.global setjmp
setjmp:
	/* Save return address from hardware stack */
	tsx
	lda $101,x
	ldy #0
	sta (__rc2),y
	lda $102,x
	iny
	sta (__rc2),y

	/* Save hardware stack pointer */
	txa
	iny
	sta (__rc2),y

	/* Save soft stack pointer */
	lda __rc0
	iny
	sta (__rc2),y
	lda __rc1
	iny
	sta (__rc2),y

	/* Save callee-saved registers (__rc18-__rc31) */
	lda __rc18
	iny
	sta (__rc2),y
	lda __rc19
	iny
	sta (__rc2),y
	lda __rc20
	iny
	sta (__rc2),y
	lda __rc21
	iny
	sta (__rc2),y
	lda __rc22
	iny
	sta (__rc2),y
	lda __rc23
	iny
	sta (__rc2),y
	lda __rc24
	iny
	sta (__rc2),y
	lda __rc25
	iny
	sta (__rc2),y
	lda __rc26
	iny
	sta (__rc2),y
	lda __rc27
	iny
	sta (__rc2),y
	lda __rc28
	iny
	sta (__rc2),y
	lda __rc29
	iny
	sta (__rc2),y
	lda __rc30
	iny
	sta (__rc2),y
	lda __rc31
	iny
	sta (__rc2),y

	/* Return zero */
	lda #0
	ldx #0
	rts

.section .text.longjmp
.global longjmp
longjmp:
	/* Save return value in scratch registers */
	sta __rc16
	stx __rc17

	/* Restore callee-saved registers (__rc31 down to __rc18) */
	ldy #18
	lda (__rc2),y
	sta __rc31
	dey
	lda (__rc2),y
	sta __rc30
	dey
	lda (__rc2),y
	sta __rc29
	dey
	lda (__rc2),y
	sta __rc28
	dey
	lda (__rc2),y
	sta __rc27
	dey
	lda (__rc2),y
	sta __rc26
	dey
	lda (__rc2),y
	sta __rc25
	dey
	lda (__rc2),y
	sta __rc24
	dey
	lda (__rc2),y
	sta __rc23
	dey
	lda (__rc2),y
	sta __rc22
	dey
	lda (__rc2),y
	sta __rc21
	dey
	lda (__rc2),y
	sta __rc20
	dey
	lda (__rc2),y
	sta __rc19
	dey
	lda (__rc2),y
	sta __rc18

	/* Restore soft stack pointer */
	dey
	lda (__rc2),y
	sta __rc1
	dey
	lda (__rc2),y
	sta __rc0

	/* Restore hardware stack pointer */
	dey
	lda (__rc2),y
	tax
	txs

	/* Restore return address to hardware stack */
	dey
	lda (__rc2),y
	sta $102,x
	dey
	lda (__rc2),y
	sta $101,x

	/* Return saved value */
	lda __rc16
	ldx __rc17

	rts
