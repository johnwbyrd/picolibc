/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Compiler runtime support for MOS 6502 - 64-bit division functions.
 * Derived from llvm-mos-sdk (Apache-2.0 WITH LLVM-exception).
 */

#include <picolibc.h>

/* Function prototypes */
unsigned long long __udivdi3(unsigned long long a, unsigned long long b);
unsigned long long __umoddi3(unsigned long long a, unsigned long long b);
unsigned long long __udivmoddi4(unsigned long long a, unsigned long long b, unsigned long long *rem);
long long __divdi3(long long a, long long b);
long long __moddi3(long long a, long long b);
long long __divmoddi4(long long a, long long b, long long *rem);

/* Helper: unsigned 64-bit division */
static unsigned long long
__udivdi(unsigned long long a, unsigned long long b)
{
    if (!b || b > a)
        return 0;

    unsigned long long q = 0;
    unsigned long long bit = 1ULL << 63;

    while (bit && !(b & (1ULL << 63))) {
        if ((b << 1) > a)
            break;
        b <<= 1;
        bit <<= 1;
    }

    while (bit) {
        if (a >= b) {
            a -= b;
            q |= bit;
        }
        b >>= 1;
        bit >>= 1;
    }

    return q;
}

/* Helper: unsigned 64-bit modulo */
static unsigned long long
__umoddi(unsigned long long a, unsigned long long b)
{
    if (!b || b > a)
        return a;

    unsigned long long bit = 1;

    while (!(b & (1ULL << 63)) && (b << 1) <= a) {
        b <<= 1;
        bit <<= 1;
    }

    while (bit) {
        if (a >= b)
            a -= b;
        b >>= 1;
        bit >>= 1;
    }

    return a;
}

/* Unsigned 64-bit divide */
unsigned long long
__udivdi3(unsigned long long a, unsigned long long b)
{
    return __udivdi(a, b);
}

/* Unsigned 64-bit modulo */
unsigned long long
__umoddi3(unsigned long long a, unsigned long long b)
{
    return __umoddi(a, b);
}

/* Combined unsigned divmod */
unsigned long long
__udivmoddi4(unsigned long long a, unsigned long long b, unsigned long long *rem)
{
    unsigned long long q = __udivdi(a, b);
    *rem = a - q * b;
    return q;
}

/* Signed 64-bit divide */
long long
__divdi3(long long a, long long b)
{
    int neg = 0;
    if (a < 0) {
        a = -a;
        neg = 1;
    }
    if (b < 0) {
        b = -b;
        neg ^= 1;
    }
    long long q = (long long)__udivdi((unsigned long long)a, (unsigned long long)b);
    return neg ? -q : q;
}

/* Signed 64-bit modulo */
long long
__moddi3(long long a, long long b)
{
    int neg = 0;
    if (a < 0) {
        a = -a;
        neg = 1;
    }
    if (b < 0)
        b = -b;
    long long r = (long long)__umoddi((unsigned long long)a, (unsigned long long)b);
    return neg ? -r : r;
}

/* Combined signed divmod */
long long
__divmoddi4(long long a, long long b, long long *rem)
{
    long long q = __divdi3(a, b);
    *rem = a - q * b;
    return q;
}
