/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * MOS 6502 sys_exit_extended implementation for ZBC semihosting.
 *
 * This overrides semihost/common/sys_exit_extended.c to avoid warnings
 * about ARM semihosting constants being truncated on 16-bit targets.
 */

#include "../common/semihost-private.h"
#include <sys/cdefs.h>

__noreturn void
sys_semihost_exit_extended(uintptr_t code)
{
    /*
     * For ZBC on MOS, pass the exit code directly.
     * ARM's ADP_Stopped_ApplicationExit (0x20026) doesn't fit in 16 bits.
     */
    (void)sys_semihost2(SYS_EXIT_EXTENDED, code, code);
    __builtin_unreachable();
}
