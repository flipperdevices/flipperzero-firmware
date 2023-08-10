/* fp_mul_comba_32.i
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



#ifdef TFM_MUL32
int fp_mul_comba32(fp_int *A, fp_int *B, fp_int *C)
{
   int out_size;
   fp_digit c0, c1, c2;
#ifndef WOLFSSL_SMALL_STACK
   fp_digit at[64];
#else
   fp_digit *at;
#endif

#ifdef WOLFSSL_SMALL_STACK
   at = (fp_digit*)XMALLOC(sizeof(fp_digit) * 64, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (at == NULL)
       return FP_MEM;
#endif

   out_size = A->used + B->used;
   XMEMCPY(at, A->dp, 32 * sizeof(fp_digit));
   XMEMCPY(at+32, B->dp, 32 * sizeof(fp_digit));
   COMBA_START;

   COMBA_CLEAR;
   /* 0 */
   MULADD(at[0], at[32]); 
   COMBA_STORE(C->dp[0]);
   /* 1 */
   COMBA_FORWARD;
   MULADD(at[0], at[33]);    MULADD(at[1], at[32]); 
   COMBA_STORE(C->dp[1]);
   /* 2 */
   COMBA_FORWARD;
   MULADD(at[0], at[34]);    MULADD(at[1], at[33]);    MULADD(at[2], at[32]); 
   COMBA_STORE(C->dp[2]);
   /* 3 */
   COMBA_FORWARD;
   MULADD(at[0], at[35]);    MULADD(at[1], at[34]);    MULADD(at[2], at[33]);    MULADD(at[3], at[32]); 
   COMBA_STORE(C->dp[3]);
   /* 4 */
   COMBA_FORWARD;
   MULADD(at[0], at[36]);    MULADD(at[1], at[35]);    MULADD(at[2], at[34]);    MULADD(at[3], at[33]);    MULADD(at[4], at[32]); 
   COMBA_STORE(C->dp[4]);
   /* 5 */
   COMBA_FORWARD;
   MULADD(at[0], at[37]);    MULADD(at[1], at[36]);    MULADD(at[2], at[35]);    MULADD(at[3], at[34]);    MULADD(at[4], at[33]);    MULADD(at[5], at[32]); 
   COMBA_STORE(C->dp[5]);
   /* 6 */
   COMBA_FORWARD;
   MULADD(at[0], at[38]);    MULADD(at[1], at[37]);    MULADD(at[2], at[36]);    MULADD(at[3], at[35]);    MULADD(at[4], at[34]);    MULADD(at[5], at[33]);    MULADD(at[6], at[32]); 
   COMBA_STORE(C->dp[6]);
   /* 7 */
   COMBA_FORWARD;
   MULADD(at[0], at[39]);    MULADD(at[1], at[38]);    MULADD(at[2], at[37]);    MULADD(at[3], at[36]);    MULADD(at[4], at[35]);    MULADD(at[5], at[34]);    MULADD(at[6], at[33]);    MULADD(at[7], at[32]); 
   COMBA_STORE(C->dp[7]);
   /* 8 */
   COMBA_FORWARD;
   MULADD(at[0], at[40]);    MULADD(at[1], at[39]);    MULADD(at[2], at[38]);    MULADD(at[3], at[37]);    MULADD(at[4], at[36]);    MULADD(at[5], at[35]);    MULADD(at[6], at[34]);    MULADD(at[7], at[33]);    MULADD(at[8], at[32]); 
   COMBA_STORE(C->dp[8]);
   /* 9 */
   COMBA_FORWARD;
   MULADD(at[0], at[41]);    MULADD(at[1], at[40]);    MULADD(at[2], at[39]);    MULADD(at[3], at[38]);    MULADD(at[4], at[37]);    MULADD(at[5], at[36]);    MULADD(at[6], at[35]);    MULADD(at[7], at[34]);    MULADD(at[8], at[33]);    MULADD(at[9], at[32]); 
   COMBA_STORE(C->dp[9]);
   /* 10 */
   COMBA_FORWARD;
   MULADD(at[0], at[42]);    MULADD(at[1], at[41]);    MULADD(at[2], at[40]);    MULADD(at[3], at[39]);    MULADD(at[4], at[38]);    MULADD(at[5], at[37]);    MULADD(at[6], at[36]);    MULADD(at[7], at[35]);    MULADD(at[8], at[34]);    MULADD(at[9], at[33]);    MULADD(at[10], at[32]); 
   COMBA_STORE(C->dp[10]);
   /* 11 */
   COMBA_FORWARD;
   MULADD(at[0], at[43]);    MULADD(at[1], at[42]);    MULADD(at[2], at[41]);    MULADD(at[3], at[40]);    MULADD(at[4], at[39]);    MULADD(at[5], at[38]);    MULADD(at[6], at[37]);    MULADD(at[7], at[36]);    MULADD(at[8], at[35]);    MULADD(at[9], at[34]);    MULADD(at[10], at[33]);    MULADD(at[11], at[32]); 
   COMBA_STORE(C->dp[11]);
   /* 12 */
   COMBA_FORWARD;
   MULADD(at[0], at[44]);    MULADD(at[1], at[43]);    MULADD(at[2], at[42]);    MULADD(at[3], at[41]);    MULADD(at[4], at[40]);    MULADD(at[5], at[39]);    MULADD(at[6], at[38]);    MULADD(at[7], at[37]);    MULADD(at[8], at[36]);    MULADD(at[9], at[35]);    MULADD(at[10], at[34]);    MULADD(at[11], at[33]);    MULADD(at[12], at[32]); 
   COMBA_STORE(C->dp[12]);
   /* 13 */
   COMBA_FORWARD;
   MULADD(at[0], at[45]);    MULADD(at[1], at[44]);    MULADD(at[2], at[43]);    MULADD(at[3], at[42]);    MULADD(at[4], at[41]);    MULADD(at[5], at[40]);    MULADD(at[6], at[39]);    MULADD(at[7], at[38]);    MULADD(at[8], at[37]);    MULADD(at[9], at[36]);    MULADD(at[10], at[35]);    MULADD(at[11], at[34]);    MULADD(at[12], at[33]);    MULADD(at[13], at[32]); 
   COMBA_STORE(C->dp[13]);
   /* 14 */
   COMBA_FORWARD;
   MULADD(at[0], at[46]);    MULADD(at[1], at[45]);    MULADD(at[2], at[44]);    MULADD(at[3], at[43]);    MULADD(at[4], at[42]);    MULADD(at[5], at[41]);    MULADD(at[6], at[40]);    MULADD(at[7], at[39]);    MULADD(at[8], at[38]);    MULADD(at[9], at[37]);    MULADD(at[10], at[36]);    MULADD(at[11], at[35]);    MULADD(at[12], at[34]);    MULADD(at[13], at[33]);    MULADD(at[14], at[32]); 
   COMBA_STORE(C->dp[14]);
   /* 15 */
   COMBA_FORWARD;
   MULADD(at[0], at[47]);    MULADD(at[1], at[46]);    MULADD(at[2], at[45]);    MULADD(at[3], at[44]);    MULADD(at[4], at[43]);    MULADD(at[5], at[42]);    MULADD(at[6], at[41]);    MULADD(at[7], at[40]);    MULADD(at[8], at[39]);    MULADD(at[9], at[38]);    MULADD(at[10], at[37]);    MULADD(at[11], at[36]);    MULADD(at[12], at[35]);    MULADD(at[13], at[34]);    MULADD(at[14], at[33]);    MULADD(at[15], at[32]); 
   COMBA_STORE(C->dp[15]);
   /* 16 */
   COMBA_FORWARD;
   MULADD(at[0], at[48]);    MULADD(at[1], at[47]);    MULADD(at[2], at[46]);    MULADD(at[3], at[45]);    MULADD(at[4], at[44]);    MULADD(at[5], at[43]);    MULADD(at[6], at[42]);    MULADD(at[7], at[41]);    MULADD(at[8], at[40]);    MULADD(at[9], at[39]);    MULADD(at[10], at[38]);    MULADD(at[11], at[37]);    MULADD(at[12], at[36]);    MULADD(at[13], at[35]);    MULADD(at[14], at[34]);    MULADD(at[15], at[33]);    MULADD(at[16], at[32]); 
   COMBA_STORE(C->dp[16]);
   /* 17 */
   COMBA_FORWARD;
   MULADD(at[0], at[49]);    MULADD(at[1], at[48]);    MULADD(at[2], at[47]);    MULADD(at[3], at[46]);    MULADD(at[4], at[45]);    MULADD(at[5], at[44]);    MULADD(at[6], at[43]);    MULADD(at[7], at[42]);    MULADD(at[8], at[41]);    MULADD(at[9], at[40]);    MULADD(at[10], at[39]);    MULADD(at[11], at[38]);    MULADD(at[12], at[37]);    MULADD(at[13], at[36]);    MULADD(at[14], at[35]);    MULADD(at[15], at[34]);    MULADD(at[16], at[33]);    MULADD(at[17], at[32]); 
   COMBA_STORE(C->dp[17]);
   /* 18 */
   COMBA_FORWARD;
   MULADD(at[0], at[50]);    MULADD(at[1], at[49]);    MULADD(at[2], at[48]);    MULADD(at[3], at[47]);    MULADD(at[4], at[46]);    MULADD(at[5], at[45]);    MULADD(at[6], at[44]);    MULADD(at[7], at[43]);    MULADD(at[8], at[42]);    MULADD(at[9], at[41]);    MULADD(at[10], at[40]);    MULADD(at[11], at[39]);    MULADD(at[12], at[38]);    MULADD(at[13], at[37]);    MULADD(at[14], at[36]);    MULADD(at[15], at[35]);    MULADD(at[16], at[34]);    MULADD(at[17], at[33]);    MULADD(at[18], at[32]); 
   COMBA_STORE(C->dp[18]);
   /* 19 */
   COMBA_FORWARD;
   MULADD(at[0], at[51]);    MULADD(at[1], at[50]);    MULADD(at[2], at[49]);    MULADD(at[3], at[48]);    MULADD(at[4], at[47]);    MULADD(at[5], at[46]);    MULADD(at[6], at[45]);    MULADD(at[7], at[44]);    MULADD(at[8], at[43]);    MULADD(at[9], at[42]);    MULADD(at[10], at[41]);    MULADD(at[11], at[40]);    MULADD(at[12], at[39]);    MULADD(at[13], at[38]);    MULADD(at[14], at[37]);    MULADD(at[15], at[36]);    MULADD(at[16], at[35]);    MULADD(at[17], at[34]);    MULADD(at[18], at[33]);    MULADD(at[19], at[32]); 
   COMBA_STORE(C->dp[19]);
   /* 20 */
   COMBA_FORWARD;
   MULADD(at[0], at[52]);    MULADD(at[1], at[51]);    MULADD(at[2], at[50]);    MULADD(at[3], at[49]);    MULADD(at[4], at[48]);    MULADD(at[5], at[47]);    MULADD(at[6], at[46]);    MULADD(at[7], at[45]);    MULADD(at[8], at[44]);    MULADD(at[9], at[43]);    MULADD(at[10], at[42]);    MULADD(at[11], at[41]);    MULADD(at[12], at[40]);    MULADD(at[13], at[39]);    MULADD(at[14], at[38]);    MULADD(at[15], at[37]);    MULADD(at[16], at[36]);    MULADD(at[17], at[35]);    MULADD(at[18], at[34]);    MULADD(at[19], at[33]);    MULADD(at[20], at[32]); 
   COMBA_STORE(C->dp[20]);
   /* 21 */
   COMBA_FORWARD;
   MULADD(at[0], at[53]);    MULADD(at[1], at[52]);    MULADD(at[2], at[51]);    MULADD(at[3], at[50]);    MULADD(at[4], at[49]);    MULADD(at[5], at[48]);    MULADD(at[6], at[47]);    MULADD(at[7], at[46]);    MULADD(at[8], at[45]);    MULADD(at[9], at[44]);    MULADD(at[10], at[43]);    MULADD(at[11], at[42]);    MULADD(at[12], at[41]);    MULADD(at[13], at[40]);    MULADD(at[14], at[39]);    MULADD(at[15], at[38]);    MULADD(at[16], at[37]);    MULADD(at[17], at[36]);    MULADD(at[18], at[35]);    MULADD(at[19], at[34]);    MULADD(at[20], at[33]);    MULADD(at[21], at[32]); 
   COMBA_STORE(C->dp[21]);
   /* 22 */
   COMBA_FORWARD;
   MULADD(at[0], at[54]);    MULADD(at[1], at[53]);    MULADD(at[2], at[52]);    MULADD(at[3], at[51]);    MULADD(at[4], at[50]);    MULADD(at[5], at[49]);    MULADD(at[6], at[48]);    MULADD(at[7], at[47]);    MULADD(at[8], at[46]);    MULADD(at[9], at[45]);    MULADD(at[10], at[44]);    MULADD(at[11], at[43]);    MULADD(at[12], at[42]);    MULADD(at[13], at[41]);    MULADD(at[14], at[40]);    MULADD(at[15], at[39]);    MULADD(at[16], at[38]);    MULADD(at[17], at[37]);    MULADD(at[18], at[36]);    MULADD(at[19], at[35]);    MULADD(at[20], at[34]);    MULADD(at[21], at[33]);    MULADD(at[22], at[32]); 
   COMBA_STORE(C->dp[22]);
   /* 23 */
   COMBA_FORWARD;
   MULADD(at[0], at[55]);    MULADD(at[1], at[54]);    MULADD(at[2], at[53]);    MULADD(at[3], at[52]);    MULADD(at[4], at[51]);    MULADD(at[5], at[50]);    MULADD(at[6], at[49]);    MULADD(at[7], at[48]);    MULADD(at[8], at[47]);    MULADD(at[9], at[46]);    MULADD(at[10], at[45]);    MULADD(at[11], at[44]);    MULADD(at[12], at[43]);    MULADD(at[13], at[42]);    MULADD(at[14], at[41]);    MULADD(at[15], at[40]);    MULADD(at[16], at[39]);    MULADD(at[17], at[38]);    MULADD(at[18], at[37]);    MULADD(at[19], at[36]);    MULADD(at[20], at[35]);    MULADD(at[21], at[34]);    MULADD(at[22], at[33]);    MULADD(at[23], at[32]); 
   COMBA_STORE(C->dp[23]);
   /* 24 */
   COMBA_FORWARD;
   MULADD(at[0], at[56]);    MULADD(at[1], at[55]);    MULADD(at[2], at[54]);    MULADD(at[3], at[53]);    MULADD(at[4], at[52]);    MULADD(at[5], at[51]);    MULADD(at[6], at[50]);    MULADD(at[7], at[49]);    MULADD(at[8], at[48]);    MULADD(at[9], at[47]);    MULADD(at[10], at[46]);    MULADD(at[11], at[45]);    MULADD(at[12], at[44]);    MULADD(at[13], at[43]);    MULADD(at[14], at[42]);    MULADD(at[15], at[41]);    MULADD(at[16], at[40]);    MULADD(at[17], at[39]);    MULADD(at[18], at[38]);    MULADD(at[19], at[37]);    MULADD(at[20], at[36]);    MULADD(at[21], at[35]);    MULADD(at[22], at[34]);    MULADD(at[23], at[33]);    MULADD(at[24], at[32]); 
   COMBA_STORE(C->dp[24]);
   /* 25 */
   COMBA_FORWARD;
   MULADD(at[0], at[57]);    MULADD(at[1], at[56]);    MULADD(at[2], at[55]);    MULADD(at[3], at[54]);    MULADD(at[4], at[53]);    MULADD(at[5], at[52]);    MULADD(at[6], at[51]);    MULADD(at[7], at[50]);    MULADD(at[8], at[49]);    MULADD(at[9], at[48]);    MULADD(at[10], at[47]);    MULADD(at[11], at[46]);    MULADD(at[12], at[45]);    MULADD(at[13], at[44]);    MULADD(at[14], at[43]);    MULADD(at[15], at[42]);    MULADD(at[16], at[41]);    MULADD(at[17], at[40]);    MULADD(at[18], at[39]);    MULADD(at[19], at[38]);    MULADD(at[20], at[37]);    MULADD(at[21], at[36]);    MULADD(at[22], at[35]);    MULADD(at[23], at[34]);    MULADD(at[24], at[33]);    MULADD(at[25], at[32]); 
   COMBA_STORE(C->dp[25]);
   /* 26 */
   COMBA_FORWARD;
   MULADD(at[0], at[58]);    MULADD(at[1], at[57]);    MULADD(at[2], at[56]);    MULADD(at[3], at[55]);    MULADD(at[4], at[54]);    MULADD(at[5], at[53]);    MULADD(at[6], at[52]);    MULADD(at[7], at[51]);    MULADD(at[8], at[50]);    MULADD(at[9], at[49]);    MULADD(at[10], at[48]);    MULADD(at[11], at[47]);    MULADD(at[12], at[46]);    MULADD(at[13], at[45]);    MULADD(at[14], at[44]);    MULADD(at[15], at[43]);    MULADD(at[16], at[42]);    MULADD(at[17], at[41]);    MULADD(at[18], at[40]);    MULADD(at[19], at[39]);    MULADD(at[20], at[38]);    MULADD(at[21], at[37]);    MULADD(at[22], at[36]);    MULADD(at[23], at[35]);    MULADD(at[24], at[34]);    MULADD(at[25], at[33]);    MULADD(at[26], at[32]); 
   COMBA_STORE(C->dp[26]);
   /* 27 */
   COMBA_FORWARD;
   MULADD(at[0], at[59]);    MULADD(at[1], at[58]);    MULADD(at[2], at[57]);    MULADD(at[3], at[56]);    MULADD(at[4], at[55]);    MULADD(at[5], at[54]);    MULADD(at[6], at[53]);    MULADD(at[7], at[52]);    MULADD(at[8], at[51]);    MULADD(at[9], at[50]);    MULADD(at[10], at[49]);    MULADD(at[11], at[48]);    MULADD(at[12], at[47]);    MULADD(at[13], at[46]);    MULADD(at[14], at[45]);    MULADD(at[15], at[44]);    MULADD(at[16], at[43]);    MULADD(at[17], at[42]);    MULADD(at[18], at[41]);    MULADD(at[19], at[40]);    MULADD(at[20], at[39]);    MULADD(at[21], at[38]);    MULADD(at[22], at[37]);    MULADD(at[23], at[36]);    MULADD(at[24], at[35]);    MULADD(at[25], at[34]);    MULADD(at[26], at[33]);    MULADD(at[27], at[32]); 
   COMBA_STORE(C->dp[27]);
   /* 28 */
   COMBA_FORWARD;
   MULADD(at[0], at[60]);    MULADD(at[1], at[59]);    MULADD(at[2], at[58]);    MULADD(at[3], at[57]);    MULADD(at[4], at[56]);    MULADD(at[5], at[55]);    MULADD(at[6], at[54]);    MULADD(at[7], at[53]);    MULADD(at[8], at[52]);    MULADD(at[9], at[51]);    MULADD(at[10], at[50]);    MULADD(at[11], at[49]);    MULADD(at[12], at[48]);    MULADD(at[13], at[47]);    MULADD(at[14], at[46]);    MULADD(at[15], at[45]);    MULADD(at[16], at[44]);    MULADD(at[17], at[43]);    MULADD(at[18], at[42]);    MULADD(at[19], at[41]);    MULADD(at[20], at[40]);    MULADD(at[21], at[39]);    MULADD(at[22], at[38]);    MULADD(at[23], at[37]);    MULADD(at[24], at[36]);    MULADD(at[25], at[35]);    MULADD(at[26], at[34]);    MULADD(at[27], at[33]);    MULADD(at[28], at[32]); 
   COMBA_STORE(C->dp[28]);
   /* 29 */
   COMBA_FORWARD;
   MULADD(at[0], at[61]);    MULADD(at[1], at[60]);    MULADD(at[2], at[59]);    MULADD(at[3], at[58]);    MULADD(at[4], at[57]);    MULADD(at[5], at[56]);    MULADD(at[6], at[55]);    MULADD(at[7], at[54]);    MULADD(at[8], at[53]);    MULADD(at[9], at[52]);    MULADD(at[10], at[51]);    MULADD(at[11], at[50]);    MULADD(at[12], at[49]);    MULADD(at[13], at[48]);    MULADD(at[14], at[47]);    MULADD(at[15], at[46]);    MULADD(at[16], at[45]);    MULADD(at[17], at[44]);    MULADD(at[18], at[43]);    MULADD(at[19], at[42]);    MULADD(at[20], at[41]);    MULADD(at[21], at[40]);    MULADD(at[22], at[39]);    MULADD(at[23], at[38]);    MULADD(at[24], at[37]);    MULADD(at[25], at[36]);    MULADD(at[26], at[35]);    MULADD(at[27], at[34]);    MULADD(at[28], at[33]);    MULADD(at[29], at[32]); 
   COMBA_STORE(C->dp[29]);
   /* 30 */
   COMBA_FORWARD;
   MULADD(at[0], at[62]);    MULADD(at[1], at[61]);    MULADD(at[2], at[60]);    MULADD(at[3], at[59]);    MULADD(at[4], at[58]);    MULADD(at[5], at[57]);    MULADD(at[6], at[56]);    MULADD(at[7], at[55]);    MULADD(at[8], at[54]);    MULADD(at[9], at[53]);    MULADD(at[10], at[52]);    MULADD(at[11], at[51]);    MULADD(at[12], at[50]);    MULADD(at[13], at[49]);    MULADD(at[14], at[48]);    MULADD(at[15], at[47]);    MULADD(at[16], at[46]);    MULADD(at[17], at[45]);    MULADD(at[18], at[44]);    MULADD(at[19], at[43]);    MULADD(at[20], at[42]);    MULADD(at[21], at[41]);    MULADD(at[22], at[40]);    MULADD(at[23], at[39]);    MULADD(at[24], at[38]);    MULADD(at[25], at[37]);    MULADD(at[26], at[36]);    MULADD(at[27], at[35]);    MULADD(at[28], at[34]);    MULADD(at[29], at[33]);    MULADD(at[30], at[32]); 
   COMBA_STORE(C->dp[30]);
   /* 31 */
   COMBA_FORWARD;
   MULADD(at[0], at[63]);    MULADD(at[1], at[62]);    MULADD(at[2], at[61]);    MULADD(at[3], at[60]);    MULADD(at[4], at[59]);    MULADD(at[5], at[58]);    MULADD(at[6], at[57]);    MULADD(at[7], at[56]);    MULADD(at[8], at[55]);    MULADD(at[9], at[54]);    MULADD(at[10], at[53]);    MULADD(at[11], at[52]);    MULADD(at[12], at[51]);    MULADD(at[13], at[50]);    MULADD(at[14], at[49]);    MULADD(at[15], at[48]);    MULADD(at[16], at[47]);    MULADD(at[17], at[46]);    MULADD(at[18], at[45]);    MULADD(at[19], at[44]);    MULADD(at[20], at[43]);    MULADD(at[21], at[42]);    MULADD(at[22], at[41]);    MULADD(at[23], at[40]);    MULADD(at[24], at[39]);    MULADD(at[25], at[38]);    MULADD(at[26], at[37]);    MULADD(at[27], at[36]);    MULADD(at[28], at[35]);    MULADD(at[29], at[34]);    MULADD(at[30], at[33]);    MULADD(at[31], at[32]); 
   COMBA_STORE(C->dp[31]);
   /* 32 */
   COMBA_FORWARD;
   MULADD(at[1], at[63]);    MULADD(at[2], at[62]);    MULADD(at[3], at[61]);    MULADD(at[4], at[60]);    MULADD(at[5], at[59]);    MULADD(at[6], at[58]);    MULADD(at[7], at[57]);    MULADD(at[8], at[56]);    MULADD(at[9], at[55]);    MULADD(at[10], at[54]);    MULADD(at[11], at[53]);    MULADD(at[12], at[52]);    MULADD(at[13], at[51]);    MULADD(at[14], at[50]);    MULADD(at[15], at[49]);    MULADD(at[16], at[48]);    MULADD(at[17], at[47]);    MULADD(at[18], at[46]);    MULADD(at[19], at[45]);    MULADD(at[20], at[44]);    MULADD(at[21], at[43]);    MULADD(at[22], at[42]);    MULADD(at[23], at[41]);    MULADD(at[24], at[40]);    MULADD(at[25], at[39]);    MULADD(at[26], at[38]);    MULADD(at[27], at[37]);    MULADD(at[28], at[36]);    MULADD(at[29], at[35]);    MULADD(at[30], at[34]);    MULADD(at[31], at[33]); 
   COMBA_STORE(C->dp[32]);
   /* 33 */
   COMBA_FORWARD;
   MULADD(at[2], at[63]);    MULADD(at[3], at[62]);    MULADD(at[4], at[61]);    MULADD(at[5], at[60]);    MULADD(at[6], at[59]);    MULADD(at[7], at[58]);    MULADD(at[8], at[57]);    MULADD(at[9], at[56]);    MULADD(at[10], at[55]);    MULADD(at[11], at[54]);    MULADD(at[12], at[53]);    MULADD(at[13], at[52]);    MULADD(at[14], at[51]);    MULADD(at[15], at[50]);    MULADD(at[16], at[49]);    MULADD(at[17], at[48]);    MULADD(at[18], at[47]);    MULADD(at[19], at[46]);    MULADD(at[20], at[45]);    MULADD(at[21], at[44]);    MULADD(at[22], at[43]);    MULADD(at[23], at[42]);    MULADD(at[24], at[41]);    MULADD(at[25], at[40]);    MULADD(at[26], at[39]);    MULADD(at[27], at[38]);    MULADD(at[28], at[37]);    MULADD(at[29], at[36]);    MULADD(at[30], at[35]);    MULADD(at[31], at[34]); 
   COMBA_STORE(C->dp[33]);
   /* 34 */
   COMBA_FORWARD;
   MULADD(at[3], at[63]);    MULADD(at[4], at[62]);    MULADD(at[5], at[61]);    MULADD(at[6], at[60]);    MULADD(at[7], at[59]);    MULADD(at[8], at[58]);    MULADD(at[9], at[57]);    MULADD(at[10], at[56]);    MULADD(at[11], at[55]);    MULADD(at[12], at[54]);    MULADD(at[13], at[53]);    MULADD(at[14], at[52]);    MULADD(at[15], at[51]);    MULADD(at[16], at[50]);    MULADD(at[17], at[49]);    MULADD(at[18], at[48]);    MULADD(at[19], at[47]);    MULADD(at[20], at[46]);    MULADD(at[21], at[45]);    MULADD(at[22], at[44]);    MULADD(at[23], at[43]);    MULADD(at[24], at[42]);    MULADD(at[25], at[41]);    MULADD(at[26], at[40]);    MULADD(at[27], at[39]);    MULADD(at[28], at[38]);    MULADD(at[29], at[37]);    MULADD(at[30], at[36]);    MULADD(at[31], at[35]); 
   COMBA_STORE(C->dp[34]);
   /* 35 */
   COMBA_FORWARD;
   MULADD(at[4], at[63]);    MULADD(at[5], at[62]);    MULADD(at[6], at[61]);    MULADD(at[7], at[60]);    MULADD(at[8], at[59]);    MULADD(at[9], at[58]);    MULADD(at[10], at[57]);    MULADD(at[11], at[56]);    MULADD(at[12], at[55]);    MULADD(at[13], at[54]);    MULADD(at[14], at[53]);    MULADD(at[15], at[52]);    MULADD(at[16], at[51]);    MULADD(at[17], at[50]);    MULADD(at[18], at[49]);    MULADD(at[19], at[48]);    MULADD(at[20], at[47]);    MULADD(at[21], at[46]);    MULADD(at[22], at[45]);    MULADD(at[23], at[44]);    MULADD(at[24], at[43]);    MULADD(at[25], at[42]);    MULADD(at[26], at[41]);    MULADD(at[27], at[40]);    MULADD(at[28], at[39]);    MULADD(at[29], at[38]);    MULADD(at[30], at[37]);    MULADD(at[31], at[36]); 
   COMBA_STORE(C->dp[35]);
   /* 36 */
   COMBA_FORWARD;
   MULADD(at[5], at[63]);    MULADD(at[6], at[62]);    MULADD(at[7], at[61]);    MULADD(at[8], at[60]);    MULADD(at[9], at[59]);    MULADD(at[10], at[58]);    MULADD(at[11], at[57]);    MULADD(at[12], at[56]);    MULADD(at[13], at[55]);    MULADD(at[14], at[54]);    MULADD(at[15], at[53]);    MULADD(at[16], at[52]);    MULADD(at[17], at[51]);    MULADD(at[18], at[50]);    MULADD(at[19], at[49]);    MULADD(at[20], at[48]);    MULADD(at[21], at[47]);    MULADD(at[22], at[46]);    MULADD(at[23], at[45]);    MULADD(at[24], at[44]);    MULADD(at[25], at[43]);    MULADD(at[26], at[42]);    MULADD(at[27], at[41]);    MULADD(at[28], at[40]);    MULADD(at[29], at[39]);    MULADD(at[30], at[38]);    MULADD(at[31], at[37]); 
   COMBA_STORE(C->dp[36]);
   /* 37 */
   COMBA_FORWARD;
   MULADD(at[6], at[63]);    MULADD(at[7], at[62]);    MULADD(at[8], at[61]);    MULADD(at[9], at[60]);    MULADD(at[10], at[59]);    MULADD(at[11], at[58]);    MULADD(at[12], at[57]);    MULADD(at[13], at[56]);    MULADD(at[14], at[55]);    MULADD(at[15], at[54]);    MULADD(at[16], at[53]);    MULADD(at[17], at[52]);    MULADD(at[18], at[51]);    MULADD(at[19], at[50]);    MULADD(at[20], at[49]);    MULADD(at[21], at[48]);    MULADD(at[22], at[47]);    MULADD(at[23], at[46]);    MULADD(at[24], at[45]);    MULADD(at[25], at[44]);    MULADD(at[26], at[43]);    MULADD(at[27], at[42]);    MULADD(at[28], at[41]);    MULADD(at[29], at[40]);    MULADD(at[30], at[39]);    MULADD(at[31], at[38]); 
   COMBA_STORE(C->dp[37]);
   /* 38 */
   COMBA_FORWARD;
   MULADD(at[7], at[63]);    MULADD(at[8], at[62]);    MULADD(at[9], at[61]);    MULADD(at[10], at[60]);    MULADD(at[11], at[59]);    MULADD(at[12], at[58]);    MULADD(at[13], at[57]);    MULADD(at[14], at[56]);    MULADD(at[15], at[55]);    MULADD(at[16], at[54]);    MULADD(at[17], at[53]);    MULADD(at[18], at[52]);    MULADD(at[19], at[51]);    MULADD(at[20], at[50]);    MULADD(at[21], at[49]);    MULADD(at[22], at[48]);    MULADD(at[23], at[47]);    MULADD(at[24], at[46]);    MULADD(at[25], at[45]);    MULADD(at[26], at[44]);    MULADD(at[27], at[43]);    MULADD(at[28], at[42]);    MULADD(at[29], at[41]);    MULADD(at[30], at[40]);    MULADD(at[31], at[39]); 
   COMBA_STORE(C->dp[38]);

   /* early out at 40 digits, 40*32==1280, or two 640 bit operands */
   if (out_size <= 40) { COMBA_STORE2(C->dp[39]); C->used = 40; C->sign = A->sign ^ B->sign; fp_clamp(C); COMBA_FINI; return FP_OKAY; }

   /* 39 */
   COMBA_FORWARD;
   MULADD(at[8], at[63]);    MULADD(at[9], at[62]);    MULADD(at[10], at[61]);    MULADD(at[11], at[60]);    MULADD(at[12], at[59]);    MULADD(at[13], at[58]);    MULADD(at[14], at[57]);    MULADD(at[15], at[56]);    MULADD(at[16], at[55]);    MULADD(at[17], at[54]);    MULADD(at[18], at[53]);    MULADD(at[19], at[52]);    MULADD(at[20], at[51]);    MULADD(at[21], at[50]);    MULADD(at[22], at[49]);    MULADD(at[23], at[48]);    MULADD(at[24], at[47]);    MULADD(at[25], at[46]);    MULADD(at[26], at[45]);    MULADD(at[27], at[44]);    MULADD(at[28], at[43]);    MULADD(at[29], at[42]);    MULADD(at[30], at[41]);    MULADD(at[31], at[40]); 
   COMBA_STORE(C->dp[39]);
   /* 40 */
   COMBA_FORWARD;
   MULADD(at[9], at[63]);    MULADD(at[10], at[62]);    MULADD(at[11], at[61]);    MULADD(at[12], at[60]);    MULADD(at[13], at[59]);    MULADD(at[14], at[58]);    MULADD(at[15], at[57]);    MULADD(at[16], at[56]);    MULADD(at[17], at[55]);    MULADD(at[18], at[54]);    MULADD(at[19], at[53]);    MULADD(at[20], at[52]);    MULADD(at[21], at[51]);    MULADD(at[22], at[50]);    MULADD(at[23], at[49]);    MULADD(at[24], at[48]);    MULADD(at[25], at[47]);    MULADD(at[26], at[46]);    MULADD(at[27], at[45]);    MULADD(at[28], at[44]);    MULADD(at[29], at[43]);    MULADD(at[30], at[42]);    MULADD(at[31], at[41]); 
   COMBA_STORE(C->dp[40]);
   /* 41 */
   COMBA_FORWARD;
   MULADD(at[10], at[63]);    MULADD(at[11], at[62]);    MULADD(at[12], at[61]);    MULADD(at[13], at[60]);    MULADD(at[14], at[59]);    MULADD(at[15], at[58]);    MULADD(at[16], at[57]);    MULADD(at[17], at[56]);    MULADD(at[18], at[55]);    MULADD(at[19], at[54]);    MULADD(at[20], at[53]);    MULADD(at[21], at[52]);    MULADD(at[22], at[51]);    MULADD(at[23], at[50]);    MULADD(at[24], at[49]);    MULADD(at[25], at[48]);    MULADD(at[26], at[47]);    MULADD(at[27], at[46]);    MULADD(at[28], at[45]);    MULADD(at[29], at[44]);    MULADD(at[30], at[43]);    MULADD(at[31], at[42]); 
   COMBA_STORE(C->dp[41]);
   /* 42 */
   COMBA_FORWARD;
   MULADD(at[11], at[63]);    MULADD(at[12], at[62]);    MULADD(at[13], at[61]);    MULADD(at[14], at[60]);    MULADD(at[15], at[59]);    MULADD(at[16], at[58]);    MULADD(at[17], at[57]);    MULADD(at[18], at[56]);    MULADD(at[19], at[55]);    MULADD(at[20], at[54]);    MULADD(at[21], at[53]);    MULADD(at[22], at[52]);    MULADD(at[23], at[51]);    MULADD(at[24], at[50]);    MULADD(at[25], at[49]);    MULADD(at[26], at[48]);    MULADD(at[27], at[47]);    MULADD(at[28], at[46]);    MULADD(at[29], at[45]);    MULADD(at[30], at[44]);    MULADD(at[31], at[43]); 
   COMBA_STORE(C->dp[42]);
   /* 43 */
   COMBA_FORWARD;
   MULADD(at[12], at[63]);    MULADD(at[13], at[62]);    MULADD(at[14], at[61]);    MULADD(at[15], at[60]);    MULADD(at[16], at[59]);    MULADD(at[17], at[58]);    MULADD(at[18], at[57]);    MULADD(at[19], at[56]);    MULADD(at[20], at[55]);    MULADD(at[21], at[54]);    MULADD(at[22], at[53]);    MULADD(at[23], at[52]);    MULADD(at[24], at[51]);    MULADD(at[25], at[50]);    MULADD(at[26], at[49]);    MULADD(at[27], at[48]);    MULADD(at[28], at[47]);    MULADD(at[29], at[46]);    MULADD(at[30], at[45]);    MULADD(at[31], at[44]); 
   COMBA_STORE(C->dp[43]);
   /* 44 */
   COMBA_FORWARD;
   MULADD(at[13], at[63]);    MULADD(at[14], at[62]);    MULADD(at[15], at[61]);    MULADD(at[16], at[60]);    MULADD(at[17], at[59]);    MULADD(at[18], at[58]);    MULADD(at[19], at[57]);    MULADD(at[20], at[56]);    MULADD(at[21], at[55]);    MULADD(at[22], at[54]);    MULADD(at[23], at[53]);    MULADD(at[24], at[52]);    MULADD(at[25], at[51]);    MULADD(at[26], at[50]);    MULADD(at[27], at[49]);    MULADD(at[28], at[48]);    MULADD(at[29], at[47]);    MULADD(at[30], at[46]);    MULADD(at[31], at[45]); 
   COMBA_STORE(C->dp[44]);
   /* 45 */
   COMBA_FORWARD;
   MULADD(at[14], at[63]);    MULADD(at[15], at[62]);    MULADD(at[16], at[61]);    MULADD(at[17], at[60]);    MULADD(at[18], at[59]);    MULADD(at[19], at[58]);    MULADD(at[20], at[57]);    MULADD(at[21], at[56]);    MULADD(at[22], at[55]);    MULADD(at[23], at[54]);    MULADD(at[24], at[53]);    MULADD(at[25], at[52]);    MULADD(at[26], at[51]);    MULADD(at[27], at[50]);    MULADD(at[28], at[49]);    MULADD(at[29], at[48]);    MULADD(at[30], at[47]);    MULADD(at[31], at[46]); 
   COMBA_STORE(C->dp[45]);
   /* 46 */
   COMBA_FORWARD;
   MULADD(at[15], at[63]);    MULADD(at[16], at[62]);    MULADD(at[17], at[61]);    MULADD(at[18], at[60]);    MULADD(at[19], at[59]);    MULADD(at[20], at[58]);    MULADD(at[21], at[57]);    MULADD(at[22], at[56]);    MULADD(at[23], at[55]);    MULADD(at[24], at[54]);    MULADD(at[25], at[53]);    MULADD(at[26], at[52]);    MULADD(at[27], at[51]);    MULADD(at[28], at[50]);    MULADD(at[29], at[49]);    MULADD(at[30], at[48]);    MULADD(at[31], at[47]); 
   COMBA_STORE(C->dp[46]);

   /* early out at 48 digits, 48*32==1536, or two 768 bit operands */
   if (out_size <= 48) { COMBA_STORE2(C->dp[47]); C->used = 48; C->sign = A->sign ^ B->sign; fp_clamp(C); COMBA_FINI; return FP_OKAY; }

   /* 47 */
   COMBA_FORWARD;
   MULADD(at[16], at[63]);    MULADD(at[17], at[62]);    MULADD(at[18], at[61]);    MULADD(at[19], at[60]);    MULADD(at[20], at[59]);    MULADD(at[21], at[58]);    MULADD(at[22], at[57]);    MULADD(at[23], at[56]);    MULADD(at[24], at[55]);    MULADD(at[25], at[54]);    MULADD(at[26], at[53]);    MULADD(at[27], at[52]);    MULADD(at[28], at[51]);    MULADD(at[29], at[50]);    MULADD(at[30], at[49]);    MULADD(at[31], at[48]); 
   COMBA_STORE(C->dp[47]);
   /* 48 */
   COMBA_FORWARD;
   MULADD(at[17], at[63]);    MULADD(at[18], at[62]);    MULADD(at[19], at[61]);    MULADD(at[20], at[60]);    MULADD(at[21], at[59]);    MULADD(at[22], at[58]);    MULADD(at[23], at[57]);    MULADD(at[24], at[56]);    MULADD(at[25], at[55]);    MULADD(at[26], at[54]);    MULADD(at[27], at[53]);    MULADD(at[28], at[52]);    MULADD(at[29], at[51]);    MULADD(at[30], at[50]);    MULADD(at[31], at[49]); 
   COMBA_STORE(C->dp[48]);
   /* 49 */
   COMBA_FORWARD;
   MULADD(at[18], at[63]);    MULADD(at[19], at[62]);    MULADD(at[20], at[61]);    MULADD(at[21], at[60]);    MULADD(at[22], at[59]);    MULADD(at[23], at[58]);    MULADD(at[24], at[57]);    MULADD(at[25], at[56]);    MULADD(at[26], at[55]);    MULADD(at[27], at[54]);    MULADD(at[28], at[53]);    MULADD(at[29], at[52]);    MULADD(at[30], at[51]);    MULADD(at[31], at[50]); 
   COMBA_STORE(C->dp[49]);
   /* 50 */
   COMBA_FORWARD;
   MULADD(at[19], at[63]);    MULADD(at[20], at[62]);    MULADD(at[21], at[61]);    MULADD(at[22], at[60]);    MULADD(at[23], at[59]);    MULADD(at[24], at[58]);    MULADD(at[25], at[57]);    MULADD(at[26], at[56]);    MULADD(at[27], at[55]);    MULADD(at[28], at[54]);    MULADD(at[29], at[53]);    MULADD(at[30], at[52]);    MULADD(at[31], at[51]); 
   COMBA_STORE(C->dp[50]);
   /* 51 */
   COMBA_FORWARD;
   MULADD(at[20], at[63]);    MULADD(at[21], at[62]);    MULADD(at[22], at[61]);    MULADD(at[23], at[60]);    MULADD(at[24], at[59]);    MULADD(at[25], at[58]);    MULADD(at[26], at[57]);    MULADD(at[27], at[56]);    MULADD(at[28], at[55]);    MULADD(at[29], at[54]);    MULADD(at[30], at[53]);    MULADD(at[31], at[52]); 
   COMBA_STORE(C->dp[51]);
   /* 52 */
   COMBA_FORWARD;
   MULADD(at[21], at[63]);    MULADD(at[22], at[62]);    MULADD(at[23], at[61]);    MULADD(at[24], at[60]);    MULADD(at[25], at[59]);    MULADD(at[26], at[58]);    MULADD(at[27], at[57]);    MULADD(at[28], at[56]);    MULADD(at[29], at[55]);    MULADD(at[30], at[54]);    MULADD(at[31], at[53]); 
   COMBA_STORE(C->dp[52]);
   /* 53 */
   COMBA_FORWARD;
   MULADD(at[22], at[63]);    MULADD(at[23], at[62]);    MULADD(at[24], at[61]);    MULADD(at[25], at[60]);    MULADD(at[26], at[59]);    MULADD(at[27], at[58]);    MULADD(at[28], at[57]);    MULADD(at[29], at[56]);    MULADD(at[30], at[55]);    MULADD(at[31], at[54]); 
   COMBA_STORE(C->dp[53]);
   /* 54 */
   COMBA_FORWARD;
   MULADD(at[23], at[63]);    MULADD(at[24], at[62]);    MULADD(at[25], at[61]);    MULADD(at[26], at[60]);    MULADD(at[27], at[59]);    MULADD(at[28], at[58]);    MULADD(at[29], at[57]);    MULADD(at[30], at[56]);    MULADD(at[31], at[55]); 
   COMBA_STORE(C->dp[54]);

   /* early out at 56 digits, 56*32==1792, or two 896 bit operands */
   if (out_size <= 56) { COMBA_STORE2(C->dp[55]); C->used = 56; C->sign = A->sign ^ B->sign; fp_clamp(C); COMBA_FINI; return FP_OKAY; }

   /* 55 */
   COMBA_FORWARD;
   MULADD(at[24], at[63]);    MULADD(at[25], at[62]);    MULADD(at[26], at[61]);    MULADD(at[27], at[60]);    MULADD(at[28], at[59]);    MULADD(at[29], at[58]);    MULADD(at[30], at[57]);    MULADD(at[31], at[56]); 
   COMBA_STORE(C->dp[55]);
   /* 56 */
   COMBA_FORWARD;
   MULADD(at[25], at[63]);    MULADD(at[26], at[62]);    MULADD(at[27], at[61]);    MULADD(at[28], at[60]);    MULADD(at[29], at[59]);    MULADD(at[30], at[58]);    MULADD(at[31], at[57]); 
   COMBA_STORE(C->dp[56]);
   /* 57 */
   COMBA_FORWARD;
   MULADD(at[26], at[63]);    MULADD(at[27], at[62]);    MULADD(at[28], at[61]);    MULADD(at[29], at[60]);    MULADD(at[30], at[59]);    MULADD(at[31], at[58]); 
   COMBA_STORE(C->dp[57]);
   /* 58 */
   COMBA_FORWARD;
   MULADD(at[27], at[63]);    MULADD(at[28], at[62]);    MULADD(at[29], at[61]);    MULADD(at[30], at[60]);    MULADD(at[31], at[59]); 
   COMBA_STORE(C->dp[58]);
   /* 59 */
   COMBA_FORWARD;
   MULADD(at[28], at[63]);    MULADD(at[29], at[62]);    MULADD(at[30], at[61]);    MULADD(at[31], at[60]); 
   COMBA_STORE(C->dp[59]);
   /* 60 */
   COMBA_FORWARD;
   MULADD(at[29], at[63]);    MULADD(at[30], at[62]);    MULADD(at[31], at[61]); 
   COMBA_STORE(C->dp[60]);
   /* 61 */
   COMBA_FORWARD;
   MULADD(at[30], at[63]);    MULADD(at[31], at[62]); 
   COMBA_STORE(C->dp[61]);
   /* 62 */
   COMBA_FORWARD;
   MULADD(at[31], at[63]); 
   COMBA_STORE(C->dp[62]);
   COMBA_STORE2(C->dp[63]);
   C->used = 64;
   C->sign = A->sign ^ B->sign;
   fp_clamp(C);
   COMBA_FINI;

#ifdef WOLFSSL_SMALL_STACK
   XFREE(at, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif
