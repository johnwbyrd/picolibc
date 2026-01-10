/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * MOS 6502 setjmp buffer definition.
 *
 * jmp_buf layout (19 bytes):
 *   [0-1]   Return address (from hardware stack)
 *   [2]     Hardware stack pointer
 *   [3-4]   Soft stack pointer (__rc0/__rc1)
 *   [5-18]  Callee-saved registers (__rc18-__rc31)
 */

#ifndef _MACHINE_SETJMP_H_
#define _MACHINE_SETJMP_H_

#define _JBLEN  19
#define _JBTYPE unsigned char

typedef _JBTYPE jmp_buf[_JBLEN];

#endif /* _MACHINE_SETJMP_H_ */
