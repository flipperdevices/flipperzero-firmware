/* fp_mul_comba_12.i
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



#ifdef TFM_MUL12
int fp_mul_comba12(fp_int *A, fp_int *B, fp_int *C)
{
   fp_digit c0, c1, c2;
#ifndef WOLFSSL_SMALL_STACK
   fp_digit at[24];
#else
   fp_digit *at;
#endif

#ifdef WOLFSSL_SMALL_STACK
   at = (fp_digit*)XMALLOC(sizeof(fp_digit) * 24, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (at == NULL)
       return FP_MEM;
#endif

   XMEMCPY(at, A->dp, 12 * sizeof(fp_digit));
   XMEMCPY(at+12, B->dp, 12 * sizeof(fp_digit));
   COMBA_START;

   COMBA_CLEAR;
   /* 0 */
   MULADD(at[0], at[12]); 
   COMBA_STORE(C->dp[0]);
   /* 1 */
   COMBA_FORWARD;
   MULADD(at[0], at[13]);    MULADD(at[1], at[12]); 
   COMBA_STORE(C->dp[1]);
   /* 2 */
   COMBA_FORWARD;
   MULADD(at[0], at[14]);    MULADD(at[1], at[13]);    MULADD(at[2], at[12]); 
   COMBA_STORE(C->dp[2]);
   /* 3 */
   COMBA_FORWARD;
   MULADD(at[0], at[15]);    MULADD(at[1], at[14]);    MULADD(at[2], at[13]);    MULADD(at[3], at[12]); 
   COMBA_STORE(C->dp[3]);
   /* 4 */
   COMBA_FORWARD;
   MULADD(at[0], at[16]);    MULADD(at[1], at[15]);    MULADD(at[2], at[14]);    MULADD(at[3], at[13]);    MULADD(at[4], at[12]); 
   COMBA_STORE(C->dp[4]);
   /* 5 */
   COMBA_FORWARD;
   MULADD(at[0], at[17]);    MULADD(at[1], at[16]);    MULADD(at[2], at[15]);    MULADD(at[3], at[14]);    MULADD(at[4], at[13]);    MULADD(at[5], at[12]); 
   COMBA_STORE(C->dp[5]);
   /* 6 */
   COMBA_FORWARD;
   MULADD(at[0], at[18]);    MULADD(at[1], at[17]);    MULADD(at[2], at[16]);    MULADD(at[3], at[15]);    MULADD(at[4], at[14]);    MULADD(at[5], at[13]);    MULADD(at[6], at[12]); 
   COMBA_STORE(C->dp[6]);
   /* 7 */
   COMBA_FORWARD;
   MULADD(at[0], at[19]);    MULADD(at[1], at[18]);    MULADD(at[2], at[17]);    MULADD(at[3], at[16]);    MULADD(at[4], at[15]);    MULADD(at[5], at[14]);    MULADD(at[6], at[13]);    MULADD(at[7], at[12]); 
   COMBA_STORE(C->dp[7]);
   /* 8 */
   COMBA_FORWARD;
   MULADD(at[0], at[20]);    MULADD(at[1], at[19]);    MULADD(at[2], at[18]);    MULADD(at[3], at[17]);    MULADD(at[4], at[16]);    MULADD(at[5], at[15]);    MULADD(at[6], at[14]);    MULADD(at[7], at[13]);    MULADD(at[8], at[12]); 
   COMBA_STORE(C->dp[8]);
   /* 9 */
   COMBA_FORWARD;
   MULADD(at[0], at[21]);    MULADD(at[1], at[20]);    MULADD(at[2], at[19]);    MULADD(at[3], at[18]);    MULADD(at[4], at[17]);    MULADD(at[5], at[16]);    MULADD(at[6], at[15]);    MULADD(at[7], at[14]);    MULADD(at[8], at[13]);    MULADD(at[9], at[12]); 
   COMBA_STORE(C->dp[9]);
   /* 10 */
   COMBA_FORWARD;
   MULADD(at[0], at[22]);    MULADD(at[1], at[21]);    MULADD(at[2], at[20]);    MULADD(at[3], at[19]);    MULADD(at[4], at[18]);    MULADD(at[5], at[17]);    MULADD(at[6], at[16]);    MULADD(at[7], at[15]);    MULADD(at[8], at[14]);    MULADD(at[9], at[13]);    MULADD(at[10], at[12]); 
   COMBA_STORE(C->dp[10]);
   /* 11 */
   COMBA_FORWARD;
   MULADD(at[0], at[23]);    MULADD(at[1], at[22]);    MULADD(at[2], at[21]);    MULADD(at[3], at[20]);    MULADD(at[4], at[19]);    MULADD(at[5], at[18]);    MULADD(at[6], at[17]);    MULADD(at[7], at[16]);    MULADD(at[8], at[15]);    MULADD(at[9], at[14]);    MULADD(at[10], at[13]);    MULADD(at[11], at[12]); 
   COMBA_STORE(C->dp[11]);
   /* 12 */
   COMBA_FORWARD;
   MULADD(at[1], at[23]);    MULADD(at[2], at[22]);    MULADD(at[3], at[21]);    MULADD(at[4], at[20]);    MULADD(at[5], at[19]);    MULADD(at[6], at[18]);    MULADD(at[7], at[17]);    MULADD(at[8], at[16]);    MULADD(at[9], at[15]);    MULADD(at[10], at[14]);    MULADD(at[11], at[13]); 
   COMBA_STORE(C->dp[12]);
   /* 13 */
   COMBA_FORWARD;
   MULADD(at[2], at[23]);    MULADD(at[3], at[22]);    MULADD(at[4], at[21]);    MULADD(at[5], at[20]);    MULADD(at[6], at[19]);    MULADD(at[7], at[18]);    MULADD(at[8], at[17]);    MULADD(at[9], at[16]);    MULADD(at[10], at[15]);    MULADD(at[11], at[14]); 
   COMBA_STORE(C->dp[13]);
   /* 14 */
   COMBA_FORWARD;
   MULADD(at[3], at[23]);    MULADD(at[4], at[22]);    MULADD(at[5], at[21]);    MULADD(at[6], at[20]);    MULADD(at[7], at[19]);    MULADD(at[8], at[18]);    MULADD(at[9], at[17]);    MULADD(at[10], at[16]);    MULADD(at[11], at[15]); 
   COMBA_STORE(C->dp[14]);
   /* 15 */
   COMBA_FORWARD;
   MULADD(at[4], at[23]);    MULADD(at[5], at[22]);    MULADD(at[6], at[21]);    MULADD(at[7], at[20]);    MULADD(at[8], at[19]);    MULADD(at[9], at[18]);    MULADD(at[10], at[17]);    MULADD(at[11], at[16]); 
   COMBA_STORE(C->dp[15]);
   /* 16 */
   COMBA_FORWARD;
   MULADD(at[5], at[23]);    MULADD(at[6], at[22]);    MULADD(at[7], at[21]);    MULADD(at[8], at[20]);    MULADD(at[9], at[19]);    MULADD(at[10], at[18]);    MULADD(at[11], at[17]); 
   COMBA_STORE(C->dp[16]);
   /* 17 */
   COMBA_FORWARD;
   MULADD(at[6], at[23]);    MULADD(at[7], at[22]);    MULADD(at[8], at[21]);    MULADD(at[9], at[20]);    MULADD(at[10], at[19]);    MULADD(at[11], at[18]); 
   COMBA_STORE(C->dp[17]);
   /* 18 */
   COMBA_FORWARD;
   MULADD(at[7], at[23]);    MULADD(at[8], at[22]);    MULADD(at[9], at[21]);    MULADD(at[10], at[20]);    MULADD(at[11], at[19]); 
   COMBA_STORE(C->dp[18]);
   /* 19 */
   COMBA_FORWARD;
   MULADD(at[8], at[23]);    MULADD(at[9], at[22]);    MULADD(at[10], at[21]);    MULADD(at[11], at[20]); 
   COMBA_STORE(C->dp[19]);
   /* 20 */
   COMBA_FORWARD;
   MULADD(at[9], at[23]);    MULADD(at[10], at[22]);    MULADD(at[11], at[21]); 
   COMBA_STORE(C->dp[20]);
   /* 21 */
   COMBA_FORWARD;
   MULADD(at[10], at[23]);    MULADD(at[11], at[22]); 
   COMBA_STORE(C->dp[21]);
   /* 22 */
   COMBA_FORWARD;
   MULADD(at[11], at[23]); 
   COMBA_STORE(C->dp[22]);
   COMBA_STORE2(C->dp[23]);
   C->used = 24;
   C->sign = A->sign ^ B->sign;
   fp_clamp(C);
   COMBA_FINI;

#ifdef WOLFSSL_SMALL_STACK
   XFREE(at, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif
