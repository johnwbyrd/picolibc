/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Compiler runtime support for MOS 6502 - shift functions.
 * Derived from llvm-mos-sdk (Apache-2.0 WITH LLVM-exception).
 */

#include <picolibc.h>

/* Left shifts */
char
__ashlqi3(char n, char amt)
{
    while (amt--)
        n <<= 1;
    return n;
}

unsigned int
__ashlhi3(unsigned int n, char amt)
{
    while (amt--)
        n <<= 1;
    return n;
}

unsigned long
__ashlsi3(unsigned long n, char amt)
{
    while (amt--)
        n <<= 1;
    return n;
}

unsigned long long
__ashldi3(unsigned long long n, char amt)
{
    while (amt--)
        n <<= 1;
    return n;
}

/* Logical right shifts (unsigned) */
unsigned char
__lshrqi3(unsigned char n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

unsigned int
__lshrhi3(unsigned int n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

unsigned long
__lshrsi3(unsigned long n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

unsigned long long
__lshrdi3(unsigned long long n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

/* Arithmetic right shifts (signed) */
signed char
__ashrqi3(signed char n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

int
__ashrhi3(int n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

long
__ashrsi3(long n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}

long long
__ashrdi3(long long n, char amt)
{
    while (amt--)
        n >>= 1;
    return n;
}
