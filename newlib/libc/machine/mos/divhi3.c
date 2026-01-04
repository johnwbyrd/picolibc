/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Compiler runtime support for MOS 6502 - 16-bit division functions.
 * Derived from llvm-mos-sdk (Apache-2.0 WITH LLVM-exception).
 */

#include <picolibc.h>

/* Function prototypes */
unsigned int __udivhi3(unsigned int a, unsigned int b);
unsigned int __umodhi3(unsigned int a, unsigned int b);
int __divhi3(int a, int b);
int __modhi3(int a, int b);
unsigned int __udivmodhi4(unsigned int a, unsigned int b, unsigned int *rem);
int __divmodhi4(int a, int b, int *rem);

/* Helper: unsigned 16-bit division */
static unsigned int
__udivhi(unsigned int a, unsigned int b)
{
    if (!b || b > a)
        return 0;

    unsigned int q = 0;
    unsigned int bit = 1U << 15;

    while (bit && !(b & (1U << 15))) {
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

/* Helper: unsigned 16-bit modulo */
static unsigned int
__umodhi(unsigned int a, unsigned int b)
{
    if (!b || b > a)
        return a;

    unsigned int bit = 1;

    while (!(b & (1U << 15)) && (b << 1) <= a) {
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

/* Unsigned 16-bit divide */
unsigned int
__udivhi3(unsigned int a, unsigned int b)
{
    return __udivhi(a, b);
}

/* Unsigned 16-bit modulo */
unsigned int
__umodhi3(unsigned int a, unsigned int b)
{
    return __umodhi(a, b);
}

/* Signed 16-bit divide */
int
__divhi3(int a, int b)
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
    int q = (int)__udivhi((unsigned int)a, (unsigned int)b);
    return neg ? -q : q;
}

/* Signed 16-bit modulo */
int
__modhi3(int a, int b)
{
    int neg = 0;
    if (a < 0) {
        a = -a;
        neg = 1;
    }
    if (b < 0)
        b = -b;
    int r = (int)__umodhi((unsigned int)a, (unsigned int)b);
    return neg ? -r : r;
}

/* Combined divmod */
unsigned int
__udivmodhi4(unsigned int a, unsigned int b, unsigned int *rem)
{
    unsigned int q = __udivhi(a, b);
    *rem = a - q * b;
    return q;
}

int
__divmodhi4(int a, int b, int *rem)
{
    int q = __divhi3(a, b);
    *rem = a - q * b;
    return q;
}
