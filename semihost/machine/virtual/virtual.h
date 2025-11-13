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

#ifndef _VIRTUAL_H_
#define _VIRTUAL_H_

#include <stdint.h>
#include <string.h>

/*
 * Virtual semihosting using RIFF-based protocol for MAME/emulator integration.
 * Serializes semihosting calls into RIFF format that can be parsed by Lua scripts.
 */

/* Default base address - top of address space for each word size */
#ifndef SEMIHOST_VIRTUAL_BASE
#  if __SIZEOF_POINTER__ == 8
#    define SEMIHOST_VIRTUAL_BASE 0xFFFFFFFFFFFFFC00UL
#  elif __SIZEOF_POINTER__ == 4
#    define SEMIHOST_VIRTUAL_BASE 0xFFFFFC00UL
#  elif __SIZEOF_POINTER__ == 2
#    define SEMIHOST_VIRTUAL_BASE 0xFC00U
#  elif __SIZEOF_POINTER__ == 1
#    define SEMIHOST_VIRTUAL_BASE 0xC0U
#  else
#    error "Unsupported pointer size"
#  endif
#endif

/* Detect endianness */
#if defined(__BYTE_ORDER__)
#  if __BYTE_ORDER__ == __ORDER_LITTLE_ENDIAN__
#    define SEMIHOST_VIRTUAL_ENDIAN 0
#  elif __BYTE_ORDER__ == __ORDER_BIG_ENDIAN__
#    define SEMIHOST_VIRTUAL_ENDIAN 1
#  elif __BYTE_ORDER__ == __ORDER_PDP_ENDIAN__
#    define SEMIHOST_VIRTUAL_ENDIAN 2
#  else
#    error "Unknown byte order"
#  endif
#else
#  error "Cannot detect byte order"
#endif

/* Memory barrier for MMIO correctness */
static inline void semihost_memory_barrier(void) {
#if defined(__GNUC__) || defined(__clang__)
    __asm__ volatile("" ::: "memory");
#endif
}

/* RIFF chunk writing helpers */
static inline void semihost_write_fourcc(volatile uint8_t *ptr, const char *fourcc) {
    ptr[0] = fourcc[0];
    ptr[1] = fourcc[1];
    ptr[2] = fourcc[2];
    ptr[3] = fourcc[3];
}

static inline void semihost_write_u32_le(volatile uint8_t *ptr, uint32_t val) {
    ptr[0] = (uint8_t)(val >> 0);
    ptr[1] = (uint8_t)(val >> 8);
    ptr[2] = (uint8_t)(val >> 16);
    ptr[3] = (uint8_t)(val >> 24);
}

static inline void semihost_write_ptr(volatile uint8_t *ptr, uintptr_t val, size_t ptr_size) {
    for (size_t i = 0; i < ptr_size; i++) {
#if SEMIHOST_VIRTUAL_ENDIAN == 0  /* Little endian */
        ptr[i] = (uint8_t)(val >> (i * 8));
#elif SEMIHOST_VIRTUAL_ENDIAN == 1  /* Big endian */
        ptr[i] = (uint8_t)(val >> ((ptr_size - 1 - i) * 8));
#else
#  error "PDP endian not supported for pointer writes"
#endif
    }
}

static inline uintptr_t semihost_read_word(volatile uint8_t *ptr, size_t word_size) {
    uintptr_t val = 0;
    for (size_t i = 0; i < word_size && i < sizeof(uintptr_t); i++) {
#if SEMIHOST_VIRTUAL_ENDIAN == 0  /* Little endian */
        val |= (uintptr_t)ptr[i] << (i * 8);
#elif SEMIHOST_VIRTUAL_ENDIAN == 1  /* Big endian */
        val |= (uintptr_t)ptr[i] << ((word_size - 1 - i) * 8);
#else
#  error "PDP endian not supported for word reads"
#endif
    }
    return val;
}

static inline uint32_t semihost_read_u32_le(volatile uint8_t *ptr) {
    return (uint32_t)ptr[0] |
           ((uint32_t)ptr[1] << 8) |
           ((uint32_t)ptr[2] << 16) |
           ((uint32_t)ptr[3] << 24);
}

/* Initialize RIFF header and CNFG chunk */
static inline void semihost_virtual_init(volatile uint8_t *base) {
    /* Write RIFF header */
    semihost_write_fourcc(base + 0, "RIFF");
    semihost_write_u32_le(base + 4, 256);  /* Size placeholder */
    semihost_write_fourcc(base + 8, "SEMI");

    /* Write CNFG chunk */
    semihost_write_fourcc(base + 12, "CNFG");
    semihost_write_u32_le(base + 16, 4);  /* Chunk size = 4 */
    base[20] = sizeof(uintptr_t);  /* word_size */
    base[21] = sizeof(uintptr_t);  /* ptr_size */
    base[22] = SEMIHOST_VIRTUAL_ENDIAN;
    base[23] = 0;  /* reserved */

    semihost_memory_barrier();
}

/* Check if CALL chunk has been replaced with RETN chunk */
static inline int semihost_is_retn_ready(volatile uint8_t *call_chunk) {
    return call_chunk[0] == 'R' &&
           call_chunk[1] == 'E' &&
           call_chunk[2] == 'T' &&
           call_chunk[3] == 'N';
}

/* Trigger mechanism - write to base address signals request ready */
static inline void semihost_trigger(volatile uint8_t *base) {
    /* Writing to the base address itself serves as trigger */
    volatile uint8_t dummy = base[0];
    (void)dummy;
    base[0] = base[0];  /* Dummy write to trigger */
    semihost_memory_barrier();
}

#endif /* _VIRTUAL_H_ */
