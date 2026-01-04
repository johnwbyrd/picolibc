/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * 6502 interrupt vectors section
 *
 * The 6502 vectors at 0xFFFA-0xFFFF are:
 *   0xFFFA: NMI vector
 *   0xFFFC: RESET vector (points to __do_init_stack)
 *   0xFFFE: IRQ/BRK vector
 *
 * Override __nmi_handler, __reset_handler, or __irq_handler to customize.
 */

PROVIDE(__nmi_handler = 0x0000);
PROVIDE(__reset_handler = __do_init_stack);
PROVIDE(__irq_handler = 0x0000);

	.vectors (TYPE = SHT_PROGBITS) : {
		SHORT(__nmi_handler);
		SHORT(__reset_handler);
		SHORT(__irq_handler);
	} > vectors AT> vectors :text
