/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above
 *    copyright notice, this list of conditions and the following
 *    disclaimer in the documentation and/or other materials provided
 *    with the distribution.
 *
 * 3. Neither the name of the copyright holder nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
 * FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
 * COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT,
 * INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * ZBC Semihosting adapter for picolibc.
 *
 * Implements sys_semihost() using the ZBC client library, enabling
 * picolibc's existing semihost/common/ layer to work with ZBC devices.
 */

#ifndef ZBC_CLIENT
#define ZBC_CLIENT
#endif
#include "zbc_semihost.h"

/* Forward declaration */
uintptr_t sys_semihost(uintptr_t op, uintptr_t param);

/*
 * Calculate ZBC device base address.
 *
 * Per ZBC specification, the device is located at:
 *   reserved_start - 512 - 32
 * where reserved_start = (1 << addr_bits) - (1 << (addr_bits/2))
 *
 * For common architectures:
 *   16-bit (6502): 0xFCE0
 *   32-bit: 0xFFFEFDE0
 *   64-bit: 0xFFFFFFFEFFFFFDE0
 */
static volatile uint8_t *
zbc_get_device_base(void)
{
    const unsigned int addr_bits = sizeof(void *) * 8;
    const unsigned int half_bits = addr_bits / 2;
    const uintptr_t    reserved_start = ~(uintptr_t)0 - ((uintptr_t)1 << half_bits) + 1;
    return (volatile uint8_t *)(reserved_start - 512 - 32);
}

/* Static client state - initialized on first call */
static zbc_client_state_t zbc_state;
static int                zbc_initialized = 0;

/* Static RIFF buffer for requests/responses */
#define ZBC_RIFF_BUF_SIZE 512
static uint8_t zbc_riff_buf[ZBC_RIFF_BUF_SIZE];

/*
 * Initialize ZBC client on first use.
 */
static void
zbc_init_once(void)
{
    if (!zbc_initialized) {
        zbc_client_init(&zbc_state, zbc_get_device_base());
        zbc_initialized = 1;
    }
}

/*
 * sys_semihost - ARM-compatible semihosting entry point.
 *
 * This function is called by picolibc's semihost/common/ layer via
 * the sys_semihost1/2/3 helper macros in semihost-private.h.
 *
 * Most ARM semihosting calls pass param as a pointer to a parameter block
 * (struct with arguments), which matches what zbc_semihost() expects.
 * However, SYS_WRITEC and SYS_WRITE0 pass param as a direct pointer to
 * the data, not a pointer to an args array. We wrap these two cases.
 *
 * @param op    SYS_* opcode (ARM semihosting compatible)
 * @param param Pointer to parameter block (struct with arguments)
 * @return      Syscall result, or (uintptr_t)-1 on error
 */
uintptr_t
sys_semihost(uintptr_t op, uintptr_t param)
{
    zbc_init_once();

    /* SYS_WRITEC and SYS_WRITE0 pass direct pointers, not args arrays */
    if (op == SH_SYS_WRITEC || op == SH_SYS_WRITE0) {
        uintptr_t args[1];
        args[0] = param;
        return zbc_semihost(&zbc_state, zbc_riff_buf, ZBC_RIFF_BUF_SIZE, op, (uintptr_t)args);
    }

    return zbc_semihost(&zbc_state, zbc_riff_buf, ZBC_RIFF_BUF_SIZE, op, param);
}
