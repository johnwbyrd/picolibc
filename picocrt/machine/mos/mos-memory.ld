/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * 6502 vector memory region
 *
 * Uses __vectors_start and __vectors_length defined in mos-regs.ld.
 * Override those symbols for 65816 or other 6502 variants.
 */

	vectors (r) : ORIGIN = __vectors_start, LENGTH = __vectors_length
