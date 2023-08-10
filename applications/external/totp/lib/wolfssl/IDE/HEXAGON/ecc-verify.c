/* ecc-verify.c
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

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/sha256.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/ecc.h>
#include <wolfssl/wolfcrypt/asn_public.h>

#define USE_CERT_BUFFERS_256
#include <wolfssl/certs_test.h>

#define MAX_BLOCK_SIZE 1024

#ifdef WOLFSSL_DSP
static char *sp_URI_value = wolfSSL_URI "&_dom=cdsp";

int hash_firmware_verify(const byte* hash, word32 hashLen, const byte* sigBuf, word32 sigLen)
{
    int ret;
    ecc_key eccKey;
    word32 idx;
    int verify;
    remote_handle64 handle = -1;

    idx = 0;
    ret = wc_EccPrivateKeyDecode(ecc_clikey_der_256, &idx, &eccKey, sizeof_ecc_clikey_der_256);
    if (ret < 0)
        goto exit;

    int retVal = wolfSSL_open(sp_URI_value, &handle);
    if (retVal != 0) {
        printf("unable to open CDSP? retVal = %d\n", retVal);
        ret = -1;
        goto exit;
    }
    wc_ecc_set_handle(&eccKey, handle);

    ret = wc_ecc_verify_hash((byte*)sigBuf, sigLen, hash, hashLen, &verify, &eccKey);
    printf("verify = %d\n", verify);
    if (ret < 0)
        goto exit;

    wolfSSL_close(handle);
exit:

    return ret;
}

int main(void)
{
    int ret;
    const byte hash[] = {
0XFB, 0XBA, 0XB2, 0X89, 0XF7, 0XF9, 0X4B, 0X25, 0X73, 0X6C, 0X58, 0XBE, 0X46, 0XA9, 0X94, 0XC4, 0X41, 0XFD, 0X02, 0X55, 0X2C, 0XC6, 0X02, 0X23, 0X52, 0XE3, 0XD8, 0X6D, 0X2F, 0XAB, 0X7C, 0X83
};
    const byte sigBuf[] = {
0X30, 0X44, 0X02, 0X20, 0X05, 0X38, 0XBC, 0X16, 0XC7, 0X67, 0X18, 0XEC, 0XE6, 0X1E, 0X43, 0X7B, 0X29, 0X8F, 0X85, 0X01, 0X33, 0XA8, 0X9B, 0XDD, 0X91, 0X32, 0X1F, 0XEC, 0XF7, 0X91, 0X18, 0X72, 0X9C, 0XE2, 0X6F, 0X31, 0X02, 0X20, 0X3E, 0X31, 0XD6, 0X40, 0XF7, 0X38, 0X3C, 0X1B, 0X6D, 0XAD, 0XE3, 0X93, 0X20, 0XE8, 0XB1, 0XBD, 0X3C, 0X59, 0XF2, 0XD2, 0X7C, 0X46, 0X1B, 0XE5, 0XE1, 0XE3, 0XAB, 0X5E, 0X76, 0X73, 0X6F, 0XFB
    };
    word32 sigLen = (word32)sizeof(sigBuf);

    wolfCrypt_Init();
    ret = hash_firmware_verify(hash, sizeof(hash), sigBuf, sigLen);
    printf("hash_firmware_verify: %d\n", ret);
    wolfCrypt_Cleanup();
    return 0;
}
#else
int main()
{
	printf("WOLFSSL_DSP expected to be defined when building\n");
	return 0;
}
#endif /* WOLFSSL_DSP */
