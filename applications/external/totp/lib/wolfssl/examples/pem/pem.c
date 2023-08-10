/* pem.c
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

#ifndef WOLFSSL_USER_SETTINGS
    #include <wolfssl/options.h>
#endif
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/asn_public.h>
#include <wolfssl/wolfcrypt/coding.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/random.h>
#include <wolfssl/wolfcrypt/wc_encrypt.h>
#ifdef DEBUG_WOLFSSL
    #include <wolfssl/wolfcrypt/logging.h>
#endif
#include <stdio.h>

#if defined(WOLFSSL_PEM_TO_DER) && !defined(NO_FILESYSTEM)

/* Increment allocated data by this much. */
#define DATA_INC_LEN        256
/* Maximum block size of a cipher. */
#define BLOCK_SIZE_MAX      16
/* Maximum PEM type string length. */
#define PEM_TYPE_MAX_LEN    32
/* Maximum salt length. */
#define SALT_MAX_LEN        64
/* Default PBE iterations. */
#define DEFAULT_ITERATIONS  100000

/* Maps a string to a value. */
typedef struct Str2Val {
    /* String to be matched. */
    const char* string;
    /* Corresponding value. */
    int val;
} String2Val;

/* Get the value corresponding to the string.
 *
 * @param [in]  map  Map of strings to values.
 * @param [in]  len  Number of entries in map.
 * @param [in]  str  String to look-up.
 * @param [out] val  Value corresponding to string.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int StringToVal(const String2Val* map, int len, const char* str,
    int* val)
{
    int ret = 1;
    int i;

    for (i = 0; i < len; i++) {
        if (strcmp(str, map[i].string) == 0) {
            *val = map[i].val;
            ret = 0;
            break;
        }
    }

    return ret;
}

/* Read the contents of a file into a dynamically allocated buffer.
 *
 * Uses realloc as input may be stdin.
 *
 * @param [in]  fp     File pointer to read from.
 * @param [out] pdata  Pointer to data.
 * @param [out] plen   Pointer to length.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int pemApp_ReadFile(FILE* fp, unsigned char** pdata, word32* plen)
{
    int ret = 0;
    word32 len = 0;
    size_t read_len;
    /* Allocate a minimum amount. */
    unsigned char* data = (unsigned char*)malloc(DATA_INC_LEN + BLOCK_SIZE_MAX);

    if (data != NULL) {
        /* Read more data. */
        while ((read_len = fread(data + len, 1, DATA_INC_LEN, fp)) != 0) {
            unsigned char* p;

            /* Add read data amount to length. */
            len += (word32)read_len;

            /* Stop if we are at end-of-file. */
            if (feof(fp)) {
                break;
            }

            /* Make space for more data to be added to buffer. */
            p = (unsigned char*)realloc(data, len + DATA_INC_LEN +
                BLOCK_SIZE_MAX);
            if (p == NULL) {
                /* Reallocation failed - free current buffer. */
                free(data);
                data = NULL;
                break;
            }
            /* Set data to new pointer. */
            data = p;
        }
        /* Done with file. */
        fclose(fp);
    }

    if (data != NULL) {
        /* Return data and length. */
        *pdata = data;
        *plen = len;
    }
    else {
        /* Failed to allocate data. */
        ret = MEMORY_E;
    }
    return ret;
}

/* Write the data to the file.
 *
 * @param [in] fp    File pointer to write to.
 * @param [in] data  Data to write.
 * @param [in] len   Length of data to write in bytes.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int WriteFile(FILE* fp, unsigned char* data, word32 len)
{
    int ret = 0;

    /* Write data to file. */
    if (fwrite(data, 1, len, fp) != len) {
       /* Not all data was written. */
       fprintf(stderr, "Failed to write\n");
       ret = 1;
    }
    /* Close file. */
    fclose(fp);

    return ret;
}

/* List of known PEM types. */
static const String2Val type_map[] = {
    { "CERTIFICATE"          , CERT_TYPE                 },
#ifdef WOLFSSL_CERT_REQ
    { "CERTIFICATE REQUEST"  , CERTREQ_TYPE              },
#endif
#ifndef NO_DH
    { "DH PARAMETERS"        , DH_PARAM_TYPE             },
    { "X9.42 DH PARAMETERS"  , X942_PARAM_TYPE           },
#endif
#ifndef NO_DSA
    { "DSA PARAMETERS"       , DSA_PARAM_TYPE            },
#endif
#ifdef HAVE_CRL
    { "X509 CRL"             , CRL_TYPE                  },
#endif
    { "RSA PRIVATE KEY"      , RSA_TYPE                  },
    { "RSA PUBLIC KEY"       , RSA_PUBLICKEY_TYPE        },
    { "PRIVATE KEY"          , PKCS8_PRIVATEKEY_TYPE     },
    { "ENCRYPTED PRIVATE KEY", PKCS8_ENC_PRIVATEKEY_TYPE },
#ifdef HAVE_ECC
    { "EC PRIVATE KEY"       , ECC_PRIVATEKEY_TYPE       },
#ifdef OPENSSL_EXTRA
    { "EC PARAMETERS"        , ECC_PARAM_TYPE            },
#endif /* OPENSSL_EXTRA */
#endif /* HAVE_ECC */
#ifndef NO_DSA
    { "DSA PRIVATE KEY"      , DSA_PRIVATEKEY_TYPE       },
#endif
    { "PUBLIC KEY"           , ECC_PUBLICKEY_TYPE        },
#if defined(HAVE_ED25519) || defined(HAVE_ED448)
    { "EDDSA PRIVATE KEY"    , EDDSA_PRIVATEKEY_TYPE     },
#endif
};
/* Number of entries in PEM type map. */
#define TYPE_MAP_LEN    ((int)(sizeof(type_map) / sizeof(*type_map)))

/* Convert string to PEM type value.
 *
 * @param [in]  str   PEM type as a string.
 * @param [out] type  PEM type as a value.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int StringToType(const char* str, int* type)
{
    int ret = StringToVal(type_map, TYPE_MAP_LEN, str, type);
    if (ret == 1) {
        fprintf(stderr, "String doesn't match known PEM types: %s\n", str);
    }
    return ret;
}

#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
/* Password callback for returning the password set in the user data.
 *
 * @param [out] passwd    Password buffer.
 * @param [in]  sz        Size of password buffer.
 * @param [in]  rw        Ignored.
 * @param [in]  userdata  Data associated with callback in EncryptedInfo.
 * @return  Length of password.
 */
static int password_from_userdata(char* passwd, int sz, int rw, void* userdata)
{
    (void)rw;
    /* Copy user data into buffer. */
    strncpy(passwd, (const char*)userdata, (size_t)sz);
    passwd[sz - 1] = '\0';
    /* Return length of password returned. */
    return (int)XSTRLEN((const char*)passwd);
}
#endif

/* Find needle in haystack.
 *
 * @param [in]  haystack       String to find needle in.
 * @param [in]  offset         Offset into haystack to start looking.
 * @param [in]  len            Length of haystack.
 * @param [in]  needle         String to find in haystack.
 * @param [in]  needle_len     Length of string to find.
 * @param [out] needle_offset  Offset into haystack at which needle was found.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int FindStr(char* haystack, word32 offset, word32 len,
    const char* needle, word32 needle_len, word32* needle_offset)
{
    /* Assume failure. */
    int ret = 1;
    word32 i;

    /* Ensure there is enough space for needle. */
    if (len >= needle_len) {
        /* Look through haystack starting at offset until not enough space for
         * needle. */
        for (i = offset; i <= len - needle_len; i++) {
            /* Check if needle found. */
            if ((haystack[i] == needle[0]) &&
                    (strncmp(haystack + i, needle, needle_len) == 0)) {
                /* Return offset at which needle found. */
                *needle_offset = i;
                /* Return success. */
                ret = 0;
                /* Stop looking. */
                break;
            }
        }
    }

    return ret;
}

/* Find the next PEM block.
 *
 * @param [in]  data    PEM data.
 * @param [in]  offset  Offset into data to start looking.
 * @param [in]  len     Length of PEM data.
 * @param [out] start   Start of Base64 encoding.
 * @param [out] end     End of Base64 encoding.
 * @param [out] type    PEM type.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int FindPem(char* data, word32 offset, word32 len, word32* start,
    word32* end, int* type)
{
    int ret = 0;
    word32 i;
    word32 type_off;
    char str[PEM_TYPE_MAX_LEN];

    /* Find header. */
    ret = FindStr(data, offset, len, "-----BEGIN ", 11, &i);
    if (ret == 1) {
        /* Got to end without finding PEM header. */
        fprintf(stderr, "No PEM header found\n");
    }
    if (ret == 0) {
        /* Return start of PEM. */
        *start = i;
        /* Get start of type. */
        type_off = i + 11;

        /* Confirm header. */
        ret = FindStr(data, i + 11, len, "-----", 5, &i);
        if (ret == 1) {
            /* Got to end without finding rest of PEM header. */
            fprintf(stderr, "Invalid PEM header\n");
        }
    }
    if (ret == 0) {
        /* Found end of header - convert type string to value. */
        word32 type_len = i - type_off;
        if (type_len >= PEM_TYPE_MAX_LEN) {
            ret = 1;
        }
        if (ret == 0) {
            if (type_len > 0)
                memcpy(str, data + type_off, type_len);
            str[type_len] = '\0';
            ret = StringToType(str, type);
        }
    }
    if (ret == 0) {
        /* Find footer. */
        ret = FindStr(data, i + 5, len, "-----END ", 9, &i);
        if (ret == 1) {
            /* Got to end without finding PEM footer. */
            fprintf(stderr, "No PEM footer found\n");
        }
    }
    if (ret == 0) {
        /* Confirm header. */
        ret = FindStr(data, i + 9, len, "-----", 5, &i);
        if (ret == 1) {
            /* Got to end without finding rest of PEM footer. */
            fprintf(stderr, "Invalid PEM footer\n");
        }
    }
    if (ret == 0) {
        /* Return end of  */
        *end = i + 6;
    }

    return ret;
}

/* Convert PEM to DER and write to file.
 *
 * @param [in]  in      Array of characters that is the PEM data.
 * @param [in]  offset  Offset into array to start looking for PEM block.
 * @param [in]  len     Length of data in array in bytes.
 * @param [out] der     Buffer holding DER encoded data.
 * @param [in]  type    PEM type. -1 indicates to determine from array.
 * @param [in]  info    Encryption information.
 * @return  0 on success.
 * @return  Not 0 on failure.
 */
static int ConvPemToDer(char* in, word32 offset, word32 len, DerBuffer** der,
    int type, EncryptedInfo* info, int padding)
{
    int ret = 0;
    word32 start = 0;
    word32 end = 0;
    /* Set point to start looking and length. */
    char* pem = in + offset;
    word32 pem_len = len - offset;

    /* Check if we need to discover PEM type. */
    if ((ret == 0) && (type == -1)) {
        /* Find PEM block and type. */
        ret = FindPem(pem, 0, pem_len, &start, &end, &type);
        if (ret != 0) {
            fprintf(stderr, "Could not find PEM header\n");
        }
        /* Update start pointer and length. */
        pem += start;
        pem_len = end - start;
    }
    if (ret == 0) {
        /* Convert to DER. */
        ret = wc_PemToDer((unsigned char*)pem, pem_len, type, der, NULL, info,
            NULL);
        if (ret != 0) {
            fprintf(stderr, "Could not convert PEM to DER\n");
        }
    }
    /* Remove padding from encryption if requested. */
    if ((ret == 0) && padding) {
        unsigned char pad = (*der)->buffer[(*der)->length - 1];
        word32 i;

        /* Simple padding validation. */
        if ((pad == 0) || (pad > (*der)->length)) {
            fprintf(stderr, "Invalid padding: %02x\n", pad);
            ret = 1;
        }
        else {
            /* Check padding is valid. */
            for (i = 1; i < pad; i++) {
                if ((*der)->buffer[(*der)->length - 1 - i] != pad) {
                    fprintf(stderr, "Invalid padding: %d\n", pad);
                    ret = 1;
                    break;
                }
            }
            if (ret == 0) {
                /* Don't write out padding. */
                (*der)->length -= pad;
            }
        }
    }

    return ret;
}

#ifdef WOLFSSL_DER_TO_PEM
#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
/* List of known PBE algorithms. */
static const String2Val pbe_map[] = {
#ifndef NO_SHA
    #ifndef NO_RC4
        { "SHA1_RC4_128"  , ENC_PKCS8_PBE_SHA1_RC4_128   },
    #endif
    #ifndef NO_DES
        { "SHA1_DES3"     , ENC_PKCS8_PBE_SHA1_DES3      },
        { "PBES1_SHA1_DES", ENC_PKCS8_PBES1_SHA1_DES     },
    #endif
    #ifdef WC_RC2
        { "SHA1_40RC2_CBC", ENC_PKCS8_PBE_SHA1_40RC2_CBC },
    #endif
#endif
#ifndef NO_MD5
    #ifndef NO_DES
        { "PBES1_MD5_DES" , ENC_PKCS8_PBES1_MD5_DES      },
    #endif
#endif
        { "PBES2"         , ENC_PKCS8_PBES2              },
};
/* Number of entries in PBE map. */
#define PBE_MAP_LEN     ((int)(sizeof(pbe_map) / sizeof(*pbe_map)))

/* Convert string to PBE value.
 *
 * @param [in]  str  PBE as a string.
 * @param [out] pbe  PBE as a value.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int StringToPbe(char* str, int* pbe)
{
    int ret = StringToVal(pbe_map, PBE_MAP_LEN, str, pbe);
    if (ret == 1) {
        fprintf(stderr, "String doesn't match known PBE algorithms: %s\n", str);
    }
    return ret;
}

/* List of known PBE versions. */
static const String2Val pbe_ver_map[] = {
    { "PKCS12"  , ENC_PKCS8_VER_PKCS12 },
    { "PKCS12v1", ENC_PKCS8_VER_PKCS12 },
    { "PKCS5"   , ENC_PKCS8_VER_PKCS5  },
};
/* Number of entries in PBE versions map. */
#define PBE_VER_MAP_LEN     ((int)(sizeof(pbe_ver_map) / sizeof(*pbe_ver_map)))

/* Convert string to PBE version value.
 *
 * @param [in]  str      PBE version as a string.
 * @param [out] pbe_ver  PBE version as a value.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int StringToPbeVer(char* str, int* pbe_ver)
{
    int ret = StringToVal(pbe_ver_map, PBE_VER_MAP_LEN, str, pbe_ver);
    if (ret == 1) {
        fprintf(stderr, "String doesn't match known PBE versions: %s\n", str);
    }
    return ret;
}

/* List of known PKCS#5v2 PBE encryption algorithms. */
static const String2Val pbe_alg_map[] = {
    { "AES-128-CBC", ENC_PKCS8_ALG_AES128CBC },
    { "AES-256-CBC", ENC_PKCS8_ALG_AES256CBC },
    { "DES"        , ENC_PKCS8_ALG_DES       },
    { "DES3"       , ENC_PKCS8_ALG_DES3      },
};
/* Number of entries in PBE algorithm map. */
#define PBE_ALG_MAP_LEN     ((int)(sizeof(pbe_alg_map) / sizeof(*pbe_alg_map)))

/* Convert string to PBE algorithm value.
 *
 * @param [in]  str      PBE algorithm as a string.
 * @param [out] pbe_alg  PBE algorithm as a value.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int StringToPbeAlg(char* str, int* pbe_alg)
{
    int ret = StringToVal(pbe_alg_map, PBE_ALG_MAP_LEN, str, pbe_alg);
    if (ret == 1) {
        fprintf(stderr, "String doesn't match known PBE algorithms: %s\n", str);
    }
    return ret;
}

/* Encrypt the DER data.
 *
 * @param [in]  in           DER data to encrypt.
 * @param [in]  in_len       Length of DER data.
 * @param [in]  password     Password to use to derive key for encryption.
 * @param [in]  iterations   Number of iterations in PBE.
 * @param [in]  salt_sz      Size of salt to use in bytes.
 * @param [in]  pbe          PBE algorithm to use.
 * @param [in]  pbe_ver      Version of PBE algorithm to use.
 * @param [in]  enc_alg_id   Encryption algorithm id for when using PBES2.
 * @param [out] enc          DER encrypted data.
 * @param [out] enc_len      Length of DER encrypted data.
 * @return  0 on success.
 * @return  1 on failure.
 */
static int EncryptDer(unsigned char* in, word32 in_len, char* password,
    unsigned int iterations, unsigned int salt_sz, int pbe, int pbe_ver,
    int enc_alg_id, unsigned char** enc, word32* enc_len)
{
    int ret;
    WC_RNG rng;
    unsigned char salt[SALT_MAX_LEN];

    if (password == NULL)
        return 1;

    XMEMSET(&rng, 0, sizeof(rng));

    /* Create a random number generator. */
    ret = wc_InitRng(&rng);
    if (ret == 0) {
        /* Get salt from random number generator. */
        ret = wc_RNG_GenerateBlock(&rng, salt, salt_sz);
    }
    if (ret == 0) {
        /* Get length of encrypted DER data. */
        ret = wc_CreateEncryptedPKCS8Key(in, in_len, NULL, enc_len, password,
            (int)strlen(password), pbe_ver, pbe, enc_alg_id, salt, salt_sz,
            (int)iterations, &rng, NULL);
        if (ret == LENGTH_ONLY_E) {
            ret = 0;
        }
        else if (ret == 0) {
            ret = 1;
        }
    }
    if (ret == 0) {
        /* Allocate memory for encrypted DER data. */
        *enc = (unsigned char*)malloc(*enc_len);
        if (*enc == NULL) {
            ret = 1;
        }
    }
    if (ret == 0) {
        /* Encrypt DER data. */
        ret = wc_CreateEncryptedPKCS8Key(in, in_len, *enc, enc_len, password,
            (int)strlen(password), pbe_ver, pbe, enc_alg_id, salt, salt_sz,
            (int)iterations, &rng, NULL);
        if (ret > 0) {
            ret = 0;
        }
    }

    wc_FreeRng(&rng);
    return ret;
}
#endif

/* Convert DER to PEM and write to file.
 *
 * @param [in]  in          Array of bytes holding the DER encoding.
 * @param [in]  offset      Offset into array of data to convert to PEM.
 * @param [in]  len         Length of data in array in bytes.
 * @param [out] out         Allocated buffer holding PEM encoding.
 * @param [out] out_len     Length of PEM encoding in bytes.
 * @param [in]  type        PEM type.
 * @param [in]  cipher_str  String to write into encrypted key.
 * @return  0 on success.
 * @return  Not 0 on failure.
 */
static int ConvDerToPem(unsigned char* in, word32 offset, word32 len,
    unsigned char** out, word32* out_len, int type, const char* cipher_str)
{
    int ret = 0;
    unsigned char* pem = NULL;
    unsigned int pem_len = 0;
    /* Set point to start looking and length. */
    unsigned char* der = in + offset;
    word32 der_len = len - offset;

    /* Get length of PEM based on DER. */
    ret = wc_DerToPemEx(der, der_len, NULL, 0, (byte*)cipher_str, type);
    if (ret <= 0) {
        fprintf(stderr, "Could not determine length of PEM\n");
    }
    pem_len = (unsigned int)ret;
    if (ret > 0) {
        ret = 0;
    }
    if ((ret == 0) && (pem_len > 0)) {
        /* Allocate memory to hold PEM encoding. */
        pem = (unsigned char*)malloc(pem_len);
        if (pem == NULL) {
            ret = 1;
        }
    }
    if (ret == 0) {
        /* Convert DER to PEM. */
        ret = wc_DerToPemEx(der, der_len, pem, pem_len, (byte*)cipher_str,
            type);
        if (ret <= 0) {
            fprintf(stderr, "Could not convert DER to PEM\n");
        }
        if (ret > 0) {
            *out = pem;
            *out_len = (word32)ret;
            ret = 0;
        }
    }

    return ret;
}
#endif

/* Usage lines to show. */
const char* usage[] = {
    "pem [OPTION]...",
    "Convert to/from PEM and DER.",
    "",
    "Options:",
    "  -?, --help       display this help and exit",
    "  -t --type        string representing type of data",
    "  -in              name of file to read (uses stdin otherwise)",
    "  -out             name of file to write to (uses stdout otherwise)",
    "  -o --offset      offset into file where data to convert starts",
#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
    "  -p --pass        password to use with encrypted keys",
#endif
#ifdef WOLFSSL_DER_TO_PEM
    "  -d --der         input is DER and output is PEM",
#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
    "  --padding        Remove padding on decrypted data",
    "  -e --encrypt     DER key is to be encrypted",
    "  -v --pbe-ver     PBE version to use when encrypting key (see below)",
    "  -p --pbe         PBE to use when encrypting key (see below)",
    "  -a --pbe-alg     PBES2 algorithm to use when encrypting key (see below)",
    "  -i --iter        number of iterations of PBE - default: 100000",
    "  -s --salt-sz     length, in bytes, of salt to generate - 0-64",
#endif
#endif
#ifdef DEBUG_WOLFSSL
    "  -l --log         turn on wolfSSL logging",
#endif
    "",
};
/* Number of usage lines. */
#define USAGE_SZ  ((int)(sizeof(usage) / sizeof(*usage)))

const struct string_usage_st {
    const char* str;
    const String2Val* map;
    int len;
} known_strings[] = {
    { "Known PEM header/trailer strings:", type_map   , TYPE_MAP_LEN    },
#if defined(WOLFSSL_DER_TO_PEM) && defined(WOLFSSL_ENCRYPTED_KEYS) && \
    !defined(NO_PWDBASED)
    { "Known PBE version strings:"       , pbe_ver_map, PBE_VER_MAP_LEN },
    { "Known PBE strings:"               , pbe_map    , PBE_MAP_LEN     },
    { "Known PBES2 algorithm strings:"   , pbe_alg_map, PBE_ALG_MAP_LEN },
#endif
};
/* Number of usage lines. */
#define KNOWN_STRINGS_SZ    \
    ((int)(sizeof(known_strings) / sizeof(*known_strings)))

/* Print out usage lines.
 */
static void Usage(void)
{
    int i;
    int j;

    /* Usage lines. */
    for (i = 0; i < USAGE_SZ; i++) {
        printf("%s\n", usage[i]);
    }
    /* Known strings for options. */
    for (j = 0; j < KNOWN_STRINGS_SZ; j++) {
        printf("%s\n", known_strings[j].str);
        for (i = 0; i < known_strings[j].len; i++) {
            printf("    %s\n", known_strings[j].map[i].string);
        }
    }
}

/* Main entry of ASN.1 printing program.
 *
 * @param [in] argc  Count of command line arguments.
 * @param [in] argv  Command line arguments.
 * @return  0 on success.
 * @return  1 on failure.
 */
int main(int argc, char* argv[])
{
    int ret = 0;
    /* Default to reading STDIN. */
    FILE* in_file = stdin;
    /* Default to writing to STDOUT. */
    FILE* out_file = stdout;
    const char* out_name = NULL;
    unsigned char* in = NULL;
    word32 in_len = 0;
    word32 offset = 0;
    unsigned char* out = NULL;
    word32 out_len = 0;
    int pem = 1;
    const char* type_str = NULL;
    int type = -1;
    DerBuffer* der = NULL;
    EncryptedInfo info;
    int padding = 0;
#if defined(WOLFSSL_DER_TO_PEM) && defined(WOLFSSL_ENCRYPTED_KEYS) && \
    !defined(NO_PWDBASED)
    int enc_der = 0;
    unsigned char* enc = NULL;
    word32 enc_len = 0;
    unsigned int iterations = DEFAULT_ITERATIONS;
    unsigned int salt_sz = 8;
    int pbe_ver = ENC_PKCS8_VER_PKCS5;
    int pbe = ENC_PKCS8_PBES2;
    int pbe_alg = ENC_PKCS8_ALG_AES256CBC;
#endif
#ifdef DEBUG_WOLFSSL
    int log = 0;
#endif

    memset(&info, 0, sizeof(info));

    /* Skip over program name. */
    argc--;
    argv++;
    while (argc > 0) {
        /* PEM header type. */
        if ((strcmp(argv[0], "-t") == 0) ||
                 (strcmp(argv[0], "--type") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No type string provided\n");
                return 1;
            }
            type_str = argv[0];
        }
        /* Name of input file. */
        else if (strcmp(argv[0], "-in") == 0) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No filename provided\n");
                return 1;
            }
            in_file = fopen(argv[0], "r");
            if (in_file == NULL) {
                fprintf(stderr, "File not able to be read: %s\n", argv[0]);
                return 1;
            }
        }
        /* Name of output file. */
        else if (strcmp(argv[0], "-out") == 0) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No filename provided\n");
                return 1;
            }
            out_name = argv[0];
        }
        /* Offset into input data to start from. */
        else if ((strcmp(argv[0], "-o") == 0) ||
                 (strcmp(argv[0], "--offset") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No filename provided\n");
                return 1;
            }
            offset = (word32)strtoul(argv[0], NULL, 10);
        }
#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
        /* Password to use when encrypting or decrypting keys with PEM. */
        else if ((strcmp(argv[0], "-p") == 0) ||
                 (strcmp(argv[0], "--pass") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No password provided\n");
                return 1;
            }
            info.passwd_cb = password_from_userdata;
            info.passwd_userdata = argv[0];
        }
#endif
#ifdef WOLFSSL_DER_TO_PEM
        /* Input is DER and we are converting to PEM. */
        else if ((strcmp(argv[0], "-d") == 0) ||
                 (strcmp(argv[0], "--der") == 0)) {
            pem = 0;
        }
#if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
        /* Remove padding leftover from decryption. */
        else if (strcmp(argv[0], "--padding") == 0) {
            padding = 1;
        }
        /* Encrypting the DER data. */
        else if ((strcmp(argv[0], "-e") == 0) ||
                 (strcmp(argv[0], "--encrypt") == 0)) {
            enc_der = 1;
        }
        /* PBE version. */
        else if ((strcmp(argv[0], "-v") == 0) ||
                 (strcmp(argv[0], "--pbe-ver") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No PBE version provided\n");
                return 1;
            }
            if (StringToPbeVer(argv[0], &pbe_ver) != 0) {
                return 1;
            }
        }
        /* PBE algorithm. */
        else if ((strcmp(argv[0], "-p") == 0) ||
                 (strcmp(argv[0], "--pbe") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No PBE provided\n");
                return 1;
            }
            if (StringToPbe(argv[0], &pbe) != 0) {
                return 1;
            }
        }
        /* PBES2 algorithm. */
        else if ((strcmp(argv[0], "-a") == 0) ||
                 (strcmp(argv[0], "--pbe-alg") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No PBE algorithm provided\n");
                return 1;
            }
            if (StringToPbeAlg(argv[0], &pbe_alg) != 0) {
                return 1;
            }
        }
        /* Number of PBE iterations. */
        else if ((strcmp(argv[0], "-i") == 0) ||
                 (strcmp(argv[0], "--iter") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No filename provided\n");
                return 1;
            }
            iterations = (unsigned int)strtoul(argv[0], NULL, 10);
        }
        /* Size of salt to be generated. */
        else if ((strcmp(argv[0], "-s") == 0) ||
                 (strcmp(argv[0], "--salt-sz") == 0)) {
            argc--;
            argv++;
            if (argc == 0) {
                fprintf(stderr, "No salt size provided\n");
                return 1;
            }
            salt_sz = (unsigned int)strtoul(argv[0], NULL, 10);
            if (salt_sz > SALT_MAX_LEN) {
                fprintf(stderr, "Salt size must be no bigger than %d: %d\n",
                    SALT_MAX_LEN, salt_sz);
                return 1;
            }
        }
#endif /* WOLFSSL_ENCRYPTED_KEYS !NO_PWDBASED */
#endif /* WOLFSSL_DER_TO_PEM */
#ifdef DEBUG_WOLFSSL
        /* Turn on logging. */
        else if ((strcmp(argv[0], "-l") == 0) ||
                 (strcmp(argv[0], "--log") == 0)) {
            log = 1;
        }
#endif
        /* Display help/usage. */
        else if ((strcmp(argv[0], "-?") == 0) ||
                 (strcmp(argv[0], "--help") == 0)) {
            Usage();
            return 0;
        }
        else {
            fprintf(stderr, "Bad option: %s\n", argv[0]);
            Usage();
            return 1;
        }

        /* Move on to next command line argument. */
        argc--;
        argv++;
    }

#ifdef DEBUG_WOLFSSL
    if (log) {
        wolfSSL_Debugging_ON();
    }
#endif

    /* Convert PEM type string to value. */
    if (type_str != NULL) {
        ret = StringToType(type_str, &type);
    }
#if defined(WOLFSSL_DER_TO_PEM) && defined(WOLFSSL_ENCRYPTED_KEYS) && \
    !defined(NO_PWDBASED)
    /* Check whether we are encrypting DER. */
    if ((!pem) && (type == PKCS8_ENC_PRIVATEKEY_TYPE)) {
        enc_der = 1;
    }
#endif
    /* Read all of PEM file. */
    if ((ret == 0) && (pemApp_ReadFile(in_file, &in, &in_len) != 0)) {
        fprintf(stderr, "Reading file failed\n");
        ret = 1;
    }

    if ((ret == 0) && pem) {
        /* Convert PEM to DER. */
        ret = ConvPemToDer((char*)in, offset, in_len, &der, type, &info,
            padding);
        if (ret == 0) {
            out = der->buffer;
            out_len = der->length;
        }
    }
    else {
#ifdef WOLFSSL_DER_TO_PEM
    #if defined(WOLFSSL_ENCRYPTED_KEYS) && !defined(NO_PWDBASED)
        if (enc_der) {
            /* Encrypt DER first. */
            ret = EncryptDer(in + offset, in_len - offset,
                (char*)info.passwd_userdata, iterations, salt_sz, pbe, pbe_ver,
                pbe_alg, &enc, &enc_len);
            if (ret == 0) {
                /* Convert encrypted DER data to PEM. */
                ret = ConvDerToPem(enc, 0, enc_len, &out, &out_len, type,
                    NULL);
            }
        }
        else
    #endif /* WOLFSSL_ENCRYPTED_KEYS && !NO_PWDBASED */
        {
            /* Convert DER data to PEM. */
            ret = ConvDerToPem(in, offset, in_len, &out, &out_len, type, NULL);
        }
#else
        fprintf(stderr, "DER to PEM not supported by wolfSSL\n");
        ret = 1;
#endif
    }

    if ((ret == 0) && (out_name != NULL)) {
        /*Open write named file to write to. */
        out_file = fopen(out_name, "w");
        if (out_file == NULL) {
            fprintf(stderr, "File not able to be written: %s\n", out_name);
            ret = 1;
        }
    }
    if (ret == 0) {
        /* Write out PEM. */
        ret = WriteFile(out_file, out, out_len);
        if (ret != 0) {
            fprintf(stderr, "Could not write file\n");
        }
    }

    /* Dispose of allocated data. */
    if (der != NULL) {
        wc_FreeDer(&der);
    }
    else if (out != NULL) {
        free(out);
    }
#if defined(WOLFSSL_DER_TO_PEM) && defined(WOLFSSL_ENCRYPTED_KEYS) && \
    !defined(NO_PWDBASED)
    if (enc != NULL) {
        free(enc);
    }
#endif
    if (in != NULL) {
        free(in);
    }
    if (ret < 0) {
        fprintf(stderr, "%s\n", wc_GetErrorString(ret));
    }
    return (ret == 0) ? 0 : 1;
}

#else

/* Main entry of ASN.1 printing program.
 *
 * @param [in] argc  Count of command line arguments.
 * @param [in] argv  Command line arguments.
 * @return  0 on success.
 * @return  1 on failure.
 */
int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    fprintf(stderr, "PEM to DER conversion of file system support not compiled"
                    " in.\n");
    return 0;
}

#endif /* WOLFSSL_PEM_TO_DER && !NO_FILESYSTEM */
