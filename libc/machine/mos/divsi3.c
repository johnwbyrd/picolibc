/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Compiler runtime support for MOS 6502 - division functions.
 * Derived from llvm-mos-sdk (Apache-2.0 WITH LLVM-exception).
 */

#include <picolibc.h>

/* Function prototypes */
unsigned long __udivsi3(unsigned long a, unsigned long b);
unsigned long __umodsi3(unsigned long a, unsigned long b);
long          __divsi3(long a, long b);
long          __modsi3(long a, long b);
unsigned long __udivmodsi4(unsigned long a, unsigned long b, unsigned long *rem);
long          __divmodsi4(long a, long b, long *rem);

/* Helper: unsigned division */
static unsigned long
__udivsi(unsigned long a, unsigned long b)
{
    if (!b || b > a)
        return 0;

    unsigned long q = 0;
    unsigned long bit = 1UL << 31;

    /* Find highest bit position where b << n <= a */
    while (bit && !(b & (1UL << 31))) {
        if ((b << 1) > a)
            break;
        b <<= 1;
        bit <<= 1;
    }

    /* Long division */
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

/* Helper: unsigned modulo */
static unsigned long
__umodsi(unsigned long a, unsigned long b)
{
    if (!b || b > a)
        return a;

    unsigned long bit = 1;

    /* Shift b left as far as possible */
    while (!(b & (1UL << 31)) && (b << 1) <= a) {
        b <<= 1;
        bit <<= 1;
    }

    /* Subtract */
    while (bit) {
        if (a >= b)
            a -= b;
        b >>= 1;
        bit >>= 1;
    }

    return a;
}

/* Unsigned 32-bit divide */
unsigned long
__udivsi3(unsigned long a, unsigned long b)
{
    return __udivsi(a, b);
}

/* Unsigned 32-bit modulo */
unsigned long
__umodsi3(unsigned long a, unsigned long b)
{
    return __umodsi(a, b);
}

/* Signed 32-bit divide */
long
__divsi3(long a, long b)
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
    long q = (long)__udivsi((unsigned long)a, (unsigned long)b);
    return neg ? -q : q;
}

/* Signed 32-bit modulo */
long
__modsi3(long a, long b)
{
    int neg = 0;
    if (a < 0) {
        a = -a;
        neg = 1;
    }
    if (b < 0)
        b = -b;
    long r = (long)__umodsi((unsigned long)a, (unsigned long)b);
    return neg ? -r : r;
}

/* Combined unsigned divmod */
unsigned long
__udivmodsi4(unsigned long a, unsigned long b, unsigned long *rem)
{
    unsigned long q = __udivsi(a, b);
    *rem = a - q * b;
    return q;
}

/* Combined signed divmod */
long
__divmodsi4(long a, long b, long *rem)
{
    long q = __divsi3(a, b);
    *rem = a - q * b;
    return q;
}
