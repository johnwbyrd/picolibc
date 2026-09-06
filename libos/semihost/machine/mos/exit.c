/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * MOS 6502 exit implementation for ZBC semihosting.
 *
 * This overrides semihost/common/exit.c to avoid warnings about
 * ARM semihosting constants (0x20026, etc.) being truncated on
 * 16-bit targets. ZBC doesn't require specific ARM exit codes.
 */

#include "../common/semihost-private.h"
#include <sys/cdefs.h>
#include <unistd.h>

__noreturn void
_exit(int code)
{
    if (sys_semihost_feature(SH_EXT_EXIT_EXTENDED)) {
        sys_semihost_exit_extended(code);
    } else {
        /*
         * For ZBC on MOS, we just pass the exit code directly.
         * The ARM ADP_Stopped_* constants don't fit in 16 bits
         * and ZBC doesn't require them.
         */
        sys_semihost_exit((uintptr_t)code, code);
    }
}
