/* fp_sqr_comba_28.i
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



#ifdef TFM_SQR28
int fp_sqr_comba28(fp_int *A, fp_int *B)
{
   fp_digit *a, c0, c1, c2, sc0 = 0, sc1 = 0, sc2 = 0;
#ifdef TFM_ISO
   fp_word tt;
#endif
#ifndef WOLFSSL_SMALL_STACK
   fp_digit b[56];
#else
   fp_digit *b;
#endif

#ifdef WOLFSSL_SMALL_STACK
   b = (fp_digit*)XMALLOC(sizeof(fp_digit) * 56, NULL, DYNAMIC_TYPE_TMP_BUFFER);
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
   SQRADDSC(a[0], a[17]); SQRADDAC(a[1], a[16]); SQRADDAC(a[2], a[15]); SQRADDAC(a[3], a[14]); SQRADDAC(a[4], a[13]); SQRADDAC(a[5], a[12]); SQRADDAC(a[6], a[11]); SQRADDAC(a[7], a[10]); SQRADDAC(a[8], a[9]); SQRADDDB; 
   COMBA_STORE(b[17]);

   /* output 18 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[18]); SQRADDAC(a[1], a[17]); SQRADDAC(a[2], a[16]); SQRADDAC(a[3], a[15]); SQRADDAC(a[4], a[14]); SQRADDAC(a[5], a[13]); SQRADDAC(a[6], a[12]); SQRADDAC(a[7], a[11]); SQRADDAC(a[8], a[10]); SQRADDDB; SQRADD(a[9], a[9]); 
   COMBA_STORE(b[18]);

   /* output 19 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[19]); SQRADDAC(a[1], a[18]); SQRADDAC(a[2], a[17]); SQRADDAC(a[3], a[16]); SQRADDAC(a[4], a[15]); SQRADDAC(a[5], a[14]); SQRADDAC(a[6], a[13]); SQRADDAC(a[7], a[12]); SQRADDAC(a[8], a[11]); SQRADDAC(a[9], a[10]); SQRADDDB; 
   COMBA_STORE(b[19]);

   /* output 20 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[20]); SQRADDAC(a[1], a[19]); SQRADDAC(a[2], a[18]); SQRADDAC(a[3], a[17]); SQRADDAC(a[4], a[16]); SQRADDAC(a[5], a[15]); SQRADDAC(a[6], a[14]); SQRADDAC(a[7], a[13]); SQRADDAC(a[8], a[12]); SQRADDAC(a[9], a[11]); SQRADDDB; SQRADD(a[10], a[10]); 
   COMBA_STORE(b[20]);

   /* output 21 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[21]); SQRADDAC(a[1], a[20]); SQRADDAC(a[2], a[19]); SQRADDAC(a[3], a[18]); SQRADDAC(a[4], a[17]); SQRADDAC(a[5], a[16]); SQRADDAC(a[6], a[15]); SQRADDAC(a[7], a[14]); SQRADDAC(a[8], a[13]); SQRADDAC(a[9], a[12]); SQRADDAC(a[10], a[11]); SQRADDDB; 
   COMBA_STORE(b[21]);

   /* output 22 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[22]); SQRADDAC(a[1], a[21]); SQRADDAC(a[2], a[20]); SQRADDAC(a[3], a[19]); SQRADDAC(a[4], a[18]); SQRADDAC(a[5], a[17]); SQRADDAC(a[6], a[16]); SQRADDAC(a[7], a[15]); SQRADDAC(a[8], a[14]); SQRADDAC(a[9], a[13]); SQRADDAC(a[10], a[12]); SQRADDDB; SQRADD(a[11], a[11]); 
   COMBA_STORE(b[22]);

   /* output 23 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[23]); SQRADDAC(a[1], a[22]); SQRADDAC(a[2], a[21]); SQRADDAC(a[3], a[20]); SQRADDAC(a[4], a[19]); SQRADDAC(a[5], a[18]); SQRADDAC(a[6], a[17]); SQRADDAC(a[7], a[16]); SQRADDAC(a[8], a[15]); SQRADDAC(a[9], a[14]); SQRADDAC(a[10], a[13]); SQRADDAC(a[11], a[12]); SQRADDDB; 
   COMBA_STORE(b[23]);

   /* output 24 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[24]); SQRADDAC(a[1], a[23]); SQRADDAC(a[2], a[22]); SQRADDAC(a[3], a[21]); SQRADDAC(a[4], a[20]); SQRADDAC(a[5], a[19]); SQRADDAC(a[6], a[18]); SQRADDAC(a[7], a[17]); SQRADDAC(a[8], a[16]); SQRADDAC(a[9], a[15]); SQRADDAC(a[10], a[14]); SQRADDAC(a[11], a[13]); SQRADDDB; SQRADD(a[12], a[12]); 
   COMBA_STORE(b[24]);

   /* output 25 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[25]); SQRADDAC(a[1], a[24]); SQRADDAC(a[2], a[23]); SQRADDAC(a[3], a[22]); SQRADDAC(a[4], a[21]); SQRADDAC(a[5], a[20]); SQRADDAC(a[6], a[19]); SQRADDAC(a[7], a[18]); SQRADDAC(a[8], a[17]); SQRADDAC(a[9], a[16]); SQRADDAC(a[10], a[15]); SQRADDAC(a[11], a[14]); SQRADDAC(a[12], a[13]); SQRADDDB; 
   COMBA_STORE(b[25]);

   /* output 26 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[26]); SQRADDAC(a[1], a[25]); SQRADDAC(a[2], a[24]); SQRADDAC(a[3], a[23]); SQRADDAC(a[4], a[22]); SQRADDAC(a[5], a[21]); SQRADDAC(a[6], a[20]); SQRADDAC(a[7], a[19]); SQRADDAC(a[8], a[18]); SQRADDAC(a[9], a[17]); SQRADDAC(a[10], a[16]); SQRADDAC(a[11], a[15]); SQRADDAC(a[12], a[14]); SQRADDDB; SQRADD(a[13], a[13]); 
   COMBA_STORE(b[26]);

   /* output 27 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[27]); SQRADDAC(a[1], a[26]); SQRADDAC(a[2], a[25]); SQRADDAC(a[3], a[24]); SQRADDAC(a[4], a[23]); SQRADDAC(a[5], a[22]); SQRADDAC(a[6], a[21]); SQRADDAC(a[7], a[20]); SQRADDAC(a[8], a[19]); SQRADDAC(a[9], a[18]); SQRADDAC(a[10], a[17]); SQRADDAC(a[11], a[16]); SQRADDAC(a[12], a[15]); SQRADDAC(a[13], a[14]); SQRADDDB; 
   COMBA_STORE(b[27]);

   /* output 28 */
   CARRY_FORWARD;
   SQRADDSC(a[1], a[27]); SQRADDAC(a[2], a[26]); SQRADDAC(a[3], a[25]); SQRADDAC(a[4], a[24]); SQRADDAC(a[5], a[23]); SQRADDAC(a[6], a[22]); SQRADDAC(a[7], a[21]); SQRADDAC(a[8], a[20]); SQRADDAC(a[9], a[19]); SQRADDAC(a[10], a[18]); SQRADDAC(a[11], a[17]); SQRADDAC(a[12], a[16]); SQRADDAC(a[13], a[15]); SQRADDDB; SQRADD(a[14], a[14]); 
   COMBA_STORE(b[28]);

   /* output 29 */
   CARRY_FORWARD;
   SQRADDSC(a[2], a[27]); SQRADDAC(a[3], a[26]); SQRADDAC(a[4], a[25]); SQRADDAC(a[5], a[24]); SQRADDAC(a[6], a[23]); SQRADDAC(a[7], a[22]); SQRADDAC(a[8], a[21]); SQRADDAC(a[9], a[20]); SQRADDAC(a[10], a[19]); SQRADDAC(a[11], a[18]); SQRADDAC(a[12], a[17]); SQRADDAC(a[13], a[16]); SQRADDAC(a[14], a[15]); SQRADDDB; 
   COMBA_STORE(b[29]);

   /* output 30 */
   CARRY_FORWARD;
   SQRADDSC(a[3], a[27]); SQRADDAC(a[4], a[26]); SQRADDAC(a[5], a[25]); SQRADDAC(a[6], a[24]); SQRADDAC(a[7], a[23]); SQRADDAC(a[8], a[22]); SQRADDAC(a[9], a[21]); SQRADDAC(a[10], a[20]); SQRADDAC(a[11], a[19]); SQRADDAC(a[12], a[18]); SQRADDAC(a[13], a[17]); SQRADDAC(a[14], a[16]); SQRADDDB; SQRADD(a[15], a[15]); 
   COMBA_STORE(b[30]);

   /* output 31 */
   CARRY_FORWARD;
   SQRADDSC(a[4], a[27]); SQRADDAC(a[5], a[26]); SQRADDAC(a[6], a[25]); SQRADDAC(a[7], a[24]); SQRADDAC(a[8], a[23]); SQRADDAC(a[9], a[22]); SQRADDAC(a[10], a[21]); SQRADDAC(a[11], a[20]); SQRADDAC(a[12], a[19]); SQRADDAC(a[13], a[18]); SQRADDAC(a[14], a[17]); SQRADDAC(a[15], a[16]); SQRADDDB; 
   COMBA_STORE(b[31]);

   /* output 32 */
   CARRY_FORWARD;
   SQRADDSC(a[5], a[27]); SQRADDAC(a[6], a[26]); SQRADDAC(a[7], a[25]); SQRADDAC(a[8], a[24]); SQRADDAC(a[9], a[23]); SQRADDAC(a[10], a[22]); SQRADDAC(a[11], a[21]); SQRADDAC(a[12], a[20]); SQRADDAC(a[13], a[19]); SQRADDAC(a[14], a[18]); SQRADDAC(a[15], a[17]); SQRADDDB; SQRADD(a[16], a[16]); 
   COMBA_STORE(b[32]);

   /* output 33 */
   CARRY_FORWARD;
   SQRADDSC(a[6], a[27]); SQRADDAC(a[7], a[26]); SQRADDAC(a[8], a[25]); SQRADDAC(a[9], a[24]); SQRADDAC(a[10], a[23]); SQRADDAC(a[11], a[22]); SQRADDAC(a[12], a[21]); SQRADDAC(a[13], a[20]); SQRADDAC(a[14], a[19]); SQRADDAC(a[15], a[18]); SQRADDAC(a[16], a[17]); SQRADDDB; 
   COMBA_STORE(b[33]);

   /* output 34 */
   CARRY_FORWARD;
   SQRADDSC(a[7], a[27]); SQRADDAC(a[8], a[26]); SQRADDAC(a[9], a[25]); SQRADDAC(a[10], a[24]); SQRADDAC(a[11], a[23]); SQRADDAC(a[12], a[22]); SQRADDAC(a[13], a[21]); SQRADDAC(a[14], a[20]); SQRADDAC(a[15], a[19]); SQRADDAC(a[16], a[18]); SQRADDDB; SQRADD(a[17], a[17]); 
   COMBA_STORE(b[34]);

   /* output 35 */
   CARRY_FORWARD;
   SQRADDSC(a[8], a[27]); SQRADDAC(a[9], a[26]); SQRADDAC(a[10], a[25]); SQRADDAC(a[11], a[24]); SQRADDAC(a[12], a[23]); SQRADDAC(a[13], a[22]); SQRADDAC(a[14], a[21]); SQRADDAC(a[15], a[20]); SQRADDAC(a[16], a[19]); SQRADDAC(a[17], a[18]); SQRADDDB; 
   COMBA_STORE(b[35]);

   /* output 36 */
   CARRY_FORWARD;
   SQRADDSC(a[9], a[27]); SQRADDAC(a[10], a[26]); SQRADDAC(a[11], a[25]); SQRADDAC(a[12], a[24]); SQRADDAC(a[13], a[23]); SQRADDAC(a[14], a[22]); SQRADDAC(a[15], a[21]); SQRADDAC(a[16], a[20]); SQRADDAC(a[17], a[19]); SQRADDDB; SQRADD(a[18], a[18]); 
   COMBA_STORE(b[36]);

   /* output 37 */
   CARRY_FORWARD;
   SQRADDSC(a[10], a[27]); SQRADDAC(a[11], a[26]); SQRADDAC(a[12], a[25]); SQRADDAC(a[13], a[24]); SQRADDAC(a[14], a[23]); SQRADDAC(a[15], a[22]); SQRADDAC(a[16], a[21]); SQRADDAC(a[17], a[20]); SQRADDAC(a[18], a[19]); SQRADDDB; 
   COMBA_STORE(b[37]);

   /* output 38 */
   CARRY_FORWARD;
   SQRADDSC(a[11], a[27]); SQRADDAC(a[12], a[26]); SQRADDAC(a[13], a[25]); SQRADDAC(a[14], a[24]); SQRADDAC(a[15], a[23]); SQRADDAC(a[16], a[22]); SQRADDAC(a[17], a[21]); SQRADDAC(a[18], a[20]); SQRADDDB; SQRADD(a[19], a[19]); 
   COMBA_STORE(b[38]);

   /* output 39 */
   CARRY_FORWARD;
   SQRADDSC(a[12], a[27]); SQRADDAC(a[13], a[26]); SQRADDAC(a[14], a[25]); SQRADDAC(a[15], a[24]); SQRADDAC(a[16], a[23]); SQRADDAC(a[17], a[22]); SQRADDAC(a[18], a[21]); SQRADDAC(a[19], a[20]); SQRADDDB; 
   COMBA_STORE(b[39]);

   /* output 40 */
   CARRY_FORWARD;
   SQRADDSC(a[13], a[27]); SQRADDAC(a[14], a[26]); SQRADDAC(a[15], a[25]); SQRADDAC(a[16], a[24]); SQRADDAC(a[17], a[23]); SQRADDAC(a[18], a[22]); SQRADDAC(a[19], a[21]); SQRADDDB; SQRADD(a[20], a[20]); 
   COMBA_STORE(b[40]);

   /* output 41 */
   CARRY_FORWARD;
   SQRADDSC(a[14], a[27]); SQRADDAC(a[15], a[26]); SQRADDAC(a[16], a[25]); SQRADDAC(a[17], a[24]); SQRADDAC(a[18], a[23]); SQRADDAC(a[19], a[22]); SQRADDAC(a[20], a[21]); SQRADDDB; 
   COMBA_STORE(b[41]);

   /* output 42 */
   CARRY_FORWARD;
   SQRADDSC(a[15], a[27]); SQRADDAC(a[16], a[26]); SQRADDAC(a[17], a[25]); SQRADDAC(a[18], a[24]); SQRADDAC(a[19], a[23]); SQRADDAC(a[20], a[22]); SQRADDDB; SQRADD(a[21], a[21]); 
   COMBA_STORE(b[42]);

   /* output 43 */
   CARRY_FORWARD;
   SQRADDSC(a[16], a[27]); SQRADDAC(a[17], a[26]); SQRADDAC(a[18], a[25]); SQRADDAC(a[19], a[24]); SQRADDAC(a[20], a[23]); SQRADDAC(a[21], a[22]); SQRADDDB; 
   COMBA_STORE(b[43]);

   /* output 44 */
   CARRY_FORWARD;
   SQRADDSC(a[17], a[27]); SQRADDAC(a[18], a[26]); SQRADDAC(a[19], a[25]); SQRADDAC(a[20], a[24]); SQRADDAC(a[21], a[23]); SQRADDDB; SQRADD(a[22], a[22]); 
   COMBA_STORE(b[44]);

   /* output 45 */
   CARRY_FORWARD;
   SQRADDSC(a[18], a[27]); SQRADDAC(a[19], a[26]); SQRADDAC(a[20], a[25]); SQRADDAC(a[21], a[24]); SQRADDAC(a[22], a[23]); SQRADDDB; 
   COMBA_STORE(b[45]);

   /* output 46 */
   CARRY_FORWARD;
   SQRADDSC(a[19], a[27]); SQRADDAC(a[20], a[26]); SQRADDAC(a[21], a[25]); SQRADDAC(a[22], a[24]); SQRADDDB; SQRADD(a[23], a[23]); 
   COMBA_STORE(b[46]);

   /* output 47 */
   CARRY_FORWARD;
   SQRADDSC(a[20], a[27]); SQRADDAC(a[21], a[26]); SQRADDAC(a[22], a[25]); SQRADDAC(a[23], a[24]); SQRADDDB; 
   COMBA_STORE(b[47]);

   /* output 48 */
   CARRY_FORWARD;
   SQRADDSC(a[21], a[27]); SQRADDAC(a[22], a[26]); SQRADDAC(a[23], a[25]); SQRADDDB; SQRADD(a[24], a[24]); 
   COMBA_STORE(b[48]);

   /* output 49 */
   CARRY_FORWARD;
   SQRADDSC(a[22], a[27]); SQRADDAC(a[23], a[26]); SQRADDAC(a[24], a[25]); SQRADDDB; 
   COMBA_STORE(b[49]);

   /* output 50 */
   CARRY_FORWARD;
   SQRADD2(a[23], a[27]); SQRADD2(a[24], a[26]); SQRADD(a[25], a[25]); 
   COMBA_STORE(b[50]);

   /* output 51 */
   CARRY_FORWARD;
   SQRADD2(a[24], a[27]); SQRADD2(a[25], a[26]); 
   COMBA_STORE(b[51]);

   /* output 52 */
   CARRY_FORWARD;
   SQRADD2(a[25], a[27]); SQRADD(a[26], a[26]); 
   COMBA_STORE(b[52]);

   /* output 53 */
   CARRY_FORWARD;
   SQRADD2(a[26], a[27]); 
   COMBA_STORE(b[53]);

   /* output 54 */
   CARRY_FORWARD;
   SQRADD(a[27], a[27]); 
   COMBA_STORE(b[54]);
   COMBA_STORE2(b[55]);
   COMBA_FINI;

   B->used = 56;
   B->sign = FP_ZPOS;
   XMEMCPY(B->dp, b, 56 * sizeof(fp_digit));
   fp_clamp(B);

#ifdef WOLFSSL_SMALL_STACK
   XFREE(b, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif


