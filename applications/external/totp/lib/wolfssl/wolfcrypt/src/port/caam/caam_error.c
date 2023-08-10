/* caam_error.c
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

#if (defined(__INTEGRITY) || defined(INTEGRITY)) || \
    (defined(__QNX__) || defined(__QNXNTO__))

#include <wolfssl/wolfcrypt/port/caam/caam_driver.h>
#include <wolfssl/wolfcrypt/port/caam/caam_error.h>

/* return a negative value if CAAM reset needed */
int caamParseCCBError(unsigned int error)
{
    int ret = 0;
    switch((error >> 4) & 0xF) {
        case 0:
            WOLFSSL_MSG("\tCHAID: CCB");
            break;

        case 1:
            WOLFSSL_MSG("\tCHAID: AESA");
            switch (error & 0xF) {
                case 0xC:
                    WOLFSSL_MSG("\tAAD size error");
            }
            break;

        case 2:
            WOLFSSL_MSG("\tCHAID: DESA");
            break;

        case 3:
            WOLFSSL_MSG("\tCHAID: AFHA (ARC4)");
            break;

        case 4:
            WOLFSSL_MSG("\tCHAID: MDHA hash");
            break;

        case 5:
            WOLFSSL_MSG("\tCHAID: RNG - ERRID:");
            ret = -1; /* treat RNG errors as fatal */
            switch(error & 0xF) {
                case 3:
                    WOLFSSL_MSG("\tRNG instantiate error");
                    break;

                case 4:
                    WOLFSSL_MSG("\tRNG not instantiated error");
                    break;

                case 5:
                    WOLFSSL_MSG("\tRNG test instantiate error");
                    break;

                case 6:
                    WOLFSSL_MSG("\tRNG prediction resistance error");
                    break;

                default:
                    WOLFSSL_MSG("\tUnknown");
            }
            break;

        case 8:
            WOLFSSL_MSG("\tCHAID: PKHA");
            break;

        default:
            WOLFSSL_MSG("\tCHAID: Unknown CCB error");
    }
    return ret;
}


/* return a negative value for an error that requires CAAM reset */
int caamParseDECOError(unsigned int error)
{
    int ret = 0;
    switch (error & 0xFF) {
        case 0x04:
            WOLFSSL_MSG("\tInvalid Descriptor Command");
            break;

        case 0x06:
            WOLFSSL_MSG("\tInvalid KEY Command");
            break;

        case 0x07:
            WOLFSSL_MSG("\tInvalid Load Command");
            break;

        case 0x08:
            WOLFSSL_MSG("\tInvalid Store Command");
            break;

        case 0x09:
            WOLFSSL_MSG("\tInvalid Operation Command");
            break;

        case 0x82:
            WOLFSSL_MSG("\tInvalid PRB setting");
            break;

        case 0x86:
            WOLFSSL_MSG("\tVerify ECC/RSA signature fail");
            break;

        default:
            WOLFSSL_MSG("\tUnknown error");
    }
    return ret;
}


/* return a negative value if CAAM should be reset after error */
int caamParseError(unsigned int error)
{
    int ret = 0;

    /* print out of index of error
    unsigned int idx;
    idx = (error >> 8) & 0xFF;
    printf("idx of error = %d\n", idx);
    */

    if ((error & 0x40000000) > 0) {
        WOLFSSL_MSG("[DECO Error]");
        ret = caamParseDECOError(error);
    }

    if ((error & 0x20000000) > 0) {
        WOLFSSL_MSG("[CCB Error]");
        ret = caamParseCCBError(error);
    }
    return ret;
}


/* parses a Job Ring Interrupt Status report
 * returns 0 if there is no error */
unsigned int caamParseJRError(unsigned int error)
{
    unsigned int err = (error >> 8) & 0x1F;

    if (error & 0x10) {
        WOLFSSL_MSG("Job Ring Interrupt ENTER_FAIL");
    }

    if (error & 0x20) {
        WOLFSSL_MSG("Job Ring Interrupt EXIT_FAIL");
    }

    switch (err) {
        case 0x00:
            /* no error */
            break;

        case 0x01:
            WOLFSSL_MSG("Error writing status to output ring");
            break;

        case 0x03:
            WOLFSSL_MSG("Bad input ring address");
            break;

        case 0x04:
            WOLFSSL_MSG("Bad output ring address");
            break;

        case 0x05:
            WOLFSSL_MSG("Fatal, invalid write to input ring");
            break;

        case 0x06:
            WOLFSSL_MSG("Invalid write to output ring");
            break;

        case 0x07:
            WOLFSSL_MSG("Job ring released before halted");
            break;

        case 0x08:
            WOLFSSL_MSG("Removed too many jobs");
            break;

        case 0x09:
            WOLFSSL_MSG("Added too many jobs");
            break;

        default:
            WOLFSSL_MSG("Unknown error");
            break;
    }
    return err;
}

#ifdef WOLFSSL_CAAM_PRINT
void DEBUG_PRINT_ARRAY(void* a, int aSz, char* str)
{
    int i;
    unsigned char* pt = (unsigned char*)a;

    printf("%s [%d] : ", str, aSz);
    for (i = 0; i < aSz; i++)
        printf("%02X", pt[i]);
    printf("\n");
}
#endif

#endif
