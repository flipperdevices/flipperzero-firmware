/* fp_mul_comba_48.i
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



#ifdef TFM_MUL48
int fp_mul_comba48(fp_int *A, fp_int *B, fp_int *C)
{
   fp_digit c0, c1, c2;
#ifndef WOLFSSL_SMALL_STACK
   fp_digit at[96];
#else
   fp_digit *at;
#endif

#ifdef WOLFSSL_SMALL_STACK
   at = (fp_digit*)XMALLOC(sizeof(fp_digit) * 96, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (at == NULL)
       return FP_MEM;
#endif

   XMEMCPY(at, A->dp, 48 * sizeof(fp_digit));
   XMEMCPY(at+48, B->dp, 48 * sizeof(fp_digit));
   COMBA_START;

   COMBA_CLEAR;
   /* 0 */
   MULADD(at[0], at[48]); 
   COMBA_STORE(C->dp[0]);
   /* 1 */
   COMBA_FORWARD;
   MULADD(at[0], at[49]);    MULADD(at[1], at[48]); 
   COMBA_STORE(C->dp[1]);
   /* 2 */
   COMBA_FORWARD;
   MULADD(at[0], at[50]);    MULADD(at[1], at[49]);    MULADD(at[2], at[48]); 
   COMBA_STORE(C->dp[2]);
   /* 3 */
   COMBA_FORWARD;
   MULADD(at[0], at[51]);    MULADD(at[1], at[50]);    MULADD(at[2], at[49]);    MULADD(at[3], at[48]); 
   COMBA_STORE(C->dp[3]);
   /* 4 */
   COMBA_FORWARD;
   MULADD(at[0], at[52]);    MULADD(at[1], at[51]);    MULADD(at[2], at[50]);    MULADD(at[3], at[49]);    MULADD(at[4], at[48]); 
   COMBA_STORE(C->dp[4]);
   /* 5 */
   COMBA_FORWARD;
   MULADD(at[0], at[53]);    MULADD(at[1], at[52]);    MULADD(at[2], at[51]);    MULADD(at[3], at[50]);    MULADD(at[4], at[49]);    MULADD(at[5], at[48]); 
   COMBA_STORE(C->dp[5]);
   /* 6 */
   COMBA_FORWARD;
   MULADD(at[0], at[54]);    MULADD(at[1], at[53]);    MULADD(at[2], at[52]);    MULADD(at[3], at[51]);    MULADD(at[4], at[50]);    MULADD(at[5], at[49]);    MULADD(at[6], at[48]); 
   COMBA_STORE(C->dp[6]);
   /* 7 */
   COMBA_FORWARD;
   MULADD(at[0], at[55]);    MULADD(at[1], at[54]);    MULADD(at[2], at[53]);    MULADD(at[3], at[52]);    MULADD(at[4], at[51]);    MULADD(at[5], at[50]);    MULADD(at[6], at[49]);    MULADD(at[7], at[48]); 
   COMBA_STORE(C->dp[7]);
   /* 8 */
   COMBA_FORWARD;
   MULADD(at[0], at[56]);    MULADD(at[1], at[55]);    MULADD(at[2], at[54]);    MULADD(at[3], at[53]);    MULADD(at[4], at[52]);    MULADD(at[5], at[51]);    MULADD(at[6], at[50]);    MULADD(at[7], at[49]);    MULADD(at[8], at[48]); 
   COMBA_STORE(C->dp[8]);
   /* 9 */
   COMBA_FORWARD;
   MULADD(at[0], at[57]);    MULADD(at[1], at[56]);    MULADD(at[2], at[55]);    MULADD(at[3], at[54]);    MULADD(at[4], at[53]);    MULADD(at[5], at[52]);    MULADD(at[6], at[51]);    MULADD(at[7], at[50]);    MULADD(at[8], at[49]);    MULADD(at[9], at[48]); 
   COMBA_STORE(C->dp[9]);
   /* 10 */
   COMBA_FORWARD;
   MULADD(at[0], at[58]);    MULADD(at[1], at[57]);    MULADD(at[2], at[56]);    MULADD(at[3], at[55]);    MULADD(at[4], at[54]);    MULADD(at[5], at[53]);    MULADD(at[6], at[52]);    MULADD(at[7], at[51]);    MULADD(at[8], at[50]);    MULADD(at[9], at[49]);    MULADD(at[10], at[48]); 
   COMBA_STORE(C->dp[10]);
   /* 11 */
   COMBA_FORWARD;
   MULADD(at[0], at[59]);    MULADD(at[1], at[58]);    MULADD(at[2], at[57]);    MULADD(at[3], at[56]);    MULADD(at[4], at[55]);    MULADD(at[5], at[54]);    MULADD(at[6], at[53]);    MULADD(at[7], at[52]);    MULADD(at[8], at[51]);    MULADD(at[9], at[50]);    MULADD(at[10], at[49]);    MULADD(at[11], at[48]); 
   COMBA_STORE(C->dp[11]);
   /* 12 */
   COMBA_FORWARD;
   MULADD(at[0], at[60]);    MULADD(at[1], at[59]);    MULADD(at[2], at[58]);    MULADD(at[3], at[57]);    MULADD(at[4], at[56]);    MULADD(at[5], at[55]);    MULADD(at[6], at[54]);    MULADD(at[7], at[53]);    MULADD(at[8], at[52]);    MULADD(at[9], at[51]);    MULADD(at[10], at[50]);    MULADD(at[11], at[49]);    MULADD(at[12], at[48]); 
   COMBA_STORE(C->dp[12]);
   /* 13 */
   COMBA_FORWARD;
   MULADD(at[0], at[61]);    MULADD(at[1], at[60]);    MULADD(at[2], at[59]);    MULADD(at[3], at[58]);    MULADD(at[4], at[57]);    MULADD(at[5], at[56]);    MULADD(at[6], at[55]);    MULADD(at[7], at[54]);    MULADD(at[8], at[53]);    MULADD(at[9], at[52]);    MULADD(at[10], at[51]);    MULADD(at[11], at[50]);    MULADD(at[12], at[49]);    MULADD(at[13], at[48]); 
   COMBA_STORE(C->dp[13]);
   /* 14 */
   COMBA_FORWARD;
   MULADD(at[0], at[62]);    MULADD(at[1], at[61]);    MULADD(at[2], at[60]);    MULADD(at[3], at[59]);    MULADD(at[4], at[58]);    MULADD(at[5], at[57]);    MULADD(at[6], at[56]);    MULADD(at[7], at[55]);    MULADD(at[8], at[54]);    MULADD(at[9], at[53]);    MULADD(at[10], at[52]);    MULADD(at[11], at[51]);    MULADD(at[12], at[50]);    MULADD(at[13], at[49]);    MULADD(at[14], at[48]); 
   COMBA_STORE(C->dp[14]);
   /* 15 */
   COMBA_FORWARD;
   MULADD(at[0], at[63]);    MULADD(at[1], at[62]);    MULADD(at[2], at[61]);    MULADD(at[3], at[60]);    MULADD(at[4], at[59]);    MULADD(at[5], at[58]);    MULADD(at[6], at[57]);    MULADD(at[7], at[56]);    MULADD(at[8], at[55]);    MULADD(at[9], at[54]);    MULADD(at[10], at[53]);    MULADD(at[11], at[52]);    MULADD(at[12], at[51]);    MULADD(at[13], at[50]);    MULADD(at[14], at[49]);    MULADD(at[15], at[48]); 
   COMBA_STORE(C->dp[15]);
   /* 16 */
   COMBA_FORWARD;
   MULADD(at[0], at[64]);    MULADD(at[1], at[63]);    MULADD(at[2], at[62]);    MULADD(at[3], at[61]);    MULADD(at[4], at[60]);    MULADD(at[5], at[59]);    MULADD(at[6], at[58]);    MULADD(at[7], at[57]);    MULADD(at[8], at[56]);    MULADD(at[9], at[55]);    MULADD(at[10], at[54]);    MULADD(at[11], at[53]);    MULADD(at[12], at[52]);    MULADD(at[13], at[51]);    MULADD(at[14], at[50]);    MULADD(at[15], at[49]);    MULADD(at[16], at[48]); 
   COMBA_STORE(C->dp[16]);
   /* 17 */
   COMBA_FORWARD;
   MULADD(at[0], at[65]);    MULADD(at[1], at[64]);    MULADD(at[2], at[63]);    MULADD(at[3], at[62]);    MULADD(at[4], at[61]);    MULADD(at[5], at[60]);    MULADD(at[6], at[59]);    MULADD(at[7], at[58]);    MULADD(at[8], at[57]);    MULADD(at[9], at[56]);    MULADD(at[10], at[55]);    MULADD(at[11], at[54]);    MULADD(at[12], at[53]);    MULADD(at[13], at[52]);    MULADD(at[14], at[51]);    MULADD(at[15], at[50]);    MULADD(at[16], at[49]);    MULADD(at[17], at[48]); 
   COMBA_STORE(C->dp[17]);
   /* 18 */
   COMBA_FORWARD;
   MULADD(at[0], at[66]);    MULADD(at[1], at[65]);    MULADD(at[2], at[64]);    MULADD(at[3], at[63]);    MULADD(at[4], at[62]);    MULADD(at[5], at[61]);    MULADD(at[6], at[60]);    MULADD(at[7], at[59]);    MULADD(at[8], at[58]);    MULADD(at[9], at[57]);    MULADD(at[10], at[56]);    MULADD(at[11], at[55]);    MULADD(at[12], at[54]);    MULADD(at[13], at[53]);    MULADD(at[14], at[52]);    MULADD(at[15], at[51]);    MULADD(at[16], at[50]);    MULADD(at[17], at[49]);    MULADD(at[18], at[48]); 
   COMBA_STORE(C->dp[18]);
   /* 19 */
   COMBA_FORWARD;
   MULADD(at[0], at[67]);    MULADD(at[1], at[66]);    MULADD(at[2], at[65]);    MULADD(at[3], at[64]);    MULADD(at[4], at[63]);    MULADD(at[5], at[62]);    MULADD(at[6], at[61]);    MULADD(at[7], at[60]);    MULADD(at[8], at[59]);    MULADD(at[9], at[58]);    MULADD(at[10], at[57]);    MULADD(at[11], at[56]);    MULADD(at[12], at[55]);    MULADD(at[13], at[54]);    MULADD(at[14], at[53]);    MULADD(at[15], at[52]);    MULADD(at[16], at[51]);    MULADD(at[17], at[50]);    MULADD(at[18], at[49]);    MULADD(at[19], at[48]); 
   COMBA_STORE(C->dp[19]);
   /* 20 */
   COMBA_FORWARD;
   MULADD(at[0], at[68]);    MULADD(at[1], at[67]);    MULADD(at[2], at[66]);    MULADD(at[3], at[65]);    MULADD(at[4], at[64]);    MULADD(at[5], at[63]);    MULADD(at[6], at[62]);    MULADD(at[7], at[61]);    MULADD(at[8], at[60]);    MULADD(at[9], at[59]);    MULADD(at[10], at[58]);    MULADD(at[11], at[57]);    MULADD(at[12], at[56]);    MULADD(at[13], at[55]);    MULADD(at[14], at[54]);    MULADD(at[15], at[53]);    MULADD(at[16], at[52]);    MULADD(at[17], at[51]);    MULADD(at[18], at[50]);    MULADD(at[19], at[49]);    MULADD(at[20], at[48]); 
   COMBA_STORE(C->dp[20]);
   /* 21 */
   COMBA_FORWARD;
   MULADD(at[0], at[69]);    MULADD(at[1], at[68]);    MULADD(at[2], at[67]);    MULADD(at[3], at[66]);    MULADD(at[4], at[65]);    MULADD(at[5], at[64]);    MULADD(at[6], at[63]);    MULADD(at[7], at[62]);    MULADD(at[8], at[61]);    MULADD(at[9], at[60]);    MULADD(at[10], at[59]);    MULADD(at[11], at[58]);    MULADD(at[12], at[57]);    MULADD(at[13], at[56]);    MULADD(at[14], at[55]);    MULADD(at[15], at[54]);    MULADD(at[16], at[53]);    MULADD(at[17], at[52]);    MULADD(at[18], at[51]);    MULADD(at[19], at[50]);    MULADD(at[20], at[49]);    MULADD(at[21], at[48]); 
   COMBA_STORE(C->dp[21]);
   /* 22 */
   COMBA_FORWARD;
   MULADD(at[0], at[70]);    MULADD(at[1], at[69]);    MULADD(at[2], at[68]);    MULADD(at[3], at[67]);    MULADD(at[4], at[66]);    MULADD(at[5], at[65]);    MULADD(at[6], at[64]);    MULADD(at[7], at[63]);    MULADD(at[8], at[62]);    MULADD(at[9], at[61]);    MULADD(at[10], at[60]);    MULADD(at[11], at[59]);    MULADD(at[12], at[58]);    MULADD(at[13], at[57]);    MULADD(at[14], at[56]);    MULADD(at[15], at[55]);    MULADD(at[16], at[54]);    MULADD(at[17], at[53]);    MULADD(at[18], at[52]);    MULADD(at[19], at[51]);    MULADD(at[20], at[50]);    MULADD(at[21], at[49]);    MULADD(at[22], at[48]); 
   COMBA_STORE(C->dp[22]);
   /* 23 */
   COMBA_FORWARD;
   MULADD(at[0], at[71]);    MULADD(at[1], at[70]);    MULADD(at[2], at[69]);    MULADD(at[3], at[68]);    MULADD(at[4], at[67]);    MULADD(at[5], at[66]);    MULADD(at[6], at[65]);    MULADD(at[7], at[64]);    MULADD(at[8], at[63]);    MULADD(at[9], at[62]);    MULADD(at[10], at[61]);    MULADD(at[11], at[60]);    MULADD(at[12], at[59]);    MULADD(at[13], at[58]);    MULADD(at[14], at[57]);    MULADD(at[15], at[56]);    MULADD(at[16], at[55]);    MULADD(at[17], at[54]);    MULADD(at[18], at[53]);    MULADD(at[19], at[52]);    MULADD(at[20], at[51]);    MULADD(at[21], at[50]);    MULADD(at[22], at[49]);    MULADD(at[23], at[48]); 
   COMBA_STORE(C->dp[23]);
   /* 24 */
   COMBA_FORWARD;
   MULADD(at[0], at[72]);    MULADD(at[1], at[71]);    MULADD(at[2], at[70]);    MULADD(at[3], at[69]);    MULADD(at[4], at[68]);    MULADD(at[5], at[67]);    MULADD(at[6], at[66]);    MULADD(at[7], at[65]);    MULADD(at[8], at[64]);    MULADD(at[9], at[63]);    MULADD(at[10], at[62]);    MULADD(at[11], at[61]);    MULADD(at[12], at[60]);    MULADD(at[13], at[59]);    MULADD(at[14], at[58]);    MULADD(at[15], at[57]);    MULADD(at[16], at[56]);    MULADD(at[17], at[55]);    MULADD(at[18], at[54]);    MULADD(at[19], at[53]);    MULADD(at[20], at[52]);    MULADD(at[21], at[51]);    MULADD(at[22], at[50]);    MULADD(at[23], at[49]);    MULADD(at[24], at[48]); 
   COMBA_STORE(C->dp[24]);
   /* 25 */
   COMBA_FORWARD;
   MULADD(at[0], at[73]);    MULADD(at[1], at[72]);    MULADD(at[2], at[71]);    MULADD(at[3], at[70]);    MULADD(at[4], at[69]);    MULADD(at[5], at[68]);    MULADD(at[6], at[67]);    MULADD(at[7], at[66]);    MULADD(at[8], at[65]);    MULADD(at[9], at[64]);    MULADD(at[10], at[63]);    MULADD(at[11], at[62]);    MULADD(at[12], at[61]);    MULADD(at[13], at[60]);    MULADD(at[14], at[59]);    MULADD(at[15], at[58]);    MULADD(at[16], at[57]);    MULADD(at[17], at[56]);    MULADD(at[18], at[55]);    MULADD(at[19], at[54]);    MULADD(at[20], at[53]);    MULADD(at[21], at[52]);    MULADD(at[22], at[51]);    MULADD(at[23], at[50]);    MULADD(at[24], at[49]);    MULADD(at[25], at[48]); 
   COMBA_STORE(C->dp[25]);
   /* 26 */
   COMBA_FORWARD;
   MULADD(at[0], at[74]);    MULADD(at[1], at[73]);    MULADD(at[2], at[72]);    MULADD(at[3], at[71]);    MULADD(at[4], at[70]);    MULADD(at[5], at[69]);    MULADD(at[6], at[68]);    MULADD(at[7], at[67]);    MULADD(at[8], at[66]);    MULADD(at[9], at[65]);    MULADD(at[10], at[64]);    MULADD(at[11], at[63]);    MULADD(at[12], at[62]);    MULADD(at[13], at[61]);    MULADD(at[14], at[60]);    MULADD(at[15], at[59]);    MULADD(at[16], at[58]);    MULADD(at[17], at[57]);    MULADD(at[18], at[56]);    MULADD(at[19], at[55]);    MULADD(at[20], at[54]);    MULADD(at[21], at[53]);    MULADD(at[22], at[52]);    MULADD(at[23], at[51]);    MULADD(at[24], at[50]);    MULADD(at[25], at[49]);    MULADD(at[26], at[48]); 
   COMBA_STORE(C->dp[26]);
   /* 27 */
   COMBA_FORWARD;
   MULADD(at[0], at[75]);    MULADD(at[1], at[74]);    MULADD(at[2], at[73]);    MULADD(at[3], at[72]);    MULADD(at[4], at[71]);    MULADD(at[5], at[70]);    MULADD(at[6], at[69]);    MULADD(at[7], at[68]);    MULADD(at[8], at[67]);    MULADD(at[9], at[66]);    MULADD(at[10], at[65]);    MULADD(at[11], at[64]);    MULADD(at[12], at[63]);    MULADD(at[13], at[62]);    MULADD(at[14], at[61]);    MULADD(at[15], at[60]);    MULADD(at[16], at[59]);    MULADD(at[17], at[58]);    MULADD(at[18], at[57]);    MULADD(at[19], at[56]);    MULADD(at[20], at[55]);    MULADD(at[21], at[54]);    MULADD(at[22], at[53]);    MULADD(at[23], at[52]);    MULADD(at[24], at[51]);    MULADD(at[25], at[50]);    MULADD(at[26], at[49]);    MULADD(at[27], at[48]); 
   COMBA_STORE(C->dp[27]);
   /* 28 */
   COMBA_FORWARD;
   MULADD(at[0], at[76]);    MULADD(at[1], at[75]);    MULADD(at[2], at[74]);    MULADD(at[3], at[73]);    MULADD(at[4], at[72]);    MULADD(at[5], at[71]);    MULADD(at[6], at[70]);    MULADD(at[7], at[69]);    MULADD(at[8], at[68]);    MULADD(at[9], at[67]);    MULADD(at[10], at[66]);    MULADD(at[11], at[65]);    MULADD(at[12], at[64]);    MULADD(at[13], at[63]);    MULADD(at[14], at[62]);    MULADD(at[15], at[61]);    MULADD(at[16], at[60]);    MULADD(at[17], at[59]);    MULADD(at[18], at[58]);    MULADD(at[19], at[57]);    MULADD(at[20], at[56]);    MULADD(at[21], at[55]);    MULADD(at[22], at[54]);    MULADD(at[23], at[53]);    MULADD(at[24], at[52]);    MULADD(at[25], at[51]);    MULADD(at[26], at[50]);    MULADD(at[27], at[49]);    MULADD(at[28], at[48]); 
   COMBA_STORE(C->dp[28]);
   /* 29 */
   COMBA_FORWARD;
   MULADD(at[0], at[77]);    MULADD(at[1], at[76]);    MULADD(at[2], at[75]);    MULADD(at[3], at[74]);    MULADD(at[4], at[73]);    MULADD(at[5], at[72]);    MULADD(at[6], at[71]);    MULADD(at[7], at[70]);    MULADD(at[8], at[69]);    MULADD(at[9], at[68]);    MULADD(at[10], at[67]);    MULADD(at[11], at[66]);    MULADD(at[12], at[65]);    MULADD(at[13], at[64]);    MULADD(at[14], at[63]);    MULADD(at[15], at[62]);    MULADD(at[16], at[61]);    MULADD(at[17], at[60]);    MULADD(at[18], at[59]);    MULADD(at[19], at[58]);    MULADD(at[20], at[57]);    MULADD(at[21], at[56]);    MULADD(at[22], at[55]);    MULADD(at[23], at[54]);    MULADD(at[24], at[53]);    MULADD(at[25], at[52]);    MULADD(at[26], at[51]);    MULADD(at[27], at[50]);    MULADD(at[28], at[49]);    MULADD(at[29], at[48]); 
   COMBA_STORE(C->dp[29]);
   /* 30 */
   COMBA_FORWARD;
   MULADD(at[0], at[78]);    MULADD(at[1], at[77]);    MULADD(at[2], at[76]);    MULADD(at[3], at[75]);    MULADD(at[4], at[74]);    MULADD(at[5], at[73]);    MULADD(at[6], at[72]);    MULADD(at[7], at[71]);    MULADD(at[8], at[70]);    MULADD(at[9], at[69]);    MULADD(at[10], at[68]);    MULADD(at[11], at[67]);    MULADD(at[12], at[66]);    MULADD(at[13], at[65]);    MULADD(at[14], at[64]);    MULADD(at[15], at[63]);    MULADD(at[16], at[62]);    MULADD(at[17], at[61]);    MULADD(at[18], at[60]);    MULADD(at[19], at[59]);    MULADD(at[20], at[58]);    MULADD(at[21], at[57]);    MULADD(at[22], at[56]);    MULADD(at[23], at[55]);    MULADD(at[24], at[54]);    MULADD(at[25], at[53]);    MULADD(at[26], at[52]);    MULADD(at[27], at[51]);    MULADD(at[28], at[50]);    MULADD(at[29], at[49]);    MULADD(at[30], at[48]); 
   COMBA_STORE(C->dp[30]);
   /* 31 */
   COMBA_FORWARD;
   MULADD(at[0], at[79]);    MULADD(at[1], at[78]);    MULADD(at[2], at[77]);    MULADD(at[3], at[76]);    MULADD(at[4], at[75]);    MULADD(at[5], at[74]);    MULADD(at[6], at[73]);    MULADD(at[7], at[72]);    MULADD(at[8], at[71]);    MULADD(at[9], at[70]);    MULADD(at[10], at[69]);    MULADD(at[11], at[68]);    MULADD(at[12], at[67]);    MULADD(at[13], at[66]);    MULADD(at[14], at[65]);    MULADD(at[15], at[64]);    MULADD(at[16], at[63]);    MULADD(at[17], at[62]);    MULADD(at[18], at[61]);    MULADD(at[19], at[60]);    MULADD(at[20], at[59]);    MULADD(at[21], at[58]);    MULADD(at[22], at[57]);    MULADD(at[23], at[56]);    MULADD(at[24], at[55]);    MULADD(at[25], at[54]);    MULADD(at[26], at[53]);    MULADD(at[27], at[52]);    MULADD(at[28], at[51]);    MULADD(at[29], at[50]);    MULADD(at[30], at[49]);    MULADD(at[31], at[48]); 
   COMBA_STORE(C->dp[31]);
   /* 32 */
   COMBA_FORWARD;
   MULADD(at[0], at[80]);    MULADD(at[1], at[79]);    MULADD(at[2], at[78]);    MULADD(at[3], at[77]);    MULADD(at[4], at[76]);    MULADD(at[5], at[75]);    MULADD(at[6], at[74]);    MULADD(at[7], at[73]);    MULADD(at[8], at[72]);    MULADD(at[9], at[71]);    MULADD(at[10], at[70]);    MULADD(at[11], at[69]);    MULADD(at[12], at[68]);    MULADD(at[13], at[67]);    MULADD(at[14], at[66]);    MULADD(at[15], at[65]);    MULADD(at[16], at[64]);    MULADD(at[17], at[63]);    MULADD(at[18], at[62]);    MULADD(at[19], at[61]);    MULADD(at[20], at[60]);    MULADD(at[21], at[59]);    MULADD(at[22], at[58]);    MULADD(at[23], at[57]);    MULADD(at[24], at[56]);    MULADD(at[25], at[55]);    MULADD(at[26], at[54]);    MULADD(at[27], at[53]);    MULADD(at[28], at[52]);    MULADD(at[29], at[51]);    MULADD(at[30], at[50]);    MULADD(at[31], at[49]);    MULADD(at[32], at[48]); 
   COMBA_STORE(C->dp[32]);
   /* 33 */
   COMBA_FORWARD;
   MULADD(at[0], at[81]);    MULADD(at[1], at[80]);    MULADD(at[2], at[79]);    MULADD(at[3], at[78]);    MULADD(at[4], at[77]);    MULADD(at[5], at[76]);    MULADD(at[6], at[75]);    MULADD(at[7], at[74]);    MULADD(at[8], at[73]);    MULADD(at[9], at[72]);    MULADD(at[10], at[71]);    MULADD(at[11], at[70]);    MULADD(at[12], at[69]);    MULADD(at[13], at[68]);    MULADD(at[14], at[67]);    MULADD(at[15], at[66]);    MULADD(at[16], at[65]);    MULADD(at[17], at[64]);    MULADD(at[18], at[63]);    MULADD(at[19], at[62]);    MULADD(at[20], at[61]);    MULADD(at[21], at[60]);    MULADD(at[22], at[59]);    MULADD(at[23], at[58]);    MULADD(at[24], at[57]);    MULADD(at[25], at[56]);    MULADD(at[26], at[55]);    MULADD(at[27], at[54]);    MULADD(at[28], at[53]);    MULADD(at[29], at[52]);    MULADD(at[30], at[51]);    MULADD(at[31], at[50]);    MULADD(at[32], at[49]);    MULADD(at[33], at[48]); 
   COMBA_STORE(C->dp[33]);
   /* 34 */
   COMBA_FORWARD;
   MULADD(at[0], at[82]);    MULADD(at[1], at[81]);    MULADD(at[2], at[80]);    MULADD(at[3], at[79]);    MULADD(at[4], at[78]);    MULADD(at[5], at[77]);    MULADD(at[6], at[76]);    MULADD(at[7], at[75]);    MULADD(at[8], at[74]);    MULADD(at[9], at[73]);    MULADD(at[10], at[72]);    MULADD(at[11], at[71]);    MULADD(at[12], at[70]);    MULADD(at[13], at[69]);    MULADD(at[14], at[68]);    MULADD(at[15], at[67]);    MULADD(at[16], at[66]);    MULADD(at[17], at[65]);    MULADD(at[18], at[64]);    MULADD(at[19], at[63]);    MULADD(at[20], at[62]);    MULADD(at[21], at[61]);    MULADD(at[22], at[60]);    MULADD(at[23], at[59]);    MULADD(at[24], at[58]);    MULADD(at[25], at[57]);    MULADD(at[26], at[56]);    MULADD(at[27], at[55]);    MULADD(at[28], at[54]);    MULADD(at[29], at[53]);    MULADD(at[30], at[52]);    MULADD(at[31], at[51]);    MULADD(at[32], at[50]);    MULADD(at[33], at[49]);    MULADD(at[34], at[48]); 
   COMBA_STORE(C->dp[34]);
   /* 35 */
   COMBA_FORWARD;
   MULADD(at[0], at[83]);    MULADD(at[1], at[82]);    MULADD(at[2], at[81]);    MULADD(at[3], at[80]);    MULADD(at[4], at[79]);    MULADD(at[5], at[78]);    MULADD(at[6], at[77]);    MULADD(at[7], at[76]);    MULADD(at[8], at[75]);    MULADD(at[9], at[74]);    MULADD(at[10], at[73]);    MULADD(at[11], at[72]);    MULADD(at[12], at[71]);    MULADD(at[13], at[70]);    MULADD(at[14], at[69]);    MULADD(at[15], at[68]);    MULADD(at[16], at[67]);    MULADD(at[17], at[66]);    MULADD(at[18], at[65]);    MULADD(at[19], at[64]);    MULADD(at[20], at[63]);    MULADD(at[21], at[62]);    MULADD(at[22], at[61]);    MULADD(at[23], at[60]);    MULADD(at[24], at[59]);    MULADD(at[25], at[58]);    MULADD(at[26], at[57]);    MULADD(at[27], at[56]);    MULADD(at[28], at[55]);    MULADD(at[29], at[54]);    MULADD(at[30], at[53]);    MULADD(at[31], at[52]);    MULADD(at[32], at[51]);    MULADD(at[33], at[50]);    MULADD(at[34], at[49]);    MULADD(at[35], at[48]); 
   COMBA_STORE(C->dp[35]);
   /* 36 */
   COMBA_FORWARD;
   MULADD(at[0], at[84]);    MULADD(at[1], at[83]);    MULADD(at[2], at[82]);    MULADD(at[3], at[81]);    MULADD(at[4], at[80]);    MULADD(at[5], at[79]);    MULADD(at[6], at[78]);    MULADD(at[7], at[77]);    MULADD(at[8], at[76]);    MULADD(at[9], at[75]);    MULADD(at[10], at[74]);    MULADD(at[11], at[73]);    MULADD(at[12], at[72]);    MULADD(at[13], at[71]);    MULADD(at[14], at[70]);    MULADD(at[15], at[69]);    MULADD(at[16], at[68]);    MULADD(at[17], at[67]);    MULADD(at[18], at[66]);    MULADD(at[19], at[65]);    MULADD(at[20], at[64]);    MULADD(at[21], at[63]);    MULADD(at[22], at[62]);    MULADD(at[23], at[61]);    MULADD(at[24], at[60]);    MULADD(at[25], at[59]);    MULADD(at[26], at[58]);    MULADD(at[27], at[57]);    MULADD(at[28], at[56]);    MULADD(at[29], at[55]);    MULADD(at[30], at[54]);    MULADD(at[31], at[53]);    MULADD(at[32], at[52]);    MULADD(at[33], at[51]);    MULADD(at[34], at[50]);    MULADD(at[35], at[49]);    MULADD(at[36], at[48]); 
   COMBA_STORE(C->dp[36]);
   /* 37 */
   COMBA_FORWARD;
   MULADD(at[0], at[85]);    MULADD(at[1], at[84]);    MULADD(at[2], at[83]);    MULADD(at[3], at[82]);    MULADD(at[4], at[81]);    MULADD(at[5], at[80]);    MULADD(at[6], at[79]);    MULADD(at[7], at[78]);    MULADD(at[8], at[77]);    MULADD(at[9], at[76]);    MULADD(at[10], at[75]);    MULADD(at[11], at[74]);    MULADD(at[12], at[73]);    MULADD(at[13], at[72]);    MULADD(at[14], at[71]);    MULADD(at[15], at[70]);    MULADD(at[16], at[69]);    MULADD(at[17], at[68]);    MULADD(at[18], at[67]);    MULADD(at[19], at[66]);    MULADD(at[20], at[65]);    MULADD(at[21], at[64]);    MULADD(at[22], at[63]);    MULADD(at[23], at[62]);    MULADD(at[24], at[61]);    MULADD(at[25], at[60]);    MULADD(at[26], at[59]);    MULADD(at[27], at[58]);    MULADD(at[28], at[57]);    MULADD(at[29], at[56]);    MULADD(at[30], at[55]);    MULADD(at[31], at[54]);    MULADD(at[32], at[53]);    MULADD(at[33], at[52]);    MULADD(at[34], at[51]);    MULADD(at[35], at[50]);    MULADD(at[36], at[49]);    MULADD(at[37], at[48]); 
   COMBA_STORE(C->dp[37]);
   /* 38 */
   COMBA_FORWARD;
   MULADD(at[0], at[86]);    MULADD(at[1], at[85]);    MULADD(at[2], at[84]);    MULADD(at[3], at[83]);    MULADD(at[4], at[82]);    MULADD(at[5], at[81]);    MULADD(at[6], at[80]);    MULADD(at[7], at[79]);    MULADD(at[8], at[78]);    MULADD(at[9], at[77]);    MULADD(at[10], at[76]);    MULADD(at[11], at[75]);    MULADD(at[12], at[74]);    MULADD(at[13], at[73]);    MULADD(at[14], at[72]);    MULADD(at[15], at[71]);    MULADD(at[16], at[70]);    MULADD(at[17], at[69]);    MULADD(at[18], at[68]);    MULADD(at[19], at[67]);    MULADD(at[20], at[66]);    MULADD(at[21], at[65]);    MULADD(at[22], at[64]);    MULADD(at[23], at[63]);    MULADD(at[24], at[62]);    MULADD(at[25], at[61]);    MULADD(at[26], at[60]);    MULADD(at[27], at[59]);    MULADD(at[28], at[58]);    MULADD(at[29], at[57]);    MULADD(at[30], at[56]);    MULADD(at[31], at[55]);    MULADD(at[32], at[54]);    MULADD(at[33], at[53]);    MULADD(at[34], at[52]);    MULADD(at[35], at[51]);    MULADD(at[36], at[50]);    MULADD(at[37], at[49]);    MULADD(at[38], at[48]); 
   COMBA_STORE(C->dp[38]);
   /* 39 */
   COMBA_FORWARD;
   MULADD(at[0], at[87]);    MULADD(at[1], at[86]);    MULADD(at[2], at[85]);    MULADD(at[3], at[84]);    MULADD(at[4], at[83]);    MULADD(at[5], at[82]);    MULADD(at[6], at[81]);    MULADD(at[7], at[80]);    MULADD(at[8], at[79]);    MULADD(at[9], at[78]);    MULADD(at[10], at[77]);    MULADD(at[11], at[76]);    MULADD(at[12], at[75]);    MULADD(at[13], at[74]);    MULADD(at[14], at[73]);    MULADD(at[15], at[72]);    MULADD(at[16], at[71]);    MULADD(at[17], at[70]);    MULADD(at[18], at[69]);    MULADD(at[19], at[68]);    MULADD(at[20], at[67]);    MULADD(at[21], at[66]);    MULADD(at[22], at[65]);    MULADD(at[23], at[64]);    MULADD(at[24], at[63]);    MULADD(at[25], at[62]);    MULADD(at[26], at[61]);    MULADD(at[27], at[60]);    MULADD(at[28], at[59]);    MULADD(at[29], at[58]);    MULADD(at[30], at[57]);    MULADD(at[31], at[56]);    MULADD(at[32], at[55]);    MULADD(at[33], at[54]);    MULADD(at[34], at[53]);    MULADD(at[35], at[52]);    MULADD(at[36], at[51]);    MULADD(at[37], at[50]);    MULADD(at[38], at[49]);    MULADD(at[39], at[48]); 
   COMBA_STORE(C->dp[39]);
   /* 40 */
   COMBA_FORWARD;
   MULADD(at[0], at[88]);    MULADD(at[1], at[87]);    MULADD(at[2], at[86]);    MULADD(at[3], at[85]);    MULADD(at[4], at[84]);    MULADD(at[5], at[83]);    MULADD(at[6], at[82]);    MULADD(at[7], at[81]);    MULADD(at[8], at[80]);    MULADD(at[9], at[79]);    MULADD(at[10], at[78]);    MULADD(at[11], at[77]);    MULADD(at[12], at[76]);    MULADD(at[13], at[75]);    MULADD(at[14], at[74]);    MULADD(at[15], at[73]);    MULADD(at[16], at[72]);    MULADD(at[17], at[71]);    MULADD(at[18], at[70]);    MULADD(at[19], at[69]);    MULADD(at[20], at[68]);    MULADD(at[21], at[67]);    MULADD(at[22], at[66]);    MULADD(at[23], at[65]);    MULADD(at[24], at[64]);    MULADD(at[25], at[63]);    MULADD(at[26], at[62]);    MULADD(at[27], at[61]);    MULADD(at[28], at[60]);    MULADD(at[29], at[59]);    MULADD(at[30], at[58]);    MULADD(at[31], at[57]);    MULADD(at[32], at[56]);    MULADD(at[33], at[55]);    MULADD(at[34], at[54]);    MULADD(at[35], at[53]);    MULADD(at[36], at[52]);    MULADD(at[37], at[51]);    MULADD(at[38], at[50]);    MULADD(at[39], at[49]);    MULADD(at[40], at[48]); 
   COMBA_STORE(C->dp[40]);
   /* 41 */
   COMBA_FORWARD;
   MULADD(at[0], at[89]);    MULADD(at[1], at[88]);    MULADD(at[2], at[87]);    MULADD(at[3], at[86]);    MULADD(at[4], at[85]);    MULADD(at[5], at[84]);    MULADD(at[6], at[83]);    MULADD(at[7], at[82]);    MULADD(at[8], at[81]);    MULADD(at[9], at[80]);    MULADD(at[10], at[79]);    MULADD(at[11], at[78]);    MULADD(at[12], at[77]);    MULADD(at[13], at[76]);    MULADD(at[14], at[75]);    MULADD(at[15], at[74]);    MULADD(at[16], at[73]);    MULADD(at[17], at[72]);    MULADD(at[18], at[71]);    MULADD(at[19], at[70]);    MULADD(at[20], at[69]);    MULADD(at[21], at[68]);    MULADD(at[22], at[67]);    MULADD(at[23], at[66]);    MULADD(at[24], at[65]);    MULADD(at[25], at[64]);    MULADD(at[26], at[63]);    MULADD(at[27], at[62]);    MULADD(at[28], at[61]);    MULADD(at[29], at[60]);    MULADD(at[30], at[59]);    MULADD(at[31], at[58]);    MULADD(at[32], at[57]);    MULADD(at[33], at[56]);    MULADD(at[34], at[55]);    MULADD(at[35], at[54]);    MULADD(at[36], at[53]);    MULADD(at[37], at[52]);    MULADD(at[38], at[51]);    MULADD(at[39], at[50]);    MULADD(at[40], at[49]);    MULADD(at[41], at[48]); 
   COMBA_STORE(C->dp[41]);
   /* 42 */
   COMBA_FORWARD;
   MULADD(at[0], at[90]);    MULADD(at[1], at[89]);    MULADD(at[2], at[88]);    MULADD(at[3], at[87]);    MULADD(at[4], at[86]);    MULADD(at[5], at[85]);    MULADD(at[6], at[84]);    MULADD(at[7], at[83]);    MULADD(at[8], at[82]);    MULADD(at[9], at[81]);    MULADD(at[10], at[80]);    MULADD(at[11], at[79]);    MULADD(at[12], at[78]);    MULADD(at[13], at[77]);    MULADD(at[14], at[76]);    MULADD(at[15], at[75]);    MULADD(at[16], at[74]);    MULADD(at[17], at[73]);    MULADD(at[18], at[72]);    MULADD(at[19], at[71]);    MULADD(at[20], at[70]);    MULADD(at[21], at[69]);    MULADD(at[22], at[68]);    MULADD(at[23], at[67]);    MULADD(at[24], at[66]);    MULADD(at[25], at[65]);    MULADD(at[26], at[64]);    MULADD(at[27], at[63]);    MULADD(at[28], at[62]);    MULADD(at[29], at[61]);    MULADD(at[30], at[60]);    MULADD(at[31], at[59]);    MULADD(at[32], at[58]);    MULADD(at[33], at[57]);    MULADD(at[34], at[56]);    MULADD(at[35], at[55]);    MULADD(at[36], at[54]);    MULADD(at[37], at[53]);    MULADD(at[38], at[52]);    MULADD(at[39], at[51]);    MULADD(at[40], at[50]);    MULADD(at[41], at[49]);    MULADD(at[42], at[48]); 
   COMBA_STORE(C->dp[42]);
   /* 43 */
   COMBA_FORWARD;
   MULADD(at[0], at[91]);    MULADD(at[1], at[90]);    MULADD(at[2], at[89]);    MULADD(at[3], at[88]);    MULADD(at[4], at[87]);    MULADD(at[5], at[86]);    MULADD(at[6], at[85]);    MULADD(at[7], at[84]);    MULADD(at[8], at[83]);    MULADD(at[9], at[82]);    MULADD(at[10], at[81]);    MULADD(at[11], at[80]);    MULADD(at[12], at[79]);    MULADD(at[13], at[78]);    MULADD(at[14], at[77]);    MULADD(at[15], at[76]);    MULADD(at[16], at[75]);    MULADD(at[17], at[74]);    MULADD(at[18], at[73]);    MULADD(at[19], at[72]);    MULADD(at[20], at[71]);    MULADD(at[21], at[70]);    MULADD(at[22], at[69]);    MULADD(at[23], at[68]);    MULADD(at[24], at[67]);    MULADD(at[25], at[66]);    MULADD(at[26], at[65]);    MULADD(at[27], at[64]);    MULADD(at[28], at[63]);    MULADD(at[29], at[62]);    MULADD(at[30], at[61]);    MULADD(at[31], at[60]);    MULADD(at[32], at[59]);    MULADD(at[33], at[58]);    MULADD(at[34], at[57]);    MULADD(at[35], at[56]);    MULADD(at[36], at[55]);    MULADD(at[37], at[54]);    MULADD(at[38], at[53]);    MULADD(at[39], at[52]);    MULADD(at[40], at[51]);    MULADD(at[41], at[50]);    MULADD(at[42], at[49]);    MULADD(at[43], at[48]); 
   COMBA_STORE(C->dp[43]);
   /* 44 */
   COMBA_FORWARD;
   MULADD(at[0], at[92]);    MULADD(at[1], at[91]);    MULADD(at[2], at[90]);    MULADD(at[3], at[89]);    MULADD(at[4], at[88]);    MULADD(at[5], at[87]);    MULADD(at[6], at[86]);    MULADD(at[7], at[85]);    MULADD(at[8], at[84]);    MULADD(at[9], at[83]);    MULADD(at[10], at[82]);    MULADD(at[11], at[81]);    MULADD(at[12], at[80]);    MULADD(at[13], at[79]);    MULADD(at[14], at[78]);    MULADD(at[15], at[77]);    MULADD(at[16], at[76]);    MULADD(at[17], at[75]);    MULADD(at[18], at[74]);    MULADD(at[19], at[73]);    MULADD(at[20], at[72]);    MULADD(at[21], at[71]);    MULADD(at[22], at[70]);    MULADD(at[23], at[69]);    MULADD(at[24], at[68]);    MULADD(at[25], at[67]);    MULADD(at[26], at[66]);    MULADD(at[27], at[65]);    MULADD(at[28], at[64]);    MULADD(at[29], at[63]);    MULADD(at[30], at[62]);    MULADD(at[31], at[61]);    MULADD(at[32], at[60]);    MULADD(at[33], at[59]);    MULADD(at[34], at[58]);    MULADD(at[35], at[57]);    MULADD(at[36], at[56]);    MULADD(at[37], at[55]);    MULADD(at[38], at[54]);    MULADD(at[39], at[53]);    MULADD(at[40], at[52]);    MULADD(at[41], at[51]);    MULADD(at[42], at[50]);    MULADD(at[43], at[49]);    MULADD(at[44], at[48]); 
   COMBA_STORE(C->dp[44]);
   /* 45 */
   COMBA_FORWARD;
   MULADD(at[0], at[93]);    MULADD(at[1], at[92]);    MULADD(at[2], at[91]);    MULADD(at[3], at[90]);    MULADD(at[4], at[89]);    MULADD(at[5], at[88]);    MULADD(at[6], at[87]);    MULADD(at[7], at[86]);    MULADD(at[8], at[85]);    MULADD(at[9], at[84]);    MULADD(at[10], at[83]);    MULADD(at[11], at[82]);    MULADD(at[12], at[81]);    MULADD(at[13], at[80]);    MULADD(at[14], at[79]);    MULADD(at[15], at[78]);    MULADD(at[16], at[77]);    MULADD(at[17], at[76]);    MULADD(at[18], at[75]);    MULADD(at[19], at[74]);    MULADD(at[20], at[73]);    MULADD(at[21], at[72]);    MULADD(at[22], at[71]);    MULADD(at[23], at[70]);    MULADD(at[24], at[69]);    MULADD(at[25], at[68]);    MULADD(at[26], at[67]);    MULADD(at[27], at[66]);    MULADD(at[28], at[65]);    MULADD(at[29], at[64]);    MULADD(at[30], at[63]);    MULADD(at[31], at[62]);    MULADD(at[32], at[61]);    MULADD(at[33], at[60]);    MULADD(at[34], at[59]);    MULADD(at[35], at[58]);    MULADD(at[36], at[57]);    MULADD(at[37], at[56]);    MULADD(at[38], at[55]);    MULADD(at[39], at[54]);    MULADD(at[40], at[53]);    MULADD(at[41], at[52]);    MULADD(at[42], at[51]);    MULADD(at[43], at[50]);    MULADD(at[44], at[49]);    MULADD(at[45], at[48]); 
   COMBA_STORE(C->dp[45]);
   /* 46 */
   COMBA_FORWARD;
   MULADD(at[0], at[94]);    MULADD(at[1], at[93]);    MULADD(at[2], at[92]);    MULADD(at[3], at[91]);    MULADD(at[4], at[90]);    MULADD(at[5], at[89]);    MULADD(at[6], at[88]);    MULADD(at[7], at[87]);    MULADD(at[8], at[86]);    MULADD(at[9], at[85]);    MULADD(at[10], at[84]);    MULADD(at[11], at[83]);    MULADD(at[12], at[82]);    MULADD(at[13], at[81]);    MULADD(at[14], at[80]);    MULADD(at[15], at[79]);    MULADD(at[16], at[78]);    MULADD(at[17], at[77]);    MULADD(at[18], at[76]);    MULADD(at[19], at[75]);    MULADD(at[20], at[74]);    MULADD(at[21], at[73]);    MULADD(at[22], at[72]);    MULADD(at[23], at[71]);    MULADD(at[24], at[70]);    MULADD(at[25], at[69]);    MULADD(at[26], at[68]);    MULADD(at[27], at[67]);    MULADD(at[28], at[66]);    MULADD(at[29], at[65]);    MULADD(at[30], at[64]);    MULADD(at[31], at[63]);    MULADD(at[32], at[62]);    MULADD(at[33], at[61]);    MULADD(at[34], at[60]);    MULADD(at[35], at[59]);    MULADD(at[36], at[58]);    MULADD(at[37], at[57]);    MULADD(at[38], at[56]);    MULADD(at[39], at[55]);    MULADD(at[40], at[54]);    MULADD(at[41], at[53]);    MULADD(at[42], at[52]);    MULADD(at[43], at[51]);    MULADD(at[44], at[50]);    MULADD(at[45], at[49]);    MULADD(at[46], at[48]); 
   COMBA_STORE(C->dp[46]);
   /* 47 */
   COMBA_FORWARD;
   MULADD(at[0], at[95]);    MULADD(at[1], at[94]);    MULADD(at[2], at[93]);    MULADD(at[3], at[92]);    MULADD(at[4], at[91]);    MULADD(at[5], at[90]);    MULADD(at[6], at[89]);    MULADD(at[7], at[88]);    MULADD(at[8], at[87]);    MULADD(at[9], at[86]);    MULADD(at[10], at[85]);    MULADD(at[11], at[84]);    MULADD(at[12], at[83]);    MULADD(at[13], at[82]);    MULADD(at[14], at[81]);    MULADD(at[15], at[80]);    MULADD(at[16], at[79]);    MULADD(at[17], at[78]);    MULADD(at[18], at[77]);    MULADD(at[19], at[76]);    MULADD(at[20], at[75]);    MULADD(at[21], at[74]);    MULADD(at[22], at[73]);    MULADD(at[23], at[72]);    MULADD(at[24], at[71]);    MULADD(at[25], at[70]);    MULADD(at[26], at[69]);    MULADD(at[27], at[68]);    MULADD(at[28], at[67]);    MULADD(at[29], at[66]);    MULADD(at[30], at[65]);    MULADD(at[31], at[64]);    MULADD(at[32], at[63]);    MULADD(at[33], at[62]);    MULADD(at[34], at[61]);    MULADD(at[35], at[60]);    MULADD(at[36], at[59]);    MULADD(at[37], at[58]);    MULADD(at[38], at[57]);    MULADD(at[39], at[56]);    MULADD(at[40], at[55]);    MULADD(at[41], at[54]);    MULADD(at[42], at[53]);    MULADD(at[43], at[52]);    MULADD(at[44], at[51]);    MULADD(at[45], at[50]);    MULADD(at[46], at[49]);    MULADD(at[47], at[48]); 
   COMBA_STORE(C->dp[47]);
   /* 48 */
   COMBA_FORWARD;
   MULADD(at[1], at[95]);    MULADD(at[2], at[94]);    MULADD(at[3], at[93]);    MULADD(at[4], at[92]);    MULADD(at[5], at[91]);    MULADD(at[6], at[90]);    MULADD(at[7], at[89]);    MULADD(at[8], at[88]);    MULADD(at[9], at[87]);    MULADD(at[10], at[86]);    MULADD(at[11], at[85]);    MULADD(at[12], at[84]);    MULADD(at[13], at[83]);    MULADD(at[14], at[82]);    MULADD(at[15], at[81]);    MULADD(at[16], at[80]);    MULADD(at[17], at[79]);    MULADD(at[18], at[78]);    MULADD(at[19], at[77]);    MULADD(at[20], at[76]);    MULADD(at[21], at[75]);    MULADD(at[22], at[74]);    MULADD(at[23], at[73]);    MULADD(at[24], at[72]);    MULADD(at[25], at[71]);    MULADD(at[26], at[70]);    MULADD(at[27], at[69]);    MULADD(at[28], at[68]);    MULADD(at[29], at[67]);    MULADD(at[30], at[66]);    MULADD(at[31], at[65]);    MULADD(at[32], at[64]);    MULADD(at[33], at[63]);    MULADD(at[34], at[62]);    MULADD(at[35], at[61]);    MULADD(at[36], at[60]);    MULADD(at[37], at[59]);    MULADD(at[38], at[58]);    MULADD(at[39], at[57]);    MULADD(at[40], at[56]);    MULADD(at[41], at[55]);    MULADD(at[42], at[54]);    MULADD(at[43], at[53]);    MULADD(at[44], at[52]);    MULADD(at[45], at[51]);    MULADD(at[46], at[50]);    MULADD(at[47], at[49]); 
   COMBA_STORE(C->dp[48]);
   /* 49 */
   COMBA_FORWARD;
   MULADD(at[2], at[95]);    MULADD(at[3], at[94]);    MULADD(at[4], at[93]);    MULADD(at[5], at[92]);    MULADD(at[6], at[91]);    MULADD(at[7], at[90]);    MULADD(at[8], at[89]);    MULADD(at[9], at[88]);    MULADD(at[10], at[87]);    MULADD(at[11], at[86]);    MULADD(at[12], at[85]);    MULADD(at[13], at[84]);    MULADD(at[14], at[83]);    MULADD(at[15], at[82]);    MULADD(at[16], at[81]);    MULADD(at[17], at[80]);    MULADD(at[18], at[79]);    MULADD(at[19], at[78]);    MULADD(at[20], at[77]);    MULADD(at[21], at[76]);    MULADD(at[22], at[75]);    MULADD(at[23], at[74]);    MULADD(at[24], at[73]);    MULADD(at[25], at[72]);    MULADD(at[26], at[71]);    MULADD(at[27], at[70]);    MULADD(at[28], at[69]);    MULADD(at[29], at[68]);    MULADD(at[30], at[67]);    MULADD(at[31], at[66]);    MULADD(at[32], at[65]);    MULADD(at[33], at[64]);    MULADD(at[34], at[63]);    MULADD(at[35], at[62]);    MULADD(at[36], at[61]);    MULADD(at[37], at[60]);    MULADD(at[38], at[59]);    MULADD(at[39], at[58]);    MULADD(at[40], at[57]);    MULADD(at[41], at[56]);    MULADD(at[42], at[55]);    MULADD(at[43], at[54]);    MULADD(at[44], at[53]);    MULADD(at[45], at[52]);    MULADD(at[46], at[51]);    MULADD(at[47], at[50]); 
   COMBA_STORE(C->dp[49]);
   /* 50 */
   COMBA_FORWARD;
   MULADD(at[3], at[95]);    MULADD(at[4], at[94]);    MULADD(at[5], at[93]);    MULADD(at[6], at[92]);    MULADD(at[7], at[91]);    MULADD(at[8], at[90]);    MULADD(at[9], at[89]);    MULADD(at[10], at[88]);    MULADD(at[11], at[87]);    MULADD(at[12], at[86]);    MULADD(at[13], at[85]);    MULADD(at[14], at[84]);    MULADD(at[15], at[83]);    MULADD(at[16], at[82]);    MULADD(at[17], at[81]);    MULADD(at[18], at[80]);    MULADD(at[19], at[79]);    MULADD(at[20], at[78]);    MULADD(at[21], at[77]);    MULADD(at[22], at[76]);    MULADD(at[23], at[75]);    MULADD(at[24], at[74]);    MULADD(at[25], at[73]);    MULADD(at[26], at[72]);    MULADD(at[27], at[71]);    MULADD(at[28], at[70]);    MULADD(at[29], at[69]);    MULADD(at[30], at[68]);    MULADD(at[31], at[67]);    MULADD(at[32], at[66]);    MULADD(at[33], at[65]);    MULADD(at[34], at[64]);    MULADD(at[35], at[63]);    MULADD(at[36], at[62]);    MULADD(at[37], at[61]);    MULADD(at[38], at[60]);    MULADD(at[39], at[59]);    MULADD(at[40], at[58]);    MULADD(at[41], at[57]);    MULADD(at[42], at[56]);    MULADD(at[43], at[55]);    MULADD(at[44], at[54]);    MULADD(at[45], at[53]);    MULADD(at[46], at[52]);    MULADD(at[47], at[51]); 
   COMBA_STORE(C->dp[50]);
   /* 51 */
   COMBA_FORWARD;
   MULADD(at[4], at[95]);    MULADD(at[5], at[94]);    MULADD(at[6], at[93]);    MULADD(at[7], at[92]);    MULADD(at[8], at[91]);    MULADD(at[9], at[90]);    MULADD(at[10], at[89]);    MULADD(at[11], at[88]);    MULADD(at[12], at[87]);    MULADD(at[13], at[86]);    MULADD(at[14], at[85]);    MULADD(at[15], at[84]);    MULADD(at[16], at[83]);    MULADD(at[17], at[82]);    MULADD(at[18], at[81]);    MULADD(at[19], at[80]);    MULADD(at[20], at[79]);    MULADD(at[21], at[78]);    MULADD(at[22], at[77]);    MULADD(at[23], at[76]);    MULADD(at[24], at[75]);    MULADD(at[25], at[74]);    MULADD(at[26], at[73]);    MULADD(at[27], at[72]);    MULADD(at[28], at[71]);    MULADD(at[29], at[70]);    MULADD(at[30], at[69]);    MULADD(at[31], at[68]);    MULADD(at[32], at[67]);    MULADD(at[33], at[66]);    MULADD(at[34], at[65]);    MULADD(at[35], at[64]);    MULADD(at[36], at[63]);    MULADD(at[37], at[62]);    MULADD(at[38], at[61]);    MULADD(at[39], at[60]);    MULADD(at[40], at[59]);    MULADD(at[41], at[58]);    MULADD(at[42], at[57]);    MULADD(at[43], at[56]);    MULADD(at[44], at[55]);    MULADD(at[45], at[54]);    MULADD(at[46], at[53]);    MULADD(at[47], at[52]); 
   COMBA_STORE(C->dp[51]);
   /* 52 */
   COMBA_FORWARD;
   MULADD(at[5], at[95]);    MULADD(at[6], at[94]);    MULADD(at[7], at[93]);    MULADD(at[8], at[92]);    MULADD(at[9], at[91]);    MULADD(at[10], at[90]);    MULADD(at[11], at[89]);    MULADD(at[12], at[88]);    MULADD(at[13], at[87]);    MULADD(at[14], at[86]);    MULADD(at[15], at[85]);    MULADD(at[16], at[84]);    MULADD(at[17], at[83]);    MULADD(at[18], at[82]);    MULADD(at[19], at[81]);    MULADD(at[20], at[80]);    MULADD(at[21], at[79]);    MULADD(at[22], at[78]);    MULADD(at[23], at[77]);    MULADD(at[24], at[76]);    MULADD(at[25], at[75]);    MULADD(at[26], at[74]);    MULADD(at[27], at[73]);    MULADD(at[28], at[72]);    MULADD(at[29], at[71]);    MULADD(at[30], at[70]);    MULADD(at[31], at[69]);    MULADD(at[32], at[68]);    MULADD(at[33], at[67]);    MULADD(at[34], at[66]);    MULADD(at[35], at[65]);    MULADD(at[36], at[64]);    MULADD(at[37], at[63]);    MULADD(at[38], at[62]);    MULADD(at[39], at[61]);    MULADD(at[40], at[60]);    MULADD(at[41], at[59]);    MULADD(at[42], at[58]);    MULADD(at[43], at[57]);    MULADD(at[44], at[56]);    MULADD(at[45], at[55]);    MULADD(at[46], at[54]);    MULADD(at[47], at[53]); 
   COMBA_STORE(C->dp[52]);
   /* 53 */
   COMBA_FORWARD;
   MULADD(at[6], at[95]);    MULADD(at[7], at[94]);    MULADD(at[8], at[93]);    MULADD(at[9], at[92]);    MULADD(at[10], at[91]);    MULADD(at[11], at[90]);    MULADD(at[12], at[89]);    MULADD(at[13], at[88]);    MULADD(at[14], at[87]);    MULADD(at[15], at[86]);    MULADD(at[16], at[85]);    MULADD(at[17], at[84]);    MULADD(at[18], at[83]);    MULADD(at[19], at[82]);    MULADD(at[20], at[81]);    MULADD(at[21], at[80]);    MULADD(at[22], at[79]);    MULADD(at[23], at[78]);    MULADD(at[24], at[77]);    MULADD(at[25], at[76]);    MULADD(at[26], at[75]);    MULADD(at[27], at[74]);    MULADD(at[28], at[73]);    MULADD(at[29], at[72]);    MULADD(at[30], at[71]);    MULADD(at[31], at[70]);    MULADD(at[32], at[69]);    MULADD(at[33], at[68]);    MULADD(at[34], at[67]);    MULADD(at[35], at[66]);    MULADD(at[36], at[65]);    MULADD(at[37], at[64]);    MULADD(at[38], at[63]);    MULADD(at[39], at[62]);    MULADD(at[40], at[61]);    MULADD(at[41], at[60]);    MULADD(at[42], at[59]);    MULADD(at[43], at[58]);    MULADD(at[44], at[57]);    MULADD(at[45], at[56]);    MULADD(at[46], at[55]);    MULADD(at[47], at[54]); 
   COMBA_STORE(C->dp[53]);
   /* 54 */
   COMBA_FORWARD;
   MULADD(at[7], at[95]);    MULADD(at[8], at[94]);    MULADD(at[9], at[93]);    MULADD(at[10], at[92]);    MULADD(at[11], at[91]);    MULADD(at[12], at[90]);    MULADD(at[13], at[89]);    MULADD(at[14], at[88]);    MULADD(at[15], at[87]);    MULADD(at[16], at[86]);    MULADD(at[17], at[85]);    MULADD(at[18], at[84]);    MULADD(at[19], at[83]);    MULADD(at[20], at[82]);    MULADD(at[21], at[81]);    MULADD(at[22], at[80]);    MULADD(at[23], at[79]);    MULADD(at[24], at[78]);    MULADD(at[25], at[77]);    MULADD(at[26], at[76]);    MULADD(at[27], at[75]);    MULADD(at[28], at[74]);    MULADD(at[29], at[73]);    MULADD(at[30], at[72]);    MULADD(at[31], at[71]);    MULADD(at[32], at[70]);    MULADD(at[33], at[69]);    MULADD(at[34], at[68]);    MULADD(at[35], at[67]);    MULADD(at[36], at[66]);    MULADD(at[37], at[65]);    MULADD(at[38], at[64]);    MULADD(at[39], at[63]);    MULADD(at[40], at[62]);    MULADD(at[41], at[61]);    MULADD(at[42], at[60]);    MULADD(at[43], at[59]);    MULADD(at[44], at[58]);    MULADD(at[45], at[57]);    MULADD(at[46], at[56]);    MULADD(at[47], at[55]); 
   COMBA_STORE(C->dp[54]);
   /* 55 */
   COMBA_FORWARD;
   MULADD(at[8], at[95]);    MULADD(at[9], at[94]);    MULADD(at[10], at[93]);    MULADD(at[11], at[92]);    MULADD(at[12], at[91]);    MULADD(at[13], at[90]);    MULADD(at[14], at[89]);    MULADD(at[15], at[88]);    MULADD(at[16], at[87]);    MULADD(at[17], at[86]);    MULADD(at[18], at[85]);    MULADD(at[19], at[84]);    MULADD(at[20], at[83]);    MULADD(at[21], at[82]);    MULADD(at[22], at[81]);    MULADD(at[23], at[80]);    MULADD(at[24], at[79]);    MULADD(at[25], at[78]);    MULADD(at[26], at[77]);    MULADD(at[27], at[76]);    MULADD(at[28], at[75]);    MULADD(at[29], at[74]);    MULADD(at[30], at[73]);    MULADD(at[31], at[72]);    MULADD(at[32], at[71]);    MULADD(at[33], at[70]);    MULADD(at[34], at[69]);    MULADD(at[35], at[68]);    MULADD(at[36], at[67]);    MULADD(at[37], at[66]);    MULADD(at[38], at[65]);    MULADD(at[39], at[64]);    MULADD(at[40], at[63]);    MULADD(at[41], at[62]);    MULADD(at[42], at[61]);    MULADD(at[43], at[60]);    MULADD(at[44], at[59]);    MULADD(at[45], at[58]);    MULADD(at[46], at[57]);    MULADD(at[47], at[56]); 
   COMBA_STORE(C->dp[55]);
   /* 56 */
   COMBA_FORWARD;
   MULADD(at[9], at[95]);    MULADD(at[10], at[94]);    MULADD(at[11], at[93]);    MULADD(at[12], at[92]);    MULADD(at[13], at[91]);    MULADD(at[14], at[90]);    MULADD(at[15], at[89]);    MULADD(at[16], at[88]);    MULADD(at[17], at[87]);    MULADD(at[18], at[86]);    MULADD(at[19], at[85]);    MULADD(at[20], at[84]);    MULADD(at[21], at[83]);    MULADD(at[22], at[82]);    MULADD(at[23], at[81]);    MULADD(at[24], at[80]);    MULADD(at[25], at[79]);    MULADD(at[26], at[78]);    MULADD(at[27], at[77]);    MULADD(at[28], at[76]);    MULADD(at[29], at[75]);    MULADD(at[30], at[74]);    MULADD(at[31], at[73]);    MULADD(at[32], at[72]);    MULADD(at[33], at[71]);    MULADD(at[34], at[70]);    MULADD(at[35], at[69]);    MULADD(at[36], at[68]);    MULADD(at[37], at[67]);    MULADD(at[38], at[66]);    MULADD(at[39], at[65]);    MULADD(at[40], at[64]);    MULADD(at[41], at[63]);    MULADD(at[42], at[62]);    MULADD(at[43], at[61]);    MULADD(at[44], at[60]);    MULADD(at[45], at[59]);    MULADD(at[46], at[58]);    MULADD(at[47], at[57]); 
   COMBA_STORE(C->dp[56]);
   /* 57 */
   COMBA_FORWARD;
   MULADD(at[10], at[95]);    MULADD(at[11], at[94]);    MULADD(at[12], at[93]);    MULADD(at[13], at[92]);    MULADD(at[14], at[91]);    MULADD(at[15], at[90]);    MULADD(at[16], at[89]);    MULADD(at[17], at[88]);    MULADD(at[18], at[87]);    MULADD(at[19], at[86]);    MULADD(at[20], at[85]);    MULADD(at[21], at[84]);    MULADD(at[22], at[83]);    MULADD(at[23], at[82]);    MULADD(at[24], at[81]);    MULADD(at[25], at[80]);    MULADD(at[26], at[79]);    MULADD(at[27], at[78]);    MULADD(at[28], at[77]);    MULADD(at[29], at[76]);    MULADD(at[30], at[75]);    MULADD(at[31], at[74]);    MULADD(at[32], at[73]);    MULADD(at[33], at[72]);    MULADD(at[34], at[71]);    MULADD(at[35], at[70]);    MULADD(at[36], at[69]);    MULADD(at[37], at[68]);    MULADD(at[38], at[67]);    MULADD(at[39], at[66]);    MULADD(at[40], at[65]);    MULADD(at[41], at[64]);    MULADD(at[42], at[63]);    MULADD(at[43], at[62]);    MULADD(at[44], at[61]);    MULADD(at[45], at[60]);    MULADD(at[46], at[59]);    MULADD(at[47], at[58]); 
   COMBA_STORE(C->dp[57]);
   /* 58 */
   COMBA_FORWARD;
   MULADD(at[11], at[95]);    MULADD(at[12], at[94]);    MULADD(at[13], at[93]);    MULADD(at[14], at[92]);    MULADD(at[15], at[91]);    MULADD(at[16], at[90]);    MULADD(at[17], at[89]);    MULADD(at[18], at[88]);    MULADD(at[19], at[87]);    MULADD(at[20], at[86]);    MULADD(at[21], at[85]);    MULADD(at[22], at[84]);    MULADD(at[23], at[83]);    MULADD(at[24], at[82]);    MULADD(at[25], at[81]);    MULADD(at[26], at[80]);    MULADD(at[27], at[79]);    MULADD(at[28], at[78]);    MULADD(at[29], at[77]);    MULADD(at[30], at[76]);    MULADD(at[31], at[75]);    MULADD(at[32], at[74]);    MULADD(at[33], at[73]);    MULADD(at[34], at[72]);    MULADD(at[35], at[71]);    MULADD(at[36], at[70]);    MULADD(at[37], at[69]);    MULADD(at[38], at[68]);    MULADD(at[39], at[67]);    MULADD(at[40], at[66]);    MULADD(at[41], at[65]);    MULADD(at[42], at[64]);    MULADD(at[43], at[63]);    MULADD(at[44], at[62]);    MULADD(at[45], at[61]);    MULADD(at[46], at[60]);    MULADD(at[47], at[59]); 
   COMBA_STORE(C->dp[58]);
   /* 59 */
   COMBA_FORWARD;
   MULADD(at[12], at[95]);    MULADD(at[13], at[94]);    MULADD(at[14], at[93]);    MULADD(at[15], at[92]);    MULADD(at[16], at[91]);    MULADD(at[17], at[90]);    MULADD(at[18], at[89]);    MULADD(at[19], at[88]);    MULADD(at[20], at[87]);    MULADD(at[21], at[86]);    MULADD(at[22], at[85]);    MULADD(at[23], at[84]);    MULADD(at[24], at[83]);    MULADD(at[25], at[82]);    MULADD(at[26], at[81]);    MULADD(at[27], at[80]);    MULADD(at[28], at[79]);    MULADD(at[29], at[78]);    MULADD(at[30], at[77]);    MULADD(at[31], at[76]);    MULADD(at[32], at[75]);    MULADD(at[33], at[74]);    MULADD(at[34], at[73]);    MULADD(at[35], at[72]);    MULADD(at[36], at[71]);    MULADD(at[37], at[70]);    MULADD(at[38], at[69]);    MULADD(at[39], at[68]);    MULADD(at[40], at[67]);    MULADD(at[41], at[66]);    MULADD(at[42], at[65]);    MULADD(at[43], at[64]);    MULADD(at[44], at[63]);    MULADD(at[45], at[62]);    MULADD(at[46], at[61]);    MULADD(at[47], at[60]); 
   COMBA_STORE(C->dp[59]);
   /* 60 */
   COMBA_FORWARD;
   MULADD(at[13], at[95]);    MULADD(at[14], at[94]);    MULADD(at[15], at[93]);    MULADD(at[16], at[92]);    MULADD(at[17], at[91]);    MULADD(at[18], at[90]);    MULADD(at[19], at[89]);    MULADD(at[20], at[88]);    MULADD(at[21], at[87]);    MULADD(at[22], at[86]);    MULADD(at[23], at[85]);    MULADD(at[24], at[84]);    MULADD(at[25], at[83]);    MULADD(at[26], at[82]);    MULADD(at[27], at[81]);    MULADD(at[28], at[80]);    MULADD(at[29], at[79]);    MULADD(at[30], at[78]);    MULADD(at[31], at[77]);    MULADD(at[32], at[76]);    MULADD(at[33], at[75]);    MULADD(at[34], at[74]);    MULADD(at[35], at[73]);    MULADD(at[36], at[72]);    MULADD(at[37], at[71]);    MULADD(at[38], at[70]);    MULADD(at[39], at[69]);    MULADD(at[40], at[68]);    MULADD(at[41], at[67]);    MULADD(at[42], at[66]);    MULADD(at[43], at[65]);    MULADD(at[44], at[64]);    MULADD(at[45], at[63]);    MULADD(at[46], at[62]);    MULADD(at[47], at[61]); 
   COMBA_STORE(C->dp[60]);
   /* 61 */
   COMBA_FORWARD;
   MULADD(at[14], at[95]);    MULADD(at[15], at[94]);    MULADD(at[16], at[93]);    MULADD(at[17], at[92]);    MULADD(at[18], at[91]);    MULADD(at[19], at[90]);    MULADD(at[20], at[89]);    MULADD(at[21], at[88]);    MULADD(at[22], at[87]);    MULADD(at[23], at[86]);    MULADD(at[24], at[85]);    MULADD(at[25], at[84]);    MULADD(at[26], at[83]);    MULADD(at[27], at[82]);    MULADD(at[28], at[81]);    MULADD(at[29], at[80]);    MULADD(at[30], at[79]);    MULADD(at[31], at[78]);    MULADD(at[32], at[77]);    MULADD(at[33], at[76]);    MULADD(at[34], at[75]);    MULADD(at[35], at[74]);    MULADD(at[36], at[73]);    MULADD(at[37], at[72]);    MULADD(at[38], at[71]);    MULADD(at[39], at[70]);    MULADD(at[40], at[69]);    MULADD(at[41], at[68]);    MULADD(at[42], at[67]);    MULADD(at[43], at[66]);    MULADD(at[44], at[65]);    MULADD(at[45], at[64]);    MULADD(at[46], at[63]);    MULADD(at[47], at[62]); 
   COMBA_STORE(C->dp[61]);
   /* 62 */
   COMBA_FORWARD;
   MULADD(at[15], at[95]);    MULADD(at[16], at[94]);    MULADD(at[17], at[93]);    MULADD(at[18], at[92]);    MULADD(at[19], at[91]);    MULADD(at[20], at[90]);    MULADD(at[21], at[89]);    MULADD(at[22], at[88]);    MULADD(at[23], at[87]);    MULADD(at[24], at[86]);    MULADD(at[25], at[85]);    MULADD(at[26], at[84]);    MULADD(at[27], at[83]);    MULADD(at[28], at[82]);    MULADD(at[29], at[81]);    MULADD(at[30], at[80]);    MULADD(at[31], at[79]);    MULADD(at[32], at[78]);    MULADD(at[33], at[77]);    MULADD(at[34], at[76]);    MULADD(at[35], at[75]);    MULADD(at[36], at[74]);    MULADD(at[37], at[73]);    MULADD(at[38], at[72]);    MULADD(at[39], at[71]);    MULADD(at[40], at[70]);    MULADD(at[41], at[69]);    MULADD(at[42], at[68]);    MULADD(at[43], at[67]);    MULADD(at[44], at[66]);    MULADD(at[45], at[65]);    MULADD(at[46], at[64]);    MULADD(at[47], at[63]); 
   COMBA_STORE(C->dp[62]);
   /* 63 */
   COMBA_FORWARD;
   MULADD(at[16], at[95]);    MULADD(at[17], at[94]);    MULADD(at[18], at[93]);    MULADD(at[19], at[92]);    MULADD(at[20], at[91]);    MULADD(at[21], at[90]);    MULADD(at[22], at[89]);    MULADD(at[23], at[88]);    MULADD(at[24], at[87]);    MULADD(at[25], at[86]);    MULADD(at[26], at[85]);    MULADD(at[27], at[84]);    MULADD(at[28], at[83]);    MULADD(at[29], at[82]);    MULADD(at[30], at[81]);    MULADD(at[31], at[80]);    MULADD(at[32], at[79]);    MULADD(at[33], at[78]);    MULADD(at[34], at[77]);    MULADD(at[35], at[76]);    MULADD(at[36], at[75]);    MULADD(at[37], at[74]);    MULADD(at[38], at[73]);    MULADD(at[39], at[72]);    MULADD(at[40], at[71]);    MULADD(at[41], at[70]);    MULADD(at[42], at[69]);    MULADD(at[43], at[68]);    MULADD(at[44], at[67]);    MULADD(at[45], at[66]);    MULADD(at[46], at[65]);    MULADD(at[47], at[64]); 
   COMBA_STORE(C->dp[63]);
   /* 64 */
   COMBA_FORWARD;
   MULADD(at[17], at[95]);    MULADD(at[18], at[94]);    MULADD(at[19], at[93]);    MULADD(at[20], at[92]);    MULADD(at[21], at[91]);    MULADD(at[22], at[90]);    MULADD(at[23], at[89]);    MULADD(at[24], at[88]);    MULADD(at[25], at[87]);    MULADD(at[26], at[86]);    MULADD(at[27], at[85]);    MULADD(at[28], at[84]);    MULADD(at[29], at[83]);    MULADD(at[30], at[82]);    MULADD(at[31], at[81]);    MULADD(at[32], at[80]);    MULADD(at[33], at[79]);    MULADD(at[34], at[78]);    MULADD(at[35], at[77]);    MULADD(at[36], at[76]);    MULADD(at[37], at[75]);    MULADD(at[38], at[74]);    MULADD(at[39], at[73]);    MULADD(at[40], at[72]);    MULADD(at[41], at[71]);    MULADD(at[42], at[70]);    MULADD(at[43], at[69]);    MULADD(at[44], at[68]);    MULADD(at[45], at[67]);    MULADD(at[46], at[66]);    MULADD(at[47], at[65]); 
   COMBA_STORE(C->dp[64]);
   /* 65 */
   COMBA_FORWARD;
   MULADD(at[18], at[95]);    MULADD(at[19], at[94]);    MULADD(at[20], at[93]);    MULADD(at[21], at[92]);    MULADD(at[22], at[91]);    MULADD(at[23], at[90]);    MULADD(at[24], at[89]);    MULADD(at[25], at[88]);    MULADD(at[26], at[87]);    MULADD(at[27], at[86]);    MULADD(at[28], at[85]);    MULADD(at[29], at[84]);    MULADD(at[30], at[83]);    MULADD(at[31], at[82]);    MULADD(at[32], at[81]);    MULADD(at[33], at[80]);    MULADD(at[34], at[79]);    MULADD(at[35], at[78]);    MULADD(at[36], at[77]);    MULADD(at[37], at[76]);    MULADD(at[38], at[75]);    MULADD(at[39], at[74]);    MULADD(at[40], at[73]);    MULADD(at[41], at[72]);    MULADD(at[42], at[71]);    MULADD(at[43], at[70]);    MULADD(at[44], at[69]);    MULADD(at[45], at[68]);    MULADD(at[46], at[67]);    MULADD(at[47], at[66]); 
   COMBA_STORE(C->dp[65]);
   /* 66 */
   COMBA_FORWARD;
   MULADD(at[19], at[95]);    MULADD(at[20], at[94]);    MULADD(at[21], at[93]);    MULADD(at[22], at[92]);    MULADD(at[23], at[91]);    MULADD(at[24], at[90]);    MULADD(at[25], at[89]);    MULADD(at[26], at[88]);    MULADD(at[27], at[87]);    MULADD(at[28], at[86]);    MULADD(at[29], at[85]);    MULADD(at[30], at[84]);    MULADD(at[31], at[83]);    MULADD(at[32], at[82]);    MULADD(at[33], at[81]);    MULADD(at[34], at[80]);    MULADD(at[35], at[79]);    MULADD(at[36], at[78]);    MULADD(at[37], at[77]);    MULADD(at[38], at[76]);    MULADD(at[39], at[75]);    MULADD(at[40], at[74]);    MULADD(at[41], at[73]);    MULADD(at[42], at[72]);    MULADD(at[43], at[71]);    MULADD(at[44], at[70]);    MULADD(at[45], at[69]);    MULADD(at[46], at[68]);    MULADD(at[47], at[67]); 
   COMBA_STORE(C->dp[66]);
   /* 67 */
   COMBA_FORWARD;
   MULADD(at[20], at[95]);    MULADD(at[21], at[94]);    MULADD(at[22], at[93]);    MULADD(at[23], at[92]);    MULADD(at[24], at[91]);    MULADD(at[25], at[90]);    MULADD(at[26], at[89]);    MULADD(at[27], at[88]);    MULADD(at[28], at[87]);    MULADD(at[29], at[86]);    MULADD(at[30], at[85]);    MULADD(at[31], at[84]);    MULADD(at[32], at[83]);    MULADD(at[33], at[82]);    MULADD(at[34], at[81]);    MULADD(at[35], at[80]);    MULADD(at[36], at[79]);    MULADD(at[37], at[78]);    MULADD(at[38], at[77]);    MULADD(at[39], at[76]);    MULADD(at[40], at[75]);    MULADD(at[41], at[74]);    MULADD(at[42], at[73]);    MULADD(at[43], at[72]);    MULADD(at[44], at[71]);    MULADD(at[45], at[70]);    MULADD(at[46], at[69]);    MULADD(at[47], at[68]); 
   COMBA_STORE(C->dp[67]);
   /* 68 */
   COMBA_FORWARD;
   MULADD(at[21], at[95]);    MULADD(at[22], at[94]);    MULADD(at[23], at[93]);    MULADD(at[24], at[92]);    MULADD(at[25], at[91]);    MULADD(at[26], at[90]);    MULADD(at[27], at[89]);    MULADD(at[28], at[88]);    MULADD(at[29], at[87]);    MULADD(at[30], at[86]);    MULADD(at[31], at[85]);    MULADD(at[32], at[84]);    MULADD(at[33], at[83]);    MULADD(at[34], at[82]);    MULADD(at[35], at[81]);    MULADD(at[36], at[80]);    MULADD(at[37], at[79]);    MULADD(at[38], at[78]);    MULADD(at[39], at[77]);    MULADD(at[40], at[76]);    MULADD(at[41], at[75]);    MULADD(at[42], at[74]);    MULADD(at[43], at[73]);    MULADD(at[44], at[72]);    MULADD(at[45], at[71]);    MULADD(at[46], at[70]);    MULADD(at[47], at[69]); 
   COMBA_STORE(C->dp[68]);
   /* 69 */
   COMBA_FORWARD;
   MULADD(at[22], at[95]);    MULADD(at[23], at[94]);    MULADD(at[24], at[93]);    MULADD(at[25], at[92]);    MULADD(at[26], at[91]);    MULADD(at[27], at[90]);    MULADD(at[28], at[89]);    MULADD(at[29], at[88]);    MULADD(at[30], at[87]);    MULADD(at[31], at[86]);    MULADD(at[32], at[85]);    MULADD(at[33], at[84]);    MULADD(at[34], at[83]);    MULADD(at[35], at[82]);    MULADD(at[36], at[81]);    MULADD(at[37], at[80]);    MULADD(at[38], at[79]);    MULADD(at[39], at[78]);    MULADD(at[40], at[77]);    MULADD(at[41], at[76]);    MULADD(at[42], at[75]);    MULADD(at[43], at[74]);    MULADD(at[44], at[73]);    MULADD(at[45], at[72]);    MULADD(at[46], at[71]);    MULADD(at[47], at[70]); 
   COMBA_STORE(C->dp[69]);
   /* 70 */
   COMBA_FORWARD;
   MULADD(at[23], at[95]);    MULADD(at[24], at[94]);    MULADD(at[25], at[93]);    MULADD(at[26], at[92]);    MULADD(at[27], at[91]);    MULADD(at[28], at[90]);    MULADD(at[29], at[89]);    MULADD(at[30], at[88]);    MULADD(at[31], at[87]);    MULADD(at[32], at[86]);    MULADD(at[33], at[85]);    MULADD(at[34], at[84]);    MULADD(at[35], at[83]);    MULADD(at[36], at[82]);    MULADD(at[37], at[81]);    MULADD(at[38], at[80]);    MULADD(at[39], at[79]);    MULADD(at[40], at[78]);    MULADD(at[41], at[77]);    MULADD(at[42], at[76]);    MULADD(at[43], at[75]);    MULADD(at[44], at[74]);    MULADD(at[45], at[73]);    MULADD(at[46], at[72]);    MULADD(at[47], at[71]); 
   COMBA_STORE(C->dp[70]);
   /* 71 */
   COMBA_FORWARD;
   MULADD(at[24], at[95]);    MULADD(at[25], at[94]);    MULADD(at[26], at[93]);    MULADD(at[27], at[92]);    MULADD(at[28], at[91]);    MULADD(at[29], at[90]);    MULADD(at[30], at[89]);    MULADD(at[31], at[88]);    MULADD(at[32], at[87]);    MULADD(at[33], at[86]);    MULADD(at[34], at[85]);    MULADD(at[35], at[84]);    MULADD(at[36], at[83]);    MULADD(at[37], at[82]);    MULADD(at[38], at[81]);    MULADD(at[39], at[80]);    MULADD(at[40], at[79]);    MULADD(at[41], at[78]);    MULADD(at[42], at[77]);    MULADD(at[43], at[76]);    MULADD(at[44], at[75]);    MULADD(at[45], at[74]);    MULADD(at[46], at[73]);    MULADD(at[47], at[72]); 
   COMBA_STORE(C->dp[71]);
   /* 72 */
   COMBA_FORWARD;
   MULADD(at[25], at[95]);    MULADD(at[26], at[94]);    MULADD(at[27], at[93]);    MULADD(at[28], at[92]);    MULADD(at[29], at[91]);    MULADD(at[30], at[90]);    MULADD(at[31], at[89]);    MULADD(at[32], at[88]);    MULADD(at[33], at[87]);    MULADD(at[34], at[86]);    MULADD(at[35], at[85]);    MULADD(at[36], at[84]);    MULADD(at[37], at[83]);    MULADD(at[38], at[82]);    MULADD(at[39], at[81]);    MULADD(at[40], at[80]);    MULADD(at[41], at[79]);    MULADD(at[42], at[78]);    MULADD(at[43], at[77]);    MULADD(at[44], at[76]);    MULADD(at[45], at[75]);    MULADD(at[46], at[74]);    MULADD(at[47], at[73]); 
   COMBA_STORE(C->dp[72]);
   /* 73 */
   COMBA_FORWARD;
   MULADD(at[26], at[95]);    MULADD(at[27], at[94]);    MULADD(at[28], at[93]);    MULADD(at[29], at[92]);    MULADD(at[30], at[91]);    MULADD(at[31], at[90]);    MULADD(at[32], at[89]);    MULADD(at[33], at[88]);    MULADD(at[34], at[87]);    MULADD(at[35], at[86]);    MULADD(at[36], at[85]);    MULADD(at[37], at[84]);    MULADD(at[38], at[83]);    MULADD(at[39], at[82]);    MULADD(at[40], at[81]);    MULADD(at[41], at[80]);    MULADD(at[42], at[79]);    MULADD(at[43], at[78]);    MULADD(at[44], at[77]);    MULADD(at[45], at[76]);    MULADD(at[46], at[75]);    MULADD(at[47], at[74]); 
   COMBA_STORE(C->dp[73]);
   /* 74 */
   COMBA_FORWARD;
   MULADD(at[27], at[95]);    MULADD(at[28], at[94]);    MULADD(at[29], at[93]);    MULADD(at[30], at[92]);    MULADD(at[31], at[91]);    MULADD(at[32], at[90]);    MULADD(at[33], at[89]);    MULADD(at[34], at[88]);    MULADD(at[35], at[87]);    MULADD(at[36], at[86]);    MULADD(at[37], at[85]);    MULADD(at[38], at[84]);    MULADD(at[39], at[83]);    MULADD(at[40], at[82]);    MULADD(at[41], at[81]);    MULADD(at[42], at[80]);    MULADD(at[43], at[79]);    MULADD(at[44], at[78]);    MULADD(at[45], at[77]);    MULADD(at[46], at[76]);    MULADD(at[47], at[75]); 
   COMBA_STORE(C->dp[74]);
   /* 75 */
   COMBA_FORWARD;
   MULADD(at[28], at[95]);    MULADD(at[29], at[94]);    MULADD(at[30], at[93]);    MULADD(at[31], at[92]);    MULADD(at[32], at[91]);    MULADD(at[33], at[90]);    MULADD(at[34], at[89]);    MULADD(at[35], at[88]);    MULADD(at[36], at[87]);    MULADD(at[37], at[86]);    MULADD(at[38], at[85]);    MULADD(at[39], at[84]);    MULADD(at[40], at[83]);    MULADD(at[41], at[82]);    MULADD(at[42], at[81]);    MULADD(at[43], at[80]);    MULADD(at[44], at[79]);    MULADD(at[45], at[78]);    MULADD(at[46], at[77]);    MULADD(at[47], at[76]); 
   COMBA_STORE(C->dp[75]);
   /* 76 */
   COMBA_FORWARD;
   MULADD(at[29], at[95]);    MULADD(at[30], at[94]);    MULADD(at[31], at[93]);    MULADD(at[32], at[92]);    MULADD(at[33], at[91]);    MULADD(at[34], at[90]);    MULADD(at[35], at[89]);    MULADD(at[36], at[88]);    MULADD(at[37], at[87]);    MULADD(at[38], at[86]);    MULADD(at[39], at[85]);    MULADD(at[40], at[84]);    MULADD(at[41], at[83]);    MULADD(at[42], at[82]);    MULADD(at[43], at[81]);    MULADD(at[44], at[80]);    MULADD(at[45], at[79]);    MULADD(at[46], at[78]);    MULADD(at[47], at[77]); 
   COMBA_STORE(C->dp[76]);
   /* 77 */
   COMBA_FORWARD;
   MULADD(at[30], at[95]);    MULADD(at[31], at[94]);    MULADD(at[32], at[93]);    MULADD(at[33], at[92]);    MULADD(at[34], at[91]);    MULADD(at[35], at[90]);    MULADD(at[36], at[89]);    MULADD(at[37], at[88]);    MULADD(at[38], at[87]);    MULADD(at[39], at[86]);    MULADD(at[40], at[85]);    MULADD(at[41], at[84]);    MULADD(at[42], at[83]);    MULADD(at[43], at[82]);    MULADD(at[44], at[81]);    MULADD(at[45], at[80]);    MULADD(at[46], at[79]);    MULADD(at[47], at[78]); 
   COMBA_STORE(C->dp[77]);
   /* 78 */
   COMBA_FORWARD;
   MULADD(at[31], at[95]);    MULADD(at[32], at[94]);    MULADD(at[33], at[93]);    MULADD(at[34], at[92]);    MULADD(at[35], at[91]);    MULADD(at[36], at[90]);    MULADD(at[37], at[89]);    MULADD(at[38], at[88]);    MULADD(at[39], at[87]);    MULADD(at[40], at[86]);    MULADD(at[41], at[85]);    MULADD(at[42], at[84]);    MULADD(at[43], at[83]);    MULADD(at[44], at[82]);    MULADD(at[45], at[81]);    MULADD(at[46], at[80]);    MULADD(at[47], at[79]); 
   COMBA_STORE(C->dp[78]);
   /* 79 */
   COMBA_FORWARD;
   MULADD(at[32], at[95]);    MULADD(at[33], at[94]);    MULADD(at[34], at[93]);    MULADD(at[35], at[92]);    MULADD(at[36], at[91]);    MULADD(at[37], at[90]);    MULADD(at[38], at[89]);    MULADD(at[39], at[88]);    MULADD(at[40], at[87]);    MULADD(at[41], at[86]);    MULADD(at[42], at[85]);    MULADD(at[43], at[84]);    MULADD(at[44], at[83]);    MULADD(at[45], at[82]);    MULADD(at[46], at[81]);    MULADD(at[47], at[80]); 
   COMBA_STORE(C->dp[79]);
   /* 80 */
   COMBA_FORWARD;
   MULADD(at[33], at[95]);    MULADD(at[34], at[94]);    MULADD(at[35], at[93]);    MULADD(at[36], at[92]);    MULADD(at[37], at[91]);    MULADD(at[38], at[90]);    MULADD(at[39], at[89]);    MULADD(at[40], at[88]);    MULADD(at[41], at[87]);    MULADD(at[42], at[86]);    MULADD(at[43], at[85]);    MULADD(at[44], at[84]);    MULADD(at[45], at[83]);    MULADD(at[46], at[82]);    MULADD(at[47], at[81]); 
   COMBA_STORE(C->dp[80]);
   /* 81 */
   COMBA_FORWARD;
   MULADD(at[34], at[95]);    MULADD(at[35], at[94]);    MULADD(at[36], at[93]);    MULADD(at[37], at[92]);    MULADD(at[38], at[91]);    MULADD(at[39], at[90]);    MULADD(at[40], at[89]);    MULADD(at[41], at[88]);    MULADD(at[42], at[87]);    MULADD(at[43], at[86]);    MULADD(at[44], at[85]);    MULADD(at[45], at[84]);    MULADD(at[46], at[83]);    MULADD(at[47], at[82]); 
   COMBA_STORE(C->dp[81]);
   /* 82 */
   COMBA_FORWARD;
   MULADD(at[35], at[95]);    MULADD(at[36], at[94]);    MULADD(at[37], at[93]);    MULADD(at[38], at[92]);    MULADD(at[39], at[91]);    MULADD(at[40], at[90]);    MULADD(at[41], at[89]);    MULADD(at[42], at[88]);    MULADD(at[43], at[87]);    MULADD(at[44], at[86]);    MULADD(at[45], at[85]);    MULADD(at[46], at[84]);    MULADD(at[47], at[83]); 
   COMBA_STORE(C->dp[82]);
   /* 83 */
   COMBA_FORWARD;
   MULADD(at[36], at[95]);    MULADD(at[37], at[94]);    MULADD(at[38], at[93]);    MULADD(at[39], at[92]);    MULADD(at[40], at[91]);    MULADD(at[41], at[90]);    MULADD(at[42], at[89]);    MULADD(at[43], at[88]);    MULADD(at[44], at[87]);    MULADD(at[45], at[86]);    MULADD(at[46], at[85]);    MULADD(at[47], at[84]); 
   COMBA_STORE(C->dp[83]);
   /* 84 */
   COMBA_FORWARD;
   MULADD(at[37], at[95]);    MULADD(at[38], at[94]);    MULADD(at[39], at[93]);    MULADD(at[40], at[92]);    MULADD(at[41], at[91]);    MULADD(at[42], at[90]);    MULADD(at[43], at[89]);    MULADD(at[44], at[88]);    MULADD(at[45], at[87]);    MULADD(at[46], at[86]);    MULADD(at[47], at[85]); 
   COMBA_STORE(C->dp[84]);
   /* 85 */
   COMBA_FORWARD;
   MULADD(at[38], at[95]);    MULADD(at[39], at[94]);    MULADD(at[40], at[93]);    MULADD(at[41], at[92]);    MULADD(at[42], at[91]);    MULADD(at[43], at[90]);    MULADD(at[44], at[89]);    MULADD(at[45], at[88]);    MULADD(at[46], at[87]);    MULADD(at[47], at[86]); 
   COMBA_STORE(C->dp[85]);
   /* 86 */
   COMBA_FORWARD;
   MULADD(at[39], at[95]);    MULADD(at[40], at[94]);    MULADD(at[41], at[93]);    MULADD(at[42], at[92]);    MULADD(at[43], at[91]);    MULADD(at[44], at[90]);    MULADD(at[45], at[89]);    MULADD(at[46], at[88]);    MULADD(at[47], at[87]); 
   COMBA_STORE(C->dp[86]);
   /* 87 */
   COMBA_FORWARD;
   MULADD(at[40], at[95]);    MULADD(at[41], at[94]);    MULADD(at[42], at[93]);    MULADD(at[43], at[92]);    MULADD(at[44], at[91]);    MULADD(at[45], at[90]);    MULADD(at[46], at[89]);    MULADD(at[47], at[88]); 
   COMBA_STORE(C->dp[87]);
   /* 88 */
   COMBA_FORWARD;
   MULADD(at[41], at[95]);    MULADD(at[42], at[94]);    MULADD(at[43], at[93]);    MULADD(at[44], at[92]);    MULADD(at[45], at[91]);    MULADD(at[46], at[90]);    MULADD(at[47], at[89]); 
   COMBA_STORE(C->dp[88]);
   /* 89 */
   COMBA_FORWARD;
   MULADD(at[42], at[95]);    MULADD(at[43], at[94]);    MULADD(at[44], at[93]);    MULADD(at[45], at[92]);    MULADD(at[46], at[91]);    MULADD(at[47], at[90]); 
   COMBA_STORE(C->dp[89]);
   /* 90 */
   COMBA_FORWARD;
   MULADD(at[43], at[95]);    MULADD(at[44], at[94]);    MULADD(at[45], at[93]);    MULADD(at[46], at[92]);    MULADD(at[47], at[91]); 
   COMBA_STORE(C->dp[90]);
   /* 91 */
   COMBA_FORWARD;
   MULADD(at[44], at[95]);    MULADD(at[45], at[94]);    MULADD(at[46], at[93]);    MULADD(at[47], at[92]); 
   COMBA_STORE(C->dp[91]);
   /* 92 */
   COMBA_FORWARD;
   MULADD(at[45], at[95]);    MULADD(at[46], at[94]);    MULADD(at[47], at[93]); 
   COMBA_STORE(C->dp[92]);
   /* 93 */
   COMBA_FORWARD;
   MULADD(at[46], at[95]);    MULADD(at[47], at[94]); 
   COMBA_STORE(C->dp[93]);
   /* 94 */
   COMBA_FORWARD;
   MULADD(at[47], at[95]); 
   COMBA_STORE(C->dp[94]);
   COMBA_STORE2(C->dp[95]);
   C->used = 96;
   C->sign = A->sign ^ B->sign;
   fp_clamp(C);
   COMBA_FINI;

#ifdef WOLFSSL_SMALL_STACK
   XFREE(at, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif
