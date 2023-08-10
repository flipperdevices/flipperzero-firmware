/* fp_sqr_comba_17.i
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



#ifdef TFM_SQR17
int fp_sqr_comba17(fp_int *A, fp_int *B)
{
   fp_digit *a, c0, c1, c2, sc0 = 0, sc1 = 0, sc2 = 0;
#ifdef TFM_ISO
   fp_word tt;
#endif
#ifndef WOLFSSL_SMALL_STACK
   fp_digit b[34];
#else
   fp_digit *b;
#endif

#ifdef WOLFSSL_SMALL_STACK
   b = (fp_digit*)XMALLOC(sizeof(fp_digit) * 34, NULL, DYNAMIC_TYPE_TMP_BUFFER);
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
   SQRADDSC(a[0], a[6]); SQRADDAC(a[1], a[5]); SQRADDAC(a[2], a[4]); SQRADDDB; SQRADD(a[3], a[3]); 
   COMBA_STORE(b[6]);

   /* output 7 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[7]); SQRADDAC(a[1], a[6]); SQRADDAC(a[2], a[5]); SQRADDAC(a[3], a[4]); SQRADDDB; 
   COMBA_STORE(b[7]);

   /* output 8 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[8]); SQRADDAC(a[1], a[7]); SQRADDAC(a[2], a[6]); SQRADDAC(a[3], a[5]); SQRADDDB; SQRADD(a[4], a[4]); 
   COMBA_STORE(b[8]);

   /* output 9 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[9]); SQRADDAC(a[1], a[8]); SQRADDAC(a[2], a[7]); SQRADDAC(a[3], a[6]); SQRADDAC(a[4], a[5]); SQRADDDB; 
   COMBA_STORE(b[9]);

   /* output 10 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[10]); SQRADDAC(a[1], a[9]); SQRADDAC(a[2], a[8]); SQRADDAC(a[3], a[7]); SQRADDAC(a[4], a[6]); SQRADDDB; SQRADD(a[5], a[5]); 
   COMBA_STORE(b[10]);

   /* output 11 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[11]); SQRADDAC(a[1], a[10]); SQRADDAC(a[2], a[9]); SQRADDAC(a[3], a[8]); SQRADDAC(a[4], a[7]); SQRADDAC(a[5], a[6]); SQRADDDB; 
   COMBA_STORE(b[11]);

   /* output 12 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[12]); SQRADDAC(a[1], a[11]); SQRADDAC(a[2], a[10]); SQRADDAC(a[3], a[9]); SQRADDAC(a[4], a[8]); SQRADDAC(a[5], a[7]); SQRADDDB; SQRADD(a[6], a[6]); 
   COMBA_STORE(b[12]);

   /* output 13 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[13]); SQRADDAC(a[1], a[12]); SQRADDAC(a[2], a[11]); SQRADDAC(a[3], a[10]); SQRADDAC(a[4], a[9]); SQRADDAC(a[5], a[8]); SQRADDAC(a[6], a[7]); SQRADDDB; 
   COMBA_STORE(b[13]);

   /* output 14 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[14]); SQRADDAC(a[1], a[13]); SQRADDAC(a[2], a[12]); SQRADDAC(a[3], a[11]); SQRADDAC(a[4], a[10]); SQRADDAC(a[5], a[9]); SQRADDAC(a[6], a[8]); SQRADDDB; SQRADD(a[7], a[7]); 
   COMBA_STORE(b[14]);

   /* output 15 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[15]); SQRADDAC(a[1], a[14]); SQRADDAC(a[2], a[13]); SQRADDAC(a[3], a[12]); SQRADDAC(a[4], a[11]); SQRADDAC(a[5], a[10]); SQRADDAC(a[6], a[9]); SQRADDAC(a[7], a[8]); SQRADDDB; 
   COMBA_STORE(b[15]);

   /* output 16 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[16]); SQRADDAC(a[1], a[15]); SQRADDAC(a[2], a[14]); SQRADDAC(a[3], a[13]); SQRADDAC(a[4], a[12]); SQRADDAC(a[5], a[11]); SQRADDAC(a[6], a[10]); SQRADDAC(a[7], a[9]); SQRADDDB; SQRADD(a[8], a[8]); 
   COMBA_STORE(b[16]);

   /* output 17 */
   CARRY_FORWARD;
   SQRADDSC(a[1], a[16]); SQRADDAC(a[2], a[15]); SQRADDAC(a[3], a[14]); SQRADDAC(a[4], a[13]); SQRADDAC(a[5], a[12]); SQRADDAC(a[6], a[11]); SQRADDAC(a[7], a[10]); SQRADDAC(a[8], a[9]); SQRADDDB; 
   COMBA_STORE(b[17]);

   /* output 18 */
   CARRY_FORWARD;
   SQRADDSC(a[2], a[16]); SQRADDAC(a[3], a[15]); SQRADDAC(a[4], a[14]); SQRADDAC(a[5], a[13]); SQRADDAC(a[6], a[12]); SQRADDAC(a[7], a[11]); SQRADDAC(a[8], a[10]); SQRADDDB; SQRADD(a[9], a[9]); 
   COMBA_STORE(b[18]);

   /* output 19 */
   CARRY_FORWARD;
   SQRADDSC(a[3], a[16]); SQRADDAC(a[4], a[15]); SQRADDAC(a[5], a[14]); SQRADDAC(a[6], a[13]); SQRADDAC(a[7], a[12]); SQRADDAC(a[8], a[11]); SQRADDAC(a[9], a[10]); SQRADDDB; 
   COMBA_STORE(b[19]);

   /* output 20 */
   CARRY_FORWARD;
   SQRADDSC(a[4], a[16]); SQRADDAC(a[5], a[15]); SQRADDAC(a[6], a[14]); SQRADDAC(a[7], a[13]); SQRADDAC(a[8], a[12]); SQRADDAC(a[9], a[11]); SQRADDDB; SQRADD(a[10], a[10]); 
   COMBA_STORE(b[20]);

   /* output 21 */
   CARRY_FORWARD;
   SQRADDSC(a[5], a[16]); SQRADDAC(a[6], a[15]); SQRADDAC(a[7], a[14]); SQRADDAC(a[8], a[13]); SQRADDAC(a[9], a[12]); SQRADDAC(a[10], a[11]); SQRADDDB; 
   COMBA_STORE(b[21]);

   /* output 22 */
   CARRY_FORWARD;
   SQRADDSC(a[6], a[16]); SQRADDAC(a[7], a[15]); SQRADDAC(a[8], a[14]); SQRADDAC(a[9], a[13]); SQRADDAC(a[10], a[12]); SQRADDDB; SQRADD(a[11], a[11]); 
   COMBA_STORE(b[22]);

   /* output 23 */
   CARRY_FORWARD;
   SQRADDSC(a[7], a[16]); SQRADDAC(a[8], a[15]); SQRADDAC(a[9], a[14]); SQRADDAC(a[10], a[13]); SQRADDAC(a[11], a[12]); SQRADDDB; 
   COMBA_STORE(b[23]);

   /* output 24 */
   CARRY_FORWARD;
   SQRADDSC(a[8], a[16]); SQRADDAC(a[9], a[15]); SQRADDAC(a[10], a[14]); SQRADDAC(a[11], a[13]); SQRADDDB; SQRADD(a[12], a[12]); 
   COMBA_STORE(b[24]);

   /* output 25 */
   CARRY_FORWARD;
   SQRADDSC(a[9], a[16]); SQRADDAC(a[10], a[15]); SQRADDAC(a[11], a[14]); SQRADDAC(a[12], a[13]); SQRADDDB; 
   COMBA_STORE(b[25]);

   /* output 26 */
   CARRY_FORWARD;
   SQRADDSC(a[10], a[16]); SQRADDAC(a[11], a[15]); SQRADDAC(a[12], a[14]); SQRADDDB; SQRADD(a[13], a[13]); 
   COMBA_STORE(b[26]);

   /* output 27 */
   CARRY_FORWARD;
   SQRADDSC(a[11], a[16]); SQRADDAC(a[12], a[15]); SQRADDAC(a[13], a[14]); SQRADDDB; 
   COMBA_STORE(b[27]);

   /* output 28 */
   CARRY_FORWARD;
   SQRADD2(a[12], a[16]); SQRADD2(a[13], a[15]); SQRADD(a[14], a[14]); 
   COMBA_STORE(b[28]);

   /* output 29 */
   CARRY_FORWARD;
   SQRADD2(a[13], a[16]); SQRADD2(a[14], a[15]); 
   COMBA_STORE(b[29]);

   /* output 30 */
   CARRY_FORWARD;
   SQRADD2(a[14], a[16]); SQRADD(a[15], a[15]); 
   COMBA_STORE(b[30]);

   /* output 31 */
   CARRY_FORWARD;
   SQRADD2(a[15], a[16]); 
   COMBA_STORE(b[31]);

   /* output 32 */
   CARRY_FORWARD;
   SQRADD(a[16], a[16]); 
   COMBA_STORE(b[32]);
   COMBA_STORE2(b[33]);
   COMBA_FINI;

   B->used = 34;
   B->sign = FP_ZPOS;
   XMEMCPY(B->dp, b, 34 * sizeof(fp_digit));
   fp_clamp(B);

#ifdef WOLFSSL_SMALL_STACK
   XFREE(b, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif


