/*
 * SPDX-License-Identifier: BSD-3-Clause
 *
 * Copyright © 2025 John Byrd
 *
 * Compiler runtime support for MOS 6502 - multiplication functions.
 * Derived from llvm-mos-sdk (Apache-2.0 WITH LLVM-exception).
 */

#include <picolibc.h>

/* Forward declarations for compiler builtins */
char               __mulqi3(char a, char b);
unsigned int       __mulhi3(unsigned int a, unsigned int b);
unsigned long      __mulsi3(unsigned long a, unsigned long b);
unsigned long long __muldi3(unsigned long long a, unsigned long long b);

/* 8-bit multiply */
char
__mulqi3(char a, char b)
{
    char result = 0;
    while (b) {
        if (b & 1)
            result += a;
        a <<= 1;
        b >>= 1;
    }
    return result;
}

/* 16-bit multiply */
unsigned int
__mulhi3(unsigned int a, unsigned int b)
{
    unsigned int result = 0;
    while (b) {
        if (b & 1)
            result += a;
        a <<= 1;
        b >>= 1;
    }
    return result;
}

/* 32-bit multiply */
unsigned long
__mulsi3(unsigned long a, unsigned long b)
{
    unsigned long result = 0;
    while (b) {
        if (b & 1)
            result += a;
        a <<= 1;
        b >>= 1;
    }
    return result;
}

/* 64-bit multiply */
unsigned long long
__muldi3(unsigned long long a, unsigned long long b)
{
    unsigned long long result = 0;
    while (b) {
        if (b & 1)
            result += a;
        a <<= 1;
        b >>= 1;
    }
    return result;
}
