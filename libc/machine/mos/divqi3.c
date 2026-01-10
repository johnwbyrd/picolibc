/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Compiler runtime support for MOS 6502 - 8-bit division functions.
 * Derived from llvm-mos-sdk (Apache-2.0 WITH LLVM-exception).
 */

#include <picolibc.h>

/* Function prototypes */
unsigned char __udivqi3(unsigned char a, unsigned char b);
unsigned char __umodqi3(unsigned char a, unsigned char b);
signed char   __divqi3(signed char a, signed char b);
signed char   __modqi3(signed char a, signed char b);
unsigned char __udivmodqi4(unsigned char a, unsigned char b, unsigned char *rem);
signed char   __divmodqi4(signed char a, signed char b, signed char *rem);

/* Helper: unsigned 8-bit division */
static unsigned char
__udivqi(unsigned char a, unsigned char b)
{
    if (!b || b > a)
        return 0;

    unsigned char q = 0;
    unsigned char bit = 1U << 7;

    while (bit && !(b & (1U << 7))) {
        if ((unsigned char)(b << 1) > a)
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

/* Helper: unsigned 8-bit modulo */
static unsigned char
__umodqi(unsigned char a, unsigned char b)
{
    if (!b || b > a)
        return a;

    unsigned char bit = 1;

    while (!(b & (1U << 7)) && (unsigned char)(b << 1) <= a) {
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

/* Unsigned 8-bit divide */
unsigned char
__udivqi3(unsigned char a, unsigned char b)
{
    return __udivqi(a, b);
}

/* Unsigned 8-bit modulo */
unsigned char
__umodqi3(unsigned char a, unsigned char b)
{
    return __umodqi(a, b);
}

/* Signed 8-bit divide */
signed char
__divqi3(signed char a, signed char b)
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
    signed char q = (signed char)__udivqi((unsigned char)a, (unsigned char)b);
    return neg ? -q : q;
}

/* Signed 8-bit modulo */
signed char
__modqi3(signed char a, signed char b)
{
    int neg = 0;
    if (a < 0) {
        a = -a;
        neg = 1;
    }
    if (b < 0)
        b = -b;
    signed char r = (signed char)__umodqi((unsigned char)a, (unsigned char)b);
    return neg ? -r : r;
}

/* Combined divmod */
unsigned char
__udivmodqi4(unsigned char a, unsigned char b, unsigned char *rem)
{
    unsigned char q = __udivqi(a, b);
    *rem = a - q * b;
    return q;
}

signed char
__divmodqi4(signed char a, signed char b, signed char *rem)
{
    signed char q = __divqi3(a, b);
    *rem = a - q * b;
    return q;
}
