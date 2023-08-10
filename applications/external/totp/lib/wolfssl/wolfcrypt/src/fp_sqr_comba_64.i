/* fp_sqr_comba_64.i
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



#ifdef TFM_SQR64
int fp_sqr_comba64(fp_int *A, fp_int *B)
{
   fp_digit *a, c0, c1, c2, sc0 = 0, sc1 = 0, sc2 = 0;
#ifdef TFM_ISO
   fp_word tt;
#endif
#ifndef WOLFSSL_SMALL_STACK
   fp_digit b[128];
#else
   fp_digit *b;
#endif

#ifdef WOLFSSL_SMALL_STACK
   b = (fp_digit*)XMALLOC(sizeof(fp_digit) * 128, NULL, DYNAMIC_TYPE_TMP_BUFFER);
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
   SQRADDSC(a[0], a[28]); SQRADDAC(a[1], a[27]); SQRADDAC(a[2], a[26]); SQRADDAC(a[3], a[25]); SQRADDAC(a[4], a[24]); SQRADDAC(a[5], a[23]); SQRADDAC(a[6], a[22]); SQRADDAC(a[7], a[21]); SQRADDAC(a[8], a[20]); SQRADDAC(a[9], a[19]); SQRADDAC(a[10], a[18]); SQRADDAC(a[11], a[17]); SQRADDAC(a[12], a[16]); SQRADDAC(a[13], a[15]); SQRADDDB; SQRADD(a[14], a[14]); 
   COMBA_STORE(b[28]);

   /* output 29 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[29]); SQRADDAC(a[1], a[28]); SQRADDAC(a[2], a[27]); SQRADDAC(a[3], a[26]); SQRADDAC(a[4], a[25]); SQRADDAC(a[5], a[24]); SQRADDAC(a[6], a[23]); SQRADDAC(a[7], a[22]); SQRADDAC(a[8], a[21]); SQRADDAC(a[9], a[20]); SQRADDAC(a[10], a[19]); SQRADDAC(a[11], a[18]); SQRADDAC(a[12], a[17]); SQRADDAC(a[13], a[16]); SQRADDAC(a[14], a[15]); SQRADDDB; 
   COMBA_STORE(b[29]);

   /* output 30 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[30]); SQRADDAC(a[1], a[29]); SQRADDAC(a[2], a[28]); SQRADDAC(a[3], a[27]); SQRADDAC(a[4], a[26]); SQRADDAC(a[5], a[25]); SQRADDAC(a[6], a[24]); SQRADDAC(a[7], a[23]); SQRADDAC(a[8], a[22]); SQRADDAC(a[9], a[21]); SQRADDAC(a[10], a[20]); SQRADDAC(a[11], a[19]); SQRADDAC(a[12], a[18]); SQRADDAC(a[13], a[17]); SQRADDAC(a[14], a[16]); SQRADDDB; SQRADD(a[15], a[15]); 
   COMBA_STORE(b[30]);

   /* output 31 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[31]); SQRADDAC(a[1], a[30]); SQRADDAC(a[2], a[29]); SQRADDAC(a[3], a[28]); SQRADDAC(a[4], a[27]); SQRADDAC(a[5], a[26]); SQRADDAC(a[6], a[25]); SQRADDAC(a[7], a[24]); SQRADDAC(a[8], a[23]); SQRADDAC(a[9], a[22]); SQRADDAC(a[10], a[21]); SQRADDAC(a[11], a[20]); SQRADDAC(a[12], a[19]); SQRADDAC(a[13], a[18]); SQRADDAC(a[14], a[17]); SQRADDAC(a[15], a[16]); SQRADDDB; 
   COMBA_STORE(b[31]);

   /* output 32 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[32]); SQRADDAC(a[1], a[31]); SQRADDAC(a[2], a[30]); SQRADDAC(a[3], a[29]); SQRADDAC(a[4], a[28]); SQRADDAC(a[5], a[27]); SQRADDAC(a[6], a[26]); SQRADDAC(a[7], a[25]); SQRADDAC(a[8], a[24]); SQRADDAC(a[9], a[23]); SQRADDAC(a[10], a[22]); SQRADDAC(a[11], a[21]); SQRADDAC(a[12], a[20]); SQRADDAC(a[13], a[19]); SQRADDAC(a[14], a[18]); SQRADDAC(a[15], a[17]); SQRADDDB; SQRADD(a[16], a[16]); 
   COMBA_STORE(b[32]);

   /* output 33 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[33]); SQRADDAC(a[1], a[32]); SQRADDAC(a[2], a[31]); SQRADDAC(a[3], a[30]); SQRADDAC(a[4], a[29]); SQRADDAC(a[5], a[28]); SQRADDAC(a[6], a[27]); SQRADDAC(a[7], a[26]); SQRADDAC(a[8], a[25]); SQRADDAC(a[9], a[24]); SQRADDAC(a[10], a[23]); SQRADDAC(a[11], a[22]); SQRADDAC(a[12], a[21]); SQRADDAC(a[13], a[20]); SQRADDAC(a[14], a[19]); SQRADDAC(a[15], a[18]); SQRADDAC(a[16], a[17]); SQRADDDB; 
   COMBA_STORE(b[33]);

   /* output 34 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[34]); SQRADDAC(a[1], a[33]); SQRADDAC(a[2], a[32]); SQRADDAC(a[3], a[31]); SQRADDAC(a[4], a[30]); SQRADDAC(a[5], a[29]); SQRADDAC(a[6], a[28]); SQRADDAC(a[7], a[27]); SQRADDAC(a[8], a[26]); SQRADDAC(a[9], a[25]); SQRADDAC(a[10], a[24]); SQRADDAC(a[11], a[23]); SQRADDAC(a[12], a[22]); SQRADDAC(a[13], a[21]); SQRADDAC(a[14], a[20]); SQRADDAC(a[15], a[19]); SQRADDAC(a[16], a[18]); SQRADDDB; SQRADD(a[17], a[17]); 
   COMBA_STORE(b[34]);

   /* output 35 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[35]); SQRADDAC(a[1], a[34]); SQRADDAC(a[2], a[33]); SQRADDAC(a[3], a[32]); SQRADDAC(a[4], a[31]); SQRADDAC(a[5], a[30]); SQRADDAC(a[6], a[29]); SQRADDAC(a[7], a[28]); SQRADDAC(a[8], a[27]); SQRADDAC(a[9], a[26]); SQRADDAC(a[10], a[25]); SQRADDAC(a[11], a[24]); SQRADDAC(a[12], a[23]); SQRADDAC(a[13], a[22]); SQRADDAC(a[14], a[21]); SQRADDAC(a[15], a[20]); SQRADDAC(a[16], a[19]); SQRADDAC(a[17], a[18]); SQRADDDB; 
   COMBA_STORE(b[35]);

   /* output 36 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[36]); SQRADDAC(a[1], a[35]); SQRADDAC(a[2], a[34]); SQRADDAC(a[3], a[33]); SQRADDAC(a[4], a[32]); SQRADDAC(a[5], a[31]); SQRADDAC(a[6], a[30]); SQRADDAC(a[7], a[29]); SQRADDAC(a[8], a[28]); SQRADDAC(a[9], a[27]); SQRADDAC(a[10], a[26]); SQRADDAC(a[11], a[25]); SQRADDAC(a[12], a[24]); SQRADDAC(a[13], a[23]); SQRADDAC(a[14], a[22]); SQRADDAC(a[15], a[21]); SQRADDAC(a[16], a[20]); SQRADDAC(a[17], a[19]); SQRADDDB; SQRADD(a[18], a[18]); 
   COMBA_STORE(b[36]);

   /* output 37 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[37]); SQRADDAC(a[1], a[36]); SQRADDAC(a[2], a[35]); SQRADDAC(a[3], a[34]); SQRADDAC(a[4], a[33]); SQRADDAC(a[5], a[32]); SQRADDAC(a[6], a[31]); SQRADDAC(a[7], a[30]); SQRADDAC(a[8], a[29]); SQRADDAC(a[9], a[28]); SQRADDAC(a[10], a[27]); SQRADDAC(a[11], a[26]); SQRADDAC(a[12], a[25]); SQRADDAC(a[13], a[24]); SQRADDAC(a[14], a[23]); SQRADDAC(a[15], a[22]); SQRADDAC(a[16], a[21]); SQRADDAC(a[17], a[20]); SQRADDAC(a[18], a[19]); SQRADDDB; 
   COMBA_STORE(b[37]);

   /* output 38 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[38]); SQRADDAC(a[1], a[37]); SQRADDAC(a[2], a[36]); SQRADDAC(a[3], a[35]); SQRADDAC(a[4], a[34]); SQRADDAC(a[5], a[33]); SQRADDAC(a[6], a[32]); SQRADDAC(a[7], a[31]); SQRADDAC(a[8], a[30]); SQRADDAC(a[9], a[29]); SQRADDAC(a[10], a[28]); SQRADDAC(a[11], a[27]); SQRADDAC(a[12], a[26]); SQRADDAC(a[13], a[25]); SQRADDAC(a[14], a[24]); SQRADDAC(a[15], a[23]); SQRADDAC(a[16], a[22]); SQRADDAC(a[17], a[21]); SQRADDAC(a[18], a[20]); SQRADDDB; SQRADD(a[19], a[19]); 
   COMBA_STORE(b[38]);

   /* output 39 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[39]); SQRADDAC(a[1], a[38]); SQRADDAC(a[2], a[37]); SQRADDAC(a[3], a[36]); SQRADDAC(a[4], a[35]); SQRADDAC(a[5], a[34]); SQRADDAC(a[6], a[33]); SQRADDAC(a[7], a[32]); SQRADDAC(a[8], a[31]); SQRADDAC(a[9], a[30]); SQRADDAC(a[10], a[29]); SQRADDAC(a[11], a[28]); SQRADDAC(a[12], a[27]); SQRADDAC(a[13], a[26]); SQRADDAC(a[14], a[25]); SQRADDAC(a[15], a[24]); SQRADDAC(a[16], a[23]); SQRADDAC(a[17], a[22]); SQRADDAC(a[18], a[21]); SQRADDAC(a[19], a[20]); SQRADDDB; 
   COMBA_STORE(b[39]);

   /* output 40 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[40]); SQRADDAC(a[1], a[39]); SQRADDAC(a[2], a[38]); SQRADDAC(a[3], a[37]); SQRADDAC(a[4], a[36]); SQRADDAC(a[5], a[35]); SQRADDAC(a[6], a[34]); SQRADDAC(a[7], a[33]); SQRADDAC(a[8], a[32]); SQRADDAC(a[9], a[31]); SQRADDAC(a[10], a[30]); SQRADDAC(a[11], a[29]); SQRADDAC(a[12], a[28]); SQRADDAC(a[13], a[27]); SQRADDAC(a[14], a[26]); SQRADDAC(a[15], a[25]); SQRADDAC(a[16], a[24]); SQRADDAC(a[17], a[23]); SQRADDAC(a[18], a[22]); SQRADDAC(a[19], a[21]); SQRADDDB; SQRADD(a[20], a[20]); 
   COMBA_STORE(b[40]);

   /* output 41 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[41]); SQRADDAC(a[1], a[40]); SQRADDAC(a[2], a[39]); SQRADDAC(a[3], a[38]); SQRADDAC(a[4], a[37]); SQRADDAC(a[5], a[36]); SQRADDAC(a[6], a[35]); SQRADDAC(a[7], a[34]); SQRADDAC(a[8], a[33]); SQRADDAC(a[9], a[32]); SQRADDAC(a[10], a[31]); SQRADDAC(a[11], a[30]); SQRADDAC(a[12], a[29]); SQRADDAC(a[13], a[28]); SQRADDAC(a[14], a[27]); SQRADDAC(a[15], a[26]); SQRADDAC(a[16], a[25]); SQRADDAC(a[17], a[24]); SQRADDAC(a[18], a[23]); SQRADDAC(a[19], a[22]); SQRADDAC(a[20], a[21]); SQRADDDB; 
   COMBA_STORE(b[41]);

   /* output 42 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[42]); SQRADDAC(a[1], a[41]); SQRADDAC(a[2], a[40]); SQRADDAC(a[3], a[39]); SQRADDAC(a[4], a[38]); SQRADDAC(a[5], a[37]); SQRADDAC(a[6], a[36]); SQRADDAC(a[7], a[35]); SQRADDAC(a[8], a[34]); SQRADDAC(a[9], a[33]); SQRADDAC(a[10], a[32]); SQRADDAC(a[11], a[31]); SQRADDAC(a[12], a[30]); SQRADDAC(a[13], a[29]); SQRADDAC(a[14], a[28]); SQRADDAC(a[15], a[27]); SQRADDAC(a[16], a[26]); SQRADDAC(a[17], a[25]); SQRADDAC(a[18], a[24]); SQRADDAC(a[19], a[23]); SQRADDAC(a[20], a[22]); SQRADDDB; SQRADD(a[21], a[21]); 
   COMBA_STORE(b[42]);

   /* output 43 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[43]); SQRADDAC(a[1], a[42]); SQRADDAC(a[2], a[41]); SQRADDAC(a[3], a[40]); SQRADDAC(a[4], a[39]); SQRADDAC(a[5], a[38]); SQRADDAC(a[6], a[37]); SQRADDAC(a[7], a[36]); SQRADDAC(a[8], a[35]); SQRADDAC(a[9], a[34]); SQRADDAC(a[10], a[33]); SQRADDAC(a[11], a[32]); SQRADDAC(a[12], a[31]); SQRADDAC(a[13], a[30]); SQRADDAC(a[14], a[29]); SQRADDAC(a[15], a[28]); SQRADDAC(a[16], a[27]); SQRADDAC(a[17], a[26]); SQRADDAC(a[18], a[25]); SQRADDAC(a[19], a[24]); SQRADDAC(a[20], a[23]); SQRADDAC(a[21], a[22]); SQRADDDB; 
   COMBA_STORE(b[43]);

   /* output 44 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[44]); SQRADDAC(a[1], a[43]); SQRADDAC(a[2], a[42]); SQRADDAC(a[3], a[41]); SQRADDAC(a[4], a[40]); SQRADDAC(a[5], a[39]); SQRADDAC(a[6], a[38]); SQRADDAC(a[7], a[37]); SQRADDAC(a[8], a[36]); SQRADDAC(a[9], a[35]); SQRADDAC(a[10], a[34]); SQRADDAC(a[11], a[33]); SQRADDAC(a[12], a[32]); SQRADDAC(a[13], a[31]); SQRADDAC(a[14], a[30]); SQRADDAC(a[15], a[29]); SQRADDAC(a[16], a[28]); SQRADDAC(a[17], a[27]); SQRADDAC(a[18], a[26]); SQRADDAC(a[19], a[25]); SQRADDAC(a[20], a[24]); SQRADDAC(a[21], a[23]); SQRADDDB; SQRADD(a[22], a[22]); 
   COMBA_STORE(b[44]);

   /* output 45 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[45]); SQRADDAC(a[1], a[44]); SQRADDAC(a[2], a[43]); SQRADDAC(a[3], a[42]); SQRADDAC(a[4], a[41]); SQRADDAC(a[5], a[40]); SQRADDAC(a[6], a[39]); SQRADDAC(a[7], a[38]); SQRADDAC(a[8], a[37]); SQRADDAC(a[9], a[36]); SQRADDAC(a[10], a[35]); SQRADDAC(a[11], a[34]); SQRADDAC(a[12], a[33]); SQRADDAC(a[13], a[32]); SQRADDAC(a[14], a[31]); SQRADDAC(a[15], a[30]); SQRADDAC(a[16], a[29]); SQRADDAC(a[17], a[28]); SQRADDAC(a[18], a[27]); SQRADDAC(a[19], a[26]); SQRADDAC(a[20], a[25]); SQRADDAC(a[21], a[24]); SQRADDAC(a[22], a[23]); SQRADDDB; 
   COMBA_STORE(b[45]);

   /* output 46 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[46]); SQRADDAC(a[1], a[45]); SQRADDAC(a[2], a[44]); SQRADDAC(a[3], a[43]); SQRADDAC(a[4], a[42]); SQRADDAC(a[5], a[41]); SQRADDAC(a[6], a[40]); SQRADDAC(a[7], a[39]); SQRADDAC(a[8], a[38]); SQRADDAC(a[9], a[37]); SQRADDAC(a[10], a[36]); SQRADDAC(a[11], a[35]); SQRADDAC(a[12], a[34]); SQRADDAC(a[13], a[33]); SQRADDAC(a[14], a[32]); SQRADDAC(a[15], a[31]); SQRADDAC(a[16], a[30]); SQRADDAC(a[17], a[29]); SQRADDAC(a[18], a[28]); SQRADDAC(a[19], a[27]); SQRADDAC(a[20], a[26]); SQRADDAC(a[21], a[25]); SQRADDAC(a[22], a[24]); SQRADDDB; SQRADD(a[23], a[23]); 
   COMBA_STORE(b[46]);

   /* output 47 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[47]); SQRADDAC(a[1], a[46]); SQRADDAC(a[2], a[45]); SQRADDAC(a[3], a[44]); SQRADDAC(a[4], a[43]); SQRADDAC(a[5], a[42]); SQRADDAC(a[6], a[41]); SQRADDAC(a[7], a[40]); SQRADDAC(a[8], a[39]); SQRADDAC(a[9], a[38]); SQRADDAC(a[10], a[37]); SQRADDAC(a[11], a[36]); SQRADDAC(a[12], a[35]); SQRADDAC(a[13], a[34]); SQRADDAC(a[14], a[33]); SQRADDAC(a[15], a[32]); SQRADDAC(a[16], a[31]); SQRADDAC(a[17], a[30]); SQRADDAC(a[18], a[29]); SQRADDAC(a[19], a[28]); SQRADDAC(a[20], a[27]); SQRADDAC(a[21], a[26]); SQRADDAC(a[22], a[25]); SQRADDAC(a[23], a[24]); SQRADDDB; 
   COMBA_STORE(b[47]);

   /* output 48 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[48]); SQRADDAC(a[1], a[47]); SQRADDAC(a[2], a[46]); SQRADDAC(a[3], a[45]); SQRADDAC(a[4], a[44]); SQRADDAC(a[5], a[43]); SQRADDAC(a[6], a[42]); SQRADDAC(a[7], a[41]); SQRADDAC(a[8], a[40]); SQRADDAC(a[9], a[39]); SQRADDAC(a[10], a[38]); SQRADDAC(a[11], a[37]); SQRADDAC(a[12], a[36]); SQRADDAC(a[13], a[35]); SQRADDAC(a[14], a[34]); SQRADDAC(a[15], a[33]); SQRADDAC(a[16], a[32]); SQRADDAC(a[17], a[31]); SQRADDAC(a[18], a[30]); SQRADDAC(a[19], a[29]); SQRADDAC(a[20], a[28]); SQRADDAC(a[21], a[27]); SQRADDAC(a[22], a[26]); SQRADDAC(a[23], a[25]); SQRADDDB; SQRADD(a[24], a[24]); 
   COMBA_STORE(b[48]);

   /* output 49 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[49]); SQRADDAC(a[1], a[48]); SQRADDAC(a[2], a[47]); SQRADDAC(a[3], a[46]); SQRADDAC(a[4], a[45]); SQRADDAC(a[5], a[44]); SQRADDAC(a[6], a[43]); SQRADDAC(a[7], a[42]); SQRADDAC(a[8], a[41]); SQRADDAC(a[9], a[40]); SQRADDAC(a[10], a[39]); SQRADDAC(a[11], a[38]); SQRADDAC(a[12], a[37]); SQRADDAC(a[13], a[36]); SQRADDAC(a[14], a[35]); SQRADDAC(a[15], a[34]); SQRADDAC(a[16], a[33]); SQRADDAC(a[17], a[32]); SQRADDAC(a[18], a[31]); SQRADDAC(a[19], a[30]); SQRADDAC(a[20], a[29]); SQRADDAC(a[21], a[28]); SQRADDAC(a[22], a[27]); SQRADDAC(a[23], a[26]); SQRADDAC(a[24], a[25]); SQRADDDB; 
   COMBA_STORE(b[49]);

   /* output 50 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[50]); SQRADDAC(a[1], a[49]); SQRADDAC(a[2], a[48]); SQRADDAC(a[3], a[47]); SQRADDAC(a[4], a[46]); SQRADDAC(a[5], a[45]); SQRADDAC(a[6], a[44]); SQRADDAC(a[7], a[43]); SQRADDAC(a[8], a[42]); SQRADDAC(a[9], a[41]); SQRADDAC(a[10], a[40]); SQRADDAC(a[11], a[39]); SQRADDAC(a[12], a[38]); SQRADDAC(a[13], a[37]); SQRADDAC(a[14], a[36]); SQRADDAC(a[15], a[35]); SQRADDAC(a[16], a[34]); SQRADDAC(a[17], a[33]); SQRADDAC(a[18], a[32]); SQRADDAC(a[19], a[31]); SQRADDAC(a[20], a[30]); SQRADDAC(a[21], a[29]); SQRADDAC(a[22], a[28]); SQRADDAC(a[23], a[27]); SQRADDAC(a[24], a[26]); SQRADDDB; SQRADD(a[25], a[25]); 
   COMBA_STORE(b[50]);

   /* output 51 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[51]); SQRADDAC(a[1], a[50]); SQRADDAC(a[2], a[49]); SQRADDAC(a[3], a[48]); SQRADDAC(a[4], a[47]); SQRADDAC(a[5], a[46]); SQRADDAC(a[6], a[45]); SQRADDAC(a[7], a[44]); SQRADDAC(a[8], a[43]); SQRADDAC(a[9], a[42]); SQRADDAC(a[10], a[41]); SQRADDAC(a[11], a[40]); SQRADDAC(a[12], a[39]); SQRADDAC(a[13], a[38]); SQRADDAC(a[14], a[37]); SQRADDAC(a[15], a[36]); SQRADDAC(a[16], a[35]); SQRADDAC(a[17], a[34]); SQRADDAC(a[18], a[33]); SQRADDAC(a[19], a[32]); SQRADDAC(a[20], a[31]); SQRADDAC(a[21], a[30]); SQRADDAC(a[22], a[29]); SQRADDAC(a[23], a[28]); SQRADDAC(a[24], a[27]); SQRADDAC(a[25], a[26]); SQRADDDB; 
   COMBA_STORE(b[51]);

   /* output 52 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[52]); SQRADDAC(a[1], a[51]); SQRADDAC(a[2], a[50]); SQRADDAC(a[3], a[49]); SQRADDAC(a[4], a[48]); SQRADDAC(a[5], a[47]); SQRADDAC(a[6], a[46]); SQRADDAC(a[7], a[45]); SQRADDAC(a[8], a[44]); SQRADDAC(a[9], a[43]); SQRADDAC(a[10], a[42]); SQRADDAC(a[11], a[41]); SQRADDAC(a[12], a[40]); SQRADDAC(a[13], a[39]); SQRADDAC(a[14], a[38]); SQRADDAC(a[15], a[37]); SQRADDAC(a[16], a[36]); SQRADDAC(a[17], a[35]); SQRADDAC(a[18], a[34]); SQRADDAC(a[19], a[33]); SQRADDAC(a[20], a[32]); SQRADDAC(a[21], a[31]); SQRADDAC(a[22], a[30]); SQRADDAC(a[23], a[29]); SQRADDAC(a[24], a[28]); SQRADDAC(a[25], a[27]); SQRADDDB; SQRADD(a[26], a[26]); 
   COMBA_STORE(b[52]);

   /* output 53 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[53]); SQRADDAC(a[1], a[52]); SQRADDAC(a[2], a[51]); SQRADDAC(a[3], a[50]); SQRADDAC(a[4], a[49]); SQRADDAC(a[5], a[48]); SQRADDAC(a[6], a[47]); SQRADDAC(a[7], a[46]); SQRADDAC(a[8], a[45]); SQRADDAC(a[9], a[44]); SQRADDAC(a[10], a[43]); SQRADDAC(a[11], a[42]); SQRADDAC(a[12], a[41]); SQRADDAC(a[13], a[40]); SQRADDAC(a[14], a[39]); SQRADDAC(a[15], a[38]); SQRADDAC(a[16], a[37]); SQRADDAC(a[17], a[36]); SQRADDAC(a[18], a[35]); SQRADDAC(a[19], a[34]); SQRADDAC(a[20], a[33]); SQRADDAC(a[21], a[32]); SQRADDAC(a[22], a[31]); SQRADDAC(a[23], a[30]); SQRADDAC(a[24], a[29]); SQRADDAC(a[25], a[28]); SQRADDAC(a[26], a[27]); SQRADDDB; 
   COMBA_STORE(b[53]);

   /* output 54 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[54]); SQRADDAC(a[1], a[53]); SQRADDAC(a[2], a[52]); SQRADDAC(a[3], a[51]); SQRADDAC(a[4], a[50]); SQRADDAC(a[5], a[49]); SQRADDAC(a[6], a[48]); SQRADDAC(a[7], a[47]); SQRADDAC(a[8], a[46]); SQRADDAC(a[9], a[45]); SQRADDAC(a[10], a[44]); SQRADDAC(a[11], a[43]); SQRADDAC(a[12], a[42]); SQRADDAC(a[13], a[41]); SQRADDAC(a[14], a[40]); SQRADDAC(a[15], a[39]); SQRADDAC(a[16], a[38]); SQRADDAC(a[17], a[37]); SQRADDAC(a[18], a[36]); SQRADDAC(a[19], a[35]); SQRADDAC(a[20], a[34]); SQRADDAC(a[21], a[33]); SQRADDAC(a[22], a[32]); SQRADDAC(a[23], a[31]); SQRADDAC(a[24], a[30]); SQRADDAC(a[25], a[29]); SQRADDAC(a[26], a[28]); SQRADDDB; SQRADD(a[27], a[27]); 
   COMBA_STORE(b[54]);

   /* output 55 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[55]); SQRADDAC(a[1], a[54]); SQRADDAC(a[2], a[53]); SQRADDAC(a[3], a[52]); SQRADDAC(a[4], a[51]); SQRADDAC(a[5], a[50]); SQRADDAC(a[6], a[49]); SQRADDAC(a[7], a[48]); SQRADDAC(a[8], a[47]); SQRADDAC(a[9], a[46]); SQRADDAC(a[10], a[45]); SQRADDAC(a[11], a[44]); SQRADDAC(a[12], a[43]); SQRADDAC(a[13], a[42]); SQRADDAC(a[14], a[41]); SQRADDAC(a[15], a[40]); SQRADDAC(a[16], a[39]); SQRADDAC(a[17], a[38]); SQRADDAC(a[18], a[37]); SQRADDAC(a[19], a[36]); SQRADDAC(a[20], a[35]); SQRADDAC(a[21], a[34]); SQRADDAC(a[22], a[33]); SQRADDAC(a[23], a[32]); SQRADDAC(a[24], a[31]); SQRADDAC(a[25], a[30]); SQRADDAC(a[26], a[29]); SQRADDAC(a[27], a[28]); SQRADDDB; 
   COMBA_STORE(b[55]);

   /* output 56 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[56]); SQRADDAC(a[1], a[55]); SQRADDAC(a[2], a[54]); SQRADDAC(a[3], a[53]); SQRADDAC(a[4], a[52]); SQRADDAC(a[5], a[51]); SQRADDAC(a[6], a[50]); SQRADDAC(a[7], a[49]); SQRADDAC(a[8], a[48]); SQRADDAC(a[9], a[47]); SQRADDAC(a[10], a[46]); SQRADDAC(a[11], a[45]); SQRADDAC(a[12], a[44]); SQRADDAC(a[13], a[43]); SQRADDAC(a[14], a[42]); SQRADDAC(a[15], a[41]); SQRADDAC(a[16], a[40]); SQRADDAC(a[17], a[39]); SQRADDAC(a[18], a[38]); SQRADDAC(a[19], a[37]); SQRADDAC(a[20], a[36]); SQRADDAC(a[21], a[35]); SQRADDAC(a[22], a[34]); SQRADDAC(a[23], a[33]); SQRADDAC(a[24], a[32]); SQRADDAC(a[25], a[31]); SQRADDAC(a[26], a[30]); SQRADDAC(a[27], a[29]); SQRADDDB; SQRADD(a[28], a[28]); 
   COMBA_STORE(b[56]);

   /* output 57 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[57]); SQRADDAC(a[1], a[56]); SQRADDAC(a[2], a[55]); SQRADDAC(a[3], a[54]); SQRADDAC(a[4], a[53]); SQRADDAC(a[5], a[52]); SQRADDAC(a[6], a[51]); SQRADDAC(a[7], a[50]); SQRADDAC(a[8], a[49]); SQRADDAC(a[9], a[48]); SQRADDAC(a[10], a[47]); SQRADDAC(a[11], a[46]); SQRADDAC(a[12], a[45]); SQRADDAC(a[13], a[44]); SQRADDAC(a[14], a[43]); SQRADDAC(a[15], a[42]); SQRADDAC(a[16], a[41]); SQRADDAC(a[17], a[40]); SQRADDAC(a[18], a[39]); SQRADDAC(a[19], a[38]); SQRADDAC(a[20], a[37]); SQRADDAC(a[21], a[36]); SQRADDAC(a[22], a[35]); SQRADDAC(a[23], a[34]); SQRADDAC(a[24], a[33]); SQRADDAC(a[25], a[32]); SQRADDAC(a[26], a[31]); SQRADDAC(a[27], a[30]); SQRADDAC(a[28], a[29]); SQRADDDB; 
   COMBA_STORE(b[57]);

   /* output 58 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[58]); SQRADDAC(a[1], a[57]); SQRADDAC(a[2], a[56]); SQRADDAC(a[3], a[55]); SQRADDAC(a[4], a[54]); SQRADDAC(a[5], a[53]); SQRADDAC(a[6], a[52]); SQRADDAC(a[7], a[51]); SQRADDAC(a[8], a[50]); SQRADDAC(a[9], a[49]); SQRADDAC(a[10], a[48]); SQRADDAC(a[11], a[47]); SQRADDAC(a[12], a[46]); SQRADDAC(a[13], a[45]); SQRADDAC(a[14], a[44]); SQRADDAC(a[15], a[43]); SQRADDAC(a[16], a[42]); SQRADDAC(a[17], a[41]); SQRADDAC(a[18], a[40]); SQRADDAC(a[19], a[39]); SQRADDAC(a[20], a[38]); SQRADDAC(a[21], a[37]); SQRADDAC(a[22], a[36]); SQRADDAC(a[23], a[35]); SQRADDAC(a[24], a[34]); SQRADDAC(a[25], a[33]); SQRADDAC(a[26], a[32]); SQRADDAC(a[27], a[31]); SQRADDAC(a[28], a[30]); SQRADDDB; SQRADD(a[29], a[29]); 
   COMBA_STORE(b[58]);

   /* output 59 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[59]); SQRADDAC(a[1], a[58]); SQRADDAC(a[2], a[57]); SQRADDAC(a[3], a[56]); SQRADDAC(a[4], a[55]); SQRADDAC(a[5], a[54]); SQRADDAC(a[6], a[53]); SQRADDAC(a[7], a[52]); SQRADDAC(a[8], a[51]); SQRADDAC(a[9], a[50]); SQRADDAC(a[10], a[49]); SQRADDAC(a[11], a[48]); SQRADDAC(a[12], a[47]); SQRADDAC(a[13], a[46]); SQRADDAC(a[14], a[45]); SQRADDAC(a[15], a[44]); SQRADDAC(a[16], a[43]); SQRADDAC(a[17], a[42]); SQRADDAC(a[18], a[41]); SQRADDAC(a[19], a[40]); SQRADDAC(a[20], a[39]); SQRADDAC(a[21], a[38]); SQRADDAC(a[22], a[37]); SQRADDAC(a[23], a[36]); SQRADDAC(a[24], a[35]); SQRADDAC(a[25], a[34]); SQRADDAC(a[26], a[33]); SQRADDAC(a[27], a[32]); SQRADDAC(a[28], a[31]); SQRADDAC(a[29], a[30]); SQRADDDB; 
   COMBA_STORE(b[59]);

   /* output 60 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[60]); SQRADDAC(a[1], a[59]); SQRADDAC(a[2], a[58]); SQRADDAC(a[3], a[57]); SQRADDAC(a[4], a[56]); SQRADDAC(a[5], a[55]); SQRADDAC(a[6], a[54]); SQRADDAC(a[7], a[53]); SQRADDAC(a[8], a[52]); SQRADDAC(a[9], a[51]); SQRADDAC(a[10], a[50]); SQRADDAC(a[11], a[49]); SQRADDAC(a[12], a[48]); SQRADDAC(a[13], a[47]); SQRADDAC(a[14], a[46]); SQRADDAC(a[15], a[45]); SQRADDAC(a[16], a[44]); SQRADDAC(a[17], a[43]); SQRADDAC(a[18], a[42]); SQRADDAC(a[19], a[41]); SQRADDAC(a[20], a[40]); SQRADDAC(a[21], a[39]); SQRADDAC(a[22], a[38]); SQRADDAC(a[23], a[37]); SQRADDAC(a[24], a[36]); SQRADDAC(a[25], a[35]); SQRADDAC(a[26], a[34]); SQRADDAC(a[27], a[33]); SQRADDAC(a[28], a[32]); SQRADDAC(a[29], a[31]); SQRADDDB; SQRADD(a[30], a[30]); 
   COMBA_STORE(b[60]);

   /* output 61 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[61]); SQRADDAC(a[1], a[60]); SQRADDAC(a[2], a[59]); SQRADDAC(a[3], a[58]); SQRADDAC(a[4], a[57]); SQRADDAC(a[5], a[56]); SQRADDAC(a[6], a[55]); SQRADDAC(a[7], a[54]); SQRADDAC(a[8], a[53]); SQRADDAC(a[9], a[52]); SQRADDAC(a[10], a[51]); SQRADDAC(a[11], a[50]); SQRADDAC(a[12], a[49]); SQRADDAC(a[13], a[48]); SQRADDAC(a[14], a[47]); SQRADDAC(a[15], a[46]); SQRADDAC(a[16], a[45]); SQRADDAC(a[17], a[44]); SQRADDAC(a[18], a[43]); SQRADDAC(a[19], a[42]); SQRADDAC(a[20], a[41]); SQRADDAC(a[21], a[40]); SQRADDAC(a[22], a[39]); SQRADDAC(a[23], a[38]); SQRADDAC(a[24], a[37]); SQRADDAC(a[25], a[36]); SQRADDAC(a[26], a[35]); SQRADDAC(a[27], a[34]); SQRADDAC(a[28], a[33]); SQRADDAC(a[29], a[32]); SQRADDAC(a[30], a[31]); SQRADDDB; 
   COMBA_STORE(b[61]);

   /* output 62 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[62]); SQRADDAC(a[1], a[61]); SQRADDAC(a[2], a[60]); SQRADDAC(a[3], a[59]); SQRADDAC(a[4], a[58]); SQRADDAC(a[5], a[57]); SQRADDAC(a[6], a[56]); SQRADDAC(a[7], a[55]); SQRADDAC(a[8], a[54]); SQRADDAC(a[9], a[53]); SQRADDAC(a[10], a[52]); SQRADDAC(a[11], a[51]); SQRADDAC(a[12], a[50]); SQRADDAC(a[13], a[49]); SQRADDAC(a[14], a[48]); SQRADDAC(a[15], a[47]); SQRADDAC(a[16], a[46]); SQRADDAC(a[17], a[45]); SQRADDAC(a[18], a[44]); SQRADDAC(a[19], a[43]); SQRADDAC(a[20], a[42]); SQRADDAC(a[21], a[41]); SQRADDAC(a[22], a[40]); SQRADDAC(a[23], a[39]); SQRADDAC(a[24], a[38]); SQRADDAC(a[25], a[37]); SQRADDAC(a[26], a[36]); SQRADDAC(a[27], a[35]); SQRADDAC(a[28], a[34]); SQRADDAC(a[29], a[33]); SQRADDAC(a[30], a[32]); SQRADDDB; SQRADD(a[31], a[31]); 
   COMBA_STORE(b[62]);

   /* output 63 */
   CARRY_FORWARD;
   SQRADDSC(a[0], a[63]); SQRADDAC(a[1], a[62]); SQRADDAC(a[2], a[61]); SQRADDAC(a[3], a[60]); SQRADDAC(a[4], a[59]); SQRADDAC(a[5], a[58]); SQRADDAC(a[6], a[57]); SQRADDAC(a[7], a[56]); SQRADDAC(a[8], a[55]); SQRADDAC(a[9], a[54]); SQRADDAC(a[10], a[53]); SQRADDAC(a[11], a[52]); SQRADDAC(a[12], a[51]); SQRADDAC(a[13], a[50]); SQRADDAC(a[14], a[49]); SQRADDAC(a[15], a[48]); SQRADDAC(a[16], a[47]); SQRADDAC(a[17], a[46]); SQRADDAC(a[18], a[45]); SQRADDAC(a[19], a[44]); SQRADDAC(a[20], a[43]); SQRADDAC(a[21], a[42]); SQRADDAC(a[22], a[41]); SQRADDAC(a[23], a[40]); SQRADDAC(a[24], a[39]); SQRADDAC(a[25], a[38]); SQRADDAC(a[26], a[37]); SQRADDAC(a[27], a[36]); SQRADDAC(a[28], a[35]); SQRADDAC(a[29], a[34]); SQRADDAC(a[30], a[33]); SQRADDAC(a[31], a[32]); SQRADDDB; 
   COMBA_STORE(b[63]);

   /* output 64 */
   CARRY_FORWARD;
   SQRADDSC(a[1], a[63]); SQRADDAC(a[2], a[62]); SQRADDAC(a[3], a[61]); SQRADDAC(a[4], a[60]); SQRADDAC(a[5], a[59]); SQRADDAC(a[6], a[58]); SQRADDAC(a[7], a[57]); SQRADDAC(a[8], a[56]); SQRADDAC(a[9], a[55]); SQRADDAC(a[10], a[54]); SQRADDAC(a[11], a[53]); SQRADDAC(a[12], a[52]); SQRADDAC(a[13], a[51]); SQRADDAC(a[14], a[50]); SQRADDAC(a[15], a[49]); SQRADDAC(a[16], a[48]); SQRADDAC(a[17], a[47]); SQRADDAC(a[18], a[46]); SQRADDAC(a[19], a[45]); SQRADDAC(a[20], a[44]); SQRADDAC(a[21], a[43]); SQRADDAC(a[22], a[42]); SQRADDAC(a[23], a[41]); SQRADDAC(a[24], a[40]); SQRADDAC(a[25], a[39]); SQRADDAC(a[26], a[38]); SQRADDAC(a[27], a[37]); SQRADDAC(a[28], a[36]); SQRADDAC(a[29], a[35]); SQRADDAC(a[30], a[34]); SQRADDAC(a[31], a[33]); SQRADDDB; SQRADD(a[32], a[32]); 
   COMBA_STORE(b[64]);

   /* output 65 */
   CARRY_FORWARD;
   SQRADDSC(a[2], a[63]); SQRADDAC(a[3], a[62]); SQRADDAC(a[4], a[61]); SQRADDAC(a[5], a[60]); SQRADDAC(a[6], a[59]); SQRADDAC(a[7], a[58]); SQRADDAC(a[8], a[57]); SQRADDAC(a[9], a[56]); SQRADDAC(a[10], a[55]); SQRADDAC(a[11], a[54]); SQRADDAC(a[12], a[53]); SQRADDAC(a[13], a[52]); SQRADDAC(a[14], a[51]); SQRADDAC(a[15], a[50]); SQRADDAC(a[16], a[49]); SQRADDAC(a[17], a[48]); SQRADDAC(a[18], a[47]); SQRADDAC(a[19], a[46]); SQRADDAC(a[20], a[45]); SQRADDAC(a[21], a[44]); SQRADDAC(a[22], a[43]); SQRADDAC(a[23], a[42]); SQRADDAC(a[24], a[41]); SQRADDAC(a[25], a[40]); SQRADDAC(a[26], a[39]); SQRADDAC(a[27], a[38]); SQRADDAC(a[28], a[37]); SQRADDAC(a[29], a[36]); SQRADDAC(a[30], a[35]); SQRADDAC(a[31], a[34]); SQRADDAC(a[32], a[33]); SQRADDDB; 
   COMBA_STORE(b[65]);

   /* output 66 */
   CARRY_FORWARD;
   SQRADDSC(a[3], a[63]); SQRADDAC(a[4], a[62]); SQRADDAC(a[5], a[61]); SQRADDAC(a[6], a[60]); SQRADDAC(a[7], a[59]); SQRADDAC(a[8], a[58]); SQRADDAC(a[9], a[57]); SQRADDAC(a[10], a[56]); SQRADDAC(a[11], a[55]); SQRADDAC(a[12], a[54]); SQRADDAC(a[13], a[53]); SQRADDAC(a[14], a[52]); SQRADDAC(a[15], a[51]); SQRADDAC(a[16], a[50]); SQRADDAC(a[17], a[49]); SQRADDAC(a[18], a[48]); SQRADDAC(a[19], a[47]); SQRADDAC(a[20], a[46]); SQRADDAC(a[21], a[45]); SQRADDAC(a[22], a[44]); SQRADDAC(a[23], a[43]); SQRADDAC(a[24], a[42]); SQRADDAC(a[25], a[41]); SQRADDAC(a[26], a[40]); SQRADDAC(a[27], a[39]); SQRADDAC(a[28], a[38]); SQRADDAC(a[29], a[37]); SQRADDAC(a[30], a[36]); SQRADDAC(a[31], a[35]); SQRADDAC(a[32], a[34]); SQRADDDB; SQRADD(a[33], a[33]); 
   COMBA_STORE(b[66]);

   /* output 67 */
   CARRY_FORWARD;
   SQRADDSC(a[4], a[63]); SQRADDAC(a[5], a[62]); SQRADDAC(a[6], a[61]); SQRADDAC(a[7], a[60]); SQRADDAC(a[8], a[59]); SQRADDAC(a[9], a[58]); SQRADDAC(a[10], a[57]); SQRADDAC(a[11], a[56]); SQRADDAC(a[12], a[55]); SQRADDAC(a[13], a[54]); SQRADDAC(a[14], a[53]); SQRADDAC(a[15], a[52]); SQRADDAC(a[16], a[51]); SQRADDAC(a[17], a[50]); SQRADDAC(a[18], a[49]); SQRADDAC(a[19], a[48]); SQRADDAC(a[20], a[47]); SQRADDAC(a[21], a[46]); SQRADDAC(a[22], a[45]); SQRADDAC(a[23], a[44]); SQRADDAC(a[24], a[43]); SQRADDAC(a[25], a[42]); SQRADDAC(a[26], a[41]); SQRADDAC(a[27], a[40]); SQRADDAC(a[28], a[39]); SQRADDAC(a[29], a[38]); SQRADDAC(a[30], a[37]); SQRADDAC(a[31], a[36]); SQRADDAC(a[32], a[35]); SQRADDAC(a[33], a[34]); SQRADDDB; 
   COMBA_STORE(b[67]);

   /* output 68 */
   CARRY_FORWARD;
   SQRADDSC(a[5], a[63]); SQRADDAC(a[6], a[62]); SQRADDAC(a[7], a[61]); SQRADDAC(a[8], a[60]); SQRADDAC(a[9], a[59]); SQRADDAC(a[10], a[58]); SQRADDAC(a[11], a[57]); SQRADDAC(a[12], a[56]); SQRADDAC(a[13], a[55]); SQRADDAC(a[14], a[54]); SQRADDAC(a[15], a[53]); SQRADDAC(a[16], a[52]); SQRADDAC(a[17], a[51]); SQRADDAC(a[18], a[50]); SQRADDAC(a[19], a[49]); SQRADDAC(a[20], a[48]); SQRADDAC(a[21], a[47]); SQRADDAC(a[22], a[46]); SQRADDAC(a[23], a[45]); SQRADDAC(a[24], a[44]); SQRADDAC(a[25], a[43]); SQRADDAC(a[26], a[42]); SQRADDAC(a[27], a[41]); SQRADDAC(a[28], a[40]); SQRADDAC(a[29], a[39]); SQRADDAC(a[30], a[38]); SQRADDAC(a[31], a[37]); SQRADDAC(a[32], a[36]); SQRADDAC(a[33], a[35]); SQRADDDB; SQRADD(a[34], a[34]); 
   COMBA_STORE(b[68]);

   /* output 69 */
   CARRY_FORWARD;
   SQRADDSC(a[6], a[63]); SQRADDAC(a[7], a[62]); SQRADDAC(a[8], a[61]); SQRADDAC(a[9], a[60]); SQRADDAC(a[10], a[59]); SQRADDAC(a[11], a[58]); SQRADDAC(a[12], a[57]); SQRADDAC(a[13], a[56]); SQRADDAC(a[14], a[55]); SQRADDAC(a[15], a[54]); SQRADDAC(a[16], a[53]); SQRADDAC(a[17], a[52]); SQRADDAC(a[18], a[51]); SQRADDAC(a[19], a[50]); SQRADDAC(a[20], a[49]); SQRADDAC(a[21], a[48]); SQRADDAC(a[22], a[47]); SQRADDAC(a[23], a[46]); SQRADDAC(a[24], a[45]); SQRADDAC(a[25], a[44]); SQRADDAC(a[26], a[43]); SQRADDAC(a[27], a[42]); SQRADDAC(a[28], a[41]); SQRADDAC(a[29], a[40]); SQRADDAC(a[30], a[39]); SQRADDAC(a[31], a[38]); SQRADDAC(a[32], a[37]); SQRADDAC(a[33], a[36]); SQRADDAC(a[34], a[35]); SQRADDDB; 
   COMBA_STORE(b[69]);

   /* output 70 */
   CARRY_FORWARD;
   SQRADDSC(a[7], a[63]); SQRADDAC(a[8], a[62]); SQRADDAC(a[9], a[61]); SQRADDAC(a[10], a[60]); SQRADDAC(a[11], a[59]); SQRADDAC(a[12], a[58]); SQRADDAC(a[13], a[57]); SQRADDAC(a[14], a[56]); SQRADDAC(a[15], a[55]); SQRADDAC(a[16], a[54]); SQRADDAC(a[17], a[53]); SQRADDAC(a[18], a[52]); SQRADDAC(a[19], a[51]); SQRADDAC(a[20], a[50]); SQRADDAC(a[21], a[49]); SQRADDAC(a[22], a[48]); SQRADDAC(a[23], a[47]); SQRADDAC(a[24], a[46]); SQRADDAC(a[25], a[45]); SQRADDAC(a[26], a[44]); SQRADDAC(a[27], a[43]); SQRADDAC(a[28], a[42]); SQRADDAC(a[29], a[41]); SQRADDAC(a[30], a[40]); SQRADDAC(a[31], a[39]); SQRADDAC(a[32], a[38]); SQRADDAC(a[33], a[37]); SQRADDAC(a[34], a[36]); SQRADDDB; SQRADD(a[35], a[35]); 
   COMBA_STORE(b[70]);

   /* output 71 */
   CARRY_FORWARD;
   SQRADDSC(a[8], a[63]); SQRADDAC(a[9], a[62]); SQRADDAC(a[10], a[61]); SQRADDAC(a[11], a[60]); SQRADDAC(a[12], a[59]); SQRADDAC(a[13], a[58]); SQRADDAC(a[14], a[57]); SQRADDAC(a[15], a[56]); SQRADDAC(a[16], a[55]); SQRADDAC(a[17], a[54]); SQRADDAC(a[18], a[53]); SQRADDAC(a[19], a[52]); SQRADDAC(a[20], a[51]); SQRADDAC(a[21], a[50]); SQRADDAC(a[22], a[49]); SQRADDAC(a[23], a[48]); SQRADDAC(a[24], a[47]); SQRADDAC(a[25], a[46]); SQRADDAC(a[26], a[45]); SQRADDAC(a[27], a[44]); SQRADDAC(a[28], a[43]); SQRADDAC(a[29], a[42]); SQRADDAC(a[30], a[41]); SQRADDAC(a[31], a[40]); SQRADDAC(a[32], a[39]); SQRADDAC(a[33], a[38]); SQRADDAC(a[34], a[37]); SQRADDAC(a[35], a[36]); SQRADDDB; 
   COMBA_STORE(b[71]);

   /* output 72 */
   CARRY_FORWARD;
   SQRADDSC(a[9], a[63]); SQRADDAC(a[10], a[62]); SQRADDAC(a[11], a[61]); SQRADDAC(a[12], a[60]); SQRADDAC(a[13], a[59]); SQRADDAC(a[14], a[58]); SQRADDAC(a[15], a[57]); SQRADDAC(a[16], a[56]); SQRADDAC(a[17], a[55]); SQRADDAC(a[18], a[54]); SQRADDAC(a[19], a[53]); SQRADDAC(a[20], a[52]); SQRADDAC(a[21], a[51]); SQRADDAC(a[22], a[50]); SQRADDAC(a[23], a[49]); SQRADDAC(a[24], a[48]); SQRADDAC(a[25], a[47]); SQRADDAC(a[26], a[46]); SQRADDAC(a[27], a[45]); SQRADDAC(a[28], a[44]); SQRADDAC(a[29], a[43]); SQRADDAC(a[30], a[42]); SQRADDAC(a[31], a[41]); SQRADDAC(a[32], a[40]); SQRADDAC(a[33], a[39]); SQRADDAC(a[34], a[38]); SQRADDAC(a[35], a[37]); SQRADDDB; SQRADD(a[36], a[36]); 
   COMBA_STORE(b[72]);

   /* output 73 */
   CARRY_FORWARD;
   SQRADDSC(a[10], a[63]); SQRADDAC(a[11], a[62]); SQRADDAC(a[12], a[61]); SQRADDAC(a[13], a[60]); SQRADDAC(a[14], a[59]); SQRADDAC(a[15], a[58]); SQRADDAC(a[16], a[57]); SQRADDAC(a[17], a[56]); SQRADDAC(a[18], a[55]); SQRADDAC(a[19], a[54]); SQRADDAC(a[20], a[53]); SQRADDAC(a[21], a[52]); SQRADDAC(a[22], a[51]); SQRADDAC(a[23], a[50]); SQRADDAC(a[24], a[49]); SQRADDAC(a[25], a[48]); SQRADDAC(a[26], a[47]); SQRADDAC(a[27], a[46]); SQRADDAC(a[28], a[45]); SQRADDAC(a[29], a[44]); SQRADDAC(a[30], a[43]); SQRADDAC(a[31], a[42]); SQRADDAC(a[32], a[41]); SQRADDAC(a[33], a[40]); SQRADDAC(a[34], a[39]); SQRADDAC(a[35], a[38]); SQRADDAC(a[36], a[37]); SQRADDDB; 
   COMBA_STORE(b[73]);

   /* output 74 */
   CARRY_FORWARD;
   SQRADDSC(a[11], a[63]); SQRADDAC(a[12], a[62]); SQRADDAC(a[13], a[61]); SQRADDAC(a[14], a[60]); SQRADDAC(a[15], a[59]); SQRADDAC(a[16], a[58]); SQRADDAC(a[17], a[57]); SQRADDAC(a[18], a[56]); SQRADDAC(a[19], a[55]); SQRADDAC(a[20], a[54]); SQRADDAC(a[21], a[53]); SQRADDAC(a[22], a[52]); SQRADDAC(a[23], a[51]); SQRADDAC(a[24], a[50]); SQRADDAC(a[25], a[49]); SQRADDAC(a[26], a[48]); SQRADDAC(a[27], a[47]); SQRADDAC(a[28], a[46]); SQRADDAC(a[29], a[45]); SQRADDAC(a[30], a[44]); SQRADDAC(a[31], a[43]); SQRADDAC(a[32], a[42]); SQRADDAC(a[33], a[41]); SQRADDAC(a[34], a[40]); SQRADDAC(a[35], a[39]); SQRADDAC(a[36], a[38]); SQRADDDB; SQRADD(a[37], a[37]); 
   COMBA_STORE(b[74]);

   /* output 75 */
   CARRY_FORWARD;
   SQRADDSC(a[12], a[63]); SQRADDAC(a[13], a[62]); SQRADDAC(a[14], a[61]); SQRADDAC(a[15], a[60]); SQRADDAC(a[16], a[59]); SQRADDAC(a[17], a[58]); SQRADDAC(a[18], a[57]); SQRADDAC(a[19], a[56]); SQRADDAC(a[20], a[55]); SQRADDAC(a[21], a[54]); SQRADDAC(a[22], a[53]); SQRADDAC(a[23], a[52]); SQRADDAC(a[24], a[51]); SQRADDAC(a[25], a[50]); SQRADDAC(a[26], a[49]); SQRADDAC(a[27], a[48]); SQRADDAC(a[28], a[47]); SQRADDAC(a[29], a[46]); SQRADDAC(a[30], a[45]); SQRADDAC(a[31], a[44]); SQRADDAC(a[32], a[43]); SQRADDAC(a[33], a[42]); SQRADDAC(a[34], a[41]); SQRADDAC(a[35], a[40]); SQRADDAC(a[36], a[39]); SQRADDAC(a[37], a[38]); SQRADDDB; 
   COMBA_STORE(b[75]);

   /* output 76 */
   CARRY_FORWARD;
   SQRADDSC(a[13], a[63]); SQRADDAC(a[14], a[62]); SQRADDAC(a[15], a[61]); SQRADDAC(a[16], a[60]); SQRADDAC(a[17], a[59]); SQRADDAC(a[18], a[58]); SQRADDAC(a[19], a[57]); SQRADDAC(a[20], a[56]); SQRADDAC(a[21], a[55]); SQRADDAC(a[22], a[54]); SQRADDAC(a[23], a[53]); SQRADDAC(a[24], a[52]); SQRADDAC(a[25], a[51]); SQRADDAC(a[26], a[50]); SQRADDAC(a[27], a[49]); SQRADDAC(a[28], a[48]); SQRADDAC(a[29], a[47]); SQRADDAC(a[30], a[46]); SQRADDAC(a[31], a[45]); SQRADDAC(a[32], a[44]); SQRADDAC(a[33], a[43]); SQRADDAC(a[34], a[42]); SQRADDAC(a[35], a[41]); SQRADDAC(a[36], a[40]); SQRADDAC(a[37], a[39]); SQRADDDB; SQRADD(a[38], a[38]); 
   COMBA_STORE(b[76]);

   /* output 77 */
   CARRY_FORWARD;
   SQRADDSC(a[14], a[63]); SQRADDAC(a[15], a[62]); SQRADDAC(a[16], a[61]); SQRADDAC(a[17], a[60]); SQRADDAC(a[18], a[59]); SQRADDAC(a[19], a[58]); SQRADDAC(a[20], a[57]); SQRADDAC(a[21], a[56]); SQRADDAC(a[22], a[55]); SQRADDAC(a[23], a[54]); SQRADDAC(a[24], a[53]); SQRADDAC(a[25], a[52]); SQRADDAC(a[26], a[51]); SQRADDAC(a[27], a[50]); SQRADDAC(a[28], a[49]); SQRADDAC(a[29], a[48]); SQRADDAC(a[30], a[47]); SQRADDAC(a[31], a[46]); SQRADDAC(a[32], a[45]); SQRADDAC(a[33], a[44]); SQRADDAC(a[34], a[43]); SQRADDAC(a[35], a[42]); SQRADDAC(a[36], a[41]); SQRADDAC(a[37], a[40]); SQRADDAC(a[38], a[39]); SQRADDDB; 
   COMBA_STORE(b[77]);

   /* output 78 */
   CARRY_FORWARD;
   SQRADDSC(a[15], a[63]); SQRADDAC(a[16], a[62]); SQRADDAC(a[17], a[61]); SQRADDAC(a[18], a[60]); SQRADDAC(a[19], a[59]); SQRADDAC(a[20], a[58]); SQRADDAC(a[21], a[57]); SQRADDAC(a[22], a[56]); SQRADDAC(a[23], a[55]); SQRADDAC(a[24], a[54]); SQRADDAC(a[25], a[53]); SQRADDAC(a[26], a[52]); SQRADDAC(a[27], a[51]); SQRADDAC(a[28], a[50]); SQRADDAC(a[29], a[49]); SQRADDAC(a[30], a[48]); SQRADDAC(a[31], a[47]); SQRADDAC(a[32], a[46]); SQRADDAC(a[33], a[45]); SQRADDAC(a[34], a[44]); SQRADDAC(a[35], a[43]); SQRADDAC(a[36], a[42]); SQRADDAC(a[37], a[41]); SQRADDAC(a[38], a[40]); SQRADDDB; SQRADD(a[39], a[39]); 
   COMBA_STORE(b[78]);

   /* output 79 */
   CARRY_FORWARD;
   SQRADDSC(a[16], a[63]); SQRADDAC(a[17], a[62]); SQRADDAC(a[18], a[61]); SQRADDAC(a[19], a[60]); SQRADDAC(a[20], a[59]); SQRADDAC(a[21], a[58]); SQRADDAC(a[22], a[57]); SQRADDAC(a[23], a[56]); SQRADDAC(a[24], a[55]); SQRADDAC(a[25], a[54]); SQRADDAC(a[26], a[53]); SQRADDAC(a[27], a[52]); SQRADDAC(a[28], a[51]); SQRADDAC(a[29], a[50]); SQRADDAC(a[30], a[49]); SQRADDAC(a[31], a[48]); SQRADDAC(a[32], a[47]); SQRADDAC(a[33], a[46]); SQRADDAC(a[34], a[45]); SQRADDAC(a[35], a[44]); SQRADDAC(a[36], a[43]); SQRADDAC(a[37], a[42]); SQRADDAC(a[38], a[41]); SQRADDAC(a[39], a[40]); SQRADDDB; 
   COMBA_STORE(b[79]);

   /* output 80 */
   CARRY_FORWARD;
   SQRADDSC(a[17], a[63]); SQRADDAC(a[18], a[62]); SQRADDAC(a[19], a[61]); SQRADDAC(a[20], a[60]); SQRADDAC(a[21], a[59]); SQRADDAC(a[22], a[58]); SQRADDAC(a[23], a[57]); SQRADDAC(a[24], a[56]); SQRADDAC(a[25], a[55]); SQRADDAC(a[26], a[54]); SQRADDAC(a[27], a[53]); SQRADDAC(a[28], a[52]); SQRADDAC(a[29], a[51]); SQRADDAC(a[30], a[50]); SQRADDAC(a[31], a[49]); SQRADDAC(a[32], a[48]); SQRADDAC(a[33], a[47]); SQRADDAC(a[34], a[46]); SQRADDAC(a[35], a[45]); SQRADDAC(a[36], a[44]); SQRADDAC(a[37], a[43]); SQRADDAC(a[38], a[42]); SQRADDAC(a[39], a[41]); SQRADDDB; SQRADD(a[40], a[40]); 
   COMBA_STORE(b[80]);

   /* output 81 */
   CARRY_FORWARD;
   SQRADDSC(a[18], a[63]); SQRADDAC(a[19], a[62]); SQRADDAC(a[20], a[61]); SQRADDAC(a[21], a[60]); SQRADDAC(a[22], a[59]); SQRADDAC(a[23], a[58]); SQRADDAC(a[24], a[57]); SQRADDAC(a[25], a[56]); SQRADDAC(a[26], a[55]); SQRADDAC(a[27], a[54]); SQRADDAC(a[28], a[53]); SQRADDAC(a[29], a[52]); SQRADDAC(a[30], a[51]); SQRADDAC(a[31], a[50]); SQRADDAC(a[32], a[49]); SQRADDAC(a[33], a[48]); SQRADDAC(a[34], a[47]); SQRADDAC(a[35], a[46]); SQRADDAC(a[36], a[45]); SQRADDAC(a[37], a[44]); SQRADDAC(a[38], a[43]); SQRADDAC(a[39], a[42]); SQRADDAC(a[40], a[41]); SQRADDDB; 
   COMBA_STORE(b[81]);

   /* output 82 */
   CARRY_FORWARD;
   SQRADDSC(a[19], a[63]); SQRADDAC(a[20], a[62]); SQRADDAC(a[21], a[61]); SQRADDAC(a[22], a[60]); SQRADDAC(a[23], a[59]); SQRADDAC(a[24], a[58]); SQRADDAC(a[25], a[57]); SQRADDAC(a[26], a[56]); SQRADDAC(a[27], a[55]); SQRADDAC(a[28], a[54]); SQRADDAC(a[29], a[53]); SQRADDAC(a[30], a[52]); SQRADDAC(a[31], a[51]); SQRADDAC(a[32], a[50]); SQRADDAC(a[33], a[49]); SQRADDAC(a[34], a[48]); SQRADDAC(a[35], a[47]); SQRADDAC(a[36], a[46]); SQRADDAC(a[37], a[45]); SQRADDAC(a[38], a[44]); SQRADDAC(a[39], a[43]); SQRADDAC(a[40], a[42]); SQRADDDB; SQRADD(a[41], a[41]); 
   COMBA_STORE(b[82]);

   /* output 83 */
   CARRY_FORWARD;
   SQRADDSC(a[20], a[63]); SQRADDAC(a[21], a[62]); SQRADDAC(a[22], a[61]); SQRADDAC(a[23], a[60]); SQRADDAC(a[24], a[59]); SQRADDAC(a[25], a[58]); SQRADDAC(a[26], a[57]); SQRADDAC(a[27], a[56]); SQRADDAC(a[28], a[55]); SQRADDAC(a[29], a[54]); SQRADDAC(a[30], a[53]); SQRADDAC(a[31], a[52]); SQRADDAC(a[32], a[51]); SQRADDAC(a[33], a[50]); SQRADDAC(a[34], a[49]); SQRADDAC(a[35], a[48]); SQRADDAC(a[36], a[47]); SQRADDAC(a[37], a[46]); SQRADDAC(a[38], a[45]); SQRADDAC(a[39], a[44]); SQRADDAC(a[40], a[43]); SQRADDAC(a[41], a[42]); SQRADDDB; 
   COMBA_STORE(b[83]);

   /* output 84 */
   CARRY_FORWARD;
   SQRADDSC(a[21], a[63]); SQRADDAC(a[22], a[62]); SQRADDAC(a[23], a[61]); SQRADDAC(a[24], a[60]); SQRADDAC(a[25], a[59]); SQRADDAC(a[26], a[58]); SQRADDAC(a[27], a[57]); SQRADDAC(a[28], a[56]); SQRADDAC(a[29], a[55]); SQRADDAC(a[30], a[54]); SQRADDAC(a[31], a[53]); SQRADDAC(a[32], a[52]); SQRADDAC(a[33], a[51]); SQRADDAC(a[34], a[50]); SQRADDAC(a[35], a[49]); SQRADDAC(a[36], a[48]); SQRADDAC(a[37], a[47]); SQRADDAC(a[38], a[46]); SQRADDAC(a[39], a[45]); SQRADDAC(a[40], a[44]); SQRADDAC(a[41], a[43]); SQRADDDB; SQRADD(a[42], a[42]); 
   COMBA_STORE(b[84]);

   /* output 85 */
   CARRY_FORWARD;
   SQRADDSC(a[22], a[63]); SQRADDAC(a[23], a[62]); SQRADDAC(a[24], a[61]); SQRADDAC(a[25], a[60]); SQRADDAC(a[26], a[59]); SQRADDAC(a[27], a[58]); SQRADDAC(a[28], a[57]); SQRADDAC(a[29], a[56]); SQRADDAC(a[30], a[55]); SQRADDAC(a[31], a[54]); SQRADDAC(a[32], a[53]); SQRADDAC(a[33], a[52]); SQRADDAC(a[34], a[51]); SQRADDAC(a[35], a[50]); SQRADDAC(a[36], a[49]); SQRADDAC(a[37], a[48]); SQRADDAC(a[38], a[47]); SQRADDAC(a[39], a[46]); SQRADDAC(a[40], a[45]); SQRADDAC(a[41], a[44]); SQRADDAC(a[42], a[43]); SQRADDDB; 
   COMBA_STORE(b[85]);

   /* output 86 */
   CARRY_FORWARD;
   SQRADDSC(a[23], a[63]); SQRADDAC(a[24], a[62]); SQRADDAC(a[25], a[61]); SQRADDAC(a[26], a[60]); SQRADDAC(a[27], a[59]); SQRADDAC(a[28], a[58]); SQRADDAC(a[29], a[57]); SQRADDAC(a[30], a[56]); SQRADDAC(a[31], a[55]); SQRADDAC(a[32], a[54]); SQRADDAC(a[33], a[53]); SQRADDAC(a[34], a[52]); SQRADDAC(a[35], a[51]); SQRADDAC(a[36], a[50]); SQRADDAC(a[37], a[49]); SQRADDAC(a[38], a[48]); SQRADDAC(a[39], a[47]); SQRADDAC(a[40], a[46]); SQRADDAC(a[41], a[45]); SQRADDAC(a[42], a[44]); SQRADDDB; SQRADD(a[43], a[43]); 
   COMBA_STORE(b[86]);

   /* output 87 */
   CARRY_FORWARD;
   SQRADDSC(a[24], a[63]); SQRADDAC(a[25], a[62]); SQRADDAC(a[26], a[61]); SQRADDAC(a[27], a[60]); SQRADDAC(a[28], a[59]); SQRADDAC(a[29], a[58]); SQRADDAC(a[30], a[57]); SQRADDAC(a[31], a[56]); SQRADDAC(a[32], a[55]); SQRADDAC(a[33], a[54]); SQRADDAC(a[34], a[53]); SQRADDAC(a[35], a[52]); SQRADDAC(a[36], a[51]); SQRADDAC(a[37], a[50]); SQRADDAC(a[38], a[49]); SQRADDAC(a[39], a[48]); SQRADDAC(a[40], a[47]); SQRADDAC(a[41], a[46]); SQRADDAC(a[42], a[45]); SQRADDAC(a[43], a[44]); SQRADDDB; 
   COMBA_STORE(b[87]);

   /* output 88 */
   CARRY_FORWARD;
   SQRADDSC(a[25], a[63]); SQRADDAC(a[26], a[62]); SQRADDAC(a[27], a[61]); SQRADDAC(a[28], a[60]); SQRADDAC(a[29], a[59]); SQRADDAC(a[30], a[58]); SQRADDAC(a[31], a[57]); SQRADDAC(a[32], a[56]); SQRADDAC(a[33], a[55]); SQRADDAC(a[34], a[54]); SQRADDAC(a[35], a[53]); SQRADDAC(a[36], a[52]); SQRADDAC(a[37], a[51]); SQRADDAC(a[38], a[50]); SQRADDAC(a[39], a[49]); SQRADDAC(a[40], a[48]); SQRADDAC(a[41], a[47]); SQRADDAC(a[42], a[46]); SQRADDAC(a[43], a[45]); SQRADDDB; SQRADD(a[44], a[44]); 
   COMBA_STORE(b[88]);

   /* output 89 */
   CARRY_FORWARD;
   SQRADDSC(a[26], a[63]); SQRADDAC(a[27], a[62]); SQRADDAC(a[28], a[61]); SQRADDAC(a[29], a[60]); SQRADDAC(a[30], a[59]); SQRADDAC(a[31], a[58]); SQRADDAC(a[32], a[57]); SQRADDAC(a[33], a[56]); SQRADDAC(a[34], a[55]); SQRADDAC(a[35], a[54]); SQRADDAC(a[36], a[53]); SQRADDAC(a[37], a[52]); SQRADDAC(a[38], a[51]); SQRADDAC(a[39], a[50]); SQRADDAC(a[40], a[49]); SQRADDAC(a[41], a[48]); SQRADDAC(a[42], a[47]); SQRADDAC(a[43], a[46]); SQRADDAC(a[44], a[45]); SQRADDDB; 
   COMBA_STORE(b[89]);

   /* output 90 */
   CARRY_FORWARD;
   SQRADDSC(a[27], a[63]); SQRADDAC(a[28], a[62]); SQRADDAC(a[29], a[61]); SQRADDAC(a[30], a[60]); SQRADDAC(a[31], a[59]); SQRADDAC(a[32], a[58]); SQRADDAC(a[33], a[57]); SQRADDAC(a[34], a[56]); SQRADDAC(a[35], a[55]); SQRADDAC(a[36], a[54]); SQRADDAC(a[37], a[53]); SQRADDAC(a[38], a[52]); SQRADDAC(a[39], a[51]); SQRADDAC(a[40], a[50]); SQRADDAC(a[41], a[49]); SQRADDAC(a[42], a[48]); SQRADDAC(a[43], a[47]); SQRADDAC(a[44], a[46]); SQRADDDB; SQRADD(a[45], a[45]); 
   COMBA_STORE(b[90]);

   /* output 91 */
   CARRY_FORWARD;
   SQRADDSC(a[28], a[63]); SQRADDAC(a[29], a[62]); SQRADDAC(a[30], a[61]); SQRADDAC(a[31], a[60]); SQRADDAC(a[32], a[59]); SQRADDAC(a[33], a[58]); SQRADDAC(a[34], a[57]); SQRADDAC(a[35], a[56]); SQRADDAC(a[36], a[55]); SQRADDAC(a[37], a[54]); SQRADDAC(a[38], a[53]); SQRADDAC(a[39], a[52]); SQRADDAC(a[40], a[51]); SQRADDAC(a[41], a[50]); SQRADDAC(a[42], a[49]); SQRADDAC(a[43], a[48]); SQRADDAC(a[44], a[47]); SQRADDAC(a[45], a[46]); SQRADDDB; 
   COMBA_STORE(b[91]);

   /* output 92 */
   CARRY_FORWARD;
   SQRADDSC(a[29], a[63]); SQRADDAC(a[30], a[62]); SQRADDAC(a[31], a[61]); SQRADDAC(a[32], a[60]); SQRADDAC(a[33], a[59]); SQRADDAC(a[34], a[58]); SQRADDAC(a[35], a[57]); SQRADDAC(a[36], a[56]); SQRADDAC(a[37], a[55]); SQRADDAC(a[38], a[54]); SQRADDAC(a[39], a[53]); SQRADDAC(a[40], a[52]); SQRADDAC(a[41], a[51]); SQRADDAC(a[42], a[50]); SQRADDAC(a[43], a[49]); SQRADDAC(a[44], a[48]); SQRADDAC(a[45], a[47]); SQRADDDB; SQRADD(a[46], a[46]); 
   COMBA_STORE(b[92]);

   /* output 93 */
   CARRY_FORWARD;
   SQRADDSC(a[30], a[63]); SQRADDAC(a[31], a[62]); SQRADDAC(a[32], a[61]); SQRADDAC(a[33], a[60]); SQRADDAC(a[34], a[59]); SQRADDAC(a[35], a[58]); SQRADDAC(a[36], a[57]); SQRADDAC(a[37], a[56]); SQRADDAC(a[38], a[55]); SQRADDAC(a[39], a[54]); SQRADDAC(a[40], a[53]); SQRADDAC(a[41], a[52]); SQRADDAC(a[42], a[51]); SQRADDAC(a[43], a[50]); SQRADDAC(a[44], a[49]); SQRADDAC(a[45], a[48]); SQRADDAC(a[46], a[47]); SQRADDDB; 
   COMBA_STORE(b[93]);

   /* output 94 */
   CARRY_FORWARD;
   SQRADDSC(a[31], a[63]); SQRADDAC(a[32], a[62]); SQRADDAC(a[33], a[61]); SQRADDAC(a[34], a[60]); SQRADDAC(a[35], a[59]); SQRADDAC(a[36], a[58]); SQRADDAC(a[37], a[57]); SQRADDAC(a[38], a[56]); SQRADDAC(a[39], a[55]); SQRADDAC(a[40], a[54]); SQRADDAC(a[41], a[53]); SQRADDAC(a[42], a[52]); SQRADDAC(a[43], a[51]); SQRADDAC(a[44], a[50]); SQRADDAC(a[45], a[49]); SQRADDAC(a[46], a[48]); SQRADDDB; SQRADD(a[47], a[47]); 
   COMBA_STORE(b[94]);

   /* output 95 */
   CARRY_FORWARD;
   SQRADDSC(a[32], a[63]); SQRADDAC(a[33], a[62]); SQRADDAC(a[34], a[61]); SQRADDAC(a[35], a[60]); SQRADDAC(a[36], a[59]); SQRADDAC(a[37], a[58]); SQRADDAC(a[38], a[57]); SQRADDAC(a[39], a[56]); SQRADDAC(a[40], a[55]); SQRADDAC(a[41], a[54]); SQRADDAC(a[42], a[53]); SQRADDAC(a[43], a[52]); SQRADDAC(a[44], a[51]); SQRADDAC(a[45], a[50]); SQRADDAC(a[46], a[49]); SQRADDAC(a[47], a[48]); SQRADDDB; 
   COMBA_STORE(b[95]);

   /* output 96 */
   CARRY_FORWARD;
   SQRADDSC(a[33], a[63]); SQRADDAC(a[34], a[62]); SQRADDAC(a[35], a[61]); SQRADDAC(a[36], a[60]); SQRADDAC(a[37], a[59]); SQRADDAC(a[38], a[58]); SQRADDAC(a[39], a[57]); SQRADDAC(a[40], a[56]); SQRADDAC(a[41], a[55]); SQRADDAC(a[42], a[54]); SQRADDAC(a[43], a[53]); SQRADDAC(a[44], a[52]); SQRADDAC(a[45], a[51]); SQRADDAC(a[46], a[50]); SQRADDAC(a[47], a[49]); SQRADDDB; SQRADD(a[48], a[48]); 
   COMBA_STORE(b[96]);

   /* output 97 */
   CARRY_FORWARD;
   SQRADDSC(a[34], a[63]); SQRADDAC(a[35], a[62]); SQRADDAC(a[36], a[61]); SQRADDAC(a[37], a[60]); SQRADDAC(a[38], a[59]); SQRADDAC(a[39], a[58]); SQRADDAC(a[40], a[57]); SQRADDAC(a[41], a[56]); SQRADDAC(a[42], a[55]); SQRADDAC(a[43], a[54]); SQRADDAC(a[44], a[53]); SQRADDAC(a[45], a[52]); SQRADDAC(a[46], a[51]); SQRADDAC(a[47], a[50]); SQRADDAC(a[48], a[49]); SQRADDDB; 
   COMBA_STORE(b[97]);

   /* output 98 */
   CARRY_FORWARD;
   SQRADDSC(a[35], a[63]); SQRADDAC(a[36], a[62]); SQRADDAC(a[37], a[61]); SQRADDAC(a[38], a[60]); SQRADDAC(a[39], a[59]); SQRADDAC(a[40], a[58]); SQRADDAC(a[41], a[57]); SQRADDAC(a[42], a[56]); SQRADDAC(a[43], a[55]); SQRADDAC(a[44], a[54]); SQRADDAC(a[45], a[53]); SQRADDAC(a[46], a[52]); SQRADDAC(a[47], a[51]); SQRADDAC(a[48], a[50]); SQRADDDB; SQRADD(a[49], a[49]); 
   COMBA_STORE(b[98]);

   /* output 99 */
   CARRY_FORWARD;
   SQRADDSC(a[36], a[63]); SQRADDAC(a[37], a[62]); SQRADDAC(a[38], a[61]); SQRADDAC(a[39], a[60]); SQRADDAC(a[40], a[59]); SQRADDAC(a[41], a[58]); SQRADDAC(a[42], a[57]); SQRADDAC(a[43], a[56]); SQRADDAC(a[44], a[55]); SQRADDAC(a[45], a[54]); SQRADDAC(a[46], a[53]); SQRADDAC(a[47], a[52]); SQRADDAC(a[48], a[51]); SQRADDAC(a[49], a[50]); SQRADDDB; 
   COMBA_STORE(b[99]);

   /* output 100 */
   CARRY_FORWARD;
   SQRADDSC(a[37], a[63]); SQRADDAC(a[38], a[62]); SQRADDAC(a[39], a[61]); SQRADDAC(a[40], a[60]); SQRADDAC(a[41], a[59]); SQRADDAC(a[42], a[58]); SQRADDAC(a[43], a[57]); SQRADDAC(a[44], a[56]); SQRADDAC(a[45], a[55]); SQRADDAC(a[46], a[54]); SQRADDAC(a[47], a[53]); SQRADDAC(a[48], a[52]); SQRADDAC(a[49], a[51]); SQRADDDB; SQRADD(a[50], a[50]); 
   COMBA_STORE(b[100]);

   /* output 101 */
   CARRY_FORWARD;
   SQRADDSC(a[38], a[63]); SQRADDAC(a[39], a[62]); SQRADDAC(a[40], a[61]); SQRADDAC(a[41], a[60]); SQRADDAC(a[42], a[59]); SQRADDAC(a[43], a[58]); SQRADDAC(a[44], a[57]); SQRADDAC(a[45], a[56]); SQRADDAC(a[46], a[55]); SQRADDAC(a[47], a[54]); SQRADDAC(a[48], a[53]); SQRADDAC(a[49], a[52]); SQRADDAC(a[50], a[51]); SQRADDDB; 
   COMBA_STORE(b[101]);

   /* output 102 */
   CARRY_FORWARD;
   SQRADDSC(a[39], a[63]); SQRADDAC(a[40], a[62]); SQRADDAC(a[41], a[61]); SQRADDAC(a[42], a[60]); SQRADDAC(a[43], a[59]); SQRADDAC(a[44], a[58]); SQRADDAC(a[45], a[57]); SQRADDAC(a[46], a[56]); SQRADDAC(a[47], a[55]); SQRADDAC(a[48], a[54]); SQRADDAC(a[49], a[53]); SQRADDAC(a[50], a[52]); SQRADDDB; SQRADD(a[51], a[51]); 
   COMBA_STORE(b[102]);

   /* output 103 */
   CARRY_FORWARD;
   SQRADDSC(a[40], a[63]); SQRADDAC(a[41], a[62]); SQRADDAC(a[42], a[61]); SQRADDAC(a[43], a[60]); SQRADDAC(a[44], a[59]); SQRADDAC(a[45], a[58]); SQRADDAC(a[46], a[57]); SQRADDAC(a[47], a[56]); SQRADDAC(a[48], a[55]); SQRADDAC(a[49], a[54]); SQRADDAC(a[50], a[53]); SQRADDAC(a[51], a[52]); SQRADDDB; 
   COMBA_STORE(b[103]);

   /* output 104 */
   CARRY_FORWARD;
   SQRADDSC(a[41], a[63]); SQRADDAC(a[42], a[62]); SQRADDAC(a[43], a[61]); SQRADDAC(a[44], a[60]); SQRADDAC(a[45], a[59]); SQRADDAC(a[46], a[58]); SQRADDAC(a[47], a[57]); SQRADDAC(a[48], a[56]); SQRADDAC(a[49], a[55]); SQRADDAC(a[50], a[54]); SQRADDAC(a[51], a[53]); SQRADDDB; SQRADD(a[52], a[52]); 
   COMBA_STORE(b[104]);

   /* output 105 */
   CARRY_FORWARD;
   SQRADDSC(a[42], a[63]); SQRADDAC(a[43], a[62]); SQRADDAC(a[44], a[61]); SQRADDAC(a[45], a[60]); SQRADDAC(a[46], a[59]); SQRADDAC(a[47], a[58]); SQRADDAC(a[48], a[57]); SQRADDAC(a[49], a[56]); SQRADDAC(a[50], a[55]); SQRADDAC(a[51], a[54]); SQRADDAC(a[52], a[53]); SQRADDDB; 
   COMBA_STORE(b[105]);

   /* output 106 */
   CARRY_FORWARD;
   SQRADDSC(a[43], a[63]); SQRADDAC(a[44], a[62]); SQRADDAC(a[45], a[61]); SQRADDAC(a[46], a[60]); SQRADDAC(a[47], a[59]); SQRADDAC(a[48], a[58]); SQRADDAC(a[49], a[57]); SQRADDAC(a[50], a[56]); SQRADDAC(a[51], a[55]); SQRADDAC(a[52], a[54]); SQRADDDB; SQRADD(a[53], a[53]); 
   COMBA_STORE(b[106]);

   /* output 107 */
   CARRY_FORWARD;
   SQRADDSC(a[44], a[63]); SQRADDAC(a[45], a[62]); SQRADDAC(a[46], a[61]); SQRADDAC(a[47], a[60]); SQRADDAC(a[48], a[59]); SQRADDAC(a[49], a[58]); SQRADDAC(a[50], a[57]); SQRADDAC(a[51], a[56]); SQRADDAC(a[52], a[55]); SQRADDAC(a[53], a[54]); SQRADDDB; 
   COMBA_STORE(b[107]);

   /* output 108 */
   CARRY_FORWARD;
   SQRADDSC(a[45], a[63]); SQRADDAC(a[46], a[62]); SQRADDAC(a[47], a[61]); SQRADDAC(a[48], a[60]); SQRADDAC(a[49], a[59]); SQRADDAC(a[50], a[58]); SQRADDAC(a[51], a[57]); SQRADDAC(a[52], a[56]); SQRADDAC(a[53], a[55]); SQRADDDB; SQRADD(a[54], a[54]); 
   COMBA_STORE(b[108]);

   /* output 109 */
   CARRY_FORWARD;
   SQRADDSC(a[46], a[63]); SQRADDAC(a[47], a[62]); SQRADDAC(a[48], a[61]); SQRADDAC(a[49], a[60]); SQRADDAC(a[50], a[59]); SQRADDAC(a[51], a[58]); SQRADDAC(a[52], a[57]); SQRADDAC(a[53], a[56]); SQRADDAC(a[54], a[55]); SQRADDDB; 
   COMBA_STORE(b[109]);

   /* output 110 */
   CARRY_FORWARD;
   SQRADDSC(a[47], a[63]); SQRADDAC(a[48], a[62]); SQRADDAC(a[49], a[61]); SQRADDAC(a[50], a[60]); SQRADDAC(a[51], a[59]); SQRADDAC(a[52], a[58]); SQRADDAC(a[53], a[57]); SQRADDAC(a[54], a[56]); SQRADDDB; SQRADD(a[55], a[55]); 
   COMBA_STORE(b[110]);

   /* output 111 */
   CARRY_FORWARD;
   SQRADDSC(a[48], a[63]); SQRADDAC(a[49], a[62]); SQRADDAC(a[50], a[61]); SQRADDAC(a[51], a[60]); SQRADDAC(a[52], a[59]); SQRADDAC(a[53], a[58]); SQRADDAC(a[54], a[57]); SQRADDAC(a[55], a[56]); SQRADDDB; 
   COMBA_STORE(b[111]);

   /* output 112 */
   CARRY_FORWARD;
   SQRADDSC(a[49], a[63]); SQRADDAC(a[50], a[62]); SQRADDAC(a[51], a[61]); SQRADDAC(a[52], a[60]); SQRADDAC(a[53], a[59]); SQRADDAC(a[54], a[58]); SQRADDAC(a[55], a[57]); SQRADDDB; SQRADD(a[56], a[56]); 
   COMBA_STORE(b[112]);

   /* output 113 */
   CARRY_FORWARD;
   SQRADDSC(a[50], a[63]); SQRADDAC(a[51], a[62]); SQRADDAC(a[52], a[61]); SQRADDAC(a[53], a[60]); SQRADDAC(a[54], a[59]); SQRADDAC(a[55], a[58]); SQRADDAC(a[56], a[57]); SQRADDDB; 
   COMBA_STORE(b[113]);

   /* output 114 */
   CARRY_FORWARD;
   SQRADDSC(a[51], a[63]); SQRADDAC(a[52], a[62]); SQRADDAC(a[53], a[61]); SQRADDAC(a[54], a[60]); SQRADDAC(a[55], a[59]); SQRADDAC(a[56], a[58]); SQRADDDB; SQRADD(a[57], a[57]); 
   COMBA_STORE(b[114]);

   /* output 115 */
   CARRY_FORWARD;
   SQRADDSC(a[52], a[63]); SQRADDAC(a[53], a[62]); SQRADDAC(a[54], a[61]); SQRADDAC(a[55], a[60]); SQRADDAC(a[56], a[59]); SQRADDAC(a[57], a[58]); SQRADDDB; 
   COMBA_STORE(b[115]);

   /* output 116 */
   CARRY_FORWARD;
   SQRADDSC(a[53], a[63]); SQRADDAC(a[54], a[62]); SQRADDAC(a[55], a[61]); SQRADDAC(a[56], a[60]); SQRADDAC(a[57], a[59]); SQRADDDB; SQRADD(a[58], a[58]); 
   COMBA_STORE(b[116]);

   /* output 117 */
   CARRY_FORWARD;
   SQRADDSC(a[54], a[63]); SQRADDAC(a[55], a[62]); SQRADDAC(a[56], a[61]); SQRADDAC(a[57], a[60]); SQRADDAC(a[58], a[59]); SQRADDDB; 
   COMBA_STORE(b[117]);

   /* output 118 */
   CARRY_FORWARD;
   SQRADDSC(a[55], a[63]); SQRADDAC(a[56], a[62]); SQRADDAC(a[57], a[61]); SQRADDAC(a[58], a[60]); SQRADDDB; SQRADD(a[59], a[59]); 
   COMBA_STORE(b[118]);

   /* output 119 */
   CARRY_FORWARD;
   SQRADDSC(a[56], a[63]); SQRADDAC(a[57], a[62]); SQRADDAC(a[58], a[61]); SQRADDAC(a[59], a[60]); SQRADDDB; 
   COMBA_STORE(b[119]);

   /* output 120 */
   CARRY_FORWARD;
   SQRADDSC(a[57], a[63]); SQRADDAC(a[58], a[62]); SQRADDAC(a[59], a[61]); SQRADDDB; SQRADD(a[60], a[60]); 
   COMBA_STORE(b[120]);

   /* output 121 */
   CARRY_FORWARD;
   SQRADDSC(a[58], a[63]); SQRADDAC(a[59], a[62]); SQRADDAC(a[60], a[61]); SQRADDDB; 
   COMBA_STORE(b[121]);

   /* output 122 */
   CARRY_FORWARD;
   SQRADD2(a[59], a[63]); SQRADD2(a[60], a[62]); SQRADD(a[61], a[61]); 
   COMBA_STORE(b[122]);

   /* output 123 */
   CARRY_FORWARD;
   SQRADD2(a[60], a[63]); SQRADD2(a[61], a[62]); 
   COMBA_STORE(b[123]);

   /* output 124 */
   CARRY_FORWARD;
   SQRADD2(a[61], a[63]); SQRADD(a[62], a[62]); 
   COMBA_STORE(b[124]);

   /* output 125 */
   CARRY_FORWARD;
   SQRADD2(a[62], a[63]); 
   COMBA_STORE(b[125]);

   /* output 126 */
   CARRY_FORWARD;
   SQRADD(a[63], a[63]); 
   COMBA_STORE(b[126]);
   COMBA_STORE2(b[127]);
   COMBA_FINI;

   B->used = 128;
   B->sign = FP_ZPOS;
   XMEMCPY(B->dp, b, 128 * sizeof(fp_digit));
   fp_clamp(B);

#ifdef WOLFSSL_SMALL_STACK
   XFREE(b, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
   return FP_OKAY;
}
#endif


