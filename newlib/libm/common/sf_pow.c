/* Single-precision pow function.
   Copyright (c) 2017-2018 Arm Ltd.  All rights reserved.

   SPDX-License-Identifier: BSD-3-Clause

   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions
   are met:
   1. Redistributions of source code must retain the above copyright
      notice, this list of conditions and the following disclaimer.
   2. Redistributions in binary form must reproduce the above copyright
      notice, this list of conditions and the following disclaimer in the
      documentation and/or other materials provided with the distribution.
   3. The name of the company may not be used to endorse or promote
      products derived from this software without specific prior written
      permission.

   THIS SOFTWARE IS PROVIDED BY ARM LTD ``AS IS'' AND ANY EXPRESS OR IMPLIED
   WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
   MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
   IN NO EVENT SHALL ARM LTD BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
   SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
   TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
   PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
   LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
   NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
   SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE. */

#include "fdlibm.h"
#if !__OBSOLETE_MATH_FLOAT

#include <math.h>
#include <stdint.h>
#include "math_config.h"

/*
POWF_LOG2_POLY_ORDER = 5
EXP2F_TABLE_BITS = 5

ULP error: 0.82 (~ 0.5 + relerr*2^24)
relerr: 1.27 * 2^-26 (Relative error ~= 128*Ln2*relerr_log2 + relerr_exp2)
relerr_log2: 1.83 * 2^-33 (Relative error of logx.)
relerr_exp2: 1.69 * 2^-34 (Relative error of exp2(ylogx).)
*/

#define N (1 << POWF_LOG2_TABLE_BITS)
#define T __powf_log2_data.tab
#define A __powf_log2_data.poly
#define OFF 0x3f330000

/* Subnormal input is normalized so ix has negative biased exponent.
   Output is multiplied by N (POWF_SCALE) if TOINT_INTRINICS is set.  */
static inline double_t
log2_inline (uint32_t ix)
{
  /* double_t for better performance on targets with FLT_EVAL_METHOD==2.  */
  double_t z, r, r2, r4, p, q, y, y0, invc, logc;
  uint32_t iz, top, tmp;
  int k, i;

  /* x = 2^k z; where z is in range [OFF,2*OFF] and exact.
     The range is split into N subintervals.
     The ith subinterval contains z and c is near its center.  */
  tmp = ix - OFF;
  i = (tmp >> (23 - POWF_LOG2_TABLE_BITS)) % N;
  top = tmp & 0xff800000;
  iz = ix - top;
  k = (int32_t) top >> (23 - POWF_SCALE_BITS); /* arithmetic shift */
  invc = T[i].invc;
  logc = T[i].logc;
  z = (double_t) asfloat (iz);

  /* log2(x) = log1p(z/c-1)/ln2 + log2(c) + k */
  r = z * invc - 1;
  y0 = logc + (double_t) k;

  /* Pipelined polynomial evaluation to approximate log1p(r)/ln2.  */
  r2 = r * r;
  y = A[0] * r + A[1];
  p = A[2] * r + A[3];
  r4 = r2 * r2;
  q = A[4] * r + y0;
  q = p * r2 + q;
  y = y * r4 + q;
  return y;
}

#undef N
#undef T
#define N (1 << EXP2F_TABLE_BITS)
#define T __exp2f_data.tab
#define SIGN_BIAS ((uint32_t) 1 << (EXP2F_TABLE_BITS + 11))

/* The output of log2 and thus the input of exp2 is either scaled by N
   (in case of fast toint intrinsics) or not.  The unscaled xd must be
   in [-1021,1023], sign_bias sets the sign of the result.  */
static inline double_t
exp2_inline (double_t xd, uint32_t sign_bias)
{
  uint64_t ki, ski, t;
  /* double_t for better performance on targets with FLT_EVAL_METHOD==2.  */
  double_t kd, z, r, r2, y, s;

#if TOINT_INTRINSICS
# define C __exp2f_data.poly_scaled
  /* N*x = k + r with r in [-1/2, 1/2] */
  kd = roundtoint (xd); /* k */
  ki = converttoint (xd);
#else
# define C __exp2f_data.poly
# define SHIFT __exp2f_data.shift_scaled
  /* x = k/N + r with r in [-1/(2N), 1/(2N)] */
  kd = (double) (xd + SHIFT); /* Rounding to double precision is required.  */
  ki = asuint64 (kd);
  kd -= SHIFT; /* k/N */
#endif
  r = xd - kd;

  /* exp2(x) = 2^(k/N) * 2^r ~= s * (C0*r^3 + C1*r^2 + C2*r + 1) */
  t = T[ki % N];
  ski = ki + sign_bias;
  t += ski << (52 - EXP2F_TABLE_BITS);
  s = asfloat64 (t);
  z = C[0] * r + C[1];
  r2 = r * r;
  y = C[2] * r + 1;
  y = z * r2 + y;
  y = y * s;
  return y;
}

/* Returns 0 if not int, 1 if odd int, 2 if even int.  The argument is
   the bit representation of a non-zero finite floating-point value.  */
static inline int
checkint (uint32_t iy)
{
  int e = iy >> 23 & 0xff;
  if (e < 0x7f)
    return 0;
  if (e > 0x7f + 23)
    return 2;
  if (iy & (((uint32_t) 1 << (0x7f + 23 - e)) - 1))
    return 0;
  if (iy & ((uint32_t) 1 << (0x7f + 23 - e)))
    return 1;
  return 2;
}

static inline int
zeroinfnan (uint32_t ix)
{
  return 2 * ix - 1 >= 2u * (uint32_t) 0x7f800000 - 1;
}

float
powf (float x, float y)
{
  uint32_t sign_bias = 0;
  uint32_t ix, iy;

  ix = asuint (x);
  iy = asuint (y);
  if (__builtin_expect (ix - 0x00800000 >= 0x7f800000 - 0x00800000
			  || zeroinfnan (iy),
			0))
    {
      /* Either (x < 0x1p-126 or inf or nan) or (y is 0 or inf or nan).  */
      if (__builtin_expect (zeroinfnan (iy), 0))
	{
	  if (2 * iy == 0)
	    return issignalingf_inline (x) ? x + y : 1.0f;
	  if (ix == 0x3f800000)
	    return issignalingf_inline (y) ? x + y : 1.0f;
	  if (2 * ix > 2u * (uint32_t) 0x7f800000 || 2 * iy > 2u * (uint32_t) 0x7f800000)
	    return x + y;
	  if (2 * ix == 2 * (uint32_t) 0x3f800000)
	    return 1.0f;
	  if ((2 * ix < 2 * (uint32_t) 0x3f800000) == !(iy & (uint32_t) 0x80000000))
	    return 0.0f; /* |x|<1 && y==inf or |x|>1 && y==-inf.  */
	  return y * y;
	}
      if (__builtin_expect (zeroinfnan (ix), 0))
	{
	  float_t x2 = x * x;
	  if (ix & 0x80000000 && checkint (iy) == 1)
	    {
	      x2 = -x2;
	      sign_bias = 1;
	    }
          if (!(iy & 0x80000000))
              return opt_barrier_float(x2);
#if WANT_ERRNO
          if (2 * ix == 0)
              return __math_divzerof (sign_bias);
#endif
          return 1 / x2;
	}
      /* x and y are non-zero finite.  */
      if (ix & 0x80000000)
	{
	  /* Finite x < 0.  */
	  int yint = checkint (iy);
	  if (yint == 0)
	    return __math_invalidf (x);
	  if (yint == 1)
	    sign_bias = SIGN_BIAS;
	  ix &= 0x7fffffff;
	}
      if (ix < 0x00800000)
	{
	  /* Normalize subnormal x so exponent becomes negative.  */
	  ix = asuint (x * 0x1p23f);
	  ix &= 0x7fffffff;
	  ix -= (uint32_t) 23 << 23;
	}
    }
  double_t logx = log2_inline (ix);
  double_t ylogx = (double) y * logx; /* Note: cannot overflow, y is single prec.  */
  if (__builtin_expect ((asuint64 (ylogx) >> 47 & 0xffff)
			  >= asuint64 (126.0 * POWF_SCALE) >> 47,
			0))
    {
      /* |y*log(x)| >= 126.  */
      if (ylogx > 0x1.fffffffd1d571p+6 * POWF_SCALE)
	/* |x^y| > 0x1.ffffffp127.  */
	return __math_oflowf (sign_bias);
#if WANT_ROUNDING && WANT_ERRNO
      if (ylogx > 0x1.fffffffa3aae2p+6 * POWF_SCALE)
	/* |x^y| > 0x1.fffffep127, check if we round away from 0.  */
	if ((!sign_bias
	     && eval_as_float (1.0f + opt_barrier_float (0x1p-25f)) != 1.0f)
	    || (sign_bias
		&& eval_as_float (-1.0f - opt_barrier_float (0x1p-25f))
		     != -1.0f))
	  return __math_oflowf (sign_bias);
#endif
      if (ylogx <= -150.0 * POWF_SCALE)
	return __math_uflowf (sign_bias);
#if WANT_ERRNO_UFLOW
      if (ylogx < -149.0 * POWF_SCALE)
	return __math_may_uflowf (sign_bias);
#endif
    }
  return (float) exp2_inline (ylogx, sign_bias);
}

#ifdef __strong_reference
#if defined(__GNUCLIKE_PRAGMA_DIAGNOSTIC) && !defined(__clang__)
#pragma GCC diagnostic ignored "-Wmissing-attributes"
#endif
__strong_reference(powf, _powf);
#endif

#endif /* !__OBSOLETE_MATH_FLOAT */
