/*  @(#) pfinnrfp.h 98/02/26 1.4 */
/***************************************************************
** Compile FP routines.
** This file is included from "pf_inner.c"
**
** These routines could be left out of an execute only version.
**
** Author: Darren Gibbs, Phil Burk
** Copyright 1994 3DO, Phil Burk, Larry Polansky, David Rosenboom
**
** Permission to use, copy, modify, and/or distribute this
** software for any purpose with or without fee is hereby granted.
**
** THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL
** WARRANTIES WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED
** WARRANTIES OF MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL
** THE AUTHOR BE LIABLE FOR ANY SPECIAL, DIRECT, INDIRECT, OR
** CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING
** FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF
** CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
** OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
**
****************************************************************
**
***************************************************************/

#ifdef PF_SUPPORT_FP

#define FP_DHI1 (((PF_FLOAT)((cell_t)1<<(sizeof(cell_t)*8-2)))*4.0)

    case ID_FP_D_TO_F: /* ( dlo dhi -- ) ( F: -- r ) */
        PUSH_FP_TOS;
        Scratch = M_POP; /* dlo */
        DBUG(("dlo = 0x%8x , ", Scratch));
        DBUG(("dhi = 0x%8x\n", TOS));

        if( ((TOS ==  0) && (Scratch >= 0)) ||
            ((TOS == -1) && (Scratch < 0)))
        {
            /* <=  32 bit precision. */
            FP_TOS = ((PF_FLOAT) Scratch);  /* Convert dlo and push on FP stack. */
        }
        else /* > 32 bit precision. */
        {
            fpTemp = ((PF_FLOAT) TOS); /* dhi */
            fpTemp *= FP_DHI1;
            fpScratch = ( (PF_FLOAT) ((ucell_t)Scratch) );  /* Convert TOS and push on FP stack. */
            FP_TOS = fpTemp + fpScratch;
        }
        M_DROP;
        /* printf("d2f = %g\n", FP_TOS); */
        break;

    case ID_FP_FSTORE: /* ( addr -- ) ( F: r -- ) */
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
        if( IN_CODE_DIC(TOS) )
        {
            WRITE_FLOAT_DIC( (PF_FLOAT *) TOS, FP_TOS );
        }
        else
        {
            *((PF_FLOAT *) TOS) = FP_TOS;
        }
#else
        *((PF_FLOAT *) TOS) = FP_TOS;
#endif
        M_FP_DROP;      /* drop FP value */
        M_DROP;         /* drop addr */
        break;

    case ID_FP_FTIMES:  /* ( F: r1 r2 -- r1*r2 ) */
        FP_TOS = M_FP_POP * FP_TOS;
        break;

    case ID_FP_FPLUS:  /* ( F: r1 r2 -- r1+r2 ) */
        FP_TOS = M_FP_POP + FP_TOS;
        break;

    case ID_FP_FMINUS:  /* ( F: r1 r2 -- r1-r2 ) */
        FP_TOS = M_FP_POP - FP_TOS;
        break;

    case ID_FP_FSLASH:  /* ( F: r1 r2 -- r1/r2 ) */
        FP_TOS = M_FP_POP / FP_TOS;
        break;

    case ID_FP_F_ZERO_LESS_THAN: /* ( -- flag )  ( F: r --  ) */
        PUSH_TOS;
        TOS = (FP_TOS < 0.0) ? FTRUE : FFALSE ;
        M_FP_DROP;
        break;

    case ID_FP_F_ZERO_EQUALS: /* ( -- flag )  ( F: r --  ) */
        PUSH_TOS;
        TOS = (FP_TOS == 0.0) ? FTRUE : FFALSE ;
        M_FP_DROP;
        break;

    case ID_FP_F_LESS_THAN: /* ( -- flag )  ( F: r1 r2 -- ) */
        PUSH_TOS;
        TOS = (M_FP_POP < FP_TOS) ? FTRUE : FFALSE ;
        M_FP_DROP;
        break;

    case ID_FP_F_TO_D: /* ( -- dlo dhi) ( F: r -- ) */
        /* printf("f2d = %g\n", FP_TOS); */
        {
            ucell_t dlo;
            cell_t dhi;
            int ifNeg;
    /* Convert absolute value, then negate D if negative. */
            PUSH_TOS;   /* Save old TOS */
            fpTemp = FP_TOS;
            M_FP_DROP;
            ifNeg = (fpTemp < 0.0);
            if( ifNeg )
            {
                fpTemp = 0.0 - fpTemp;
            }
            fpScratch = fpTemp / FP_DHI1;
        /* printf("f2d - fpScratch = %g\n", fpScratch); */
            dhi = (cell_t) fpScratch;  /* dhi */
            fpScratch = ((PF_FLOAT) dhi) * FP_DHI1;
        /* printf("f2d - , dhi = 0x%x, fpScratch = %g\n", dhi, fpScratch); */

            fpTemp = fpTemp - fpScratch; /* Remainder */
            dlo = (ucell_t) fpTemp;
        /* printf("f2d - , dlo = 0x%x, fpTemp = %g\n", dlo, fpTemp); */
            if( ifNeg )
            {
                dlo = 0 - dlo;
                dhi = 0 - dhi - 1;
            }
    /* Push onto stack. */
            TOS = dlo;
            PUSH_TOS;
            TOS = dhi;
        }
        break;

    case ID_FP_FFETCH:  /* ( addr -- ) ( F: -- r ) */
        PUSH_FP_TOS;
#if (defined(PF_BIG_ENDIAN_DIC) || defined(PF_LITTLE_ENDIAN_DIC))
        if( IN_CODE_DIC(TOS) )
        {
            FP_TOS = READ_FLOAT_DIC( (PF_FLOAT *) TOS );
        }
        else
        {
            FP_TOS = *((PF_FLOAT *) TOS);
        }
#else
        FP_TOS = *((PF_FLOAT *) TOS);
#endif
        M_DROP;
        break;

    case ID_FP_FDEPTH: /* ( -- n ) ( F: -- ) */
        PUSH_TOS;
    /* Add 1 to account for FP_TOS in cached in register. */
        TOS = (( M_FP_SPZERO - FP_STKPTR) + 1);
        break;

    case ID_FP_FDROP: /* ( -- ) ( F: r -- ) */
        M_FP_DROP;
        break;

    case ID_FP_FDUP: /* ( -- ) ( F: r -- r r ) */
        PUSH_FP_TOS;
        break;

    case ID_FP_FLOAT_PLUS: /* ( addr1 -- addr2 ) ( F: -- ) */
        TOS = TOS + sizeof(PF_FLOAT);
        break;

    case ID_FP_FLOATS: /* ( n -- size ) ( F: -- ) */
        TOS = TOS * sizeof(PF_FLOAT);
        break;

    case ID_FP_FLOOR: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_floor( FP_TOS );
        break;

    case ID_FP_FMAX: /* ( -- ) ( F: r1 r2 -- r3 ) */
        fpScratch = M_FP_POP;
        FP_TOS = ( FP_TOS > fpScratch ) ? FP_TOS : fpScratch ;
        break;

    case ID_FP_FMIN: /* ( -- ) ( F: r1 r2 -- r3 ) */
        fpScratch = M_FP_POP;
        FP_TOS = ( FP_TOS < fpScratch ) ? FP_TOS : fpScratch ;
        break;

    case ID_FP_FNEGATE:
        FP_TOS = -FP_TOS;
        break;

    case ID_FP_FOVER: /* ( -- ) ( F: r1 r2 -- r1 r2 r1 ) */
        PUSH_FP_TOS;
        FP_TOS = M_FP_STACK(1);
        break;

    case ID_FP_FROT: /* ( -- ) ( F: r1 r2 r3 -- r2 r3 r1 ) */
        fpScratch = M_FP_POP;       /* r2 */
        fpTemp = M_FP_POP;          /* r1 */
        M_FP_PUSH( fpScratch );     /* r2 */
        PUSH_FP_TOS;                /* r3 */
        FP_TOS = fpTemp;            /* r1 */
        break;

    case ID_FP_FROUND:
        /* This was broken before and used to push its result to the
         * integer data stack! Now it conforms to the ANSI standard.
         * https://github.com/philburk/pforth/issues/69
         */
        FP_TOS = (PF_FLOAT)fp_round(FP_TOS);
        break;

    case ID_FP_FSWAP: /* ( -- ) ( F: r1 r2 -- r2 r1 ) */
        fpScratch = FP_TOS;
        FP_TOS = *FP_STKPTR;
        *FP_STKPTR = fpScratch;
        break;

    case ID_FP_FSTAR_STAR: /* ( -- ) ( F: r1 r2 -- r1^r2 ) */
        fpScratch = M_FP_POP;
        FP_TOS = (PF_FLOAT) fp_pow(fpScratch, FP_TOS);
        break;

    case ID_FP_FABS: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_fabs( FP_TOS );
        break;

    case ID_FP_FACOS: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_acos( FP_TOS );
        break;

    case ID_FP_FACOSH: /* ( -- ) ( F: r1 -- r2 ) */
        /* fp_acosh(x) = fp_log(y + sqrt(y^2 - 1) */
        FP_TOS = (PF_FLOAT) fp_log(FP_TOS + (fp_sqrt((FP_TOS * FP_TOS) - 1)));
        break;

    case ID_FP_FALOG: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_pow(10.0,FP_TOS);
        break;

    case ID_FP_FASIN: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_asin( FP_TOS );
        break;

    case ID_FP_FASINH: /* ( -- ) ( F: r1 -- r2 ) */
        /* asinh(x) = fp_log(y + fp_sqrt(y^2 + 1) */
        FP_TOS = (PF_FLOAT) fp_log(FP_TOS + (fp_sqrt((FP_TOS * FP_TOS) + 1)));
        break;

    case ID_FP_FATAN: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_atan( FP_TOS );
        break;

    case ID_FP_FATAN2: /* ( -- ) ( F: r1 r2 -- atan(r1/r2) ) */
        fpTemp = M_FP_POP;
        FP_TOS = (PF_FLOAT) fp_atan2( fpTemp, FP_TOS );
        break;

    case ID_FP_FATANH: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) (0.5 * fp_log((1 + FP_TOS) / (1 - FP_TOS)));
        break;

    case ID_FP_FCOS: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_cos( FP_TOS );
        break;

    case ID_FP_FCOSH: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_cosh( FP_TOS );
        break;

#ifndef PF_NO_SHELL
    case ID_FP_FLITERAL:
        ffFPLiteral( FP_TOS );
        M_FP_DROP;
        endcase;
#endif  /* !PF_NO_SHELL */

    case ID_FP_FLITERAL_P:
        PUSH_FP_TOS;
#if 0
/* Some wimpy compilers can't handle this! */
        FP_TOS = *(((PF_FLOAT *)InsPtr)++);
#else
        {
            PF_FLOAT *fptr;
            fptr = (PF_FLOAT *)InsPtr;
            FP_TOS = READ_FLOAT_DIC( fptr++ );
            InsPtr = (cell_t *) fptr;
        }
#endif
        endcase;

    case ID_FP_FLN: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_log(FP_TOS);
        break;

    case ID_FP_FLNP1: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) (fp_log(FP_TOS) + 1.0);
        break;

    case ID_FP_FLOG: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_log10( FP_TOS );
        break;

    case ID_FP_FSIN: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_sin( FP_TOS );
        break;

    case ID_FP_FSINCOS: /* ( -- ) ( F: r1 -- r2 r3 ) */
        M_FP_PUSH((PF_FLOAT) fp_sin(FP_TOS));
        FP_TOS = (PF_FLOAT) fp_cos(FP_TOS);
        break;

    case ID_FP_FSINH: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_sinh( FP_TOS );
        break;

    case ID_FP_FSQRT: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_sqrt( FP_TOS );
        break;

    case ID_FP_FTAN: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_tan( FP_TOS );
        break;

    case ID_FP_FTANH: /* ( -- ) ( F: r1 -- r2 ) */
        FP_TOS = (PF_FLOAT) fp_tanh( FP_TOS );
        break;

    case ID_FP_FPICK: /* ( n -- ) ( F: -- f[n] ) */
        PUSH_FP_TOS;  /* push cached floats into RAM */
        FP_TOS = FP_STKPTR[TOS];  /* 0 FPICK gets top of FP stack */
        M_DROP;
        break;


#endif
