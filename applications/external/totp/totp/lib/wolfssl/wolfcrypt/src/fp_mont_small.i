/* fp_mont_small.i
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



#ifdef TFM_SMALL_MONT_SET
/* computes x/R == x (mod N) via Montgomery Reduction */
int fp_montgomery_reduce_small(fp_int *a, fp_int *m, fp_digit mp)
{
#ifndef WOLFSSL_SMALL_STACK
   fp_digit c[FP_SIZE];
#else
   fp_digit *c;
#endif
   fp_digit *_c, *tmpm, mu, cy;
   int      oldused, x, y, pa;

#ifdef WOLFSSL_SMALL_STACK
   /* only allocate space for what's needed for window plus res */
   c = (fp_digit*)XMALLOC(sizeof(fp_digit)*FP_SIZE, NULL, DYNAMIC_TYPE_TMP_BUFFER);
   if (c == NULL) {
      return FP_MEM;
   }
#endif

   /* now zero the buff */
   XMEMSET(c, 0, sizeof(fp_digit)*(FP_SIZE));

   pa = m->used;

   /* copy the input */
   oldused = a->used;
   for (x = 0; x < oldused; x++) {
       c[x] = a->dp[x];
   }

   MONT_START;

   switch (pa) {
      case 1:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 2:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 3:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 4:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 5:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 6:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 7:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 8:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 9:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 10:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 11:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 10; cy   = 0;
            LOOP_START;
            _c   = c + 10;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 12:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 10; cy   = 0;
            LOOP_START;
            _c   = c + 10;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 11; cy   = 0;
            LOOP_START;
            _c   = c + 11;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 13:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 10; cy   = 0;
            LOOP_START;
            _c   = c + 10;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 11; cy   = 0;
            LOOP_START;
            _c   = c + 11;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 12; cy   = 0;
            LOOP_START;
            _c   = c + 12;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 14:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 10; cy   = 0;
            LOOP_START;
            _c   = c + 10;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 11; cy   = 0;
            LOOP_START;
            _c   = c + 11;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 12; cy   = 0;
            LOOP_START;
            _c   = c + 12;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 13; cy   = 0;
            LOOP_START;
            _c   = c + 13;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 15:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 10; cy   = 0;
            LOOP_START;
            _c   = c + 10;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 11; cy   = 0;
            LOOP_START;
            _c   = c + 11;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 12; cy   = 0;
            LOOP_START;
            _c   = c + 12;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 13; cy   = 0;
            LOOP_START;
            _c   = c + 13;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 14; cy   = 0;
            LOOP_START;
            _c   = c + 14;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
      case 16:
            x = 0; cy   = 0;
            LOOP_START;
            _c   = c + 0;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 1; cy   = 0;
            LOOP_START;
            _c   = c + 1;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 2; cy   = 0;
            LOOP_START;
            _c   = c + 2;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 3; cy   = 0;
            LOOP_START;
            _c   = c + 3;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 4; cy   = 0;
            LOOP_START;
            _c   = c + 4;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 5; cy   = 0;
            LOOP_START;
            _c   = c + 5;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 6; cy   = 0;
            LOOP_START;
            _c   = c + 6;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 7; cy   = 0;
            LOOP_START;
            _c   = c + 7;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 8; cy   = 0;
            LOOP_START;
            _c   = c + 8;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 9; cy   = 0;
            LOOP_START;
            _c   = c + 9;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 10; cy   = 0;
            LOOP_START;
            _c   = c + 10;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 11; cy   = 0;
            LOOP_START;
            _c   = c + 11;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 12; cy   = 0;
            LOOP_START;
            _c   = c + 12;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 13; cy   = 0;
            LOOP_START;
            _c   = c + 13;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 14; cy   = 0;
            LOOP_START;
            _c   = c + 14;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
            x = 15; cy   = 0;
            LOOP_START;
            _c   = c + 15;
            tmpm = m->dp;
#ifdef INNERMUL8
            INNERMUL8; _c += 8; tmpm += 8;
            INNERMUL8; _c += 8; tmpm += 8;
#else
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
            INNERMUL; ++_c;
#endif
            LOOP_END;
            while (cy) {
               PROPCARRY;
               ++_c;
            }
         break;
  }
  /* now copy out */
  _c   = c + pa;
  tmpm = a->dp;
  for (x = 0; x < pa+1; x++) {
     *tmpm++ = *_c++;
  }

  for (; x < oldused; x++)   {
     *tmpm++ = 0;
  }

  MONT_FINI;

  a->used = pa+1;
  fp_clamp(a);

  /* if A >= m then A = A - m */
  if (fp_cmp_mag (a, m) != FP_LT) {
    s_fp_sub (a, m, a);
  }

#ifdef WOLFSSL_SMALL_STACK
  XFREE(c, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
  return FP_OKAY;
}

#endif
