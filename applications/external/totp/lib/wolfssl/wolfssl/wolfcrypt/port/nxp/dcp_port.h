/* dcp_port.h
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
#ifndef _DCP_PORT_H_
#define _DCP_PORT_H_

#include <wolfssl/wolfcrypt/settings.h>
#include "fsl_dcp.h"

#ifndef NO_SHA256
#include <wolfssl/wolfcrypt/sha256.h>
void DCPSha256Free(wc_Sha256 *sha256);
#endif

#ifndef NO_SHA
#include <wolfssl/wolfcrypt/sha.h>
void DCPShaFree(wc_Sha *sha);
#endif

int wc_dcp_init(void);

#ifndef NO_AES
#include <wolfssl/wolfcrypt/aes.h>
int  DCPAesInit(Aes* aes);
void DCPAesFree(Aes *aes);

int  DCPAesSetKey(Aes* aes, const byte* key, word32 len, const byte* iv,
                          int dir);
int  DCPAesCbcEncrypt(Aes* aes, byte* out, const byte* in, word32 sz);
int  DCPAesCbcDecrypt(Aes* aes, byte* out, const byte* in, word32 sz);
#endif

#ifdef HAVE_AES_ECB
int  DCPAesEcbEncrypt(Aes* aes, byte* out, const byte* in, word32 sz);
int  DCPAesEcbDecrypt(Aes* aes, byte* out, const byte* in, word32 sz);
#endif


#endif
