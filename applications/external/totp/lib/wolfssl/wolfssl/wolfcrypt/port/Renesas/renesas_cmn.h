/* renesas_cmn.h
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
#ifndef __RENESAS_CMN_H__
#define __RENESAS_CMN_H__

#include <wolfssl/ssl.h>
#include <wolfssl/internal.h>

/* Common Callbacks */
WOLFSSL_LOCAL int Renesas_cmn_RsaSignCb(WOLFSSL* ssl,
                                const unsigned char* in, unsigned int inSz,
                                unsigned char* out, word32* outSz,
                                const unsigned char* keyDer, unsigned int keySz,
                                void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_EccSignCb(WOLFSSL* ssl,
                                const unsigned char* in, unsigned int inSz,
                                unsigned char* out, word32* outSz,
                                const unsigned char* keyDer, unsigned int keySz,
                                void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_genMasterSecret(WOLFSSL* ssl, void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_generatePremasterSecret(WOLFSSL* ssl, 
                                byte *premaster, word32 preSz, void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_RsaEnc(WOLFSSL* ssl, const unsigned char* in, 
       unsigned int inSz, unsigned char* out, word32* outSz,
       const unsigned char* keyDer, unsigned int keySz, void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_VerifyHmac(WOLFSSL *ssl, const byte* message, 
                    word32 messageSz, word32 macSz, word32 content, void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_EccVerify(WOLFSSL* ssl, const unsigned char* sig,
		unsigned int sigSz, const unsigned char* hash, unsigned int hashSz,
        const unsigned char* key, unsigned int keySz, int* result, void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_RsaVerify(WOLFSSL* ssl,
                                unsigned char* sig, unsigned int sigSz,
                                unsigned char** out, const unsigned char* key,
                                unsigned int keySz, void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_RsaSignCheckCb(WOLFSSL* ssl,
                                unsigned char* sig, unsigned int sigSz,
                                unsigned char** out,
                                const unsigned char* keyDer, unsigned int keySz,
                                void* ctx);
                                
WOLFSSL_LOCAL int Renesas_cmn_TLS_hmac(WOLFSSL* ssl, byte* digest, const byte* in,
             word32 sz, int padSz, int content, int verify, int epochOrder);
WOLFSSL_LOCAL int Renesas_cmn_usable(const WOLFSSL *ssl, byte seskey_gennerated);
WOLFSSL_LOCAL int Renesas_cmn_SigPkCbRsaVerify(unsigned char* sig, unsigned int sigSz,
       unsigned char** out, const unsigned char* keyDer, unsigned int keySz,
       void* ctx);
WOLFSSL_LOCAL int Renesas_cmn_SigPkCbEccVerify(const unsigned char* sig, unsigned int sigSz,
       const unsigned char* hash, unsigned int hashSz,
       const unsigned char* keyDer, unsigned int keySz,
       int* result, void* ctx);

/* Common Methods */
int wc_CryptoCb_CryptInitRenesasCmn(WOLFSSL* ssl, void* ctx);
void wc_CryptoCb_CleanupRenesasCmn(int* id);
int wc_Renesas_cmn_RootCertVerify(const byte* cert, word32 cert_len, 
        word32 key_n_start, word32 key_n_len, word32 key_e_start, 
        word32 key_e_len, word32 cm_row);
WOLFSSL_LOCAL int Renesas_cmn_Cleanup(WOLFSSL* ssl);
WOLFSSL_LOCAL byte Renesas_cmn_checkCA(word32 cmIdx);
#endif /* __RENESAS_CMN_H__ */
