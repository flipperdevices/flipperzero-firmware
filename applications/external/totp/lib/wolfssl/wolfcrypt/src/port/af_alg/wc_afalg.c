/* wc_afalg.c
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
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/logging.h>

#if defined(WOLFSSL_AFALG) || defined(WOLFSSL_AFALG_XILINX)

#include <wolfssl/wolfcrypt/port/af_alg/wc_afalg.h>
#include <linux/if_alg.h>


/* Sets the type of socket address to use */
void wc_Afalg_SockAddr(struct sockaddr_alg* in, const char* type, const char* name)
{
    int typeSz = (int)XSTRLEN(type) + 1; /* +1 for null terminator */
    int nameSz = (int)XSTRLEN(name) + 1; /* +1 for null terminator */

    if (typeSz > (int)sizeof(in->salg_type) ||
		    nameSz > (int)sizeof(in->salg_name)) {
        WOLFSSL_MSG("type or name was too large");
        return;
    }

    in->salg_family = AF_ALG;
    XSTRNCPY((char*)in->salg_type, type, typeSz);
    in->salg_type[typeSz - 1] = '\0';
    XSTRNCPY((char*)in->salg_name, name, nameSz);
    in->salg_name[nameSz - 1] = '\0';
}


/* returns the socket accepting on with success
 * negative values are returned in fail cases */
int wc_Afalg_Accept(struct sockaddr_alg* in, int inSz, int sock)
{
    if (bind(sock, (const struct sockaddr*)in, inSz) < 0) {
        WOLFSSL_MSG("Failed to bind with AF_ALG");
        return WC_AFALG_SOCK_E;
    }

    return accept(sock, NULL, 0);
}


/* creates a new AF_ALG socket and returns it
 * negative values are returned in fail cases */
int wc_Afalg_Socket(void)
{
    int sock;

    if ((sock = socket(AF_ALG, SOCK_SEQPACKET, 0)) < 0) {
        WOLFSSL_MSG("Failed to get AF_ALG socket");
        return WC_AFALG_SOCK_E;
    }

    return sock;
}


/* binds and creates the read fd */
int wc_Afalg_CreateRead(int sock, const char* type, const char* name)
{
    struct sockaddr_alg sa = {0};
    wc_Afalg_SockAddr(&sa, type, name);
    return wc_Afalg_Accept(&sa, sizeof(sa), sock);
}


/* sets the IV in CMSG structure, returns 0 on success */
int wc_Afalg_SetIv(struct cmsghdr* cmsg, byte* iv, word32 ivSz)
{
    struct af_alg_iv* afIv;

    if (cmsg == NULL || iv == NULL) {
        WOLFSSL_MSG("Null cmsg or iv passed in");
        return BAD_FUNC_ARG;
    }

    cmsg->cmsg_level = SOL_ALG;
    cmsg->cmsg_type  = ALG_SET_IV;
    cmsg->cmsg_len   = CMSG_LEN(sizeof(struct af_alg_iv) + ivSz);
    afIv = (void*)CMSG_DATA(cmsg);
    afIv->ivlen = ivSz;
    XMEMCPY(afIv->iv, iv, ivSz);

    return 0;
}


/* sets the AAD size in CMSG structure, returns 0 on success */
int wc_Afalg_SetAad(struct cmsghdr* cmsg, word32 sz)
{
    if (cmsg == NULL) {
        WOLFSSL_MSG("Null cmsg passed in");
        return BAD_FUNC_ARG;
    }

    cmsg->cmsg_level = SOL_ALG;
    cmsg->cmsg_type  = ALG_SET_AEAD_ASSOCLEN;
    cmsg->cmsg_len   = CMSG_LEN(sizeof(word32));
    *((word32*)CMSG_DATA(cmsg)) = sz;

    return 0;
}


/* sets the operation type in CMSG structure, returns 0 on success
 *
 * dir 0 is encryption 1 is decryption
 */
int wc_Afalg_SetOp(struct cmsghdr* cmsg, int dir)
{
    if (cmsg == NULL) {
        return BAD_FUNC_ARG;
    }

    cmsg->cmsg_level = SOL_ALG;
    cmsg->cmsg_type  = ALG_SET_OP;
    cmsg->cmsg_len   = CMSG_LEN(4);
    *((word32*)CMSG_DATA(cmsg)) = (dir == 1)? ALG_OP_DECRYPT : ALG_OP_ENCRYPT;

    return 0;
}

#endif /* !NO_AES && WOLFSSL_AFALG */

