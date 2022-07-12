/* aeabi.h - declares the ARM run-time helper-function ABI for programs written in C.
 *
 * This is free and unencumbered software released into the public domain.
 *
 * Anyone is free to copy, modify, publish, use, compile, sell, or
 * distribute this software, either in source code form or as a compiled
 * binary, for any purpose, commercial or non-commercial, and by any
 * means.

 * In jurisdictions that recognize copyright laws, the author or authors
 * of this software dedicate any and all copyright interest in the
 * software to the public domain. We make this dedication for the benefit
 * of the public at large and to the detriment of our heirs and
 * suvcessors. We intend this dedication to be an overt act of
 * relinquishment in perpetuity of all present and future rights to this
 * software under copyright law.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
 * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 */

/* The functions declared here is based on the document, number,
 * "ARM IHI 0043D, current through ABI release 2.09", which
 * defines the run-time helper-function ABI for programs written
 * in ARM-Thumb assembly language, C, and C++.
 *
 * ARM is a registered trademark of ARM Limited.
 */

#ifndef __AEABI_H
#define __AEABI_H

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Standard double precision floating-point
 * arithmetic helper functions
 */
extern double __aeabi_dadd(double, double);
extern double __aeabi_ddiv(double n, double d);
extern double __aeabi_dmul(double, double);
extern double __aeabi_drsub(double x, double y);
extern double __aeabi_dsub(double x, double y);

/* double precision floating-point comparison
 * helper functions
 */
extern void __aeabi_cdcmpeq(double, double);
extern void __aeabi_cdcmple(double, double);
extern void __aeabi_cdrcmple(double, double);
extern int __aeabi_dcmpeq(double, double);
extern int __aeabi_dcmplt(double, double);
extern int __aeabi_dcmple(double, double);
extern int __aeabi_dcmpge(double, double);
extern int __aeabi_dcmpgt(double, double);
extern int __aeabi_dcmpun(double, double);

/* Standard single precision floating-point
 * arithmetic helper functions
 */
extern float __aeabi_fadd(float, float);
extern float __aeabi_fdiv(float n, float d);
extern float __aeabi_fmul(float, float);
extern float __aeabi_frsub(float x, float y);
extern float __aeabi_fsub(float x, float y);

/* Standard single precision floating-point
 * comparison helper functions
 */
extern void __aeabi_cfcmpeq(float, float);
extern void __aeabi_cfcmple(float, float);
extern void __aeabi_cfrcmple(float, float);
extern int __aeabi_fcmpeq(float, float);
extern int __aeabi_fcmplt(float, float);
extern int __aeabi_fcmple(float, float);
extern int __aeabi_fcmpge(float, float);
extern int __aeabi_fcmpgt(float, float);
extern int __aeabi_fcmpun(float, float);

/* Standard floating-point to integer
 * conversions
 */
extern int __aeabi_d2iz(double);
extern unsigned __aeabi_d2uiz(double);
extern long long __aeabi_d2lz(double);
extern unsigned long long __aeabi_d2ulz(double);
extern int __aeabi_f2iz(float);
extern unsigned __aeabi_f2uiz(float);
extern long long __aeabi_f2lz(float);
extern unsigned long long __aeabi_f2ulz(float);

/* Standard conversions between floating
 * types
 */
extern float __aeabi_d2f(double);
extern double __aeabi_f2d(float);
extern float __aeabi_h2f(short hf);
extern float __aeabi_h2f_alt(short hf);
extern short __aeabi_f2h(float f);
extern short __aeabi_f2h_alt(float f);
extern short __aeabi_d2h(double);
extern short __aeabi_d2h_alt(double);

/* Standard integer to floating-point
 * conversions
 */
extern double __aeabi_i2d(int);
extern double __aeabi_ui2d(unsigned);
extern double __aeabi_l2d(long long);
extern double __aeabi_ul2d(unsigned long long);
extern float __aeabi_i2f(int);
extern float __aeabi_ui2f(unsigned);
extern float __aeabi_l2f(long long);
extern float __aeabi_ul2f(unsigned long long);

/* Long long functions
 */
extern long long __aeabi_lmul(long long, long long);
extern lldiv_t __aeabi_ldivmod(long long n, long long d);

/* Integer (32/32 -> 32) division functions
 */
extern int __aeabi_idiv(int numerator, int denominator);
extern unsigned __aeabi_uidiv(unsigned numerator, unsigned denominator);
typedef struct {
    int quot;
    int rem;
} idiv_return;
typedef struct {
    unsigned quot;
    unsigned rem;
} uidiv_return;
extern idiv_return __aeabi_idivmod(int numerator, int denominator);
extern uidiv_return __aeabi_uidivmod(unsigned numerator, unsigned denominator);

/* Division by zero
 */
extern int __aeabi_idiv0(int return_value);
extern long long __aeabi_ldiv0(long long return_value);

/* Unaligned memory access
 */
extern int __aeabi_uread4(void* address);
extern int __aeabi_uwrite4(int value, void* address);
extern long long __aeabi_uread8(void* address);
extern long long __aeabi_uwrite8(long long value, void* address);

/* Memory copying
 */
extern void __aeabi_memcpy8(void* dest, const void* src, size_t n);
extern void __aeabi_memcpy4(void* dest, const void* src, size_t n);
extern void __aeabi_memcpy(void* dest, const void* src, size_t n);
extern void __aeabi_memmove8(void* dest, const void* src, size_t n);
extern void __aeabi_memmove4(void* dest, const void* src, size_t n);
extern void __aeabi_memmove(void* dest, const void* src, size_t n);

/* Memory clearing and setting
 */
extern void __aeabi_memset8(void* dest, size_t n, int c);
extern void __aeabi_memset4(void* dest, size_t n, int c);
extern void __aeabi_memset(void* dest, size_t n, int c);
extern void __aeabi_memclr8(void* dest, size_t n);
extern void __aeabi_memclr4(void* dest, size_t n);
extern void __aeabi_memclr(void* dest, size_t n);

#ifdef __cplusplus
}
#endif

#endif /* __AEABI_H */