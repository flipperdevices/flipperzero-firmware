/* fp_mul_comba_small_set.i
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



#if defined(TFM_SMALL_SET)
int fp_mul_comba_small(fp_int *A, fp_int *B, fp_int *C)
{
   fp_digit c0, c1, c2;
#ifndef WOLFSSL_SMALL_STACK
   fp_digit at[32];
#else
   fp_digit *at;
#endif

#ifdef WOLFSSL_SMALL_STACK
   at = (fp_digit*)XMALLOC(sizeof(fp_digit) * 32, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (at == NULL)
       return FP_MEM;
#endif

   switch (MAX(A->used, B->used)) { 

   case 1:
      XMEMCPY(at, A->dp, 1 * sizeof(fp_digit));
      XMEMCPY(at+1, B->dp, 1 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[1]); 
      COMBA_STORE(C->dp[0]);
      COMBA_STORE2(C->dp[1]);
      C->used = 2;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 2:
      XMEMCPY(at, A->dp, 2 * sizeof(fp_digit));
      XMEMCPY(at+2, B->dp, 2 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[2]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[3]);       MULADD(at[1], at[2]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[1], at[3]); 
      COMBA_STORE(C->dp[2]);
      COMBA_STORE2(C->dp[3]);
      C->used = 4;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 3:
      XMEMCPY(at, A->dp, 3 * sizeof(fp_digit));
      XMEMCPY(at+3, B->dp, 3 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[3]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[4]);       MULADD(at[1], at[3]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[5]);       MULADD(at[1], at[4]);       MULADD(at[2], at[3]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[1], at[5]);       MULADD(at[2], at[4]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[2], at[5]); 
      COMBA_STORE(C->dp[4]);
      COMBA_STORE2(C->dp[5]);
      C->used = 6;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 4:
      XMEMCPY(at, A->dp, 4 * sizeof(fp_digit));
      XMEMCPY(at+4, B->dp, 4 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[4]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[5]);       MULADD(at[1], at[4]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[6]);       MULADD(at[1], at[5]);       MULADD(at[2], at[4]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[7]);       MULADD(at[1], at[6]);       MULADD(at[2], at[5]);       MULADD(at[3], at[4]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[1], at[7]);       MULADD(at[2], at[6]);       MULADD(at[3], at[5]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[2], at[7]);       MULADD(at[3], at[6]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[3], at[7]); 
      COMBA_STORE(C->dp[6]);
      COMBA_STORE2(C->dp[7]);
      C->used = 8;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 5:
      XMEMCPY(at, A->dp, 5 * sizeof(fp_digit));
      XMEMCPY(at+5, B->dp, 5 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[5]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[6]);       MULADD(at[1], at[5]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[7]);       MULADD(at[1], at[6]);       MULADD(at[2], at[5]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[8]);       MULADD(at[1], at[7]);       MULADD(at[2], at[6]);       MULADD(at[3], at[5]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[9]);       MULADD(at[1], at[8]);       MULADD(at[2], at[7]);       MULADD(at[3], at[6]);       MULADD(at[4], at[5]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[1], at[9]);       MULADD(at[2], at[8]);       MULADD(at[3], at[7]);       MULADD(at[4], at[6]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[2], at[9]);       MULADD(at[3], at[8]);       MULADD(at[4], at[7]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[3], at[9]);       MULADD(at[4], at[8]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[4], at[9]); 
      COMBA_STORE(C->dp[8]);
      COMBA_STORE2(C->dp[9]);
      C->used = 10;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 6:
      XMEMCPY(at, A->dp, 6 * sizeof(fp_digit));
      XMEMCPY(at+6, B->dp, 6 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[6]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[7]);       MULADD(at[1], at[6]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[8]);       MULADD(at[1], at[7]);       MULADD(at[2], at[6]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[9]);       MULADD(at[1], at[8]);       MULADD(at[2], at[7]);       MULADD(at[3], at[6]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[10]);       MULADD(at[1], at[9]);       MULADD(at[2], at[8]);       MULADD(at[3], at[7]);       MULADD(at[4], at[6]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[11]);       MULADD(at[1], at[10]);       MULADD(at[2], at[9]);       MULADD(at[3], at[8]);       MULADD(at[4], at[7]);       MULADD(at[5], at[6]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[1], at[11]);       MULADD(at[2], at[10]);       MULADD(at[3], at[9]);       MULADD(at[4], at[8]);       MULADD(at[5], at[7]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[2], at[11]);       MULADD(at[3], at[10]);       MULADD(at[4], at[9]);       MULADD(at[5], at[8]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[3], at[11]);       MULADD(at[4], at[10]);       MULADD(at[5], at[9]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[4], at[11]);       MULADD(at[5], at[10]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[5], at[11]); 
      COMBA_STORE(C->dp[10]);
      COMBA_STORE2(C->dp[11]);
      C->used = 12;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 7:
      XMEMCPY(at, A->dp, 7 * sizeof(fp_digit));
      XMEMCPY(at+7, B->dp, 7 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[7]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[8]);       MULADD(at[1], at[7]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[9]);       MULADD(at[1], at[8]);       MULADD(at[2], at[7]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[10]);       MULADD(at[1], at[9]);       MULADD(at[2], at[8]);       MULADD(at[3], at[7]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[11]);       MULADD(at[1], at[10]);       MULADD(at[2], at[9]);       MULADD(at[3], at[8]);       MULADD(at[4], at[7]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[12]);       MULADD(at[1], at[11]);       MULADD(at[2], at[10]);       MULADD(at[3], at[9]);       MULADD(at[4], at[8]);       MULADD(at[5], at[7]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[13]);       MULADD(at[1], at[12]);       MULADD(at[2], at[11]);       MULADD(at[3], at[10]);       MULADD(at[4], at[9]);       MULADD(at[5], at[8]);       MULADD(at[6], at[7]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[1], at[13]);       MULADD(at[2], at[12]);       MULADD(at[3], at[11]);       MULADD(at[4], at[10]);       MULADD(at[5], at[9]);       MULADD(at[6], at[8]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[2], at[13]);       MULADD(at[3], at[12]);       MULADD(at[4], at[11]);       MULADD(at[5], at[10]);       MULADD(at[6], at[9]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[3], at[13]);       MULADD(at[4], at[12]);       MULADD(at[5], at[11]);       MULADD(at[6], at[10]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[4], at[13]);       MULADD(at[5], at[12]);       MULADD(at[6], at[11]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[5], at[13]);       MULADD(at[6], at[12]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[6], at[13]); 
      COMBA_STORE(C->dp[12]);
      COMBA_STORE2(C->dp[13]);
      C->used = 14;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 8:
      XMEMCPY(at, A->dp, 8 * sizeof(fp_digit));
      XMEMCPY(at+8, B->dp, 8 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[8]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[9]);       MULADD(at[1], at[8]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[10]);       MULADD(at[1], at[9]);       MULADD(at[2], at[8]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[11]);       MULADD(at[1], at[10]);       MULADD(at[2], at[9]);       MULADD(at[3], at[8]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[12]);       MULADD(at[1], at[11]);       MULADD(at[2], at[10]);       MULADD(at[3], at[9]);       MULADD(at[4], at[8]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[13]);       MULADD(at[1], at[12]);       MULADD(at[2], at[11]);       MULADD(at[3], at[10]);       MULADD(at[4], at[9]);       MULADD(at[5], at[8]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[14]);       MULADD(at[1], at[13]);       MULADD(at[2], at[12]);       MULADD(at[3], at[11]);       MULADD(at[4], at[10]);       MULADD(at[5], at[9]);       MULADD(at[6], at[8]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]);       MULADD(at[2], at[13]);       MULADD(at[3], at[12]);       MULADD(at[4], at[11]);       MULADD(at[5], at[10]);       MULADD(at[6], at[9]);       MULADD(at[7], at[8]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[1], at[15]);       MULADD(at[2], at[14]);       MULADD(at[3], at[13]);       MULADD(at[4], at[12]);       MULADD(at[5], at[11]);       MULADD(at[6], at[10]);       MULADD(at[7], at[9]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[2], at[15]);       MULADD(at[3], at[14]);       MULADD(at[4], at[13]);       MULADD(at[5], at[12]);       MULADD(at[6], at[11]);       MULADD(at[7], at[10]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[3], at[15]);       MULADD(at[4], at[14]);       MULADD(at[5], at[13]);       MULADD(at[6], at[12]);       MULADD(at[7], at[11]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[4], at[15]);       MULADD(at[5], at[14]);       MULADD(at[6], at[13]);       MULADD(at[7], at[12]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[5], at[15]);       MULADD(at[6], at[14]);       MULADD(at[7], at[13]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[6], at[15]);       MULADD(at[7], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[7], at[15]); 
      COMBA_STORE(C->dp[14]);
      COMBA_STORE2(C->dp[15]);
      C->used = 16;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 9:
      XMEMCPY(at, A->dp, 9 * sizeof(fp_digit));
      XMEMCPY(at+9, B->dp, 9 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[9]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[10]);       MULADD(at[1], at[9]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[11]);       MULADD(at[1], at[10]);       MULADD(at[2], at[9]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[12]);       MULADD(at[1], at[11]);       MULADD(at[2], at[10]);       MULADD(at[3], at[9]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[13]);       MULADD(at[1], at[12]);       MULADD(at[2], at[11]);       MULADD(at[3], at[10]);       MULADD(at[4], at[9]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[14]);       MULADD(at[1], at[13]);       MULADD(at[2], at[12]);       MULADD(at[3], at[11]);       MULADD(at[4], at[10]);       MULADD(at[5], at[9]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]);       MULADD(at[2], at[13]);       MULADD(at[3], at[12]);       MULADD(at[4], at[11]);       MULADD(at[5], at[10]);       MULADD(at[6], at[9]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]);       MULADD(at[2], at[14]);       MULADD(at[3], at[13]);       MULADD(at[4], at[12]);       MULADD(at[5], at[11]);       MULADD(at[6], at[10]);       MULADD(at[7], at[9]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]);       MULADD(at[3], at[14]);       MULADD(at[4], at[13]);       MULADD(at[5], at[12]);       MULADD(at[6], at[11]);       MULADD(at[7], at[10]);       MULADD(at[8], at[9]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]);       MULADD(at[4], at[14]);       MULADD(at[5], at[13]);       MULADD(at[6], at[12]);       MULADD(at[7], at[11]);       MULADD(at[8], at[10]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]);       MULADD(at[5], at[14]);       MULADD(at[6], at[13]);       MULADD(at[7], at[12]);       MULADD(at[8], at[11]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]);       MULADD(at[6], at[14]);       MULADD(at[7], at[13]);       MULADD(at[8], at[12]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]);       MULADD(at[7], at[14]);       MULADD(at[8], at[13]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]);       MULADD(at[8], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[7], at[17]);       MULADD(at[8], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[8], at[17]); 
      COMBA_STORE(C->dp[16]);
      COMBA_STORE2(C->dp[17]);
      C->used = 18;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 10:
      XMEMCPY(at, A->dp, 10 * sizeof(fp_digit));
      XMEMCPY(at+10, B->dp, 10 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[10]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[11]);       MULADD(at[1], at[10]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[12]);       MULADD(at[1], at[11]);       MULADD(at[2], at[10]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[13]);       MULADD(at[1], at[12]);       MULADD(at[2], at[11]);       MULADD(at[3], at[10]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[14]);       MULADD(at[1], at[13]);       MULADD(at[2], at[12]);       MULADD(at[3], at[11]);       MULADD(at[4], at[10]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]);       MULADD(at[2], at[13]);       MULADD(at[3], at[12]);       MULADD(at[4], at[11]);       MULADD(at[5], at[10]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]);       MULADD(at[2], at[14]);       MULADD(at[3], at[13]);       MULADD(at[4], at[12]);       MULADD(at[5], at[11]);       MULADD(at[6], at[10]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]);       MULADD(at[3], at[14]);       MULADD(at[4], at[13]);       MULADD(at[5], at[12]);       MULADD(at[6], at[11]);       MULADD(at[7], at[10]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]);       MULADD(at[4], at[14]);       MULADD(at[5], at[13]);       MULADD(at[6], at[12]);       MULADD(at[7], at[11]);       MULADD(at[8], at[10]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]);       MULADD(at[5], at[14]);       MULADD(at[6], at[13]);       MULADD(at[7], at[12]);       MULADD(at[8], at[11]);       MULADD(at[9], at[10]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]);       MULADD(at[6], at[14]);       MULADD(at[7], at[13]);       MULADD(at[8], at[12]);       MULADD(at[9], at[11]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]);       MULADD(at[7], at[14]);       MULADD(at[8], at[13]);       MULADD(at[9], at[12]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]);       MULADD(at[8], at[14]);       MULADD(at[9], at[13]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]);       MULADD(at[9], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]);       MULADD(at[9], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[8], at[19]);       MULADD(at[9], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[9], at[19]); 
      COMBA_STORE(C->dp[18]);
      COMBA_STORE2(C->dp[19]);
      C->used = 20;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 11:
      XMEMCPY(at, A->dp, 11 * sizeof(fp_digit));
      XMEMCPY(at+11, B->dp, 11 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[11]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[12]);       MULADD(at[1], at[11]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[13]);       MULADD(at[1], at[12]);       MULADD(at[2], at[11]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[14]);       MULADD(at[1], at[13]);       MULADD(at[2], at[12]);       MULADD(at[3], at[11]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]);       MULADD(at[2], at[13]);       MULADD(at[3], at[12]);       MULADD(at[4], at[11]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]);       MULADD(at[2], at[14]);       MULADD(at[3], at[13]);       MULADD(at[4], at[12]);       MULADD(at[5], at[11]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]);       MULADD(at[3], at[14]);       MULADD(at[4], at[13]);       MULADD(at[5], at[12]);       MULADD(at[6], at[11]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]);       MULADD(at[4], at[14]);       MULADD(at[5], at[13]);       MULADD(at[6], at[12]);       MULADD(at[7], at[11]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]);       MULADD(at[5], at[14]);       MULADD(at[6], at[13]);       MULADD(at[7], at[12]);       MULADD(at[8], at[11]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[20]);       MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]);       MULADD(at[6], at[14]);       MULADD(at[7], at[13]);       MULADD(at[8], at[12]);       MULADD(at[9], at[11]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[0], at[21]);       MULADD(at[1], at[20]);       MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]);       MULADD(at[7], at[14]);       MULADD(at[8], at[13]);       MULADD(at[9], at[12]);       MULADD(at[10], at[11]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[1], at[21]);       MULADD(at[2], at[20]);       MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]);       MULADD(at[8], at[14]);       MULADD(at[9], at[13]);       MULADD(at[10], at[12]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[2], at[21]);       MULADD(at[3], at[20]);       MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]);       MULADD(at[9], at[14]);       MULADD(at[10], at[13]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[3], at[21]);       MULADD(at[4], at[20]);       MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]);       MULADD(at[9], at[15]);       MULADD(at[10], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[4], at[21]);       MULADD(at[5], at[20]);       MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]);       MULADD(at[10], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[5], at[21]);       MULADD(at[6], at[20]);       MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]);       MULADD(at[10], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[6], at[21]);       MULADD(at[7], at[20]);       MULADD(at[8], at[19]);       MULADD(at[9], at[18]);       MULADD(at[10], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[7], at[21]);       MULADD(at[8], at[20]);       MULADD(at[9], at[19]);       MULADD(at[10], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[8], at[21]);       MULADD(at[9], at[20]);       MULADD(at[10], at[19]); 
      COMBA_STORE(C->dp[18]);
      /* 19 */
      COMBA_FORWARD;
      MULADD(at[9], at[21]);       MULADD(at[10], at[20]); 
      COMBA_STORE(C->dp[19]);
      /* 20 */
      COMBA_FORWARD;
      MULADD(at[10], at[21]); 
      COMBA_STORE(C->dp[20]);
      COMBA_STORE2(C->dp[21]);
      C->used = 22;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 12:
      XMEMCPY(at, A->dp, 12 * sizeof(fp_digit));
      XMEMCPY(at+12, B->dp, 12 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[12]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[13]);       MULADD(at[1], at[12]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[14]);       MULADD(at[1], at[13]);       MULADD(at[2], at[12]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]);       MULADD(at[2], at[13]);       MULADD(at[3], at[12]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]);       MULADD(at[2], at[14]);       MULADD(at[3], at[13]);       MULADD(at[4], at[12]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]);       MULADD(at[3], at[14]);       MULADD(at[4], at[13]);       MULADD(at[5], at[12]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]);       MULADD(at[4], at[14]);       MULADD(at[5], at[13]);       MULADD(at[6], at[12]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]);       MULADD(at[5], at[14]);       MULADD(at[6], at[13]);       MULADD(at[7], at[12]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[20]);       MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]);       MULADD(at[6], at[14]);       MULADD(at[7], at[13]);       MULADD(at[8], at[12]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[21]);       MULADD(at[1], at[20]);       MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]);       MULADD(at[7], at[14]);       MULADD(at[8], at[13]);       MULADD(at[9], at[12]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[0], at[22]);       MULADD(at[1], at[21]);       MULADD(at[2], at[20]);       MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]);       MULADD(at[8], at[14]);       MULADD(at[9], at[13]);       MULADD(at[10], at[12]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[0], at[23]);       MULADD(at[1], at[22]);       MULADD(at[2], at[21]);       MULADD(at[3], at[20]);       MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]);       MULADD(at[9], at[14]);       MULADD(at[10], at[13]);       MULADD(at[11], at[12]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[1], at[23]);       MULADD(at[2], at[22]);       MULADD(at[3], at[21]);       MULADD(at[4], at[20]);       MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]);       MULADD(at[9], at[15]);       MULADD(at[10], at[14]);       MULADD(at[11], at[13]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[2], at[23]);       MULADD(at[3], at[22]);       MULADD(at[4], at[21]);       MULADD(at[5], at[20]);       MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]);       MULADD(at[10], at[15]);       MULADD(at[11], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[3], at[23]);       MULADD(at[4], at[22]);       MULADD(at[5], at[21]);       MULADD(at[6], at[20]);       MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]);       MULADD(at[10], at[16]);       MULADD(at[11], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[4], at[23]);       MULADD(at[5], at[22]);       MULADD(at[6], at[21]);       MULADD(at[7], at[20]);       MULADD(at[8], at[19]);       MULADD(at[9], at[18]);       MULADD(at[10], at[17]);       MULADD(at[11], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[5], at[23]);       MULADD(at[6], at[22]);       MULADD(at[7], at[21]);       MULADD(at[8], at[20]);       MULADD(at[9], at[19]);       MULADD(at[10], at[18]);       MULADD(at[11], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[6], at[23]);       MULADD(at[7], at[22]);       MULADD(at[8], at[21]);       MULADD(at[9], at[20]);       MULADD(at[10], at[19]);       MULADD(at[11], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[7], at[23]);       MULADD(at[8], at[22]);       MULADD(at[9], at[21]);       MULADD(at[10], at[20]);       MULADD(at[11], at[19]); 
      COMBA_STORE(C->dp[18]);
      /* 19 */
      COMBA_FORWARD;
      MULADD(at[8], at[23]);       MULADD(at[9], at[22]);       MULADD(at[10], at[21]);       MULADD(at[11], at[20]); 
      COMBA_STORE(C->dp[19]);
      /* 20 */
      COMBA_FORWARD;
      MULADD(at[9], at[23]);       MULADD(at[10], at[22]);       MULADD(at[11], at[21]); 
      COMBA_STORE(C->dp[20]);
      /* 21 */
      COMBA_FORWARD;
      MULADD(at[10], at[23]);       MULADD(at[11], at[22]); 
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
      break;

   case 13:
      XMEMCPY(at, A->dp, 13 * sizeof(fp_digit));
      XMEMCPY(at+13, B->dp, 13 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[13]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[14]);       MULADD(at[1], at[13]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]);       MULADD(at[2], at[13]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]);       MULADD(at[2], at[14]);       MULADD(at[3], at[13]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]);       MULADD(at[3], at[14]);       MULADD(at[4], at[13]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]);       MULADD(at[4], at[14]);       MULADD(at[5], at[13]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]);       MULADD(at[5], at[14]);       MULADD(at[6], at[13]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[20]);       MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]);       MULADD(at[6], at[14]);       MULADD(at[7], at[13]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[21]);       MULADD(at[1], at[20]);       MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]);       MULADD(at[7], at[14]);       MULADD(at[8], at[13]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[22]);       MULADD(at[1], at[21]);       MULADD(at[2], at[20]);       MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]);       MULADD(at[8], at[14]);       MULADD(at[9], at[13]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[0], at[23]);       MULADD(at[1], at[22]);       MULADD(at[2], at[21]);       MULADD(at[3], at[20]);       MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]);       MULADD(at[9], at[14]);       MULADD(at[10], at[13]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[0], at[24]);       MULADD(at[1], at[23]);       MULADD(at[2], at[22]);       MULADD(at[3], at[21]);       MULADD(at[4], at[20]);       MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]);       MULADD(at[9], at[15]);       MULADD(at[10], at[14]);       MULADD(at[11], at[13]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[0], at[25]);       MULADD(at[1], at[24]);       MULADD(at[2], at[23]);       MULADD(at[3], at[22]);       MULADD(at[4], at[21]);       MULADD(at[5], at[20]);       MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]);       MULADD(at[10], at[15]);       MULADD(at[11], at[14]);       MULADD(at[12], at[13]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[1], at[25]);       MULADD(at[2], at[24]);       MULADD(at[3], at[23]);       MULADD(at[4], at[22]);       MULADD(at[5], at[21]);       MULADD(at[6], at[20]);       MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]);       MULADD(at[10], at[16]);       MULADD(at[11], at[15]);       MULADD(at[12], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[2], at[25]);       MULADD(at[3], at[24]);       MULADD(at[4], at[23]);       MULADD(at[5], at[22]);       MULADD(at[6], at[21]);       MULADD(at[7], at[20]);       MULADD(at[8], at[19]);       MULADD(at[9], at[18]);       MULADD(at[10], at[17]);       MULADD(at[11], at[16]);       MULADD(at[12], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[3], at[25]);       MULADD(at[4], at[24]);       MULADD(at[5], at[23]);       MULADD(at[6], at[22]);       MULADD(at[7], at[21]);       MULADD(at[8], at[20]);       MULADD(at[9], at[19]);       MULADD(at[10], at[18]);       MULADD(at[11], at[17]);       MULADD(at[12], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[4], at[25]);       MULADD(at[5], at[24]);       MULADD(at[6], at[23]);       MULADD(at[7], at[22]);       MULADD(at[8], at[21]);       MULADD(at[9], at[20]);       MULADD(at[10], at[19]);       MULADD(at[11], at[18]);       MULADD(at[12], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[5], at[25]);       MULADD(at[6], at[24]);       MULADD(at[7], at[23]);       MULADD(at[8], at[22]);       MULADD(at[9], at[21]);       MULADD(at[10], at[20]);       MULADD(at[11], at[19]);       MULADD(at[12], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[6], at[25]);       MULADD(at[7], at[24]);       MULADD(at[8], at[23]);       MULADD(at[9], at[22]);       MULADD(at[10], at[21]);       MULADD(at[11], at[20]);       MULADD(at[12], at[19]); 
      COMBA_STORE(C->dp[18]);
      /* 19 */
      COMBA_FORWARD;
      MULADD(at[7], at[25]);       MULADD(at[8], at[24]);       MULADD(at[9], at[23]);       MULADD(at[10], at[22]);       MULADD(at[11], at[21]);       MULADD(at[12], at[20]); 
      COMBA_STORE(C->dp[19]);
      /* 20 */
      COMBA_FORWARD;
      MULADD(at[8], at[25]);       MULADD(at[9], at[24]);       MULADD(at[10], at[23]);       MULADD(at[11], at[22]);       MULADD(at[12], at[21]); 
      COMBA_STORE(C->dp[20]);
      /* 21 */
      COMBA_FORWARD;
      MULADD(at[9], at[25]);       MULADD(at[10], at[24]);       MULADD(at[11], at[23]);       MULADD(at[12], at[22]); 
      COMBA_STORE(C->dp[21]);
      /* 22 */
      COMBA_FORWARD;
      MULADD(at[10], at[25]);       MULADD(at[11], at[24]);       MULADD(at[12], at[23]); 
      COMBA_STORE(C->dp[22]);
      /* 23 */
      COMBA_FORWARD;
      MULADD(at[11], at[25]);       MULADD(at[12], at[24]); 
      COMBA_STORE(C->dp[23]);
      /* 24 */
      COMBA_FORWARD;
      MULADD(at[12], at[25]); 
      COMBA_STORE(C->dp[24]);
      COMBA_STORE2(C->dp[25]);
      C->used = 26;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 14:
      XMEMCPY(at, A->dp, 14 * sizeof(fp_digit));
      XMEMCPY(at+14, B->dp, 14 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[14]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[15]);       MULADD(at[1], at[14]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]);       MULADD(at[2], at[14]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]);       MULADD(at[3], at[14]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]);       MULADD(at[4], at[14]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]);       MULADD(at[5], at[14]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[20]);       MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]);       MULADD(at[6], at[14]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[21]);       MULADD(at[1], at[20]);       MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]);       MULADD(at[7], at[14]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[22]);       MULADD(at[1], at[21]);       MULADD(at[2], at[20]);       MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]);       MULADD(at[8], at[14]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[23]);       MULADD(at[1], at[22]);       MULADD(at[2], at[21]);       MULADD(at[3], at[20]);       MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]);       MULADD(at[9], at[14]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[0], at[24]);       MULADD(at[1], at[23]);       MULADD(at[2], at[22]);       MULADD(at[3], at[21]);       MULADD(at[4], at[20]);       MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]);       MULADD(at[9], at[15]);       MULADD(at[10], at[14]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[0], at[25]);       MULADD(at[1], at[24]);       MULADD(at[2], at[23]);       MULADD(at[3], at[22]);       MULADD(at[4], at[21]);       MULADD(at[5], at[20]);       MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]);       MULADD(at[10], at[15]);       MULADD(at[11], at[14]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[0], at[26]);       MULADD(at[1], at[25]);       MULADD(at[2], at[24]);       MULADD(at[3], at[23]);       MULADD(at[4], at[22]);       MULADD(at[5], at[21]);       MULADD(at[6], at[20]);       MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]);       MULADD(at[10], at[16]);       MULADD(at[11], at[15]);       MULADD(at[12], at[14]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[0], at[27]);       MULADD(at[1], at[26]);       MULADD(at[2], at[25]);       MULADD(at[3], at[24]);       MULADD(at[4], at[23]);       MULADD(at[5], at[22]);       MULADD(at[6], at[21]);       MULADD(at[7], at[20]);       MULADD(at[8], at[19]);       MULADD(at[9], at[18]);       MULADD(at[10], at[17]);       MULADD(at[11], at[16]);       MULADD(at[12], at[15]);       MULADD(at[13], at[14]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[1], at[27]);       MULADD(at[2], at[26]);       MULADD(at[3], at[25]);       MULADD(at[4], at[24]);       MULADD(at[5], at[23]);       MULADD(at[6], at[22]);       MULADD(at[7], at[21]);       MULADD(at[8], at[20]);       MULADD(at[9], at[19]);       MULADD(at[10], at[18]);       MULADD(at[11], at[17]);       MULADD(at[12], at[16]);       MULADD(at[13], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[2], at[27]);       MULADD(at[3], at[26]);       MULADD(at[4], at[25]);       MULADD(at[5], at[24]);       MULADD(at[6], at[23]);       MULADD(at[7], at[22]);       MULADD(at[8], at[21]);       MULADD(at[9], at[20]);       MULADD(at[10], at[19]);       MULADD(at[11], at[18]);       MULADD(at[12], at[17]);       MULADD(at[13], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[3], at[27]);       MULADD(at[4], at[26]);       MULADD(at[5], at[25]);       MULADD(at[6], at[24]);       MULADD(at[7], at[23]);       MULADD(at[8], at[22]);       MULADD(at[9], at[21]);       MULADD(at[10], at[20]);       MULADD(at[11], at[19]);       MULADD(at[12], at[18]);       MULADD(at[13], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[4], at[27]);       MULADD(at[5], at[26]);       MULADD(at[6], at[25]);       MULADD(at[7], at[24]);       MULADD(at[8], at[23]);       MULADD(at[9], at[22]);       MULADD(at[10], at[21]);       MULADD(at[11], at[20]);       MULADD(at[12], at[19]);       MULADD(at[13], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[5], at[27]);       MULADD(at[6], at[26]);       MULADD(at[7], at[25]);       MULADD(at[8], at[24]);       MULADD(at[9], at[23]);       MULADD(at[10], at[22]);       MULADD(at[11], at[21]);       MULADD(at[12], at[20]);       MULADD(at[13], at[19]); 
      COMBA_STORE(C->dp[18]);
      /* 19 */
      COMBA_FORWARD;
      MULADD(at[6], at[27]);       MULADD(at[7], at[26]);       MULADD(at[8], at[25]);       MULADD(at[9], at[24]);       MULADD(at[10], at[23]);       MULADD(at[11], at[22]);       MULADD(at[12], at[21]);       MULADD(at[13], at[20]); 
      COMBA_STORE(C->dp[19]);
      /* 20 */
      COMBA_FORWARD;
      MULADD(at[7], at[27]);       MULADD(at[8], at[26]);       MULADD(at[9], at[25]);       MULADD(at[10], at[24]);       MULADD(at[11], at[23]);       MULADD(at[12], at[22]);       MULADD(at[13], at[21]); 
      COMBA_STORE(C->dp[20]);
      /* 21 */
      COMBA_FORWARD;
      MULADD(at[8], at[27]);       MULADD(at[9], at[26]);       MULADD(at[10], at[25]);       MULADD(at[11], at[24]);       MULADD(at[12], at[23]);       MULADD(at[13], at[22]); 
      COMBA_STORE(C->dp[21]);
      /* 22 */
      COMBA_FORWARD;
      MULADD(at[9], at[27]);       MULADD(at[10], at[26]);       MULADD(at[11], at[25]);       MULADD(at[12], at[24]);       MULADD(at[13], at[23]); 
      COMBA_STORE(C->dp[22]);
      /* 23 */
      COMBA_FORWARD;
      MULADD(at[10], at[27]);       MULADD(at[11], at[26]);       MULADD(at[12], at[25]);       MULADD(at[13], at[24]); 
      COMBA_STORE(C->dp[23]);
      /* 24 */
      COMBA_FORWARD;
      MULADD(at[11], at[27]);       MULADD(at[12], at[26]);       MULADD(at[13], at[25]); 
      COMBA_STORE(C->dp[24]);
      /* 25 */
      COMBA_FORWARD;
      MULADD(at[12], at[27]);       MULADD(at[13], at[26]); 
      COMBA_STORE(C->dp[25]);
      /* 26 */
      COMBA_FORWARD;
      MULADD(at[13], at[27]); 
      COMBA_STORE(C->dp[26]);
      COMBA_STORE2(C->dp[27]);
      C->used = 28;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 15:
      XMEMCPY(at, A->dp, 15 * sizeof(fp_digit));
      XMEMCPY(at+15, B->dp, 15 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[15]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[16]);       MULADD(at[1], at[15]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]);       MULADD(at[2], at[15]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]);       MULADD(at[3], at[15]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]);       MULADD(at[4], at[15]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[20]);       MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]);       MULADD(at[5], at[15]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[21]);       MULADD(at[1], at[20]);       MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]);       MULADD(at[6], at[15]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[22]);       MULADD(at[1], at[21]);       MULADD(at[2], at[20]);       MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]);       MULADD(at[7], at[15]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[23]);       MULADD(at[1], at[22]);       MULADD(at[2], at[21]);       MULADD(at[3], at[20]);       MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]);       MULADD(at[8], at[15]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[24]);       MULADD(at[1], at[23]);       MULADD(at[2], at[22]);       MULADD(at[3], at[21]);       MULADD(at[4], at[20]);       MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]);       MULADD(at[9], at[15]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[0], at[25]);       MULADD(at[1], at[24]);       MULADD(at[2], at[23]);       MULADD(at[3], at[22]);       MULADD(at[4], at[21]);       MULADD(at[5], at[20]);       MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]);       MULADD(at[10], at[15]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[0], at[26]);       MULADD(at[1], at[25]);       MULADD(at[2], at[24]);       MULADD(at[3], at[23]);       MULADD(at[4], at[22]);       MULADD(at[5], at[21]);       MULADD(at[6], at[20]);       MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]);       MULADD(at[10], at[16]);       MULADD(at[11], at[15]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[0], at[27]);       MULADD(at[1], at[26]);       MULADD(at[2], at[25]);       MULADD(at[3], at[24]);       MULADD(at[4], at[23]);       MULADD(at[5], at[22]);       MULADD(at[6], at[21]);       MULADD(at[7], at[20]);       MULADD(at[8], at[19]);       MULADD(at[9], at[18]);       MULADD(at[10], at[17]);       MULADD(at[11], at[16]);       MULADD(at[12], at[15]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[0], at[28]);       MULADD(at[1], at[27]);       MULADD(at[2], at[26]);       MULADD(at[3], at[25]);       MULADD(at[4], at[24]);       MULADD(at[5], at[23]);       MULADD(at[6], at[22]);       MULADD(at[7], at[21]);       MULADD(at[8], at[20]);       MULADD(at[9], at[19]);       MULADD(at[10], at[18]);       MULADD(at[11], at[17]);       MULADD(at[12], at[16]);       MULADD(at[13], at[15]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[0], at[29]);       MULADD(at[1], at[28]);       MULADD(at[2], at[27]);       MULADD(at[3], at[26]);       MULADD(at[4], at[25]);       MULADD(at[5], at[24]);       MULADD(at[6], at[23]);       MULADD(at[7], at[22]);       MULADD(at[8], at[21]);       MULADD(at[9], at[20]);       MULADD(at[10], at[19]);       MULADD(at[11], at[18]);       MULADD(at[12], at[17]);       MULADD(at[13], at[16]);       MULADD(at[14], at[15]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[1], at[29]);       MULADD(at[2], at[28]);       MULADD(at[3], at[27]);       MULADD(at[4], at[26]);       MULADD(at[5], at[25]);       MULADD(at[6], at[24]);       MULADD(at[7], at[23]);       MULADD(at[8], at[22]);       MULADD(at[9], at[21]);       MULADD(at[10], at[20]);       MULADD(at[11], at[19]);       MULADD(at[12], at[18]);       MULADD(at[13], at[17]);       MULADD(at[14], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[2], at[29]);       MULADD(at[3], at[28]);       MULADD(at[4], at[27]);       MULADD(at[5], at[26]);       MULADD(at[6], at[25]);       MULADD(at[7], at[24]);       MULADD(at[8], at[23]);       MULADD(at[9], at[22]);       MULADD(at[10], at[21]);       MULADD(at[11], at[20]);       MULADD(at[12], at[19]);       MULADD(at[13], at[18]);       MULADD(at[14], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[3], at[29]);       MULADD(at[4], at[28]);       MULADD(at[5], at[27]);       MULADD(at[6], at[26]);       MULADD(at[7], at[25]);       MULADD(at[8], at[24]);       MULADD(at[9], at[23]);       MULADD(at[10], at[22]);       MULADD(at[11], at[21]);       MULADD(at[12], at[20]);       MULADD(at[13], at[19]);       MULADD(at[14], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[4], at[29]);       MULADD(at[5], at[28]);       MULADD(at[6], at[27]);       MULADD(at[7], at[26]);       MULADD(at[8], at[25]);       MULADD(at[9], at[24]);       MULADD(at[10], at[23]);       MULADD(at[11], at[22]);       MULADD(at[12], at[21]);       MULADD(at[13], at[20]);       MULADD(at[14], at[19]); 
      COMBA_STORE(C->dp[18]);
      /* 19 */
      COMBA_FORWARD;
      MULADD(at[5], at[29]);       MULADD(at[6], at[28]);       MULADD(at[7], at[27]);       MULADD(at[8], at[26]);       MULADD(at[9], at[25]);       MULADD(at[10], at[24]);       MULADD(at[11], at[23]);       MULADD(at[12], at[22]);       MULADD(at[13], at[21]);       MULADD(at[14], at[20]); 
      COMBA_STORE(C->dp[19]);
      /* 20 */
      COMBA_FORWARD;
      MULADD(at[6], at[29]);       MULADD(at[7], at[28]);       MULADD(at[8], at[27]);       MULADD(at[9], at[26]);       MULADD(at[10], at[25]);       MULADD(at[11], at[24]);       MULADD(at[12], at[23]);       MULADD(at[13], at[22]);       MULADD(at[14], at[21]); 
      COMBA_STORE(C->dp[20]);
      /* 21 */
      COMBA_FORWARD;
      MULADD(at[7], at[29]);       MULADD(at[8], at[28]);       MULADD(at[9], at[27]);       MULADD(at[10], at[26]);       MULADD(at[11], at[25]);       MULADD(at[12], at[24]);       MULADD(at[13], at[23]);       MULADD(at[14], at[22]); 
      COMBA_STORE(C->dp[21]);
      /* 22 */
      COMBA_FORWARD;
      MULADD(at[8], at[29]);       MULADD(at[9], at[28]);       MULADD(at[10], at[27]);       MULADD(at[11], at[26]);       MULADD(at[12], at[25]);       MULADD(at[13], at[24]);       MULADD(at[14], at[23]); 
      COMBA_STORE(C->dp[22]);
      /* 23 */
      COMBA_FORWARD;
      MULADD(at[9], at[29]);       MULADD(at[10], at[28]);       MULADD(at[11], at[27]);       MULADD(at[12], at[26]);       MULADD(at[13], at[25]);       MULADD(at[14], at[24]); 
      COMBA_STORE(C->dp[23]);
      /* 24 */
      COMBA_FORWARD;
      MULADD(at[10], at[29]);       MULADD(at[11], at[28]);       MULADD(at[12], at[27]);       MULADD(at[13], at[26]);       MULADD(at[14], at[25]); 
      COMBA_STORE(C->dp[24]);
      /* 25 */
      COMBA_FORWARD;
      MULADD(at[11], at[29]);       MULADD(at[12], at[28]);       MULADD(at[13], at[27]);       MULADD(at[14], at[26]); 
      COMBA_STORE(C->dp[25]);
      /* 26 */
      COMBA_FORWARD;
      MULADD(at[12], at[29]);       MULADD(at[13], at[28]);       MULADD(at[14], at[27]); 
      COMBA_STORE(C->dp[26]);
      /* 27 */
      COMBA_FORWARD;
      MULADD(at[13], at[29]);       MULADD(at[14], at[28]); 
      COMBA_STORE(C->dp[27]);
      /* 28 */
      COMBA_FORWARD;
      MULADD(at[14], at[29]); 
      COMBA_STORE(C->dp[28]);
      COMBA_STORE2(C->dp[29]);
      C->used = 30;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   case 16:
      XMEMCPY(at, A->dp, 16 * sizeof(fp_digit));
      XMEMCPY(at+16, B->dp, 16 * sizeof(fp_digit));
      COMBA_START;

      COMBA_CLEAR;
      /* 0 */
      MULADD(at[0], at[16]); 
      COMBA_STORE(C->dp[0]);
      /* 1 */
      COMBA_FORWARD;
      MULADD(at[0], at[17]);       MULADD(at[1], at[16]); 
      COMBA_STORE(C->dp[1]);
      /* 2 */
      COMBA_FORWARD;
      MULADD(at[0], at[18]);       MULADD(at[1], at[17]);       MULADD(at[2], at[16]); 
      COMBA_STORE(C->dp[2]);
      /* 3 */
      COMBA_FORWARD;
      MULADD(at[0], at[19]);       MULADD(at[1], at[18]);       MULADD(at[2], at[17]);       MULADD(at[3], at[16]); 
      COMBA_STORE(C->dp[3]);
      /* 4 */
      COMBA_FORWARD;
      MULADD(at[0], at[20]);       MULADD(at[1], at[19]);       MULADD(at[2], at[18]);       MULADD(at[3], at[17]);       MULADD(at[4], at[16]); 
      COMBA_STORE(C->dp[4]);
      /* 5 */
      COMBA_FORWARD;
      MULADD(at[0], at[21]);       MULADD(at[1], at[20]);       MULADD(at[2], at[19]);       MULADD(at[3], at[18]);       MULADD(at[4], at[17]);       MULADD(at[5], at[16]); 
      COMBA_STORE(C->dp[5]);
      /* 6 */
      COMBA_FORWARD;
      MULADD(at[0], at[22]);       MULADD(at[1], at[21]);       MULADD(at[2], at[20]);       MULADD(at[3], at[19]);       MULADD(at[4], at[18]);       MULADD(at[5], at[17]);       MULADD(at[6], at[16]); 
      COMBA_STORE(C->dp[6]);
      /* 7 */
      COMBA_FORWARD;
      MULADD(at[0], at[23]);       MULADD(at[1], at[22]);       MULADD(at[2], at[21]);       MULADD(at[3], at[20]);       MULADD(at[4], at[19]);       MULADD(at[5], at[18]);       MULADD(at[6], at[17]);       MULADD(at[7], at[16]); 
      COMBA_STORE(C->dp[7]);
      /* 8 */
      COMBA_FORWARD;
      MULADD(at[0], at[24]);       MULADD(at[1], at[23]);       MULADD(at[2], at[22]);       MULADD(at[3], at[21]);       MULADD(at[4], at[20]);       MULADD(at[5], at[19]);       MULADD(at[6], at[18]);       MULADD(at[7], at[17]);       MULADD(at[8], at[16]); 
      COMBA_STORE(C->dp[8]);
      /* 9 */
      COMBA_FORWARD;
      MULADD(at[0], at[25]);       MULADD(at[1], at[24]);       MULADD(at[2], at[23]);       MULADD(at[3], at[22]);       MULADD(at[4], at[21]);       MULADD(at[5], at[20]);       MULADD(at[6], at[19]);       MULADD(at[7], at[18]);       MULADD(at[8], at[17]);       MULADD(at[9], at[16]); 
      COMBA_STORE(C->dp[9]);
      /* 10 */
      COMBA_FORWARD;
      MULADD(at[0], at[26]);       MULADD(at[1], at[25]);       MULADD(at[2], at[24]);       MULADD(at[3], at[23]);       MULADD(at[4], at[22]);       MULADD(at[5], at[21]);       MULADD(at[6], at[20]);       MULADD(at[7], at[19]);       MULADD(at[8], at[18]);       MULADD(at[9], at[17]);       MULADD(at[10], at[16]); 
      COMBA_STORE(C->dp[10]);
      /* 11 */
      COMBA_FORWARD;
      MULADD(at[0], at[27]);       MULADD(at[1], at[26]);       MULADD(at[2], at[25]);       MULADD(at[3], at[24]);       MULADD(at[4], at[23]);       MULADD(at[5], at[22]);       MULADD(at[6], at[21]);       MULADD(at[7], at[20]);       MULADD(at[8], at[19]);       MULADD(at[9], at[18]);       MULADD(at[10], at[17]);       MULADD(at[11], at[16]); 
      COMBA_STORE(C->dp[11]);
      /* 12 */
      COMBA_FORWARD;
      MULADD(at[0], at[28]);       MULADD(at[1], at[27]);       MULADD(at[2], at[26]);       MULADD(at[3], at[25]);       MULADD(at[4], at[24]);       MULADD(at[5], at[23]);       MULADD(at[6], at[22]);       MULADD(at[7], at[21]);       MULADD(at[8], at[20]);       MULADD(at[9], at[19]);       MULADD(at[10], at[18]);       MULADD(at[11], at[17]);       MULADD(at[12], at[16]); 
      COMBA_STORE(C->dp[12]);
      /* 13 */
      COMBA_FORWARD;
      MULADD(at[0], at[29]);       MULADD(at[1], at[28]);       MULADD(at[2], at[27]);       MULADD(at[3], at[26]);       MULADD(at[4], at[25]);       MULADD(at[5], at[24]);       MULADD(at[6], at[23]);       MULADD(at[7], at[22]);       MULADD(at[8], at[21]);       MULADD(at[9], at[20]);       MULADD(at[10], at[19]);       MULADD(at[11], at[18]);       MULADD(at[12], at[17]);       MULADD(at[13], at[16]); 
      COMBA_STORE(C->dp[13]);
      /* 14 */
      COMBA_FORWARD;
      MULADD(at[0], at[30]);       MULADD(at[1], at[29]);       MULADD(at[2], at[28]);       MULADD(at[3], at[27]);       MULADD(at[4], at[26]);       MULADD(at[5], at[25]);       MULADD(at[6], at[24]);       MULADD(at[7], at[23]);       MULADD(at[8], at[22]);       MULADD(at[9], at[21]);       MULADD(at[10], at[20]);       MULADD(at[11], at[19]);       MULADD(at[12], at[18]);       MULADD(at[13], at[17]);       MULADD(at[14], at[16]); 
      COMBA_STORE(C->dp[14]);
      /* 15 */
      COMBA_FORWARD;
      MULADD(at[0], at[31]);       MULADD(at[1], at[30]);       MULADD(at[2], at[29]);       MULADD(at[3], at[28]);       MULADD(at[4], at[27]);       MULADD(at[5], at[26]);       MULADD(at[6], at[25]);       MULADD(at[7], at[24]);       MULADD(at[8], at[23]);       MULADD(at[9], at[22]);       MULADD(at[10], at[21]);       MULADD(at[11], at[20]);       MULADD(at[12], at[19]);       MULADD(at[13], at[18]);       MULADD(at[14], at[17]);       MULADD(at[15], at[16]); 
      COMBA_STORE(C->dp[15]);
      /* 16 */
      COMBA_FORWARD;
      MULADD(at[1], at[31]);       MULADD(at[2], at[30]);       MULADD(at[3], at[29]);       MULADD(at[4], at[28]);       MULADD(at[5], at[27]);       MULADD(at[6], at[26]);       MULADD(at[7], at[25]);       MULADD(at[8], at[24]);       MULADD(at[9], at[23]);       MULADD(at[10], at[22]);       MULADD(at[11], at[21]);       MULADD(at[12], at[20]);       MULADD(at[13], at[19]);       MULADD(at[14], at[18]);       MULADD(at[15], at[17]); 
      COMBA_STORE(C->dp[16]);
      /* 17 */
      COMBA_FORWARD;
      MULADD(at[2], at[31]);       MULADD(at[3], at[30]);       MULADD(at[4], at[29]);       MULADD(at[5], at[28]);       MULADD(at[6], at[27]);       MULADD(at[7], at[26]);       MULADD(at[8], at[25]);       MULADD(at[9], at[24]);       MULADD(at[10], at[23]);       MULADD(at[11], at[22]);       MULADD(at[12], at[21]);       MULADD(at[13], at[20]);       MULADD(at[14], at[19]);       MULADD(at[15], at[18]); 
      COMBA_STORE(C->dp[17]);
      /* 18 */
      COMBA_FORWARD;
      MULADD(at[3], at[31]);       MULADD(at[4], at[30]);       MULADD(at[5], at[29]);       MULADD(at[6], at[28]);       MULADD(at[7], at[27]);       MULADD(at[8], at[26]);       MULADD(at[9], at[25]);       MULADD(at[10], at[24]);       MULADD(at[11], at[23]);       MULADD(at[12], at[22]);       MULADD(at[13], at[21]);       MULADD(at[14], at[20]);       MULADD(at[15], at[19]); 
      COMBA_STORE(C->dp[18]);
      /* 19 */
      COMBA_FORWARD;
      MULADD(at[4], at[31]);       MULADD(at[5], at[30]);       MULADD(at[6], at[29]);       MULADD(at[7], at[28]);       MULADD(at[8], at[27]);       MULADD(at[9], at[26]);       MULADD(at[10], at[25]);       MULADD(at[11], at[24]);       MULADD(at[12], at[23]);       MULADD(at[13], at[22]);       MULADD(at[14], at[21]);       MULADD(at[15], at[20]); 
      COMBA_STORE(C->dp[19]);
      /* 20 */
      COMBA_FORWARD;
      MULADD(at[5], at[31]);       MULADD(at[6], at[30]);       MULADD(at[7], at[29]);       MULADD(at[8], at[28]);       MULADD(at[9], at[27]);       MULADD(at[10], at[26]);       MULADD(at[11], at[25]);       MULADD(at[12], at[24]);       MULADD(at[13], at[23]);       MULADD(at[14], at[22]);       MULADD(at[15], at[21]); 
      COMBA_STORE(C->dp[20]);
      /* 21 */
      COMBA_FORWARD;
      MULADD(at[6], at[31]);       MULADD(at[7], at[30]);       MULADD(at[8], at[29]);       MULADD(at[9], at[28]);       MULADD(at[10], at[27]);       MULADD(at[11], at[26]);       MULADD(at[12], at[25]);       MULADD(at[13], at[24]);       MULADD(at[14], at[23]);       MULADD(at[15], at[22]); 
      COMBA_STORE(C->dp[21]);
      /* 22 */
      COMBA_FORWARD;
      MULADD(at[7], at[31]);       MULADD(at[8], at[30]);       MULADD(at[9], at[29]);       MULADD(at[10], at[28]);       MULADD(at[11], at[27]);       MULADD(at[12], at[26]);       MULADD(at[13], at[25]);       MULADD(at[14], at[24]);       MULADD(at[15], at[23]); 
      COMBA_STORE(C->dp[22]);
      /* 23 */
      COMBA_FORWARD;
      MULADD(at[8], at[31]);       MULADD(at[9], at[30]);       MULADD(at[10], at[29]);       MULADD(at[11], at[28]);       MULADD(at[12], at[27]);       MULADD(at[13], at[26]);       MULADD(at[14], at[25]);       MULADD(at[15], at[24]); 
      COMBA_STORE(C->dp[23]);
      /* 24 */
      COMBA_FORWARD;
      MULADD(at[9], at[31]);       MULADD(at[10], at[30]);       MULADD(at[11], at[29]);       MULADD(at[12], at[28]);       MULADD(at[13], at[27]);       MULADD(at[14], at[26]);       MULADD(at[15], at[25]); 
      COMBA_STORE(C->dp[24]);
      /* 25 */
      COMBA_FORWARD;
      MULADD(at[10], at[31]);       MULADD(at[11], at[30]);       MULADD(at[12], at[29]);       MULADD(at[13], at[28]);       MULADD(at[14], at[27]);       MULADD(at[15], at[26]); 
      COMBA_STORE(C->dp[25]);
      /* 26 */
      COMBA_FORWARD;
      MULADD(at[11], at[31]);       MULADD(at[12], at[30]);       MULADD(at[13], at[29]);       MULADD(at[14], at[28]);       MULADD(at[15], at[27]); 
      COMBA_STORE(C->dp[26]);
      /* 27 */
      COMBA_FORWARD;
      MULADD(at[12], at[31]);       MULADD(at[13], at[30]);       MULADD(at[14], at[29]);       MULADD(at[15], at[28]); 
      COMBA_STORE(C->dp[27]);
      /* 28 */
      COMBA_FORWARD;
      MULADD(at[13], at[31]);       MULADD(at[14], at[30]);       MULADD(at[15], at[29]); 
      COMBA_STORE(C->dp[28]);
      /* 29 */
      COMBA_FORWARD;
      MULADD(at[14], at[31]);       MULADD(at[15], at[30]); 
      COMBA_STORE(C->dp[29]);
      /* 30 */
      COMBA_FORWARD;
      MULADD(at[15], at[31]); 
      COMBA_STORE(C->dp[30]);
      COMBA_STORE2(C->dp[31]);
      C->used = 32;
      C->sign = A->sign ^ B->sign;
      fp_clamp(C);
      COMBA_FINI;
      break;

   default:
      break;
   }

#ifdef WOLFSSL_SMALL_STACK
   XFREE(at, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}

#endif
