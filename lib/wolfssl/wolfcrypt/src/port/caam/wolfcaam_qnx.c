/* wolfcaam_qnx.c
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

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_QNX_CAAM) && (defined(__QNX__) || defined(__QNXNTO__))

#include <wolfssl/wolfcrypt/logging.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/port/caam/wolfcaam.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <devctl.h>

#include <errno.h>

/* for devctl use */
int caamFd = -1;
static wolfSSL_Mutex caamMutex;

/* return 0 on success */
int wc_CAAMInitInterface()
{
    if (wc_InitMutex(&caamMutex) != 0) {
        WOLFSSL_MSG("Could not init mutex");
        return -1;
    }

    caamFd = open("/dev/wolfCrypt", O_RDWR);
    if (caamFd < 0) {
        WOLFSSL_MSG("Could not open /dev/wolfCrypt");
        return -1;
    }

    return 0;
}


void wc_CAAMFreeInterface()
{
    wc_FreeMutex(&caamMutex);
    if (caamFd >= 0)
        close(caamFd);
}

#define WC_TRNG_CMD __DIOTF(_DCMD_ALL, CAAM_ENTROPY, iov_t)
#define WC_CAAM_GET_PART __DIOTF(_DCMD_ALL, CAAM_GET_PART, iov_t)
#define WC_CAAM_FREE_PART __DIOT(_DCMD_ALL, CAAM_FREE_PART, iov_t)
#define WC_CAAM_FIND_PART __DIOTF(_DCMD_ALL, CAAM_FIND_PART, iov_t)
#define WC_CAAM_READ_PART __DIOTF(_DCMD_ALL, CAAM_READ_PART, iov_t)
#define WC_CAAM_WRITE_PART __DIOT(_DCMD_ALL, CAAM_WRITE_PART, iov_t)

#define WC_CAAM_ECDSA_KEYPAIR __DIOTF(_DCMD_ALL, CAAM_ECDSA_KEYPAIR, iov_t)
#define WC_CAAM_ECDSA_VERIFY __DIOT(_DCMD_ALL, CAAM_ECDSA_VERIFY, iov_t)
#define WC_CAAM_ECDSA_SIGN __DIOTF(_DCMD_ALL, CAAM_ECDSA_SIGN, iov_t)
#define WC_CAAM_ECDSA_ECDH __DIOTF(_DCMD_ALL, CAAM_ECDSA_ECDH, iov_t)

#define WC_CAAM_BLOB_ENCAP __DIOTF(_DCMD_ALL, CAAM_BLOB_ENCAP, iov_t)
#define WC_CAAM_BLOB_DECAP __DIOTF(_DCMD_ALL, CAAM_BLOB_DECAP, iov_t)

#define WC_CAAM_CMAC __DIOTF(_DCMD_ALL, CAAM_CMAC, iov_t)
#define WC_CAAM_AESECB __DIOTF(_DCMD_ALL, CAAM_AESECB, iov_t)
#define WC_CAAM_AESCTR __DIOTF(_DCMD_ALL, CAAM_AESCTR, iov_t)
#define WC_CAAM_AESCBC __DIOTF(_DCMD_ALL, CAAM_AESCBC, iov_t)
#define WC_CAAM_AESCCM __DIOTF(_DCMD_ALL, CAAM_AESCCM, iov_t)
#define WC_CAAM_AESGCM __DIOTF(_DCMD_ALL, CAAM_AESGCM, iov_t)

#define WC_CAAM_FIFO_S __DIOTF(_DCMD_ALL, CAAM_FIFO_S, iov_t)

#define MAX_IN_IOVS 6
#define MAX_OUT_IOVS 3

/* Do a synchronous operations and block till done
 * returns 0 on success */
int SynchronousSendRequest(int type, unsigned int args[4], CAAM_BUFFER *buf,
        int sz)
{
    int ret, inIdx = 0, outIdx = 0;
    int cmd = 0;
    iov_t in[MAX_IN_IOVS], out[MAX_OUT_IOVS];
    CAAM_ADDRESS privkey;

    if (args != NULL) {
        SETIOV(&in[inIdx], args, sizeof(unsigned int) * 4);
        inIdx = inIdx + 1;
    }
    else {
        unsigned int localArgs[4] = {0};
        SETIOV(&in[inIdx], localArgs, sizeof(unsigned int) * 4);
        inIdx = inIdx + 1;
    }

    switch (type) {
    case CAAM_ENTROPY:
        SETIOV(&out[outIdx], (buf->TheAddress), (buf->Length));
        outIdx = outIdx + 1;
        cmd = WC_TRNG_CMD;
        break;

    case CAAM_GET_PART:
        SETIOV(&out[outIdx], (buf->TheAddress), (buf->Length));
        outIdx = outIdx + 1;
        cmd = WC_CAAM_GET_PART;
        break;

    case CAAM_FREE_PART:
        cmd = WC_CAAM_FREE_PART;
        break;

    case CAAM_FIND_PART:
        SETIOV(&out[outIdx], (buf->TheAddress), (buf->Length));
        outIdx = outIdx + 1;
        cmd = WC_CAAM_FIND_PART;
        break;

    case CAAM_READ_PART:
        SETIOV(&out[outIdx], (buf->TheAddress), (buf->Length));
        outIdx = outIdx + 1;
        cmd = WC_CAAM_READ_PART;
        break;

    case CAAM_WRITE_PART:
        SETIOV(&in[inIdx], (buf->TheAddress), (buf->Length));
        inIdx = inIdx + 1;
        cmd = WC_CAAM_WRITE_PART;
        break;

    case CAAM_ECDSA_KEYPAIR:
        /* set output to store directly to CAAM_BUFFER's */
        SETIOV(&out[outIdx], buf[0].TheAddress, buf[0].Length);
        outIdx = outIdx + 1;

        SETIOV(&out[outIdx], buf[1].TheAddress, buf[1].Length);
        outIdx = outIdx + 1;

        /* get args for updated partition number used */
        SETIOV(&out[outIdx], args, sizeof(unsigned int) * 4);
        outIdx = outIdx + 1;

        cmd = WC_CAAM_ECDSA_KEYPAIR;
        break;

    case CAAM_ECDSA_VERIFY:
        /* public key */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* msg */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* r */
        SETIOV(&in[inIdx], buf[2].TheAddress, buf[2].Length);
        inIdx = inIdx + 1;

        /* s */
        SETIOV(&in[inIdx], buf[3].TheAddress, buf[3].Length);
        inIdx = inIdx + 1;

        cmd = WC_CAAM_ECDSA_VERIFY;
        break;

    case CAAM_ECDSA_SIGN:
        /* private key */
        if (args[0] == 1) {
            privkey = buf[0].TheAddress;
            SETIOV(&in[inIdx], &privkey, sizeof(unsigned int));
            inIdx = inIdx + 1;
        }
        else {
            SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
            inIdx = inIdx + 1;
        }

        /* msg */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* r out */
        SETIOV(&out[outIdx], buf[2].TheAddress, buf[2].Length);
        outIdx = outIdx + 1;

        /* s out */
        SETIOV(&out[outIdx], buf[3].TheAddress, buf[3].Length);
        outIdx = outIdx + 1;

        cmd = WC_CAAM_ECDSA_SIGN;
        break;

    case CAAM_ECDSA_ECDH:
        /* when using memory in secure partition just send the address */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* private key */
        if (args[0] == CAAM_BLACK_KEY_SM) {
            privkey = buf[1].TheAddress;
            SETIOV(&in[inIdx], &privkey, sizeof(unsigned int));
            inIdx = inIdx + 1;
        }
        else {
            SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
            inIdx = inIdx + 1;
        }

        /* shared secret */
        SETIOV(&out[outIdx], buf[2].TheAddress, buf[2].Length);
        outIdx = outIdx + 1;

        cmd = WC_CAAM_ECDSA_ECDH;
        break;

    case CAAM_BLOB_ENCAP:
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        if (args[0] == 1) {
            SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length + WC_CAAM_MAC_SZ);
            inIdx = inIdx + 1;
        }
        else {
            SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
            inIdx = inIdx + 1;
        }

        SETIOV(&out[outIdx], buf[2].TheAddress, buf[2].Length);
        outIdx = outIdx + 1;
        cmd = WC_CAAM_BLOB_ENCAP;
        break;

    case CAAM_BLOB_DECAP:
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        if (args[0] == 1) {
            SETIOV(&out[outIdx], buf[2].TheAddress,
                    buf[2].Length + WC_CAAM_MAC_SZ);
            outIdx = outIdx + 1;
        }
        else {
            SETIOV(&out[outIdx], buf[2].TheAddress, buf[2].Length);
            outIdx = outIdx + 1;
        }

        cmd = WC_CAAM_BLOB_DECAP;
        break;

    case CAAM_AESECB:
        /* key */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* in */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* out */
        SETIOV(&out[outIdx], buf[2].TheAddress, buf[2].Length);
        outIdx = outIdx + 1;
        cmd = WC_CAAM_AESECB;
        break;

    case CAAM_AESCBC:
        /* key */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* in */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* iv */
        SETIOV(&in[inIdx], buf[2].TheAddress, buf[2].Length);
        inIdx = inIdx + 1;

        /* out */
        SETIOV(&out[outIdx], buf[3].TheAddress, buf[3].Length);
        outIdx = outIdx + 1;

        /* out updated IV */
        SETIOV(&out[outIdx], buf[4].TheAddress, buf[4].Length);
        outIdx = outIdx + 1;

        cmd = WC_CAAM_AESCBC;
        break;

    case CAAM_AESCTR:
        /* key */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* iv */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* in */
        SETIOV(&in[inIdx], buf[2].TheAddress, buf[2].Length);
        inIdx = inIdx + 1;

        /* out */
        SETIOV(&out[outIdx], buf[3].TheAddress, buf[3].Length);
        outIdx = outIdx + 1;

        /* out updated IV */
        SETIOV(&out[outIdx], buf[4].TheAddress, buf[4].Length);
        outIdx = outIdx + 1;

        cmd = WC_CAAM_AESCTR;
        break;

    case CAAM_AESCCM:
        /* key */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* iv */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* in */
        SETIOV(&in[inIdx], buf[2].TheAddress, buf[2].Length);
        inIdx = inIdx + 1;

        /* out */
        SETIOV(&out[outIdx], buf[3].TheAddress, buf[3].Length);
        outIdx = outIdx + 1;

        /* set TAG as input or output */
        if ((args[0] & 0xFFFF) == CAAM_ENC) {
            SETIOV(&out[outIdx], buf[4].TheAddress, buf[4].Length);
            outIdx = outIdx + 1;
        }
        else {
            SETIOV(&in[inIdx], buf[4].TheAddress, buf[4].Length);
            inIdx = inIdx + 1;
        }

        /* aad */
        SETIOV(&in[inIdx], buf[5].TheAddress, buf[5].Length);
        inIdx = inIdx + 1;

        cmd = WC_CAAM_AESCCM;
        break;

    case CAAM_AESGCM:
        /* key */
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        /* iv */
        SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
        inIdx = inIdx + 1;

        /* in */
        SETIOV(&in[inIdx], buf[2].TheAddress, buf[2].Length);
        inIdx = inIdx + 1;

        /* out */
        SETIOV(&out[outIdx], buf[3].TheAddress, buf[3].Length);
        outIdx = outIdx + 1;

        /* set TAG as input or output */
        if ((args[0] & 0xFFFF) == CAAM_ENC) {
            SETIOV(&out[outIdx], buf[4].TheAddress, buf[4].Length);
            outIdx = outIdx + 1;
        }
        else {
            SETIOV(&in[inIdx], buf[4].TheAddress, buf[4].Length);
            inIdx = inIdx + 1;
        }

        /* aad */
        SETIOV(&in[inIdx], buf[5].TheAddress, buf[5].Length);
        inIdx = inIdx + 1;

        cmd = WC_CAAM_AESGCM;
        break;

    case CAAM_CMAC:
        {
            int i;

            if (args[2] == 1) {
                SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length + 16);
                inIdx = inIdx + 1;
            }
            else {
                SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
                inIdx = inIdx + 1;
            }

            SETIOV(&in[inIdx], buf[1].TheAddress, buf[1].Length);
            inIdx = inIdx + 1;

            /* get input buffers */
            args[3] = 0;
            for (i = 2; i < sz && i < MAX_IN_IOVS; i++) {
                SETIOV(&in[inIdx], buf[i].TheAddress, buf[i].Length);
                inIdx = inIdx + 1;
                args[3] += buf[i].Length;
            }

            SETIOV(&out[outIdx], buf[1].TheAddress, buf[1].Length);
            outIdx = outIdx + 1;
        }
        cmd = WC_CAAM_CMAC;
        break;

    case CAAM_FIFO_S:
        SETIOV(&in[inIdx], buf[0].TheAddress, buf[0].Length);
        inIdx = inIdx + 1;

        SETIOV(&out[outIdx], buf[1].TheAddress, buf[1].Length + WC_CAAM_MAC_SZ);
        outIdx = outIdx + 1;
        cmd = WC_CAAM_FIFO_S;
        break;

    default:
        WOLFSSL_MSG("Unknown/unsupported type");
        return -1;
    }

    ret = wc_LockMutex(&caamMutex);
    if (ret == 0) {
        ret = devctlv(caamFd, cmd, inIdx, outIdx, in, out, NULL);
        wc_UnLockMutex(&caamMutex);
    }

    if (ret != 0) {
        if (ret == EFAULT) {
            WOLFSSL_MSG("bad address on one of the in/out buffers");
            return -1;
        }

        if (ret == EAGAIN && type == CAAM_ENTROPY) {
            return CAAM_WAITING;
        }
        return -1;
    }

    return Success;
}

#endif /* WOLFSSL_QNX_CAAM && (__QNX__ || __QNXNTO__) */
