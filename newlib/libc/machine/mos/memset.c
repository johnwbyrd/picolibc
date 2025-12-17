/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * memset for MOS 6502.
 * Simple byte-by-byte implementation since 6502 has no benefit from
 * word-aligned access.
 */

#include <picolibc.h>
#include <string.h>

/*
 * Standard memset.
 */
void *
memset(void *m, int c, size_t n)
{
    char *s = (char *)m;
    while (n--)
        *s++ = (char)c;
    return m;
}

/*
 * llvm-mos uses __memset with char value instead of int.
 * This is more efficient for 8-bit CPUs.
 */
void
__memset(char *ptr, char value, size_t num)
{
    while (num--)
        *ptr++ = value;
}
