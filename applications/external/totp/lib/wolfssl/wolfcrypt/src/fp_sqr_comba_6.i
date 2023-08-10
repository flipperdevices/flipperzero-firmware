/* fp_sqr_comba_6.i
 *
 * Copyright (C) 2006-2023 wolfSSL Inc.
 *
 * This file is part of wolfSSL.
 *
 * wolfSSL is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * wolfSSL is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1335, USA
 */



#ifdef TFM_SQR6
int fp_sqr_comba6(fp_int *A, fp_int *B)
{
   fp_digit *a, c0, c1, c2, sc0 = 0, sc1 = 0, sc2 = 0;
#ifdef TFM_ISO
   fp_word tt;
#endif
#ifndef WOLFSSL_SMALL_STACK
   fp_digit b[12];
#else
   fp_digit *b;
#endif

#ifdef WOLFSSL_SMALL_STACK
   b = (fp_digit*)XMALLOC(sizeof(fp_digit) * 12, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (b == NULL)
      return FP_MEM;
#endif

   a = A->dp;
   COMBA_START; 

   /* clear carries */
   CLEAR_CARRY;

   /* output 0 */
   SQRADD(a[0],a[0]);
   COMBA_STORE(b[0]);

   /* output 1 */
   CARRY_FORWARD;
   SQRADD2(a[0], a[1]); 
   COMBA_STORE(b[1]);

   /* output 2 */
   CARRY_FORWARD;
   SQRADD2(a[0], a[2]); SQRADD(a[1], a[1]); 
   COMBA_STORE(b[2]);

   /* output 3 */
   CARRY_FORWARD;
   SQRADD2(a[0], a[3]); SQRADD2(a[1], a[2]); 
   COMBA_STORE(b[3]);

   /* output 4 */
   CARRY_FORWARD;
   SQRADD2(a[0], a[4]); SQRADD2(a[1], a[3]); SQRADD(a[2], a[2]); 
   COMBA_STORE(b[4]);

   /* output 5 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[5]); SQRADDAC(a[1], a[4]); SQRADDAC(a[2], a[3]); SQRADDDB; 
   COMBA_STORE(b[5]);

   /* output 6 */
   CARRY_FORWARD;
   SQRADD2(a[1], a[5]); SQRADD2(a[2], a[4]); SQRADD(a[3], a[3]); 
   COMBA_STORE(b[6]);

   /* output 7 */
   CARRY_FORWARD;
   SQRADD2(a[2], a[5]); SQRADD2(a[3], a[4]); 
   COMBA_STORE(b[7]);

   /* output 8 */
   CARRY_FORWARD;
   SQRADD2(a[3], a[5]); SQRADD(a[4], a[4]); 
   COMBA_STORE(b[8]);

   /* output 9 */
   CARRY_FORWARD;
   SQRADD2(a[4], a[5]); 
   COMBA_STORE(b[9]);

   /* output 10 */
   CARRY_FORWARD;
   SQRADD(a[5], a[5]); 
   COMBA_STORE(b[10]);
   COMBA_STORE2(b[11]);
   COMBA_FINI;

   B->used = 12;
   B->sign = FP_ZPOS;
   XMEMCPY(B->dp, b, 12 * sizeof(fp_digit));
   fp_clamp(B);

#ifdef WOLFSSL_SMALL_STACK
   XFREE(b, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif


