/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025, James Byrne
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

#include <picolibc.h>
#include <stdint.h>
#include <errno.h>
#include "virtual.h"

/*
 * Virtual semihosting implementation for MAME/emulator integration.
 * Serializes syscalls into RIFF format for parsing by Lua scripts.
 *
 * Memory layout at SEMIHOST_VIRTUAL_BASE:
 *   +0x00: RIFF header (12 bytes)
 *   +0x0C: CNFG chunk (12 bytes)
 *   +0x18: CALL chunk (variable, replaced by RETN when complete)
 */

#define CALL_CHUNK_OFFSET 24

uintptr_t sys_semihost(uintptr_t op, uintptr_t param)
{
    volatile uint8_t *base = (volatile uint8_t *)SEMIHOST_VIRTUAL_BASE;
    volatile uint8_t *call_chunk = base + CALL_CHUNK_OFFSET;
    static int initialized = 0;

    /* Lazy initialization on first call */
    if (!initialized) {
        semihost_virtual_init(base);
        initialized = 1;
    }

    /* Write CALL chunk */
    semihost_write_fourcc(call_chunk, "CALL");
    semihost_write_u32_le(call_chunk + 4, 4 + sizeof(uintptr_t));  /* chunk_size */
    call_chunk[8] = (uint8_t)op;  /* opcode */
    call_chunk[9] = 0;            /* reserved */
    call_chunk[10] = 0;           /* reserved */
    call_chunk[11] = 0;           /* reserved */
    semihost_write_ptr(call_chunk + 12, param, sizeof(uintptr_t));  /* arg_ptr */

    semihost_memory_barrier();

    /* Trigger MAME device by writing to base address */
    semihost_trigger(base);

    /* Busy-wait for RETN chunk to replace CALL chunk */
    while (!semihost_is_retn_ready(call_chunk)) {
        /* Polling loop - MAME will replace CALL with RETN */
    }

    semihost_memory_barrier();

    /* Read RETN chunk */
    /* Format: 'RETN' [size] [result:word_size] [errno:4] */
    uintptr_t result = semihost_read_word(call_chunk + 8, sizeof(uintptr_t));
    uint32_t errno_val = semihost_read_u32_le(call_chunk + 8 + sizeof(uintptr_t));

    /* Set errno if syscall failed */
    if (errno_val != 0) {
        errno = errno_val;
    }

    return result;
}
