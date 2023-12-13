/*
 * This file is part of the OpenMV project.
 * Copyright (c) 2013/2014 Ibrahim Abdelkader <i.abdalkader@gmail.com>
 * This work is licensed under the MIT license, see the file LICENSE for details.
 *
 * Fast approximate math functions.
 *
 */
#ifndef __FMATH_H
#define __FMATH_H
#include <stdint.h>
#include <math.h>

static inline float fast_sqrtf(float x)
{
  //return sqrtf(x);
  asm("fsqrt.s %0, %1"
      : "=f"(x)
      : "f"(x));
  return x;
}
static inline int fast_floorf(float x)
{
  return (int)(x);
}

static inline int fast_ceilf(float x)
{
  return (int)(x + 0.9999f);
}

static inline int fast_roundf(float x)
{
  return (int)(x);
}

static inline float fast_fabsf(float d)
{
  return fabsf(d);
}

extern int fast_floorf(float x);
extern int fast_ceilf(float x);
extern int fast_roundf(float x);
extern float fast_atanf(float x);
extern float fast_atan2f(float y, float x);
extern float fast_expf(float x);
extern float fast_cbrtf(float d);
extern float fast_fabsf(float d);
extern float fast_log(float x);
extern float fast_log2(float x);
extern float fast_powf(float a, float b);

/*#define fast_sqrtf(x) (sqrtf(x))
#define fast_floorf(x) ((int)floorf(x))
#define fast_ceilf(x) ((int)ceilf(x))
#define fast_roundf(x) ((int)roundf(x))
#define fast_atanf(x) (atanf(x))
#define fast_atan2f(x,y) (atan2f((x),(y)))
#define fast_expf(x) (expf(x))
#define fast_cbrtf(x) (cbrtf(x))
#define fast_fabsf(x) (fabsf(x))
#define fast_log(x) (log(x))
#define fast_log2(x) (log2(x))
#define fast_powf(x,y) (powf((x),(y)))
*/

extern const float cos_table[360];
extern const float sin_table[360];
#endif // __FMATH_H
