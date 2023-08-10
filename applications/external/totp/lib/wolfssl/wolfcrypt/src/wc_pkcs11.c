/* wc_pkcs11.c
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

#ifdef HAVE_PKCS11

#ifndef HAVE_PKCS11_STATIC
#include <dlfcn.h>
#endif

#include <wolfssl/wolfcrypt/wc_pkcs11.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/asn.h>
#include <wolfssl/wolfcrypt/logging.h>
#ifndef NO_RSA
    #include <wolfssl/wolfcrypt/rsa.h>
#endif
#ifdef NO_INLINE
    #include <wolfssl/wolfcrypt/misc.h>
#else
    #define WOLFSSL_MISC_INCLUDED
    #include <wolfcrypt/src/misc.c>
#endif

#ifndef WOLFSSL_HAVE_ECC_KEY_GET_PRIV
    /* FIPS build has replaced ecc.h. */
    #define wc_ecc_key_get_priv(key) (&((key)->k))
    #define WOLFSSL_HAVE_ECC_KEY_GET_PRIV
#endif

#if defined(NO_PKCS11_RSA) && !defined(NO_RSA)
    #define NO_RSA
#endif
#if defined(NO_PKCS11_ECC) && defined(HAVE_ECC)
    #undef HAVE_ECC
#endif
#if defined(NO_PKCS11_AES) && !defined(NO_AES)
    #define NO_AES
#endif
#if defined(NO_PKCS11_AESGCM) && defined(HAVE_AESGCM)
    #undef HAVE_AESGCM
#endif
#if defined(NO_PKCS11_AESCBC) && defined(HAVE_AES_CBC)
    #undef HAVE_AES_CBC
#endif
#if defined(NO_PKCS11_HMAC) && !defined(NO_HMAC)
    #define NO_HMAC
#endif
#if defined(NO_PKCS11_RNG) && !defined(WC_NO_RNG)
    #define WC_NO_RNG
#endif


/* Maximum length of the EC parameter string. */
#define MAX_EC_PARAM_LEN   16


#if defined(HAVE_ECC) && !defined(NO_PKCS11_ECDH)
/* Pointer to false required for templates. */
static CK_BBOOL ckFalse = CK_FALSE;
#endif
#if !defined(NO_RSA) || defined(HAVE_ECC) || (!defined(NO_AES) && \
           (defined(HAVE_AESGCM) || defined(HAVE_AES_CBC))) || !defined(NO_HMAC)
/* Pointer to true required for templates. */
static CK_BBOOL ckTrue  = CK_TRUE;
#endif

#ifndef NO_RSA
/* Pointer to RSA key type required for templates. */
static CK_KEY_TYPE rsaKeyType  = CKK_RSA;
#endif
#ifdef HAVE_ECC
/* Pointer to EC key type required for templates. */
static CK_KEY_TYPE ecKeyType   = CKK_EC;
#endif
#if !defined(NO_RSA) || defined(HAVE_ECC)
/* Pointer to public key class required for templates. */
static CK_OBJECT_CLASS pubKeyClass     = CKO_PUBLIC_KEY;
/* Pointer to private key class required for templates. */
static CK_OBJECT_CLASS privKeyClass    = CKO_PRIVATE_KEY;
#endif
#if (!defined(NO_AES) && (defined(HAVE_AESGCM) || defined(HAVE_AES_CBC))) || \
            !defined(NO_HMAC) || (defined(HAVE_ECC) && !defined(NO_PKCS11_ECDH))
/* Pointer to secret key class required for templates. */
static CK_OBJECT_CLASS secretKeyClass  = CKO_SECRET_KEY;
#endif

#ifdef WOLFSSL_DEBUG_PKCS11
/* Enable logging of PKCS#11 calls and return value. */
#define PKCS11_RV(op, rv)       pkcs11_rv(op, rv)
/* Enable logging of PKCS#11 calls and value. */
#define PKCS11_VAL(op, val)     pkcs11_val(op, val)
/* Enable logging of PKCS#11 template. */
#define PKCS11_DUMP_TEMPLATE(name, templ, cnt)  \
    pkcs11_dump_template(name, templ, cnt)

/* Formats of template items - used to instruct how to log information. */
enum PKCS11_TYPE_FORMATS {
    PKCS11_FMT_BOOLEAN,
    PKCS11_FMT_CLASS,
    PKCS11_FMT_KEY_TYPE,
    PKCS11_FMT_STRING,
    PKCS11_FMT_NUMBER,
    PKCS11_FMT_DATA,
    PKCS11_FMT_POINTER
};
/* Information for logging a template item. */
static struct PKCS11_TYPE_STR {
    /** Attribute type in template. */
    CK_ATTRIBUTE_TYPE type;
    /** String to log corresponding to attribute type. */
    const char* str;
    /** Format of data associated with template item. */
    int format;
} typeStr[] = {
    { CKA_CLASS,            "CKA_CLASS",              PKCS11_FMT_CLASS      },
    { CKA_TOKEN,            "CKA_TOKEN",              PKCS11_FMT_POINTER    },
    { CKA_PRIVATE,          "CKA_PRIVATE",            PKCS11_FMT_BOOLEAN    },
    { CKA_LABEL,            "CKA_LABEL",              PKCS11_FMT_STRING     },
    { CKA_VALUE,            "CKA_VALUE",              PKCS11_FMT_DATA       },
    { CKA_OBJECT_ID,        "CKA_OBJECT_ID",          PKCS11_FMT_POINTER    },
    { CKA_KEY_TYPE,         "CKA_KEY_TYPE",           PKCS11_FMT_KEY_TYPE   },
    { CKA_ID,               "CKA_ID",                 PKCS11_FMT_DATA       },
    { CKA_SENSITIVE,        "CKA_SENSITIVE",          PKCS11_FMT_BOOLEAN    },
    { CKA_ENCRYPT,          "CKA_ENCRYPT",            PKCS11_FMT_BOOLEAN    },
    { CKA_DECRYPT,          "CKA_DECRYPT",            PKCS11_FMT_BOOLEAN    },
    { CKA_SIGN,             "CKA_SIGN",               PKCS11_FMT_BOOLEAN    },
    { CKA_VERIFY,           "CKA_VERIFY",             PKCS11_FMT_BOOLEAN    },
    { CKA_DERIVE,           "CKA_DERIVE",             PKCS11_FMT_BOOLEAN    },
    { CKA_MODULUS_BITS,     "CKA_MODULUS_BITS",       PKCS11_FMT_NUMBER     },
    { CKA_MODULUS,          "CKA_MODULUS",            PKCS11_FMT_DATA       },
    { CKA_PUBLIC_EXPONENT,  "CKA_PUBLIC_EXPONENT",    PKCS11_FMT_DATA       },
    { CKA_PRIVATE_EXPONENT, "CKA_PRIVATE_EXPONENT",   PKCS11_FMT_DATA       },
    { CKA_PRIME_1,          "CKA_PRIME_1",            PKCS11_FMT_DATA       },
    { CKA_PRIME_2,          "CKA_PRIME_2",            PKCS11_FMT_DATA       },
    { CKA_EXPONENT_1,       "CKA_EXPONENT_1",         PKCS11_FMT_DATA       },
    { CKA_EXPONENT_2,       "CKA_EXPONENT_2",         PKCS11_FMT_DATA       },
    { CKA_VALUE_LEN,        "CKA_VALUE_LEN",          PKCS11_FMT_NUMBER     },
    { CKA_COEFFICIENT,      "CKA_COEFFICIENT",        PKCS11_FMT_DATA       },
    { CKA_EXTRACTABLE,      "CKA_EXTRACTABLE",        PKCS11_FMT_BOOLEAN    },
    { CKA_EC_PARAMS,        "CKA_EC_PARAMS",          PKCS11_FMT_DATA       },
    { CKA_EC_POINT,         "CKA_EC_POINT",           PKCS11_FMT_DATA       },
};
/* Count of known attribute types for logging. */
#define PKCS11_TYPE_STR_CNT  ((int)(sizeof(typeStr) / sizeof(*typeStr)))

/*
 * Dump/log the PKCS #11 template.
 *
 * This is only for debugging purposes. Only the values needed are recognised.
 *
 * @param  [in]  name   PKCS #11 template name.
 * @param  [in]  templ  PKCS #11 template to dump.
 * @param  [in]  cnt    Count of template entries.
 */
static void pkcs11_dump_template(const char* name, CK_ATTRIBUTE* templ,
                                 CK_ULONG cnt)
{
    CK_ULONG i;
    int j;
    char line[80];
    char type[25];
    int format;
    CK_KEY_TYPE keyType;
    CK_OBJECT_CLASS keyClass;

    WOLFSSL_MSG(name);

    for (i = 0; i < cnt; i++) {
        format = PKCS11_FMT_POINTER;

        for (j = 0; j < PKCS11_TYPE_STR_CNT; j++) {
            if (templ[i].type == typeStr[j].type) {
                XSNPRINTF(type, sizeof(type), "%s", typeStr[j].str);
                format = typeStr[j].format;
                break;
            }
        }
        if (j == PKCS11_TYPE_STR_CNT) {
            XSNPRINTF(type, sizeof(type), "%08lxUL", templ[i].type);
        }

        switch (format) {
        case PKCS11_FMT_BOOLEAN:
#if !defined(NO_RSA) || defined(HAVE_ECC) || (!defined(NO_AES) && \
           (defined(HAVE_AESGCM) || defined(HAVE_AES_CBC))) || !defined(NO_HMAC)
            if (templ[i].pValue == &ckTrue) {
                XSNPRINTF(line, sizeof(line), "%25s: TRUE", type);
                WOLFSSL_MSG(line);
            }
            else
#endif
#if defined(HAVE_ECC) && !defined(NO_PKCS11_ECDH)
            if (templ[i].pValue == &ckFalse) {
                XSNPRINTF(line, sizeof(line), "%25s: FALSE", type);
                WOLFSSL_MSG(line);
            }
            else
#endif
            {
                XSNPRINTF(line, sizeof(line), "%25s: INVALID (%p)", type,
                          templ[i].pValue);
                WOLFSSL_MSG(line);
            }
            break;
        case PKCS11_FMT_CLASS:
            keyClass = *(CK_OBJECT_CLASS*)templ[i].pValue;
            if (keyClass == CKO_PUBLIC_KEY) {
                XSNPRINTF(line, sizeof(line), "%25s: PUBLIC", type);
                WOLFSSL_MSG(line);
            }
            else if (keyClass == CKO_PRIVATE_KEY) {
                XSNPRINTF(line, sizeof(line), "%25s: PRIVATE", type);
                WOLFSSL_MSG(line);
            }
            else if (keyClass == CKO_SECRET_KEY) {
                XSNPRINTF(line, sizeof(line), "%25s: SECRET", type);
                WOLFSSL_MSG(line);
            }
            else
            {
                XSNPRINTF(line, sizeof(line), "%25s: UNKNOWN (%p)", type,
                          templ[i].pValue);
                WOLFSSL_MSG(line);
            }
            break;
        case PKCS11_FMT_KEY_TYPE:
            keyType = *(CK_KEY_TYPE*)templ[i].pValue;
            switch (keyType) {
            case CKK_RSA:
                XSNPRINTF(line, sizeof(line), "%25s: RSA", type);
                break;
            case CKK_DH:
                XSNPRINTF(line, sizeof(line), "%25s: DH", type);
                break;
            case CKK_EC:
                XSNPRINTF(line, sizeof(line), "%25s: EC", type);
                break;
            case CKK_GENERIC_SECRET:
                XSNPRINTF(line, sizeof(line), "%25s: GENERIC_SECRET", type);
                break;
            case CKK_AES:
                XSNPRINTF(line, sizeof(line), "%25s: AES", type);
                break;
            case CKK_MD5_HMAC:
                XSNPRINTF(line, sizeof(line), "%25s: MD5_HMAC", type);
                break;
            case CKK_SHA_1_HMAC:
                XSNPRINTF(line, sizeof(line), "%25s: SHA_1_HMAC", type);
                break;
            case CKK_SHA256_HMAC:
                XSNPRINTF(line, sizeof(line), "%25s: SHA256_HMAC", type);
                break;
            case CKK_SHA384_HMAC:
                XSNPRINTF(line, sizeof(line), "%25s: SHA384_HMAC", type);
                break;
            case CKK_SHA512_HMAC:
                XSNPRINTF(line, sizeof(line), "%25s: SHA512_HMAC", type);
                break;
            case CKK_SHA224_HMAC:
                XSNPRINTF(line, sizeof(line), "%25s: SHA224_HMAC", type);
                break;
            default:
                XSNPRINTF(line, sizeof(line), "%25s: UNKNOWN (%08lx)", type,
                          keyType);
                break;
            }
            WOLFSSL_MSG(line);
            break;
        case PKCS11_FMT_STRING:
            XSNPRINTF(line, sizeof(line), "%25s: %s", type,
                      (char*)templ[i].pValue);
            WOLFSSL_MSG(line);
            break;
        case PKCS11_FMT_NUMBER:
            if (templ[i].ulValueLen <= 1) {
                XSNPRINTF(line, sizeof(line), "%25s: 0x%02x (%d)", type,
                          *(byte*)templ[i].pValue, *(byte*)templ[i].pValue);
            }
            else if (templ[i].ulValueLen <= 2) {
                XSNPRINTF(line, sizeof(line), "%25s: 0x%04x (%d)", type,
                          *(word16*)templ[i].pValue, *(word16*)templ[i].pValue);
            }
            else if (templ[i].ulValueLen <= 4) {
                XSNPRINTF(line, sizeof(line), "%25s: 0x%08x (%d)", type,
                          *(word32*)templ[i].pValue, *(word32*)templ[i].pValue);
            }
            else if (templ[i].ulValueLen <= 8) {
                XSNPRINTF(line, sizeof(line), "%25s: 0x%016lx (%ld)", type,
                          *(word64*)templ[i].pValue, *(word64*)templ[i].pValue);
            }
            else {
                XSNPRINTF(line, sizeof(line), "%25s: INVALID (%ld)", type,
                          templ[i].ulValueLen);
            }
            WOLFSSL_MSG(line);
            break;
        case PKCS11_FMT_DATA:
            XSNPRINTF(line, sizeof(line), "%25s: %ld", type,
                      templ[i].ulValueLen);
            WOLFSSL_MSG(line);
            if (templ[i].pValue == NULL) {
                XSNPRINTF(line, sizeof(line), "%27s(nil)", "");
                WOLFSSL_MSG(line);
                break;
            }
            XSNPRINTF(line, sizeof(line), "%27s", "");
            for (j = 0; j < (int)templ[i].ulValueLen && j < 80; j++) {
                char hex[6];
                XSNPRINTF(hex, sizeof(hex), "0x%02x,",
                          ((byte*)templ[i].pValue)[j]);
                XSTRNCAT(line, hex, 5);
                if ((j % 8) == 7) {
                    WOLFSSL_MSG(line);
                    XSNPRINTF(line, sizeof(line), "%27s", "");
                }
            }
            if (j == (int)templ[i].ulValueLen) {
                if ((j % 8) != 0) {
                    WOLFSSL_MSG(line);
                }
            }
            else if (j < (int)templ[i].ulValueLen) {
                XSNPRINTF(line, sizeof(line), "%27s...", "");
                WOLFSSL_MSG(line);
            }
            break;
        case PKCS11_FMT_POINTER:
            XSNPRINTF(line, sizeof(line), "%25s: %p %ld", type, templ[i].pValue,
                      templ[i].ulValueLen);
            WOLFSSL_MSG(line);
            break;
        }
    }
}

/*
 * Log a PKCS #11 return value with the name of function called.
 *
 * This is only for debugging purposes. Only the values needed are recognized.
 *
 * @param  [in]  op  PKCS #11 operation that was attempted.
 * @param  [in]  rv  PKCS #11 return value.
 */
static void pkcs11_rv(const char* op, CK_RV rv)
{
    char line[80];

    if (rv == CKR_OK) {
        XSNPRINTF(line, 80, "%s: OK", op);
    }
    else if (rv == CKR_MECHANISM_INVALID) {
        XSNPRINTF(line, 80, "%s: MECHANISM_INVALID", op);
    }
    else if (rv == CKR_SIGNATURE_INVALID) {
        XSNPRINTF(line, 80, "%s: SIGNATURE_INVALID", op);
    }
    else {
        XSNPRINTF(line, 80, "%s: %08lxUL (FAILED)", op, rv);
    }

    WOLFSSL_MSG(line);
}

/*
 * Log a value from a PKCS #11 operation.
 *
 * This is only for debugging purposes.
 *
 * @param  [in]  op   PKCS #11 operation that was attempted.
 * @param  [in]  val  Value to log.
 */
static void pkcs11_val(const char* op, CK_ULONG val)
{
    char line[80];

    XSNPRINTF(line, 80, "%s: %ld", op, val);

    WOLFSSL_MSG(line);
}
#else
/* Disable logging of PKCS#11 calls and return value. */
#define PKCS11_RV(op, ev) WC_DO_NOTHING
/* Disable logging of PKCS#11 calls and value. */
#define PKCS11_VAL(op, val) WC_DO_NOTHING
/* Disable logging of PKCS#11 template. */
#define PKCS11_DUMP_TEMPLATE(name, templ, cnt) WC_DO_NOTHING
#endif

/**
 * Load library, get function list and initialize PKCS#11.
 *
 * @param  [in]  dev      Device object.
 * @param  [in]  library  Library name including path.
 * @param  [in]  heap     Heap hint.
 * @return  BAD_FUNC_ARG when dev or library are NULL pointers.
 * @return  BAD_PATH_ERROR when dynamic library cannot be opened.
 * @return  WC_INIT_E when the initialization PKCS#11 fails.
 * @return  WC_HW_E when unable to get PKCS#11 function list.
 * @return  0 on success.
 */
int wc_Pkcs11_Initialize(Pkcs11Dev* dev, const char* library, void* heap)
{
    return wc_Pkcs11_Initialize_ex(dev, library, heap, NULL);
}

/**
 * Load library, get function list and initialize PKCS#11.
 *
 * @param  [in]   dev      Device object.
 * @param  [in]   library  Library name including path.
 * @param  [in]   heap     Heap hint.
 * @param  [out]  rvp      PKCS#11 return value. Last return value seen.
 *                         May be NULL.
 * @return  BAD_FUNC_ARG when dev or library are NULL pointers.
 * @return  BAD_PATH_ERROR when dynamic library cannot be opened.
 * @return  WC_INIT_E when the initialization PKCS#11 fails.
 * @return  WC_HW_E when unable to get PKCS#11 function list.
 * @return  0 on success.
 */
int wc_Pkcs11_Initialize_ex(Pkcs11Dev* dev, const char* library, void* heap,
                            CK_RV* rvp)
{
    int                  ret = 0;
    CK_RV                rv = CKR_OK;
#ifndef HAVE_PKCS11_STATIC
    void*                func;
#endif
    CK_C_INITIALIZE_ARGS args;

    if (dev == NULL || library == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        dev->heap = heap;
#ifndef HAVE_PKCS11_STATIC
        dev->dlHandle = dlopen(library, RTLD_NOW | RTLD_LOCAL);
        if (dev->dlHandle == NULL) {
            WOLFSSL_MSG(dlerror());
            ret = BAD_PATH_ERROR;
        }
    }

    if (ret == 0) {
        dev->func = NULL;
        func = dlsym(dev->dlHandle, "C_GetFunctionList");
        if (func == NULL) {
            WOLFSSL_MSG(dlerror());
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        rv = ((CK_C_GetFunctionList)func)(&dev->func);
#else
        rv = C_GetFunctionList(&dev->func);
#endif
        if (rv != CKR_OK) {
            PKCS11_RV("CK_C_GetFunctionList", ret);
            ret = WC_HW_E;
        }
    }

    if (ret == 0) {
        XMEMSET(&args, 0x00, sizeof(args));
        args.flags = CKF_OS_LOCKING_OK;
        rv = dev->func->C_Initialize(&args);
        if (rv != CKR_OK) {
            PKCS11_RV("C_Initialize", ret);
            ret = WC_INIT_E;
        }
    }

    if (rvp != NULL) {
        *rvp = rv;
    }

    if (ret != 0) {
        wc_Pkcs11_Finalize(dev);
    }

    return ret;
}

/**
 * Close the Pkcs#11 library.
 *
 * @param  [in]  dev  Device object.
 */
void wc_Pkcs11_Finalize(Pkcs11Dev* dev)
{
    if (dev != NULL
#ifndef HAVE_PKCS11_STATIC
        && dev->dlHandle != NULL
#endif
        ) {
        if (dev->func != NULL) {
            dev->func->C_Finalize(NULL);
            dev->func = NULL;
        }
#ifndef HAVE_PKCS11_STATIC
        dlclose(dev->dlHandle);
        dev->dlHandle = NULL;
#endif
    }
}

/* lookup by token name and return slotId or (-1) if not found */
static int Pkcs11Slot_FindByTokenName(Pkcs11Dev* dev,
    const char* tokenName, size_t tokenNameSz)
{
    CK_RV         rv;
    CK_ULONG      slotCnt = 0;
    CK_TOKEN_INFO tinfo;
    int           slotId = -1;
    rv = dev->func->C_GetSlotList(CK_TRUE, NULL, &slotCnt);
    if (rv == CKR_OK) {
        for (slotId = 0; slotId < (int)slotCnt; slotId++) {
            rv = dev->func->C_GetTokenInfo(slotId, &tinfo);
            PKCS11_RV("C_GetTokenInfo", rv);
            if (rv == CKR_OK &&
                XMEMCMP(tinfo.label, tokenName, tokenNameSz) == 0) {
                return slotId;
            }
        }
    }
    return -1;
}

/* lookup by slotId or tokenName */
static int Pkcs11Token_Init(Pkcs11Token* token, Pkcs11Dev* dev, int slotId,
    const char* tokenName, size_t tokenNameSz)
{
    int         ret = 0;
    CK_RV       rv;
    CK_SLOT_ID* slot = NULL;
    CK_ULONG    slotCnt = 0;

    if (token == NULL || dev == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        if (slotId < 0) {
            rv = dev->func->C_GetSlotList(CK_TRUE, NULL, &slotCnt);
            PKCS11_RV("C_GetSlotList", rv);
            if (rv != CKR_OK) {
                ret = WC_HW_E;
            }
            if (ret == 0) {
                slot = (CK_SLOT_ID*)XMALLOC(slotCnt * sizeof(*slot), dev->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
                if (slot == NULL)
                    ret = MEMORY_E;
            }
            if (ret == 0) {
                rv = dev->func->C_GetSlotList(CK_TRUE, slot, &slotCnt);
                PKCS11_RV("C_GetSlotList", rv);
                if (rv != CKR_OK) {
                    ret = WC_HW_E;
                }
            }
            if (ret == 0) {
                if (tokenName != NULL && tokenNameSz > 0) {
                    /* find based on token name */
                    slotId = Pkcs11Slot_FindByTokenName(dev,
                        tokenName, tokenNameSz);
                }
                else {
                    /* Use first available slot with a token. */
                    slotId = (int)slot[0];
                }
            }
        }
        else {
            /* verify slotId is valid */
            CK_SLOT_INFO sinfo;
            rv = dev->func->C_GetSlotInfo(slotId, &sinfo);
            PKCS11_RV("C_GetSlotInfo", rv);
            if (rv != CKR_OK) {
                ret = WC_INIT_E;
            }
        }
    }
    if (ret == 0) {
        token->func = dev->func;
        token->slotId = (CK_SLOT_ID)slotId;
        token->handle = NULL_PTR;
        token->userPin = NULL_PTR;
        token->userPinSz = 0;
        token->userPinLogin = 0;
    }

    if (slot != NULL) {
        XFREE(slot, dev->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}

/**
 * Set up a token for use. Lookup by slotId or tokenName. Set User PIN.
 *
 * @param  [in]  token      Token object.
 * @param  [in]  dev        PKCS#11 device object.
 * @param  [in]  slotId     Slot number of the token.<br>
 *                          Passing -1 uses the first available slot.
 * @param  [in]  tokenName  Name of token to initialize (optional)
 * @param  [in]  userPin    PIN to use to login as user.
 * @param  [in]  userPinSz  Number of bytes in PIN.
 * @return  BAD_FUNC_ARG when token, dev and/or tokenName is NULL.
 * @return  WC_INIT_E when initializing token fails.
 * @return  WC_HW_E when another PKCS#11 library call fails.
 * @return  0 on success.
 */
int wc_Pkcs11Token_Init(Pkcs11Token* token, Pkcs11Dev* dev, int slotId,
    const char* tokenName, const unsigned char* userPin, int userPinSz)
{
    int ret;
    size_t tokenNameSz = 0;

    if (tokenName != NULL) {
        tokenNameSz = XSTRLEN(tokenName);
    }
    ret = Pkcs11Token_Init(token, dev, slotId, tokenName, tokenNameSz);
    if (ret == 0) {
        token->userPin = (CK_UTF8CHAR_PTR)userPin;
        token->userPinSz = (CK_ULONG)userPinSz;
        token->userPinLogin = 1;
    }

    return ret;
}

/**
 * Set up a token for use. Lookup by slotId or tokenName.
 *
 * @param  [in]  token      Token object.
 * @param  [in]  dev        PKCS#11 device object.
 * @param  [in]  slotId     Slot number of the token.<br>
 *                          Passing -1 uses the first available slot.
 * @param  [in]  tokenName  Name of token to initialize (optional)
 * @return  BAD_FUNC_ARG when token, dev and/or tokenName is NULL.
 * @return  WC_INIT_E when initializing token fails.
 * @return  WC_HW_E when another PKCS#11 library call fails.
 * @return  0 on success.
 */
int wc_Pkcs11Token_Init_NoLogin(Pkcs11Token* token, Pkcs11Dev* dev, int slotId,
    const char* tokenName)
{
    size_t tokenNameSz = 0;
    if (tokenName != NULL) {
        tokenNameSz = XSTRLEN(tokenName);
    }
    return Pkcs11Token_Init(token, dev, slotId, tokenName, tokenNameSz);
}

/**
 * Set up a token for use. Lookup by slotId or tokenName/size. Set User PIN.
 *
 * @param  [in]  token       Token object.
 * @param  [in]  dev         PKCS#11 device object.
 * @param  [in]  tokenName   Name of token to initialize.
 * @param  [in]  tokenNameSz Name size for token
 * @param  [in]  userPin     PIN to use to login as user.
 * @param  [in]  userPinSz   Number of bytes in PIN.
 * @return  BAD_FUNC_ARG when token, dev and/or tokenName is NULL.
 * @return  WC_INIT_E when initializing token fails.
 * @return  WC_HW_E when another PKCS#11 library call fails.
 * @return  0 on success.
 */
int wc_Pkcs11Token_InitName(Pkcs11Token* token, Pkcs11Dev* dev,
    const char* tokenName, int tokenNameSz,
    const unsigned char* userPin, int userPinSz)
{
    int ret = Pkcs11Token_Init(token, dev, -1, tokenName, (size_t)tokenNameSz);
    if (ret == 0) {
        token->userPin = (CK_UTF8CHAR_PTR)userPin;
        token->userPinSz = (CK_ULONG)userPinSz;
        token->userPinLogin = 1;
    }

    return ret;
}

/**
 * Set up a token for use. Lookup by slotId or tokenName/size.
 *
 * @param  [in]  token       Token object.
 * @param  [in]  dev         PKCS#11 device object.
 * @param  [in]  tokenName   Name of token to initialize.
 * @param  [in]  tokenNameSz Name size for token
 * @param  [in]  userPin     PIN to use to login as user.
 * @param  [in]  userPinSz   Number of bytes in PIN.
 * @return  BAD_FUNC_ARG when token, dev and/or tokenName is NULL.
 * @return  WC_INIT_E when initializing token fails.
 * @return  WC_HW_E when another PKCS#11 library call fails.
 * @return  0 on success.
 */
int wc_Pkcs11Token_InitName_NoLogin(Pkcs11Token* token, Pkcs11Dev* dev,
    const char* tokenName, int tokenNameSz)
{
    return Pkcs11Token_Init(token, dev, -1, tokenName, (size_t)tokenNameSz);
}

/**
 * Finalize token.
 * Closes all sessions on token.
 *
 * @param  [in]  token  Token object.
 */
void wc_Pkcs11Token_Final(Pkcs11Token* token)
{
    if (token != NULL && token->func != NULL) {
        token->func->C_CloseAllSessions(token->slotId);
        token->handle = NULL_PTR;
        ForceZero(token->userPin, (word32)token->userPinSz);
    }
}

/**
 * Open a session on a token.
 *
 * @param  [in]  token      Token object.
 * @param  [in]  session    Session object.
 * @param  [in]  readWrite  Boolean indicating to open session for Read/Write.
 * @return  BAD_FUNC_ARG when token or session is NULL.
 * @return  WC_HW_E when opening the session fails.
 * @return  0 on success.
 */
static int Pkcs11OpenSession(Pkcs11Token* token, Pkcs11Session* session,
                             int readWrite)
{
    int   ret = 0;
    CK_RV rv;

    if (token == NULL || session == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        if (token->handle != NULL_PTR)
            session->handle = token->handle;
        else {
            /* Create a new session. */
            CK_FLAGS flags = CKF_SERIAL_SESSION;

            if (readWrite)
                flags |= CKF_RW_SESSION;

            rv = token->func->C_OpenSession(token->slotId, flags,
                                            (CK_VOID_PTR)NULL, (CK_NOTIFY)NULL,
                                            &session->handle);
            PKCS11_RV("C_OpenSession", rv);
            if (rv != CKR_OK) {
                ret = WC_HW_E;
            }
            if (ret == 0 && token->userPinLogin) {
                rv = token->func->C_Login(session->handle, CKU_USER,
                                              token->userPin, token->userPinSz);
                PKCS11_RV("C_Login", rv);
                if (rv != CKR_OK) {
                    ret = WC_HW_E;
                }
            }
        }
    }
    if (ret == 0) {
        session->func = token->func;
        session->slotId = token->slotId;
    }

    return ret;
}

/**
 * Close a session on a token.
 * Won't close a session created externally.
 *
 * @param  [in]  token    Token object.
 * @param  [in]  session  Session object.
 */
static void Pkcs11CloseSession(Pkcs11Token* token, Pkcs11Session* session)
{
    if (token != NULL && session != NULL && token->handle != session->handle) {
        if (token->userPin != NULL)
            session->func->C_Logout(session->handle);
        session->func->C_CloseSession(session->handle);
    }
}

/**
 * Open a session on the token to be used for all operations.
 *
 * @param  [in]  token      Token object.
 * @param  [in]  readWrite  Boolean indicating to open session for Read/Write.
 * @return  BAD_FUNC_ARG when token is NULL.
 * @return  WC_HW_E when opening the session fails.
 * @return  0 on success.
 */
int wc_Pkcs11Token_Open(Pkcs11Token* token, int readWrite)
{
    int ret = 0;
    Pkcs11Session session;

    if (token == NULL)
        ret = BAD_FUNC_ARG;

    if (ret == 0) {
        ret = Pkcs11OpenSession(token, &session, readWrite);
        token->handle = session.handle;
    }

    return ret;
}

/**
 * Close the token's session.
 * All object, like keys, will be destroyed.
 *
 * @param  [in]  token  Token object.
 */
void wc_Pkcs11Token_Close(Pkcs11Token* token)
{
    Pkcs11Session session;

    if (token != NULL) {
        session.func = token->func;
        session.handle = token->handle;
        token->handle = NULL_PTR;
        Pkcs11CloseSession(token, &session);
    }
}


#if (!defined(NO_AES) && (defined(HAVE_AESGCM) || defined(HAVE_AES_CBC))) || \
                                                               !defined(NO_HMAC)
/*
 * Create a secret key.
 *
 * @param  [out]  key       Handle to key object.
 * @param  [in]   session   Session object.
 * @param  [in]   keyType   Type of secret key to create.
 * @param  [in]   data      Data of the secret key.
 * @param  [in]   len       Length of data in bytes.
 * @param  [in]   id        Identifier to set against key.
 * @param  [in]   idLen     Length of identifier.
 * @param  [in]   label     Label to set against key.
 * @param  [in]   labelLen  Length of label.
 * @param  [in]   op        Operation to support with key.
 * @return   WC_HW_E when another PKCS#11 library call fails.
 * @return   0 on success.
 */
static int Pkcs11CreateSecretKey(CK_OBJECT_HANDLE* key, Pkcs11Session* session,
                                 CK_KEY_TYPE keyType, unsigned char* data,
                                 int len, unsigned char* id, int idLen,
                                 char* label, int labelLen, int op)
{
    int              ret = 0;
    CK_RV            rv;
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE     keyTemplateEncDec[] = {
        { CKA_CLASS,    &secretKeyClass, sizeof(secretKeyClass) },
        { CKA_KEY_TYPE, &keyType,        sizeof(keyType)        },
        { CKA_ENCRYPT,  &ckTrue,         sizeof(ckTrue)         },
        { CKA_DECRYPT,  &ckTrue,         sizeof(ckTrue)         },
        { CKA_VALUE,    NULL,            0                      },
        { 0,            NULL,            0                      },
        { 0,            NULL,            0                      }
    };
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE     keyTemplateSignVfy[] = {
        { CKA_CLASS,    &secretKeyClass, sizeof(secretKeyClass) },
        { CKA_KEY_TYPE, &keyType,        sizeof(keyType)        },
        { CKA_SIGN,     &ckTrue,         sizeof(ckTrue)         },
        { CKA_VERIFY,   &ckTrue,         sizeof(ckTrue)         },
        { CKA_VALUE,    NULL,            0                      },
        { 0,            NULL,            0                      },
        { 0,            NULL,            0                      }
    };
    CK_ATTRIBUTE*    keyTemplate = NULL;
    /* 5 mandatory entries + 2 optional. */
    int              keyTmplCnt = 5;

    WOLFSSL_MSG("PKCS#11: Create Secret Key");

    if (op == CKA_ENCRYPT || op == CKA_DECRYPT) {
        keyTemplate = keyTemplateEncDec;
    }
    else if (op == CKA_SIGN) {
        keyTemplate = keyTemplateSignVfy;
    }
    else {
        WOLFSSL_MSG("PKCS#11: Invalid operation type");
        ret = WC_HW_E;
    }
    if (ret == 0) {
        /* Set the secret to store. */
        keyTemplate[keyTmplCnt-1].pValue     = data;
        keyTemplate[keyTmplCnt-1].ulValueLen = (CK_ULONG)len;

        if (labelLen > 0) {
            keyTemplate[keyTmplCnt].type       = CKA_LABEL;
            keyTemplate[keyTmplCnt].pValue     = label;
            keyTemplate[keyTmplCnt].ulValueLen = labelLen;
            keyTmplCnt++;
        }
        if (idLen > 0) {
            keyTemplate[keyTmplCnt].type       = CKA_ID;
            keyTemplate[keyTmplCnt].pValue     = id;
            keyTemplate[keyTmplCnt].ulValueLen = idLen;
            keyTmplCnt++;
        }

        PKCS11_DUMP_TEMPLATE("Secret Key", keyTemplate, keyTmplCnt);
        /* Create an object containing key data for device to use. */
        rv = session->func->C_CreateObject(session->handle, keyTemplate,
                                           keyTmplCnt, key);
        PKCS11_RV("C_CreateObject", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    return ret;
}
#endif

#ifndef NO_RSA
/**
 * Create a PKCS#11 object containing the RSA private key data.
 *
 * @param  [out]  privateKey  Handle to private key object.
 * @param  [in]   session     Session object.
 * @param  [in]   rsaKey      RSA key with private key data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11CreateRsaPrivateKey(CK_OBJECT_HANDLE* privateKey,
                                     Pkcs11Session* session,
                                     RsaKey* rsaKey, int permanent)
{
    int             ret = 0;
    CK_RV           rv;
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE    keyTemplate[] = {
        { CKA_CLASS,            &privKeyClass, sizeof(privKeyClass) },
        { CKA_KEY_TYPE,         &rsaKeyType,   sizeof(rsaKeyType)   },
        { CKA_DECRYPT,          &ckTrue,       sizeof(ckTrue)       },
        { CKA_SIGN,             &ckTrue,       sizeof(ckTrue)       },
        { CKA_MODULUS,          NULL,          0                    },
        { CKA_PRIVATE_EXPONENT, NULL,          0                    },
        { CKA_PRIME_1,          NULL,          0                    },
        { CKA_PRIME_2,          NULL,          0                    },
        { CKA_EXPONENT_1,       NULL,          0                    },
        { CKA_EXPONENT_2,       NULL,          0                    },
        { CKA_COEFFICIENT,      NULL,          0                    },
        { CKA_PUBLIC_EXPONENT,  NULL,          0                    },
        { 0,                    NULL,          0                    },
        { 0,                    NULL,          0                    }
    };
    /* Mandatory entries + 2 optional. */
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate) - 2;

    /* Set the modulus and private key data. */
    keyTemplate[ 4].pValue     = rsaKey->n.raw.buf;
    keyTemplate[ 4].ulValueLen = rsaKey->n.raw.len;
    keyTemplate[ 5].pValue     = rsaKey->d.raw.buf;
    keyTemplate[ 5].ulValueLen = rsaKey->d.raw.len;
    keyTemplate[ 6].pValue     = rsaKey->p.raw.buf;
    keyTemplate[ 6].ulValueLen = rsaKey->p.raw.len;
    keyTemplate[ 7].pValue     = rsaKey->q.raw.buf;
    keyTemplate[ 7].ulValueLen = rsaKey->q.raw.len;
    keyTemplate[ 8].pValue     = rsaKey->dP.raw.buf;
    keyTemplate[ 8].ulValueLen = rsaKey->dP.raw.len;
    keyTemplate[ 9].pValue     = rsaKey->dQ.raw.buf;
    keyTemplate[ 9].ulValueLen = rsaKey->dQ.raw.len;
    keyTemplate[10].pValue     = rsaKey->u.raw.buf;
    keyTemplate[10].ulValueLen = rsaKey->u.raw.len;
    keyTemplate[11].pValue     = rsaKey->e.raw.buf;
    keyTemplate[11].ulValueLen = rsaKey->e.raw.len;

    if (permanent && rsaKey->labelLen > 0) {
        keyTemplate[keyTmplCnt].type       = CKA_LABEL;
        keyTemplate[keyTmplCnt].pValue     = rsaKey->label;
        keyTemplate[keyTmplCnt].ulValueLen = rsaKey->labelLen;
        keyTmplCnt++;
    }
    if (permanent && rsaKey->idLen > 0) {
        keyTemplate[keyTmplCnt].type       = CKA_ID;
        keyTemplate[keyTmplCnt].pValue     = rsaKey->id;
        keyTemplate[keyTmplCnt].ulValueLen = rsaKey->idLen;
        keyTmplCnt++;
    }

    PKCS11_DUMP_TEMPLATE("RSA Private Key", keyTemplate, keyTmplCnt);
    rv = session->func->C_CreateObject(session->handle, keyTemplate, keyTmplCnt,
                                                                    privateKey);
    PKCS11_RV("C_CreateObject", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }

    return ret;
}
#endif

#ifdef HAVE_ECC
/**
 * Set the ECC parameters into the template.
 *
 * @param  [in]  key   ECC key.
 * @param  [in]  tmpl  PKCS#11 template.
 * @param  [in]  idx   Index of template to put parameters into.
 * @return  NOT_COMPILED_IN when the EC parameters are not known.
 * @return  0 on success.
 */
static int Pkcs11EccSetParams(ecc_key* key, CK_ATTRIBUTE* tmpl, int idx)
{
    int ret = 0;

    if (key->dp != NULL && key->dp->oid != NULL) {
        unsigned char* derParams = tmpl[idx].pValue;
        /* ASN.1 encoding: OBJ + ecc parameters OID */
        tmpl[idx].ulValueLen = key->dp->oidSz + 2;
        derParams[0] = ASN_OBJECT_ID;
        derParams[1] = key->dp->oidSz;
        XMEMCPY(derParams + 2, key->dp->oid, key->dp->oidSz);
    }
    else
        ret = NOT_COMPILED_IN;

    return ret;
}

/**
 * Create a PKCS#11 object containing the ECC public key data.
 * Encode the public key as an OCTET_STRING of the encoded point.
 *
 * @param  [out]  publicKey    Handle to public key object.
 * @param  [in]   session      Session object.
 * @param  [in]   public_key   ECC public key.
 * @param  [in]   operation    Cryptographic operation key is to be used for.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11CreateEccPublicKey(CK_OBJECT_HANDLE* publicKey,
                                    Pkcs11Session* session,
                                    ecc_key* public_key,
                                    CK_ATTRIBUTE_TYPE operation)
{
    int             ret = 0;
    int             i;
    unsigned char*  ecPoint = NULL;
    word32          len;
    CK_RV           rv;
    CK_UTF8CHAR     params[MAX_EC_PARAM_LEN];
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE    keyTemplate[] = {
        { CKA_CLASS,     &pubKeyClass, sizeof(pubKeyClass) },
        { CKA_KEY_TYPE,  &ecKeyType,   sizeof(ecKeyType)   },
        { operation,     &ckTrue,      sizeof(ckTrue)      },
        { CKA_EC_PARAMS, params,       0                   },
        { CKA_EC_POINT,  NULL,         0                   },
        { 0,             NULL,         0                   },
        { 0,             NULL,         0                   }
    };
    /* Mandatory entries + 2 optional. */
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate) - 2;

    if (public_key->labelLen > 0) {
        keyTemplate[keyTmplCnt].type       = CKA_LABEL;
        keyTemplate[keyTmplCnt].pValue     = public_key->label;
        keyTemplate[keyTmplCnt].ulValueLen = public_key->labelLen;
        keyTmplCnt++;
    }
    if (public_key->idLen > 0) {
        keyTemplate[keyTmplCnt].type       = CKA_ID;
        keyTemplate[keyTmplCnt].pValue     = public_key->id;
        keyTemplate[keyTmplCnt].ulValueLen = public_key->idLen;
        keyTmplCnt++;
    }

    ret = Pkcs11EccSetParams(public_key, keyTemplate, 3);
    if (ret == 0) {
        /* ASN1 encoded: OCT + uncompressed point */
        len = 3 + 1 + 2 * public_key->dp->size;
        ecPoint = (unsigned char*)XMALLOC(len, public_key->heap,
                                                              DYNAMIC_TYPE_ECC);
        if (ecPoint == NULL)
            ret = MEMORY_E;
    }
    if (ret == 0) {
        len -= 3;
        i = 0;
        ecPoint[i++] = ASN_OCTET_STRING;
        if (len >= ASN_LONG_LENGTH)
            ecPoint[i++] = ASN_LONG_LENGTH | 1;
        ecPoint[i++] = len;
        if (public_key->type == 0)
            public_key->type = ECC_PUBLICKEY;
        PRIVATE_KEY_UNLOCK();
        ret = wc_ecc_export_x963(public_key, ecPoint + i, &len);
        PRIVATE_KEY_LOCK();
    }
    if (ret == 0) {
        keyTemplate[4].pValue     = ecPoint;
        keyTemplate[4].ulValueLen = len + i;

        PKCS11_DUMP_TEMPLATE("Ec Public Key", keyTemplate, keyTmplCnt);
        rv = session->func->C_CreateObject(session->handle, keyTemplate,
                                                         keyTmplCnt, publicKey);
        PKCS11_RV("C_CreateObject", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (ecPoint != NULL)
        XFREE(ecPoint, public_key->heap, DYNAMIC_TYPE_ECC);

    return ret;
}

/**
 * Create a PKCS#11 object containing the ECC private key data.
 *
 * @param  privateKey   [out]  Handle to private key object.
 * @param  session      [in]   Session object.
 * @param  private_key  [in]   ECC private key.
 * @param  operation    [in]   Cryptographic operation key is to be used for.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11CreateEccPrivateKey(CK_OBJECT_HANDLE* privateKey,
                                     Pkcs11Session* session,
                                     ecc_key* private_key,
                                     CK_ATTRIBUTE_TYPE operation)
{
    int             ret = 0;
    CK_RV           rv;
    CK_UTF8CHAR     params[MAX_EC_PARAM_LEN];
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE    keyTemplate[] = {
        { CKA_CLASS,     &privKeyClass, sizeof(privKeyClass) },
        { CKA_KEY_TYPE,  &ecKeyType,    sizeof(ecKeyType)    },
        { operation,     &ckTrue,       sizeof(ckTrue)       },
        { CKA_EC_PARAMS, params,        0                    },
        { CKA_VALUE,     NULL,          0                    },
        { 0,             NULL,          0                    },
        { 0,             NULL,          0                    }
    };
    /* Mandatory entries + 2 optional. */
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate) - 2;

    if (private_key->labelLen > 0) {
        keyTemplate[keyTmplCnt].type       = CKA_LABEL;
        keyTemplate[keyTmplCnt].pValue     = private_key->label;
        keyTemplate[keyTmplCnt].ulValueLen = private_key->labelLen;
        keyTmplCnt++;
    }
    if (private_key->idLen > 0) {
        keyTemplate[keyTmplCnt].type       = CKA_ID;
        keyTemplate[keyTmplCnt].pValue     = private_key->id;
        keyTemplate[keyTmplCnt].ulValueLen = private_key->idLen;
        keyTmplCnt++;
    }

    ret = Pkcs11EccSetParams(private_key, keyTemplate, 3);
    if (ret == 0) {
        keyTemplate[4].pValue     = wc_ecc_key_get_priv(private_key)->raw.buf;
        keyTemplate[4].ulValueLen = wc_ecc_key_get_priv(private_key)->raw.len;

        PKCS11_DUMP_TEMPLATE("Ec Private Key", keyTemplate, keyTmplCnt);
        rv = session->func->C_CreateObject(session->handle, keyTemplate,
                                                        keyTmplCnt, privateKey);
        PKCS11_RV("C_CreateObject", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    return ret;
}
#endif

#if !defined(NO_RSA) || defined(HAVE_ECC) || (!defined(NO_AES) && \
           (defined(HAVE_AESGCM) || defined(HAVE_AES_CBC))) || !defined(NO_HMAC)
/**
 * Check if mechanism is available in session on token.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  mech     Mechanism to look for.
 * @return  NOT_COMPILED_IN when mechanism not available.
 * @return  0 when mechanism is available.
 */
static int Pkcs11MechAvail(Pkcs11Session* session, CK_MECHANISM_TYPE mech)
{
    int               ret = 0;
    CK_RV             rv;
    CK_MECHANISM_INFO mechInfo;

    rv = session->func->C_GetMechanismInfo(session->slotId, mech, &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK) {
        ret = NOT_COMPILED_IN;
    }

    return ret;
}
#endif

#ifndef NO_HMAC
/**
 * Return the mechanism type and key type for the digest type when using HMAC.
 *
 * @param  [in]  macType   Digest type - e.g. WC_SHA256.
 * @param  [in]  mechType  Mechanism type - e.g. CKM_SHA256_HMAC.
 * @param  [in]  keyType   Key type - e.g. CKK_SHA256_HMAC.
 * @return  NOT_COMPILED_IN if the digest algorithm isn't recognised.
 * @return  0 otherwise.
 */
static int Pkcs11HmacTypes(int macType, int* mechType, int* keyType)
{
    int ret = 0;

    switch (macType)
    {
    #ifndef NO_MD5
        case WC_MD5:
            *mechType = CKM_MD5_HMAC;
            *keyType = CKK_MD5_HMAC;
            break;
    #endif
    #ifndef NO_SHA
        case WC_SHA:
            *mechType = CKM_SHA_1_HMAC;
            *keyType = CKK_SHA_1_HMAC;
            break;
    #endif
    #ifdef WOLFSSL_SHA224
        case WC_SHA224:
            *mechType = CKM_SHA224_HMAC;
            *keyType = CKK_SHA224_HMAC;
            break;
    #endif
    #ifndef NO_SHA256
        case WC_SHA256:
            *mechType = CKM_SHA256_HMAC;
            *keyType = CKK_SHA256_HMAC;
            break;
    #endif
    #ifdef WOLFSSL_SHA384
        case WC_SHA384:
            *mechType = CKM_SHA384_HMAC;
            *keyType = CKK_SHA384_HMAC;
            break;
    #endif
    #ifdef WOLFSSL_SHA512
        case WC_SHA512:
            *mechType = CKM_SHA512_HMAC;
            *keyType = CKK_SHA512_HMAC;
            break;
    #endif
        default:
            ret = NOT_COMPILED_IN;
            break;
    }

    return ret;
}
#endif

/**
 * Store the private key on the token in the session.
 *
 * @param  [in]  token  Token to store private key on.
 * @param  [in]  type   Key type.
 * @param  [in]  clear  Clear out the private data from software key.
 * @param  [in]  key    Key type specific object.
 * @return  NOT_COMPILED_IN when mechanism not available.
 * @return  0 on success.
 */
int wc_Pkcs11StoreKey(Pkcs11Token* token, int type, int clear, void* key)
{
    int               ret = 0;
    Pkcs11Session     session;
    CK_OBJECT_HANDLE  privKey = NULL_PTR;

    ret = Pkcs11OpenSession(token, &session, 1);
    if (ret == 0) {
        switch (type) {
    #if !defined(NO_AES) && defined(HAVE_AESGCM)
            case PKCS11_KEY_TYPE_AES_GCM: {
                Aes* aes = (Aes*)key;

                ret = Pkcs11MechAvail(&session, CKM_AES_GCM);
                if (ret == 0) {
                    ret = Pkcs11CreateSecretKey(&privKey, &session, CKK_AES,
                                                (unsigned char*)aes->devKey,
                                                aes->keylen,
                                                (unsigned char*)aes->id,
                                                aes->idLen, aes->label,
                                                aes->labelLen, CKA_ENCRYPT);
                }
                if (ret == 0 && clear)
                    ForceZero(aes->devKey, aes->keylen);
                break;
            }
    #endif
    #if !defined(NO_AES) && defined(HAVE_AES_CBC)
            case PKCS11_KEY_TYPE_AES_CBC: {
                Aes* aes = (Aes*)key;

                ret = Pkcs11MechAvail(&session, CKM_AES_CBC);
                if (ret == 0) {
                    ret = Pkcs11CreateSecretKey(&privKey, &session, CKK_AES,
                                                (unsigned char*)aes->devKey,
                                                aes->keylen,
                                                (unsigned char*)aes->id,
                                                aes->idLen, aes->label,
                                                aes->labelLen, CKA_ENCRYPT);
                }
                if (ret == 0 && clear)
                    ForceZero(aes->devKey, aes->keylen);
                break;
            }
    #endif
    #ifndef NO_HMAC
            case PKCS11_KEY_TYPE_HMAC: {
                Hmac* hmac = (Hmac*)key;
                int mechType;
                int keyType;

                ret = Pkcs11HmacTypes(hmac->macType, &mechType, &keyType);
                if (ret == NOT_COMPILED_IN)
                    break;

                if (ret == 0)
                    ret = Pkcs11MechAvail(&session, mechType);
                if (ret == 0) {
                    ret = Pkcs11CreateSecretKey(&privKey, &session, keyType,
                                                (unsigned char*)hmac->keyRaw,
                                                hmac->keyLen,
                                                (unsigned char*)hmac->id,
                                                hmac->idLen, hmac->label,
                                                hmac->labelLen, CKA_SIGN);
                    if (ret == WC_HW_E) {
                        ret = Pkcs11CreateSecretKey(&privKey, &session,
                                                   CKK_GENERIC_SECRET,
                                                   (unsigned char*)hmac->keyRaw,
                                                   hmac->keyLen,
                                                   (unsigned char*)hmac->id,
                                                   hmac->idLen, hmac->label,
                                                   hmac->labelLen, CKA_SIGN);
                    }
                }
                break;
            }
    #endif
    #ifndef NO_RSA
            case PKCS11_KEY_TYPE_RSA: {
                RsaKey* rsaKey = (RsaKey*)key;

                ret = Pkcs11MechAvail(&session, CKM_RSA_X_509);
                if (ret == 0)
                    ret = Pkcs11CreateRsaPrivateKey(&privKey, &session, rsaKey,
                                                    1);
                if (ret == 0 && clear) {
                    mp_forcezero(&rsaKey->u);
                    mp_forcezero(&rsaKey->dQ);
                    mp_forcezero(&rsaKey->dP);
                    mp_forcezero(&rsaKey->q);
                    mp_forcezero(&rsaKey->p);
                    mp_forcezero(&rsaKey->d);
                }
                break;
            }
    #endif
    #ifdef HAVE_ECC
            case PKCS11_KEY_TYPE_EC: {
                ecc_key* eccKey = (ecc_key*)key;
                int      ret2 = NOT_COMPILED_IN;

        #ifndef NO_PKCS11_ECDH
                if ((eccKey->flags & WC_ECC_FLAG_DEC_SIGN) == 0) {
                    /* Try ECDH mechanism first. */
                    ret = Pkcs11MechAvail(&session, CKM_ECDH1_DERIVE);
                    if (ret == 0) {
                        ret = Pkcs11CreateEccPrivateKey(&privKey, &session,
                                                            eccKey, CKA_DERIVE);
                    }
                }
         #endif
                if (ret == 0 || ret == NOT_COMPILED_IN) {
                    /* Try ECDSA mechanism next. */
                    ret2 = Pkcs11MechAvail(&session, CKM_ECDSA);
                    if (ret2 == 0) {
                        ret2 = Pkcs11CreateEccPrivateKey(&privKey, &session,
                                                              eccKey, CKA_SIGN);
                        if (ret2 == 0) {
                            CK_OBJECT_HANDLE pubKey = NULL_PTR;
                            /* Store public key for validation with cert. */
                            ret2 = Pkcs11CreateEccPublicKey(&pubKey, &session,
                                                            eccKey, CKA_VERIFY);
                        }
                    }
                    /* OK for this to fail if set for ECDH. */
                    if (ret == NOT_COMPILED_IN)
                        ret = ret2;
                }
                if (ret == 0 && clear)
                    mp_forcezero(wc_ecc_key_get_priv(eccKey));
                break;
            }
    #endif
            default:
                ret = NOT_COMPILED_IN;
                break;
        }

        Pkcs11CloseSession(token, &session);
    }

    (void)privKey;
    (void)clear;
    (void)key;

    return ret;
}

#if !defined(NO_RSA) || defined(HAVE_ECC) || (!defined(NO_AES) && \
           (defined(HAVE_AESGCM) || defined(HAVE_AES_CBC))) || !defined(NO_HMAC)

/**
 * Find the PKCS#11 object containing key data using template.
 *
 * @param  [out]  key          Handle to key object.
 * @param  [in]   session      Session object.
 * @param  [in]   keyTemplate  PKCS #11 template to use in search.
 * @param  [in]   keyTmplCnt   Count of entries in PKCS #11 template.
 * @param  [out]  count        Number of keys matching template.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11FindKeyByTemplate(CK_OBJECT_HANDLE* key,
                                   Pkcs11Session* session,
                                   CK_ATTRIBUTE *keyTemplate,
                                   CK_ULONG keyTmplCnt,
                                   CK_ULONG *count)
{
    int             ret = 0;
    CK_RV           rv;

    WOLFSSL_MSG("PKCS#11: Find Key By Template");

    PKCS11_DUMP_TEMPLATE("Find Key", keyTemplate, keyTmplCnt);
    rv = session->func->C_FindObjectsInit(session->handle, keyTemplate,
                                                                    keyTmplCnt);
    PKCS11_RV("C_FindObjectsInit", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    if (ret == 0) {
        rv = session->func->C_FindObjects(session->handle, key, 1, count);
        PKCS11_RV("C_FindObjects", rv);
        PKCS11_VAL("C_FindObjects Count", *count);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
        rv = session->func->C_FindObjectsFinal(session->handle);
        PKCS11_RV("C_FindObjectsFinal", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    return ret;
}

/**
 * Find the PKCS#11 object containing the private key data by label.
 *
 * @param  [out]  key       Handle to key object.
 * @param  [in]   keyClass  Public or private key class.
 * @param  [in]   keyType   Type of key.
 * @param  [in]   session   Session object.
 * @param  [in]   id        Identifier set against a key.
 * @param  [in]   idLen     Length of identifier.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11FindKeyByLabel(CK_OBJECT_HANDLE* key, CK_OBJECT_CLASS keyClass,
                                CK_KEY_TYPE keyType, Pkcs11Session* session,
                                char* label, int labelLen)
{
    int             ret = 0;
    CK_ULONG        count;
    CK_ATTRIBUTE    keyTemplate[] = {
        { CKA_CLASS,           &keyClass, sizeof(keyClass)   },
        { CKA_KEY_TYPE,        &keyType,  sizeof(keyType)    },
        { CKA_LABEL,           label,     (CK_ULONG)labelLen }
    };
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate);

    WOLFSSL_MSG("PKCS#11: Find Key By Label");

    ret = Pkcs11FindKeyByTemplate(key, session, keyTemplate, keyTmplCnt,
                                                                        &count);
    if (ret == 0 && count == 0)
        ret = WC_HW_E;

    return ret;
}

/**
 * Find the PKCS#11 object containing the private key data by ID.
 *
 * @param  [out]  key       Handle to key object.
 * @param  [in]   keyClass  Public or private key class.
 * @param  [in]   keyType   Type of key.
 * @param  [in]   session   Session object.
 * @param  [in]   id        Identifier set against a key.
 * @param  [in]   idLen     Length of identifier.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11FindKeyById(CK_OBJECT_HANDLE* key, CK_OBJECT_CLASS keyClass,
                             CK_KEY_TYPE keyType, Pkcs11Session* session,
                             byte* id, int idLen)
{
    int             ret = 0;
    CK_ULONG        count;
    CK_ATTRIBUTE    keyTemplate[] = {
#ifndef WC_PKCS11_FIND_WITH_ID_ONLY
        { CKA_CLASS,           &keyClass, sizeof(keyClass) },
        { CKA_KEY_TYPE,        &keyType,  sizeof(keyType)  },
#endif
        { CKA_ID,              id,        (CK_ULONG)idLen  }
    };
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate);

    WOLFSSL_MSG("PKCS#11: Find Key By Id");

    ret = Pkcs11FindKeyByTemplate(key, session, keyTemplate, keyTmplCnt,
                                                                        &count);
    if (ret == 0 && count == 0)
        ret = WC_HW_E;

    return ret;
}
#endif

#ifndef NO_RSA
/**
 * Find the PKCS#11 object containing the RSA public or private key data with
 * the modulus specified.
 *
 * @param  [out]  key       Handle to key object.
 * @param  [in]   keyClass  Public or private key class.
 * @param  [in]   session   Session object.
 * @param  [in]   rsaKey    RSA key with modulus to search on.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11FindRsaKey(CK_OBJECT_HANDLE* key, CK_OBJECT_CLASS keyClass,
                            Pkcs11Session* session, RsaKey* rsaKey)
{
    CK_ULONG        count;
    CK_ATTRIBUTE    keyTemplate[] = {
        { CKA_CLASS,           &keyClass,   sizeof(keyClass)   },
        { CKA_KEY_TYPE,        &rsaKeyType, sizeof(rsaKeyType) },
        { CKA_MODULUS,         NULL,        0                  },
    };
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate);

    /* Set the modulus. */
    keyTemplate[2].pValue     = rsaKey->n.raw.buf;
    keyTemplate[2].ulValueLen = rsaKey->n.raw.len;

    return Pkcs11FindKeyByTemplate(key, session, keyTemplate, keyTmplCnt,
                                                                        &count);
}

/**
 * Make a handle to a public RSA key.
 *
 * @param  [in]   session     Session object.
 * @param  [in]   rsaKey      RSA key with modulus to search on.
 * @param  [in]   sessionKey  Whether to create a session key.
 * @param  [out]  publicKey   Handle to public key object.
 */
static int Pkcs11RsaPublicKey(Pkcs11Session* session, RsaKey* rsaKey,
                              int sessionKey, CK_OBJECT_HANDLE* publicKey)
{
    int              ret = 0;
    CK_RV            rv;
    CK_ATTRIBUTE     keyTemplate[] = {
        { CKA_CLASS,           &pubKeyClass, sizeof(pubKeyClass) },
        { CKA_KEY_TYPE,        &rsaKeyType,  sizeof(rsaKeyType)  },
        { CKA_ENCRYPT,         &ckTrue,      sizeof(ckTrue)      },
        { CKA_MODULUS,         NULL,         0                   },
        { CKA_PUBLIC_EXPONENT, NULL,         0                   }
    };
    CK_ULONG        keyTmplCnt = sizeof(keyTemplate) / sizeof(*keyTemplate);

    if (sessionKey) {
        /* Set the modulus and public exponent data. */
        keyTemplate[3].pValue     = rsaKey->n.raw.buf;
        keyTemplate[3].ulValueLen = rsaKey->n.raw.len;
        keyTemplate[4].pValue     = rsaKey->e.raw.buf;
        keyTemplate[4].ulValueLen = rsaKey->e.raw.len;

        PKCS11_DUMP_TEMPLATE("RSA Public Key", keyTemplate, keyTmplCnt);
        /* Create an object containing public key data for device to use. */
        rv = session->func->C_CreateObject(session->handle, keyTemplate,
                                                    keyTmplCnt, publicKey);
        PKCS11_RV("C_CreateObject", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    else if (rsaKey->labelLen > 0) {
        ret = Pkcs11FindKeyByLabel(publicKey, CKO_PUBLIC_KEY, CKK_RSA,
                                   session, rsaKey->label, rsaKey->labelLen);
    }
    else {
        ret = Pkcs11FindKeyById(publicKey, CKO_PUBLIC_KEY, CKK_RSA,
                                session, rsaKey->id, rsaKey->idLen);
    }

    return ret;
}

/**
 * Get the RSA public key data from the PKCS#11 object.
 *
 * @param  [in]  key      RSA key to put the data into.
 * @param  [in]  session  Session object.
 * @param  [in]  pubkey   Public key object.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11GetRsaPublicKey(RsaKey* key, Pkcs11Session* session,
                                 CK_OBJECT_HANDLE pubKey)
{
    int            ret = 0;
    unsigned char* mod = NULL;
    unsigned char* exp = NULL;
    int            modSz, expSz;
    CK_ATTRIBUTE   tmpl[] = {
        { CKA_MODULUS,         NULL_PTR, 0 },
        { CKA_PUBLIC_EXPONENT, NULL_PTR, 0 }
    };
    CK_ULONG       tmplCnt = sizeof(tmpl) / sizeof(*tmpl);
    CK_RV rv;

    PKCS11_DUMP_TEMPLATE("Get RSA Public Key Length", tmpl, tmplCnt);
    rv = session->func->C_GetAttributeValue(session->handle, pubKey, tmpl,
                                                                       tmplCnt);
    PKCS11_RV("C_GetAttributeValue", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    PKCS11_DUMP_TEMPLATE("RSA Public Key Length", tmpl, tmplCnt);

    if (ret == 0) {
        modSz = (int)tmpl[0].ulValueLen;
        expSz = (int)tmpl[1].ulValueLen;
        mod = (unsigned char*)XMALLOC(modSz, key->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (mod == NULL)
            ret = MEMORY_E;
    }
    if (ret == 0) {
        exp = (unsigned char*)XMALLOC(expSz, key->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (exp == NULL)
            ret = MEMORY_E;
    }
    if (ret == 0) {
        tmpl[0].pValue = mod;
        tmpl[1].pValue = exp;

        PKCS11_DUMP_TEMPLATE("Get RSA Public Key", tmpl, tmplCnt);
        rv = session->func->C_GetAttributeValue(session->handle, pubKey,
                                                                 tmpl, tmplCnt);
        PKCS11_RV("C_GetAttributeValue", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
        PKCS11_DUMP_TEMPLATE("RSA Public Key", tmpl, tmplCnt);
    }
    if (ret == 0)
        ret = wc_RsaPublicKeyDecodeRaw(mod, modSz, exp, expSz, key);

    if (exp != NULL)
        XFREE(exp, key->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (mod != NULL)
        XFREE(mod, key->heap, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}

/**
 * Get the RSA modulus size in bytes from the PKCS#11 object.
 *
 * @param  [in]   session  Session object.
 * @param  [in]   pubkey   Public key object.
 * @param  [out]  modSize  Size of the modulus in bytes.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11GetRsaModulusSize(Pkcs11Session* session,
                                   CK_OBJECT_HANDLE pubKey, int* modSize)
{
    int            ret = 0;
    CK_ATTRIBUTE   tmpl[] = {
        { CKA_MODULUS,         NULL_PTR, 0 }
    };
    CK_ULONG       tmplCnt = sizeof(tmpl) / sizeof(*tmpl);
    CK_RV rv;

    PKCS11_DUMP_TEMPLATE("Get RSA Modulus Length", tmpl, tmplCnt);
    rv = session->func->C_GetAttributeValue(session->handle, pubKey, tmpl,
                                                                       tmplCnt);
    PKCS11_RV("C_GetAttributeValue", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    PKCS11_DUMP_TEMPLATE("RSA Modulus Length", tmpl, tmplCnt);

    if (ret == 0) {
        *modSize = (int)tmpl[0].ulValueLen;
    }

    return ret;
}

/**
 * Make a handle to a private RSA key.
 *
 * @param  [in]   session     Session object.
 * @param  [in]   rsaKey      RSA key with modulus to search on.
 * @param  [in]   sessionKey  Whether to create a session key.
 * @param  [out]  publicKey   Handle to private key object.
 */
static int Pkcs11RsaPrivateKey(Pkcs11Session* session, RsaKey* rsaKey,
                               int sessionKey, CK_OBJECT_HANDLE* privateKey)
{
    int     ret;

    if (sessionKey) {
        ret = Pkcs11CreateRsaPrivateKey(privateKey, session, rsaKey, 0);
    }
    else if (rsaKey->labelLen > 0) {
        ret = Pkcs11FindKeyByLabel(privateKey, CKO_PRIVATE_KEY, CKK_RSA,
                                   session, rsaKey->label, rsaKey->labelLen);
    }
    else if (rsaKey->idLen > 0) {
        ret = Pkcs11FindKeyById(privateKey, CKO_PRIVATE_KEY, CKK_RSA, session,
                                rsaKey->id, rsaKey->idLen);
    }
    else {
        ret = Pkcs11FindRsaKey(privateKey, CKO_PRIVATE_KEY, session, rsaKey);
    }

    if ((ret == 0) && (!sessionKey)) {
        ret = Pkcs11GetRsaPublicKey(rsaKey, session, *privateKey);
    }

    return ret;
}

/**
 * Exponentiate the input with the public part of the RSA key.
 * Used in public encrypt and decrypt.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 *          0 on success.
 */
static int Pkcs11RsaEncrypt(Pkcs11Session* session, wc_CryptoInfo* info,
                            CK_OBJECT_HANDLE key)
{
    int              ret = 0;
    CK_RV            rv;
    CK_MECHANISM     mech;
    CK_ULONG         outLen;

    WOLFSSL_MSG("PKCS#11: RSA Public Key Operation");

    if (info->pk.rsa.outLen == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        /* Raw RSA encrypt/decrypt operation. */
        mech.mechanism      = CKM_RSA_X_509;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        rv = session->func->C_EncryptInit(session->handle, &mech, key);
        PKCS11_RV("C_EncryptInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        outLen = (CK_ULONG)*info->pk.rsa.outLen;
        rv = session->func->C_Encrypt(session->handle,
                (CK_BYTE_PTR)info->pk.rsa.in, info->pk.rsa.inLen,
                info->pk.rsa.out, &outLen);
        PKCS11_RV("C_Encrypt", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        *info->pk.rsa.outLen = (word32)outLen;
    }

    return ret;
}

/**
 * Exponentiate the input with the private part of the RSA key.
 * Used in private encrypt and decrypt.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @param  [in]  func     Function to perform - decrypt or sign.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11RsaDecrypt(Pkcs11Session* session, wc_CryptoInfo* info,
                            CK_OBJECT_HANDLE key)
{
    int              ret = 0;
    CK_RV            rv;
    CK_MECHANISM     mech;
    CK_ULONG         outLen;

    WOLFSSL_MSG("PKCS#11: RSA Private Key Operation");

    if (info->pk.rsa.outLen == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        /* Raw RSA encrypt/decrypt operation. */
        mech.mechanism      = CKM_RSA_X_509;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        rv = session->func->C_DecryptInit(session->handle, &mech, key);
        PKCS11_RV("C_DecryptInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        PKCS11_VAL("C_Decrypt inLen", info->pk.rsa.inLen);
        PKCS11_VAL("C_Decrypt outLen", *info->pk.rsa.outLen);
        outLen = (CK_ULONG)*info->pk.rsa.outLen;
        rv = session->func->C_Decrypt(session->handle,
                (CK_BYTE_PTR)info->pk.rsa.in, info->pk.rsa.inLen,
                info->pk.rsa.out, &outLen);
        PKCS11_RV("C_Decrypt", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        *info->pk.rsa.outLen = (word32)outLen;
    }

    return ret;
}

/**
 * Exponentiate the input with the private part of the RSA key.
 * Used in private encrypt and decrypt.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @param  [in]  func     Function to perform - decrypt or sign.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11RsaSign(Pkcs11Session* session, wc_CryptoInfo* info,
                         CK_OBJECT_HANDLE key)
{
    int              ret = 0;
    CK_RV            rv;
    CK_MECHANISM     mech;
    CK_ULONG         outLen;

    WOLFSSL_MSG("PKCS#11: RSA Private Key Operation");

    if (info->pk.rsa.outLen == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        /* Raw RSA encrypt/decrypt operation. */
        mech.mechanism      = CKM_RSA_X_509;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        rv = session->func->C_SignInit(session->handle, &mech, key);
        PKCS11_RV("C_SignInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        PKCS11_VAL("C_Sign inLen", info->pk.rsa.inLen);
        PKCS11_VAL("C_Sign outLen", *info->pk.rsa.outLen);
        outLen = (CK_ULONG)*info->pk.rsa.outLen;
        rv = session->func->C_Sign(session->handle,
                (CK_BYTE_PTR)info->pk.rsa.in, info->pk.rsa.inLen,
                info->pk.rsa.out, &outLen);
        PKCS11_RV("C_Sign", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0)
        *info->pk.rsa.outLen = (word32)outLen;


    return ret;
}

/**
 * Perform an RSA operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11Rsa(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int               ret = 0;
    CK_RV             rv;
    CK_MECHANISM_INFO mechInfo;
    int               sessionKey = 0;
    CK_OBJECT_HANDLE  key;
    RsaKey*           rsaKey = info->pk.rsa.key;
    int               type = info->pk.rsa.type;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_RSA_X_509,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK) {
        ret = NOT_COMPILED_IN;
    }

    if (ret == 0) {
        if ((type == RSA_PUBLIC_ENCRYPT) || (type == RSA_PUBLIC_DECRYPT)) {
            sessionKey = !mp_iszero(&rsaKey->e);

            /* Make a handle to a public key. */
            ret = Pkcs11RsaPublicKey(session, rsaKey, sessionKey, &key);
        }
        else {
            sessionKey = !mp_iszero(&rsaKey->d);

            /* Make a handle to a private key. */
            ret = Pkcs11RsaPrivateKey(session, rsaKey, sessionKey, &key);
        }

        if (type == RSA_PUBLIC_ENCRYPT) {
            WOLFSSL_MSG("PKCS#11: Public Encrypt");
            if ((mechInfo.flags & CKF_ENCRYPT) != 0) {
                ret = Pkcs11RsaEncrypt(session, info, key);
            }
            else {
                ret = NOT_COMPILED_IN;
            }
        }
        else if (type == RSA_PUBLIC_DECRYPT) {
            WOLFSSL_MSG("PKCS#11: Public Decrypt");
            if ((mechInfo.flags & CKF_DECRYPT) != 0) {
                ret = Pkcs11RsaEncrypt(session, info, key);
            }
            else {
                ret = NOT_COMPILED_IN;
            }
        }
        else if (type == RSA_PRIVATE_DECRYPT) {
            WOLFSSL_MSG("PKCS#11: Private Decrypt");
            if ((mechInfo.flags & CKF_DECRYPT) != 0) {
                ret = Pkcs11RsaDecrypt(session, info, key);
            }
            else {
                ret = Pkcs11RsaSign(session, info, key);
            }
        }
        else if (type == RSA_PRIVATE_ENCRYPT) {
            WOLFSSL_MSG("PKCS#11: Private Encrypt");
            if ((mechInfo.flags & CKF_SIGN) != 0) {
                ret = Pkcs11RsaSign(session, info, key);
            }
            else {
                ret = Pkcs11RsaDecrypt(session, info, key);
            }
        }
    }

    if (sessionKey) {
        session->func->C_DestroyObject(session->handle, key);
    }

    return ret;
}

#ifdef WOLFSSL_KEY_GEN
/**
 * Perform an RSA key generation operation.
 * The private key data stays on the device.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11RsaKeyGen(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int               ret = 0;
    RsaKey*           key = info->pk.rsakg.key;
    CK_RV             rv;
    CK_ULONG          bits = info->pk.rsakg.size;
    CK_OBJECT_HANDLE  pubKey = NULL_PTR, privKey = NULL_PTR;
    CK_MECHANISM      mech;
    static CK_BYTE    pub_exp[] = { 0x01, 0x00, 0x01, 0x00 };
    CK_ATTRIBUTE      pubKeyTmpl[] = {
        { CKA_MODULUS_BITS,    &bits,    sizeof(bits)    },
        { CKA_ENCRYPT,         &ckTrue,  sizeof(ckTrue)  },
        { CKA_VERIFY,          &ckTrue,  sizeof(ckTrue)  },
        { CKA_PUBLIC_EXPONENT, &pub_exp, sizeof(pub_exp) }
    };
    CK_ULONG          pubTmplCnt = sizeof(pubKeyTmpl)/sizeof(*pubKeyTmpl);
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE      privKeyTmpl[] = {
        { CKA_DECRYPT,  &ckTrue, sizeof(ckTrue) },
        { CKA_SIGN,     &ckTrue, sizeof(ckTrue) },
        { 0,            NULL,    0              },
        { 0,            NULL,    0              }
    };
    /* 2 mandatory entries + 2 optional. */
    int               privTmplCnt = 2;
    int               i;

    ret = Pkcs11MechAvail(session, CKM_RSA_PKCS_KEY_PAIR_GEN);
    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: RSA Key Generation Operation");

        /* Most commonly used public exponent value (array initialized). */
        if (info->pk.rsakg.e != WC_RSA_EXPONENT) {
            for (i = 0; i < (int)sizeof(pub_exp); i++)
                pub_exp[i] = (info->pk.rsakg.e >> (8 * i)) & 0xff;
        }
        for (i = (int)sizeof(pub_exp) - 1; pub_exp[i] == 0; i--) {
        }
        pubKeyTmpl[3].ulValueLen = i + 1;

        if (key->labelLen != 0) {
            privKeyTmpl[privTmplCnt].type       = CKA_LABEL;
            privKeyTmpl[privTmplCnt].pValue     = key->label;
            privKeyTmpl[privTmplCnt].ulValueLen = key->labelLen;
            privTmplCnt++;
        }
        if (key->idLen != 0) {
            privKeyTmpl[privTmplCnt].type       = CKA_ID;
            privKeyTmpl[privTmplCnt].pValue     = key->id;
            privKeyTmpl[privTmplCnt].ulValueLen = key->idLen;
            privTmplCnt++;
        }

        mech.mechanism      = CKM_RSA_PKCS_KEY_PAIR_GEN;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        PKCS11_DUMP_TEMPLATE("Public Key", pubKeyTmpl, pubTmplCnt);
        PKCS11_DUMP_TEMPLATE("Private Key", privKeyTmpl, privTmplCnt);
        rv = session->func->C_GenerateKeyPair(session->handle, &mech,
                                                       pubKeyTmpl, pubTmplCnt,
                                                       privKeyTmpl, privTmplCnt,
                                                       &pubKey, &privKey);
        PKCS11_RV("C_GenerateKeyPair", rv);
        if (rv != CKR_OK) {
            ret = -1;
        }
    }

    if (ret == 0)
        ret = Pkcs11GetRsaPublicKey(key, session, pubKey);

    if (pubKey != NULL_PTR)
        ret = (int)session->func->C_DestroyObject(session->handle, pubKey);
    if (ret != 0 && privKey != NULL_PTR)
        ret = (int)session->func->C_DestroyObject(session->handle, privKey);

    return ret;
}
#endif /* WOLFSSL_KEY_GEN */
#endif /* !NO_RSA */

#ifdef HAVE_ECC
/**
 * Find the PKCS#11 object containing the ECC public or private key data.
 * Search for public key by public point.
 *
 * @param  [out]  key       Handle to key object.
 * @param  [in]   keyClass  Public or private key class.
 * @param  [in]   session   Session object.
 * @param  [in]   eccKey    ECC key with parameters.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11FindEccKey(CK_OBJECT_HANDLE* key, CK_OBJECT_CLASS keyClass,
                            Pkcs11Session* session, ecc_key* eccKey,
                            int op)
{
    int             ret = 0;
    int             i;
    unsigned char*  ecPoint = NULL;
    word32          len = 0;
    CK_RV           rv;
    CK_ULONG        count;
    CK_UTF8CHAR     params[MAX_EC_PARAM_LEN];
    CK_ATTRIBUTE    keyTemplate[] = {
        { CKA_CLASS,           &keyClass,  sizeof(keyClass)  },
        { CKA_KEY_TYPE,        &ecKeyType, sizeof(ecKeyType) },
        { CKA_EC_PARAMS,       params,     0                 },
        { op,                  &ckTrue,    sizeof(ckTrue)    },
        { CKA_EC_POINT,        NULL,       0                 },
    };
    CK_ULONG        attrCnt = 4;

    ret = Pkcs11EccSetParams(eccKey, keyTemplate, 2);
    if (ret == 0 && keyClass == CKO_PUBLIC_KEY) {
        /* ASN1 encoded: OCT + uncompressed point */
        len = 3 + 1 + 2 * eccKey->dp->size;
        ecPoint = (unsigned char*)XMALLOC(len, eccKey->heap, DYNAMIC_TYPE_ECC);
        if (ecPoint == NULL)
            ret = MEMORY_E;
    }
    if (ret == 0 && keyClass == CKO_PUBLIC_KEY) {
        len -= 3;
        i = 0;
        ecPoint[i++] = ASN_OCTET_STRING;
        if (len >= ASN_LONG_LENGTH)
            ecPoint[i++] = (ASN_LONG_LENGTH | 1);
        ecPoint[i++] = len;
        if (eccKey->type == 0)
            eccKey->type = ECC_PUBLICKEY;
        PRIVATE_KEY_UNLOCK();
        ret = wc_ecc_export_x963(eccKey, ecPoint + i, &len);
        PRIVATE_KEY_LOCK();
    }
    if (ret == 0 && keyClass == CKO_PUBLIC_KEY) {
        keyTemplate[attrCnt].pValue     = ecPoint;
        keyTemplate[attrCnt].ulValueLen = len + i;
        attrCnt++;
    }
    if (ret == 0) {
        PKCS11_DUMP_TEMPLATE("Find Ec Key", keyTemplate, attrCnt);
        rv = session->func->C_FindObjectsInit(session->handle, keyTemplate,
                                                                       attrCnt);
        PKCS11_RV("C_FindObjectsInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        rv = session->func->C_FindObjects(session->handle, key, 1, &count);
        PKCS11_RV("C_FindObjects", rv);
        PKCS11_VAL("C_FindObjects Count", count);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
        rv = session->func->C_FindObjectsFinal(session->handle);
        PKCS11_RV("C_FindObjectsFinal", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (ecPoint != NULL)
        XFREE(ecPoint, eccKey->heap, DYNAMIC_TYPE_ECC);

    return ret;
}

/**
 * Gets the public key data from the PKCS#11 object and puts into the ECC key.
 *
 * @param  [in]  key      ECC public key.
 * @param  [in]  session  Session object.
 * @param  [in]  pubKey   ECC public key PKCS#11 object.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11GetEccPublicKey(ecc_key* key, Pkcs11Session* session,
                                 CK_OBJECT_HANDLE pubKey)
{
    int            ret = 0;
    word32         i = 0;
    int            curveIdx;
    unsigned char* point = NULL;
    int            pointSz = 0;
    byte           tag;
    CK_RV          rv;
    CK_ATTRIBUTE   tmpl[] = {
        { CKA_EC_POINT,  NULL_PTR, 0 },
    };
    CK_ULONG       tmplCnt = sizeof(tmpl) / sizeof(*tmpl);

    rv = session->func->C_GetAttributeValue(session->handle, pubKey, tmpl,
                                                                       tmplCnt);
    PKCS11_RV("C_GetAttributeValue", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }

    if (ret == 0) {
        pointSz = (int)tmpl[0].ulValueLen;
        point = (unsigned char*)XMALLOC(pointSz, key->heap, DYNAMIC_TYPE_ECC);
        if (point == NULL)
            ret = MEMORY_E;
    }
    if (ret == 0) {
        tmpl[0].pValue = point;

        PKCS11_DUMP_TEMPLATE("Get Ec Public Key", tmpl, tmplCnt);
        rv = session->func->C_GetAttributeValue(session->handle, pubKey,
                                                                 tmpl, tmplCnt);
        PKCS11_RV("C_GetAttributeValue", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
        PKCS11_DUMP_TEMPLATE("Ec Public Key", tmpl, tmplCnt);
    }

    /* Make sure the data is big enough for ASN.1: OCT + uncompressed point */
    if (ret == 0 && pointSz < key->dp->size * 2 + 1 + 2)
        ret = ASN_PARSE_E;
    /* Step over the OCTET_STRING wrapper. */
    if (ret == 0 && GetASNTag(point, &i, &tag, pointSz) != 0)
        ret = ASN_PARSE_E;
    if (ret == 0 && tag != ASN_OCTET_STRING)
        ret = ASN_PARSE_E;
    if (ret == 0 && point[i] >= ASN_LONG_LENGTH) {
        if (point[i++] != (ASN_LONG_LENGTH | 1))
            ret = ASN_PARSE_E;
        else if (pointSz < key->dp->size * 2 + 1 + 3)
            ret = ASN_PARSE_E;
    }
    if (ret == 0 && point[i++] != key->dp->size * 2 + 1)
        ret = ASN_PARSE_E;

    if (ret == 0) {
        curveIdx = wc_ecc_get_curve_idx(key->dp->id);
        ret = wc_ecc_import_point_der(point + i, pointSz - i, curveIdx,
                                                                  &key->pubkey);
    }
    /* make sure the ecc_key type has been set */
    if (ret == 0 && key->type == 0) {
        key->type = ECC_PUBLICKEY;
    }

    if (point != NULL)
        XFREE(point, key->heap, DYNAMIC_TYPE_ECC);

    return ret;
}

#ifndef NO_PKCS11_EC_KEYGEN
/**
 * Perform an ECC key generation operation.
 * The private key data stays on the device.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11EcKeyGen(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int               ret = 0;
    ecc_key*          key = info->pk.eckg.key;
    CK_RV             rv;
    CK_OBJECT_HANDLE  pubKey = NULL_PTR, privKey = NULL_PTR;
    CK_MECHANISM      mech;
    CK_UTF8CHAR       params[MAX_EC_PARAM_LEN];
    CK_ATTRIBUTE      pubKeyTmpl[] = {
        { CKA_EC_PARAMS,       params,   0               },
        { CKA_VERIFY,          &ckTrue,  sizeof(ckTrue)  },
        { CKA_ENCRYPT,         &ckTrue,  sizeof(ckTrue)  },
    };
    int               pubTmplCnt = 1;
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE      privKeyTmplDerive[] = {
        { CKA_DERIVE,  &ckTrue, sizeof(ckTrue) },
        { 0,           NULL,    0              },
        { 0,           NULL,    0              },
    };
    /* Empty entries for optional label/ID. */
    CK_ATTRIBUTE      privKeyTmplEncSign[] = {
        { CKA_SIGN,    &ckTrue, sizeof(ckTrue) },
        { CKA_DECRYPT, &ckTrue, sizeof(ckTrue) },
        { 0,           NULL,    0              },
        { 0,           NULL,    0              },
    };
    CK_ATTRIBUTE*     privKeyTmpl = privKeyTmplDerive;
    /* Mandatory entries + 2 optional. */
    int               privTmplCnt = 1;

    ret = Pkcs11MechAvail(session, CKM_EC_KEY_PAIR_GEN);
    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: EC Key Generation Operation");

        ret = Pkcs11EccSetParams(key, pubKeyTmpl, 0);
    }
    if (ret == 0) {
        /* Default is to use for derivation. */
        if ((key->flags & WC_ECC_FLAG_DEC_SIGN) == WC_ECC_FLAG_DEC_SIGN) {
            privKeyTmpl = privKeyTmplEncSign;
            privTmplCnt = 2;
            pubTmplCnt = 2;
        }
        if (key->labelLen != 0) {
            privKeyTmpl[privTmplCnt].type       = CKA_LABEL;
            privKeyTmpl[privTmplCnt].pValue     = key->label;
            privKeyTmpl[privTmplCnt].ulValueLen = key->labelLen;
            privTmplCnt++;
        }
        if (key->idLen != 0) {
            privKeyTmpl[privTmplCnt].type       = CKA_ID;
            privKeyTmpl[privTmplCnt].pValue     = key->id;
            privKeyTmpl[privTmplCnt].ulValueLen = key->idLen;
            privTmplCnt++;
        }

        mech.mechanism      = CKM_EC_KEY_PAIR_GEN;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        PKCS11_DUMP_TEMPLATE("Ec Private", privKeyTmpl, privTmplCnt);
        PKCS11_DUMP_TEMPLATE("Ec Public", pubKeyTmpl, pubTmplCnt);

        rv = session->func->C_GenerateKeyPair(session->handle, &mech,
                                                       pubKeyTmpl, pubTmplCnt,
                                                       privKeyTmpl, privTmplCnt,
                                                       &pubKey, &privKey);
        PKCS11_RV("C_GenerateKeyPair", rv);
        if (rv != CKR_OK) {
            ret = -1;
        }
    }

    if (ret == 0)
        ret = Pkcs11GetEccPublicKey(key, session, pubKey);

    if (pubKey != NULL_PTR)
        session->func->C_DestroyObject(session->handle, pubKey);
    if (ret != 0 && privKey != NULL_PTR)
        session->func->C_DestroyObject(session->handle, privKey);

    return ret;
}
#endif

#ifndef NO_PKCS11_ECDH
/**
 * Extracts the secret key data from the PKCS#11 object.
 *
 * @param  [in]      session  Session object.
 * @param  [in]      secret   PKCS#11 object with the secret key data.
 * @param  [in]      out      Buffer to hold secret data.
 * @param  [in,out]  outLen   On in, length of buffer.
 *                            On out, the length of data in buffer.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11ExtractSecret(Pkcs11Session* session, CK_OBJECT_HANDLE secret,
    byte* out, word32* outLen)
{
    int ret = 0;
    CK_ATTRIBUTE tmpl[] = {
      {CKA_VALUE, NULL_PTR, 0}
    };
    CK_ULONG     tmplCnt = sizeof(tmpl) / sizeof(*tmpl);
    CK_RV rv;

    PKCS11_DUMP_TEMPLATE("Get Secret Length", tmpl, tmplCnt);
    rv = session->func->C_GetAttributeValue(session->handle, secret, tmpl,
                                                                       tmplCnt);
    PKCS11_RV("C_GetAttributeValue", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    PKCS11_DUMP_TEMPLATE("Secret Length", tmpl, tmplCnt);
    if (ret == 0) {
        if (tmpl[0].ulValueLen > *outLen)
            ret = BUFFER_E;
    }
    if (ret == 0) {
        tmpl[0].pValue = out;
        PKCS11_DUMP_TEMPLATE("Get Secret", tmpl, tmplCnt);
        rv = session->func->C_GetAttributeValue(session->handle, secret,
                                                                 tmpl, tmplCnt);
        PKCS11_RV("C_GetAttributeValue", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
        PKCS11_DUMP_TEMPLATE("Secret", tmpl, tmplCnt);
        *outLen = (word32)tmpl[0].ulValueLen;
    }

    return ret;
}

/**
 * Performs the ECDH secret generation operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 *          0 on success.
 */
static int Pkcs11ECDH(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                    ret = 0;
    int                    sessionKey = 0;
    unsigned char*         point = NULL;
    word32                 pointLen;
    CK_RV                  rv;
    CK_KEY_TYPE            keyType = CKK_GENERIC_SECRET;
    CK_MECHANISM           mech;
    CK_ECDH1_DERIVE_PARAMS params;
    CK_OBJECT_HANDLE       privateKey = NULL_PTR;
    CK_OBJECT_HANDLE       secret = CK_INVALID_HANDLE;
    CK_ULONG               secSz;
    CK_ATTRIBUTE           tmpl[] = {
        { CKA_CLASS,       &secretKeyClass, sizeof(secretKeyClass) },
        { CKA_KEY_TYPE,    &keyType,        sizeof(keyType)        },
        { CKA_PRIVATE,     &ckFalse,        sizeof(ckFalse)        },
        { CKA_SENSITIVE,   &ckFalse,        sizeof(ckFalse)        },
        { CKA_EXTRACTABLE, &ckTrue,         sizeof(ckTrue)         },
        { CKA_VALUE_LEN,   &secSz,          sizeof(secSz)          }
    };
    CK_ULONG               tmplCnt = sizeof(tmpl) / sizeof(*tmpl);

    ret = Pkcs11MechAvail(session, CKM_ECDH1_DERIVE);
    if (ret == 0 && info->pk.ecdh.outlen == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: EC Key Derivation Operation");

        if ((sessionKey = !mp_iszero(
                wc_ecc_key_get_priv(info->pk.ecdh.private_key))))
            ret = Pkcs11CreateEccPrivateKey(&privateKey, session,
                                         info->pk.ecdh.private_key, CKA_DERIVE);
        else if (info->pk.ecdh.private_key->labelLen > 0) {
            ret = Pkcs11FindKeyByLabel(&privateKey, CKO_PRIVATE_KEY, CKK_EC,
                                       session,
                                       info->pk.ecdh.private_key->label,
                                       info->pk.ecdh.private_key->labelLen);
        }
        else if (info->pk.ecdh.private_key->idLen > 0) {
            ret = Pkcs11FindKeyById(&privateKey, CKO_PRIVATE_KEY, CKK_EC,
                                    session, info->pk.ecdh.private_key->id,
                                    info->pk.ecdh.private_key->idLen);
        }
        else {
            ret = Pkcs11FindEccKey(&privateKey, CKO_PRIVATE_KEY, session,
                                          info->pk.ecdh.public_key, CKA_DERIVE);
        }
    }
    if (ret == 0) {
        PRIVATE_KEY_UNLOCK();
        ret = wc_ecc_export_x963(info->pk.ecdh.public_key, NULL, &pointLen);
        PRIVATE_KEY_LOCK();
        if (ret == LENGTH_ONLY_E) {
            point = (unsigned char*)XMALLOC(pointLen,
                                                 info->pk.ecdh.public_key->heap,
                                                       DYNAMIC_TYPE_ECC_BUFFER);
            PRIVATE_KEY_UNLOCK();
            ret = wc_ecc_export_x963(info->pk.ecdh.public_key, point,
                                                                     &pointLen);
            PRIVATE_KEY_LOCK();
        }
    }

    if (ret == 0) {
        secSz = *info->pk.ecdh.outlen;
        if (secSz > (CK_ULONG)info->pk.ecdh.private_key->dp->size)
            secSz = info->pk.ecdh.private_key->dp->size;

        params.kdf             = CKD_NULL;
        params.pSharedData     = NULL;
        params.ulSharedDataLen = 0;
        params.pPublicData     = point;
        params.ulPublicDataLen = pointLen;

        mech.mechanism      = CKM_ECDH1_DERIVE;
        mech.ulParameterLen = sizeof(params);
        mech.pParameter     = &params;

        PKCS11_DUMP_TEMPLATE("ECDH key", tmpl, tmplCnt);
        rv = session->func->C_DeriveKey(session->handle, &mech, privateKey,
                                                        tmpl, tmplCnt, &secret);
        PKCS11_RV("C_DeriveKey", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (ret == 0) {
        ret = Pkcs11ExtractSecret(session, secret, info->pk.ecdh.out,
                                                          info->pk.ecdh.outlen);
    }

    if (sessionKey)
        session->func->C_DestroyObject(session->handle, privateKey);

    if (point != NULL)
        XFREE(point, info->pk.ecdh.public_key->heap, DYNAMIC_TYPE_ECC_BUFFER);

    return ret;
}
#endif

/**
 * Encode, in place, the ECDSA signature.
 * Two fixed width values into ASN.1 DER encoded SEQ { INT, INT }
 *
 * @param  [in,out]  sig  Signature data.
 * @param  [in]      sz   Size of original signature data.
 * @return  Length of the ASN.1 DER encoded signature.
 */
static word32 Pkcs11ECDSASig_Encode(byte* sig, word32 sz)
{
    word32 rHigh, sHigh, seqLen;
    word32 rStart = 0, sStart = 0;
    word32 sigSz, rSz, rLen, sSz, sLen;
    word32 i;

    /* Find first byte of data in r and s. */
    while (rStart < sz - 1 && sig[rStart] == 0x00)
        rStart++;
    while (sStart < sz - 1 && sig[sz + sStart] == 0x00)
        sStart++;
    /* Check if 0 needs to be prepended to make integer a positive number. */
    rHigh = sig[rStart] >> 7;
    sHigh = sig[sz + sStart] >> 7;
    /* Calculate length of integer to put into ASN.1 encoding. */
    rLen = sz - rStart;
    sLen = sz - sStart;
    /* r and s: INT (2 bytes) + [ 0x00 ] + integer */
    rSz = 2 + rHigh + rLen;
    sSz = 2 + sHigh + sLen;
    /* Calculate the complete ASN.1 DER encoded size. */
    sigSz = rSz + sSz;
    if (sigSz >= ASN_LONG_LENGTH)
        seqLen = 3;
    else
        seqLen = 2;

    /* Move s and then r integers into their final places. */
    XMEMMOVE(sig + seqLen + rSz + (sSz - sLen), sig + sz + sStart, sLen);
    XMEMMOVE(sig + seqLen       + (rSz - rLen), sig      + rStart, rLen);

    /* Put the ASN.1 DER encoding around data. */
    i = 0;
    sig[i++] = ASN_CONSTRUCTED | ASN_SEQUENCE;
    if (seqLen == 3)
        sig[i++] = ASN_LONG_LENGTH | 0x01;
    sig[i++] = sigSz;
    sig[i++] = ASN_INTEGER;
    sig[i++] = rHigh + (sz - rStart);
    if (rHigh)
        sig[i++] = 0x00;
    i += sz - rStart;
    sig[i++] = ASN_INTEGER;
    sig[i++] = sHigh + (sz - sStart);
    if (sHigh)
        sig[i] = 0x00;

    return seqLen + sigSz;
}

/**
 * Decode the ECDSA signature.
 * ASN.1 DER encode SEQ { INT, INT } converted to two fixed with values.
 *
 * @param  [in]  in    ASN.1 DER encoded signature.
 * @param  [in]  inSz  Size of ASN.1 signature.
 * @param  [in]  sig   Output buffer.
 * @param  [in]  sz    Size of output buffer.
 * @return  ASN_PARSE_E when the ASN.1 encoding is invalid.
 * @return  0 on success.
 */
static int Pkcs11ECDSASig_Decode(const byte* in, word32 inSz, byte* sig,
                                 word32 sz)
{
    int ret = 0;
    word32 i = 0;
    byte   tag;
    int len, seqLen = 2;

    /* Make sure zeros in place when decoding short integers. */
    XMEMSET(sig, 0, sz * 2);

    /* Check min data for: SEQ + INT. */
    if (inSz < 5)
        ret = ASN_PARSE_E;
    /* Check SEQ */
    if (ret == 0 && in[i++] != (ASN_CONSTRUCTED | ASN_SEQUENCE))
        ret = ASN_PARSE_E;
    if (ret == 0 && in[i] >= ASN_LONG_LENGTH) {
        if (in[i] != (ASN_LONG_LENGTH | 0x01))
            ret = ASN_PARSE_E;
        else {
            i++;
            seqLen++;
        }
    }
    if (ret == 0 && in[i++] != inSz - seqLen)
        ret = ASN_PARSE_E;

    /* Check INT */
    if (ret == 0 && GetASNTag(in, &i, &tag, inSz) != 0)
        ret = ASN_PARSE_E;
    if (ret == 0 && tag != ASN_INTEGER)
        ret = ASN_PARSE_E;
    if (ret == 0 && (len = in[i++]) > sz + 1)
        ret = ASN_PARSE_E;
    /* Check there is space for INT data */
    if (ret == 0 && i + len > inSz)
        ret = ASN_PARSE_E;
    if (ret == 0) {
        /* Skip leading zero */
        if (in[i] == 0x00) {
            i++;
            len--;
        }
        /* Copy r into sig. */
        XMEMCPY(sig + sz - len, in + i, len);
        i += len;
    }

    /* Check min data for: INT. */
    if (ret == 0 && i + 2 > inSz)
        ret = ASN_PARSE_E;
    /* Check INT */
    if (ret == 0 && GetASNTag(in, &i, &tag, inSz) != 0)
        ret = ASN_PARSE_E;
    if (ret == 0 && tag != ASN_INTEGER)
        ret = ASN_PARSE_E;
    if (ret == 0 && (len = in[i++]) > sz + 1)
        ret = ASN_PARSE_E;
    /* Check there is space for INT data */
    if (ret == 0 && i + len > inSz)
        ret = ASN_PARSE_E;
    if (ret == 0) {
        /* Skip leading zero */
        if (in[i] == 0x00) {
            i++;
            len--;
        }
        /* Copy s into sig. */
        XMEMCPY(sig + sz + sz - len, in + i, len);
    }

    return ret;
}

/**
 * Get the parameters from the private key on the device.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  privKey  PKCS #11 object handle of private key..
 * @param  [in]  key      Ecc key to set parameters against.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11GetEccParams(Pkcs11Session* session, CK_OBJECT_HANDLE privKey,
                              ecc_key* key)
{
    int          ret = 0;
    int          curveId;
    CK_RV        rv;
    byte         oid[16];
    CK_ATTRIBUTE template[] = {
        { CKA_EC_PARAMS, (CK_VOID_PTR)oid, sizeof(oid) }
    };

    PKCS11_DUMP_TEMPLATE("Get Ec Params", template, 1);
    rv = session->func->C_GetAttributeValue(session->handle, privKey, template,
                                                                             1);
    PKCS11_RV("C_GetAttributeValue", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    PKCS11_DUMP_TEMPLATE("Ec Params", template, 1);
    if (ret == 0) {
        /* PKCS #11 wraps the OID in ASN.1 */
        curveId = wc_ecc_get_curve_id_from_oid(oid + 2,
                                            (word32)template[0].ulValueLen - 2);
        if (curveId == ECC_CURVE_INVALID)
            ret = WC_HW_E;
    }
    if (ret == 0)
        ret = wc_ecc_set_curve(key, 0, curveId);

    return ret;
}

/**
 * Performs the ECDSA signing operation.
 *
 * @param  session  [in]  Session object.
 * @param  info     [in]  Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11ECDSA_Sign(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                    ret = 0;
    int                    sessionKey = 0;
    word32                 sz;
    CK_RV                  rv;
    CK_ULONG               outLen;
    CK_MECHANISM           mech;
    CK_MECHANISM_INFO      mechInfo;
    CK_OBJECT_HANDLE       privateKey = NULL_PTR;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_ECDSA,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK || (mechInfo.flags & CKF_SIGN) == 0)
        ret = NOT_COMPILED_IN;

    if (ret == 0 && info->pk.eccsign.outlen == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0 && info->pk.eccsign.out == NULL) {
        ret = BAD_FUNC_ARG;
    }
    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: EC Signing Operation");

        if ((sessionKey = !mp_iszero(
                wc_ecc_key_get_priv(info->pk.eccsign.key))))
            ret = Pkcs11CreateEccPrivateKey(&privateKey, session,
                                                info->pk.eccsign.key, CKA_SIGN);
        else if (info->pk.eccsign.key->labelLen > 0) {
            ret = Pkcs11FindKeyByLabel(&privateKey, CKO_PRIVATE_KEY, CKK_EC,
                                       session, info->pk.eccsign.key->label,
                                       info->pk.eccsign.key->labelLen);
            if (ret == 0 && info->pk.eccsign.key->dp == NULL) {
                ret = Pkcs11GetEccParams(session, privateKey,
                                                          info->pk.eccsign.key);
            }
        }
        else if (info->pk.eccsign.key->idLen > 0) {
            ret = Pkcs11FindKeyById(&privateKey, CKO_PRIVATE_KEY, CKK_EC,
                                    session, info->pk.eccsign.key->id,
                                    info->pk.eccsign.key->idLen);
            if (ret == 0 && info->pk.eccsign.key->dp == NULL) {
                ret = Pkcs11GetEccParams(session, privateKey,
                                                          info->pk.eccsign.key);
            }
        }
        else {
            ret = Pkcs11FindEccKey(&privateKey, CKO_PRIVATE_KEY, session,
                                                info->pk.eccsign.key, CKA_SIGN);
        }
    }

    if (ret == 0) {
        sz = info->pk.eccsign.key->dp->size;
        /* Maximum encoded size is two ordinates + 8 bytes of ASN.1. */
        if (*info->pk.eccsign.outlen < (word32)wc_ecc_sig_size_calc(sz))
            ret = BUFFER_E;
    }

    if (ret == 0) {
        mech.mechanism      = CKM_ECDSA;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        rv = session->func->C_SignInit(session->handle, &mech, privateKey);
        PKCS11_RV("C_SignInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (ret == 0) {
        outLen = *info->pk.eccsign.outlen;
        rv = session->func->C_Sign(session->handle,
                                   (CK_BYTE_PTR)info->pk.eccsign.in,
                                   info->pk.eccsign.inlen, info->pk.eccsign.out,
                                   &outLen);
        PKCS11_RV("C_Sign", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (ret == 0) {
        *info->pk.eccsign.outlen = Pkcs11ECDSASig_Encode(info->pk.eccsign.out,
                                                         sz);
    }

    if (sessionKey)
        session->func->C_DestroyObject(session->handle, privateKey);

    return ret;
}

/**
 * Performs the ECDSA verification operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11ECDSA_Verify(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                    ret = 0;
    CK_RV                  rv;
    CK_MECHANISM           mech;
    CK_MECHANISM_INFO      mechInfo;
    CK_OBJECT_HANDLE       publicKey = NULL_PTR;
    unsigned char*         sig = NULL;
    word32                 sz = info->pk.eccverify.key->dp->size;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_ECDSA,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK || (mechInfo.flags & CKF_VERIFY) == 0)
        ret = NOT_COMPILED_IN;

    if (ret == 0 && info->pk.eccverify.res == NULL) {
        ret = BAD_FUNC_ARG;
    }

    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: EC Verification Operation");

        ret = Pkcs11CreateEccPublicKey(&publicKey, session,
                                            info->pk.eccverify.key, CKA_VERIFY);
    }

    if (ret == 0) {
        sig = (unsigned char *)XMALLOC(sz * 2, info->pk.eccverify.key->heap,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
        if (sig == NULL)
            ret = MEMORY_E;
    }

    if (ret == 0) {
        ret = Pkcs11ECDSASig_Decode(info->pk.eccverify.sig,
                                    info->pk.eccverify.siglen, sig, sz);
    }
    if (ret == 0) {
        mech.mechanism      = CKM_ECDSA;
        mech.ulParameterLen = 0;
        mech.pParameter     = NULL;

        rv = session->func->C_VerifyInit(session->handle, &mech, publicKey);
        PKCS11_RV("C_VerifyInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (ret == 0) {
        *info->pk.eccverify.res = 0;
        rv = session->func->C_Verify(session->handle,
                                     (CK_BYTE_PTR)info->pk.eccverify.hash,
                                     info->pk.eccverify.hashlen,
                                     (CK_BYTE_PTR)sig, sz * 2);
        PKCS11_RV("C_Verify", rv);
        if (rv == CKR_SIGNATURE_INVALID) {
        }
        else if (rv != CKR_OK)
            ret = WC_HW_E;
        else
            *info->pk.eccverify.res = 1;
    }

    if (publicKey != NULL_PTR)
        session->func->C_DestroyObject(session->handle, publicKey);

    if (sig != NULL)
        XFREE(sig, info->pk.eccverify.key->heap, DYNAMIC_TYPE_TMP_BUFFER);

    return ret;
}
#endif

#ifndef NO_RSA
/**
 * Check the private RSA key matches the public key.
 *
 * @param  [in]  priv        RSA private key.
 * @param  [in]  publicKey   Encoded RSA public key.
 * @param  [in]  pubKeySize  Length of encoded RSA public key.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  MP_CMP_E when the public parts are different.
 * @return  0 on success.
 */
static int wc_Pkcs11CheckPrivKey_Rsa(RsaKey* priv,
    const unsigned char* publicKey, word32 pubKeySize)
{
    int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        RsaKey* pub = NULL;
    #else
        RsaKey pub[1];
    #endif
    word32 keyIdx = 0;

    #ifdef WOLFSSL_SMALL_STACK
        pub = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_RSA);
        if (pub == NULL) {
            ret = MEMORY_E;
        }
    #endif

    if ((ret == 0) && (ret = wc_InitRsaKey(pub, NULL)) == 0) {
        if (ret == 0) {
            ret = wc_RsaPublicKeyDecode(publicKey, &keyIdx, pub, pubKeySize);
        }
        if (ret == 0) {
            /* both keys extracted successfully now check n and e
             * values are the same. This is dereferencing RsaKey */
            if (mp_cmp(&(priv->n), &(pub->n)) != MP_EQ ||
                mp_cmp(&(priv->e), &(pub->e)) != MP_EQ) {
                ret = MP_CMP_E;
            }
            else
                ret = 0;
        }
        wc_FreeRsaKey(pub);
    }
    #ifdef WOLFSSL_SMALL_STACK
        if (pub != NULL) {
            XFREE(pub, NULL, DYNAMIC_TYPE_RSA);
        }
    #endif

    return ret;
}

/**
 * Checks the RSA private key matches the RSA public key.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  MP_CMP_E when the public parts are different.
 * @return  0 on success.
 */
static int Pkcs11RsaCheckPrivKey(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int ret = 0;
    CK_OBJECT_HANDLE privateKey;
    RsaKey* priv = info->pk.rsa_check.key;

    if (mp_iszero(&priv->n) || mp_iszero(&priv->e)) {
        /* Get the RSA private key object. */
        if (priv->labelLen > 0) {
            ret = Pkcs11FindKeyByLabel(&privateKey, CKO_PRIVATE_KEY,
                                                  CKK_RSA, session, priv->label,
                                                  priv->labelLen);
        }
        else if (priv->idLen > 0) {
            ret = Pkcs11FindKeyById(&privateKey, CKO_PRIVATE_KEY, CKK_RSA,
                                    session, priv->id, priv->idLen);
        }
        else {
            ret = Pkcs11FindRsaKey(&privateKey, CKO_PRIVATE_KEY, session, priv);
        }

        if (ret == 0) {
            /* Extract the public key components. */
            ret = Pkcs11GetRsaPublicKey(priv, session, privateKey);
        }
    }
    if (ret == 0) {
        /* Compare the extracted public parts with the public key. */
        ret = wc_Pkcs11CheckPrivKey_Rsa(priv, info->pk.rsa_check.pubKey,
                                                   info->pk.rsa_check.pubKeySz);
    }

    return ret;
}

/**
 * Get the size of the RSA key in bytes.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  NOT_COMPILED_IN when no modulus, label or id.
 * @return  0 on success.
 */
static int Pkcs11RsaGetSize(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int ret = 0;
    CK_OBJECT_HANDLE privateKey;
    const RsaKey* priv = info->pk.rsa_get_size.key;

    if (!mp_iszero(&priv->n)) {
        /* Use the key's modulus MP integer to determine size. */
        *info->pk.rsa_get_size.keySize = mp_unsigned_bin_size(&priv->n);
    }
    else {
        /* Get the RSA private key object. */
        if (priv->labelLen > 0) {
            ret = Pkcs11FindKeyByLabel(&privateKey, CKO_PRIVATE_KEY,
                                           CKK_RSA, session, (char*)priv->label,
                                           priv->labelLen);
        }
        else if (priv->idLen > 0) {
            ret = Pkcs11FindKeyById(&privateKey, CKO_PRIVATE_KEY, CKK_RSA,
                                              session, (unsigned char*)priv->id,
                                              priv->idLen);
        }
        else {
            /* Lookup is by modulus which is not present. */
            ret = NOT_COMPILED_IN;
        }

        if (ret == 0) {
            /* Lookup the modulus size in bytes. */
            ret = Pkcs11GetRsaModulusSize(session, privateKey,
                                                 info->pk.rsa_get_size.keySize);
        }
    }

    return ret;
}
#endif

#ifdef HAVE_ECC
/**
 * Check the private ECC key matches the public key.
 * Do this by looking up the public key data from the associated public key.
 * The public key object handle is passed in for the private key.
 *
 * @param  [in]  privateKey  Handle to private key object.
 * @param  [in]  publicKey   Encoded EC public key.
 * @param  [in]  pubKeySize  Length of encoded EC public key.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  MP_CMP_E when the public parts are different.
 * @return  0 on success.
 */
static int wc_Pkcs11CheckPrivKey_Ecc(ecc_key* priv,
    const unsigned char* publicKey, word32 pubKeySize)
{
    int ret = 0;
    #ifdef WOLFSSL_SMALL_STACK
        ecc_key* pub = NULL;
    #else
        ecc_key pub[1];
    #endif
    word32 keyIdx = 0;

    #ifdef WOLFSSL_SMALL_STACK
        pub = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
        if (pub == NULL) {
            ret = MEMORY_E;
        }
    #endif

    if ((ret == 0) && (ret = wc_ecc_init(pub)) == 0) {
        ret = wc_EccPublicKeyDecode(publicKey, &keyIdx, pub, pubKeySize);
        if (ret == 0) {
            /* both keys extracted successfully now check curve and
             * pubkey. */
            if ((pub->idx != priv->idx) || (wc_ecc_cmp_point(&priv->pubkey,
                                                      &pub->pubkey) != MP_EQ)) {
                ret = MP_CMP_E;
            }
            else {
                ret = 0;
            }
        }
        wc_ecc_free(pub);
    }
    #ifdef WOLFSSL_SMALL_STACK
        if (pub != NULL) {
            XFREE(pub, NULL, DYNAMIC_TYPE_ECC);
        }
    #endif

    return ret;
}

/**
 * Checks the ECC private key matches the ECC public key.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  MP_CMP_E when the public parts are different.
 * @return  0 on success.
 */
static int Pkcs11EccCheckPrivKey(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int ret = 0;
    ecc_key* priv = info->pk.ecc_check.key;
    CK_OBJECT_HANDLE privateKey;

    if (mp_iszero(priv->pubkey.x) || mp_iszero(priv->pubkey.y)) {
        /* Get the public key object as the private key object doesn't have
         * the public point stored in it.
         */
        if (priv->labelLen > 0) {
            ret = Pkcs11FindKeyByLabel(&privateKey, CKO_PUBLIC_KEY, CKK_EC,
                                          session, priv->label, priv->labelLen);
        }
        else if (priv->idLen > 0) {
            ret = Pkcs11FindKeyById(&privateKey, CKO_PUBLIC_KEY, CKK_EC,
                                    session, priv->id, priv->idLen);
        }
        else {
            ret = Pkcs11FindEccKey(&privateKey, CKO_PUBLIC_KEY, session, priv,
                                                                      CKA_SIGN);
        }
        if (ret == 0 && priv->dp == NULL) {
            /* Extract the group id. */
            ret = Pkcs11GetEccParams(session, privateKey, priv);
        }
        if (ret == 0) {
            /* Extract the public point. */
            ret = Pkcs11GetEccPublicKey(priv, session, privateKey);
       }
    }
    if (ret == 0) {
        /* Compare the extracted public parts with the public key. */
        ret = wc_Pkcs11CheckPrivKey_Ecc(priv, info->pk.ecc_check.pubKey,
                                                   info->pk.ecc_check.pubKeySz);
    }

    return ret;
}
#endif

#if !defined(NO_AES) && defined(HAVE_AESGCM)
/**
 * Performs the AES-GCM encryption operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11AesGcmEncrypt(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;
    Aes*               aes = info->cipher.aesgcm_enc.aes;
    CK_GCM_PARAMS      params;
    CK_MECHANISM_INFO  mechInfo;
    CK_OBJECT_HANDLE   key = NULL_PTR;
    CK_MECHANISM       mech;
    CK_ULONG           outLen;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_AES_GCM,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK || (mechInfo.flags & CKF_ENCRYPT) == 0)
        ret = NOT_COMPILED_IN;

    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: AES-GCM Encryption Operation");

        /* Create a private key object or find by label or id. */
        if (aes->idLen == 0 && aes->labelLen == 0) {
            ret = Pkcs11CreateSecretKey(&key, session, CKK_AES,
                                        (unsigned char*)aes->devKey,
                                        aes->keylen, NULL, 0, NULL, 0,
                                        CKA_ENCRYPT);
        }
        else if (aes->labelLen != 0) {
            ret = Pkcs11FindKeyByLabel(&key, CKO_SECRET_KEY, CKK_AES, session,
                                       aes->label, aes->labelLen);
        }
        else {
            ret = Pkcs11FindKeyById(&key, CKO_SECRET_KEY, CKK_AES, session,
                                    aes->id, aes->idLen);
        }
    }

    if (ret == 0) {
        params.pIv       = (CK_BYTE_PTR)info->cipher.aesgcm_enc.iv;
        params.ulIvLen   = info->cipher.aesgcm_enc.ivSz;
        params.pAAD      = (CK_BYTE_PTR)info->cipher.aesgcm_enc.authIn;
        params.ulAADLen  = info->cipher.aesgcm_enc.authInSz;
        params.ulTagBits = info->cipher.aesgcm_enc.authTagSz * 8;

        mech.mechanism      = CKM_AES_GCM;
        mech.ulParameterLen = sizeof(params);
        mech.pParameter     = &params;

        rv = session->func->C_EncryptInit(session->handle, &mech, key);
        PKCS11_RV("C_EncryptInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        outLen = info->cipher.aesgcm_enc.sz;
        rv = session->func->C_EncryptUpdate(session->handle,
                                        (CK_BYTE_PTR)info->cipher.aesgcm_enc.in,
                                        info->cipher.aesgcm_enc.sz,
                                        info->cipher.aesgcm_enc.out,
                                        &outLen);
        PKCS11_RV("C_EncryptUpdate", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        /* Authentication tag comes out in final block. */
        outLen = info->cipher.aesgcm_enc.authTagSz;
        rv = session->func->C_EncryptFinal(session->handle,
                                           info->cipher.aesgcm_enc.authTag,
                                           &outLen);
        PKCS11_RV("C_EncryptFinal", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (aes->idLen == 0 && aes->labelLen == 0 && key != NULL_PTR)
        session->func->C_DestroyObject(session->handle, key);

    return ret;
}

/**
 * Performs the AES-GCM decryption operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11AesGcmDecrypt(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;
    Aes*               aes = info->cipher.aesgcm_enc.aes;
    CK_GCM_PARAMS      params;
    CK_MECHANISM_INFO  mechInfo;
    CK_OBJECT_HANDLE   key = NULL_PTR;
    CK_MECHANISM       mech;
    CK_ULONG           outLen;
    word32             len;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_AES_GCM,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK || (mechInfo.flags & CKF_DECRYPT) == 0)
        ret = NOT_COMPILED_IN;

    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: AES-GCM Decryption Operation");

        /* Create a private key object or find by id. */
        if (aes->idLen == 0 && aes->labelLen == 0) {
            ret = Pkcs11CreateSecretKey(&key, session, CKK_AES,
                                        (unsigned char*)aes->devKey,
                                        aes->keylen, NULL, 0, NULL, 0,
                                        CKA_DECRYPT);
        }
        else if (aes->labelLen != 0) {
            ret = Pkcs11FindKeyByLabel(&key, CKO_SECRET_KEY, CKK_AES, session,
                                       aes->label, aes->labelLen);
        }
        else {
            ret = Pkcs11FindKeyById(&key, CKO_SECRET_KEY, CKK_AES, session,
                                    aes->id, aes->idLen);
        }
    }

    if (ret == 0) {
        params.pIv       = (CK_BYTE_PTR)info->cipher.aesgcm_dec.iv;
        params.ulIvLen   = info->cipher.aesgcm_dec.ivSz;
        params.pAAD      = (CK_BYTE_PTR)info->cipher.aesgcm_dec.authIn;
        params.ulAADLen  = info->cipher.aesgcm_dec.authInSz;
        params.ulTagBits = info->cipher.aesgcm_dec.authTagSz * 8;

        mech.mechanism      = CKM_AES_GCM;
        mech.ulParameterLen = sizeof(params);
        mech.pParameter     = &params;

        rv = session->func->C_DecryptInit(session->handle, &mech, key);
        PKCS11_RV("C_DecryptInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        outLen = len = info->cipher.aesgcm_dec.sz;
        rv = session->func->C_DecryptUpdate(session->handle,
                                        (CK_BYTE_PTR)info->cipher.aesgcm_dec.in,
                                        info->cipher.aesgcm_dec.sz,
                                        info->cipher.aesgcm_dec.out,
                                        &outLen);
        PKCS11_RV("C_DecryptUpdate", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        /* Put authentication tag in as encrypted data. */
        outLen = len = (len + info->cipher.aesgcm_dec.authTagSz -
                                                                (word32)outLen);
        rv = session->func->C_DecryptUpdate(session->handle,
                                   (CK_BYTE_PTR)info->cipher.aesgcm_dec.authTag,
                                   info->cipher.aesgcm_dec.authTagSz,
                                   info->cipher.aesgcm_dec.out,
                                   &outLen);
        PKCS11_RV("C_DecryptUpdate", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        outLen = len = (len - (word32)outLen);
        /* Decrypted data comes out now. */
        rv = session->func->C_DecryptFinal(session->handle,
                                           info->cipher.aesgcm_dec.out,
                                           &outLen);
        PKCS11_RV("C_DecryptFinal", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (aes->idLen == 0 && aes->labelLen == 0 && key != NULL_PTR)
        session->func->C_DestroyObject(session->handle, key);

    return ret;
}
#endif

#if !defined(NO_AES) && defined(HAVE_AES_CBC)
/**
 * Performs the AES-CBC encryption operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11AesCbcEncrypt(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;
    Aes*               aes = info->cipher.aescbc.aes;
    CK_MECHANISM_INFO  mechInfo;
    CK_OBJECT_HANDLE   key = NULL_PTR;
    CK_MECHANISM       mech;
    CK_ULONG           outLen;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_AES_CBC,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK || (mechInfo.flags & CKF_ENCRYPT) == 0)
        ret = NOT_COMPILED_IN;

    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: AES-CBC Encryption Operation");

        /* Create a private key object or find by id. */
        if (aes->idLen == 0 && aes->labelLen == 0) {
            ret = Pkcs11CreateSecretKey(&key, session, CKK_AES,
                                        (unsigned char*)aes->devKey,
                                        aes->keylen, NULL, 0, NULL, 0,
                                        CKA_ENCRYPT);
        }
        else if (aes->labelLen != 0) {
            ret = Pkcs11FindKeyByLabel(&key, CKO_SECRET_KEY, CKK_AES, session,
                                       aes->label, aes->labelLen);
        }
        else {
            ret = Pkcs11FindKeyById(&key, CKO_SECRET_KEY, CKK_AES, session,
                                    aes->id, aes->idLen);
        }
    }

    if (ret == 0) {
        mech.mechanism      = CKM_AES_CBC;
        mech.ulParameterLen = AES_BLOCK_SIZE;
        mech.pParameter     = (CK_BYTE_PTR)info->cipher.aescbc.aes->reg;

        rv = session->func->C_EncryptInit(session->handle, &mech, key);
        PKCS11_RV("C_EncryptInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        outLen = info->cipher.aescbc.sz;
        rv = session->func->C_Encrypt(session->handle,
                                      (CK_BYTE_PTR)info->cipher.aescbc.in,
                                      info->cipher.aescbc.sz,
                                      info->cipher.aescbc.out,
                                      &outLen);
        PKCS11_RV("C_Encrypt", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (aes->idLen == 0 && aes->labelLen == 0 && key != NULL_PTR)
        session->func->C_DestroyObject(session->handle, key);

    return ret;
}

/**
 * Performs the AES-CBC decryption operation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  MEMORY_E when a memory allocation fails.
 * @return  0 on success.
 */
static int Pkcs11AesCbcDecrypt(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;
    Aes*               aes = info->cipher.aescbc.aes;
    CK_MECHANISM_INFO  mechInfo;
    CK_OBJECT_HANDLE   key = NULL_PTR;
    CK_MECHANISM       mech;
    CK_ULONG           outLen;

    /* Check operation is supported. */
    rv = session->func->C_GetMechanismInfo(session->slotId, CKM_AES_CBC,
                                                                     &mechInfo);
    PKCS11_RV("C_GetMechanismInfo", rv);
    if (rv != CKR_OK || (mechInfo.flags & CKF_DECRYPT) == 0)
        ret = NOT_COMPILED_IN;

    if (ret == 0) {
        WOLFSSL_MSG("PKCS#11: AES-CBC Decryption Operation");

        /* Create a private key object or find by id. */
        if (aes->idLen == 0 && aes->labelLen == 0) {
            ret = Pkcs11CreateSecretKey(&key, session, CKK_AES,
                                        (unsigned char*)aes->devKey,
                                        aes->keylen, NULL, 0, NULL, 0,
                                        CKA_DECRYPT);
        }
        else if (aes->labelLen != 0) {
            ret = Pkcs11FindKeyByLabel(&key, CKO_SECRET_KEY, CKK_AES, session,
                                       aes->label, aes->labelLen);
        }
        else {
            ret = Pkcs11FindKeyById(&key, CKO_SECRET_KEY, CKK_AES, session,
                                    aes->id, aes->idLen);
        }
    }

    if (ret == 0) {
        mech.mechanism      = CKM_AES_CBC;
        mech.ulParameterLen = AES_BLOCK_SIZE;
        mech.pParameter     = (CK_BYTE_PTR)info->cipher.aescbc.aes->reg;

        rv = session->func->C_DecryptInit(session->handle, &mech, key);
        PKCS11_RV("C_DecryptInit", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }
    if (ret == 0) {
        outLen = info->cipher.aescbc.sz;
        rv = session->func->C_Decrypt(session->handle,
                                        (CK_BYTE_PTR)info->cipher.aescbc.in,
                                        info->cipher.aescbc.sz,
                                        info->cipher.aescbc.out,
                                        &outLen);
        PKCS11_RV("C_Decrypt", rv);
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
    }

    if (aes->idLen == 0 && aes->labelLen == 0 && key != NULL_PTR)
        session->func->C_DestroyObject(session->handle, key);

    return ret;
}
#endif

#ifndef NO_HMAC
/**
 * Updates or calculates the HMAC of the data.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11Hmac(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;
    Hmac*              hmac = info->hmac.hmac;
    CK_MECHANISM_INFO  mechInfo;
    CK_OBJECT_HANDLE   key = NULL_PTR;
    CK_MECHANISM       mech;
    CK_ULONG           outLen;
    int                mechType;
    int                keyType;

    if (hmac->innerHashKeyed == WC_HMAC_INNER_HASH_KEYED_SW)
        ret = NOT_COMPILED_IN;

    if (ret == 0)
        ret = Pkcs11HmacTypes(info->hmac.macType, &mechType, &keyType);
    if (ret == 0) {
        /* Check operation is supported. */
        rv = session->func->C_GetMechanismInfo(session->slotId, mechType,
                                                                     &mechInfo);
        PKCS11_RV("C_GetMechanismInfo", rv);
        if (rv != CKR_OK || (mechInfo.flags & CKF_SIGN) == 0)
            ret = NOT_COMPILED_IN;
    }

    /* Check whether key been used to initialized. */
    if (ret == 0 && !hmac->innerHashKeyed) {
        WOLFSSL_MSG("PKCS#11: HMAC Init");

        /* Check device supports key length. */
        if (mechInfo.ulMaxKeySize > 0 &&
                                       (hmac->keyLen < mechInfo.ulMinKeySize ||
                                        hmac->keyLen > mechInfo.ulMaxKeySize)) {
            WOLFSSL_MSG("PKCS#11: Key Length not supported");
            ret = NOT_COMPILED_IN;
        }

        /* Create a private key object or find by id. */
        if (ret == 0 && hmac->idLen == 0 && hmac->labelLen == 0) {
            ret = Pkcs11CreateSecretKey(&key, session, keyType,
                                    (unsigned char*)hmac->keyRaw, hmac->keyLen,
                                    NULL, 0, NULL, 0, CKA_SIGN);
            if (ret == WC_HW_E) {
                ret = Pkcs11CreateSecretKey(&key, session, CKK_GENERIC_SECRET,
                                    (unsigned char*)hmac->keyRaw, hmac->keyLen,
                                    NULL, 0, NULL, 0, CKA_SIGN);
            }

        }
        else if (ret == 0 && hmac->labelLen != 0) {
            ret = Pkcs11FindKeyByLabel(&key, CKO_SECRET_KEY, keyType, session,
                                       hmac->label, hmac->labelLen);
            if (ret == WC_HW_E) {
                ret = Pkcs11FindKeyByLabel(&key, CKO_SECRET_KEY,
                                           CKK_GENERIC_SECRET, session,
                                           hmac->label, hmac->labelLen);
            }
        }
        else if (ret == 0) {
            ret = Pkcs11FindKeyById(&key, CKO_SECRET_KEY, keyType, session,
                                    hmac->id, hmac->idLen);
            if (ret == WC_HW_E) {
                ret = Pkcs11FindKeyById(&key, CKO_SECRET_KEY,
                                        CKK_GENERIC_SECRET, session, hmac->id,
                                        hmac->idLen);
            }
        }

        /* Initialize HMAC operation */
        if (ret == 0) {
            mech.mechanism      = mechType;
            mech.ulParameterLen = 0;
            mech.pParameter     = NULL;

            rv = session->func->C_SignInit(session->handle, &mech, key);
            PKCS11_RV("C_SignInit", rv);
            if (rv != CKR_OK) {
                ret = WC_HW_E;
            }
        }

        /* Don't initialize HMAC again if this succeeded */
        if (ret == 0)
            hmac->innerHashKeyed = WC_HMAC_INNER_HASH_KEYED_DEV;
    }
    /* Update the HMAC if input data passed in. */
    if (ret == 0 && info->hmac.inSz > 0) {
        WOLFSSL_MSG("PKCS#11: HMAC Update");

        rv = session->func->C_SignUpdate(session->handle,
                                         (CK_BYTE_PTR)info->hmac.in,
                                         info->hmac.inSz);
        PKCS11_RV("C_SignUpdate", rv);
        /* Some algorithm implementations only support C_Sign. */
        if (rv == CKR_MECHANISM_INVALID) {
            WOLFSSL_MSG("PKCS#11: HMAC Update/Final not supported");
            ret = NOT_COMPILED_IN;
            /* Allow software implementation to set key. */
            hmac->innerHashKeyed = 0;
        }
        else if (rv != CKR_OK)
            ret = WC_HW_E;
    }
    /* Calculate the HMAC result if output buffer specified. */
    if (ret == 0 && info->hmac.digest != NULL) {
        WOLFSSL_MSG("PKCS#11: HMAC Final");

        outLen = WC_MAX_DIGEST_SIZE;
        rv = session->func->C_SignFinal(session->handle,
                                        (CK_BYTE_PTR)info->hmac.digest,
                                        &outLen);
        PKCS11_RV("C_SignFinal", rv);
        /* Some algorithm implementations only support C_Sign. */
        if (rv != CKR_OK) {
            ret = WC_HW_E;
        }
        else
            hmac->innerHashKeyed = 0;
    }

    if (hmac->idLen == 0 && hmac->labelLen == 0 && key != NULL_PTR)
        session->func->C_DestroyObject(session->handle, key);

    return ret;
}
#endif

#ifndef WC_NO_RNG
#ifndef HAVE_HASHDRBG
/**
 * Performs random number generation.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11RandomBlock(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;

    WOLFSSL_MSG("PKCS#11: Generate Random for Block");

    rv = session->func->C_GenerateRandom(session->handle, info->rng.out,
                                                                  info->rng.sz);
    PKCS11_RV("C_GenerateRandom", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    return ret;
}
#endif

/**
 * Generates entropy (seed) data.
 *
 * @param  [in]  session  Session object.
 * @param  [in]  info     Cryptographic operation data.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
static int Pkcs11RandomSeed(Pkcs11Session* session, wc_CryptoInfo* info)
{
    int                ret = 0;
    CK_RV              rv;

    WOLFSSL_MSG("PKCS#11: Generate Random for Seed");

    rv = session->func->C_GenerateRandom(session->handle, info->seed.seed,
                                                                 info->seed.sz);
    PKCS11_RV("C_GenerateRandom", rv);
    if (rv != CKR_OK) {
        ret = WC_HW_E;
    }
    return ret;
}
#endif

/**
 * Perform a cryptographic operation using PKCS#11 device.
 *
 * @param  [in]  devId  Device identifier.
 * @param  [in]  info   Cryptographic operation data.
 * @param  [in]  ctx    Context data for device - the token object.
 * @return  WC_HW_E when a PKCS#11 library call fails.
 * @return  0 on success.
 */
int wc_Pkcs11_CryptoDevCb(int devId, wc_CryptoInfo* info, void* ctx)
{
    int ret = 0;
    Pkcs11Token* token = (Pkcs11Token*)ctx;
    Pkcs11Session session;
    int readWrite = 0;

    if (devId <= INVALID_DEVID || info == NULL || ctx == NULL)
        ret = BAD_FUNC_ARG;

    /* Open and close a session around each operation as the operation may not
     * be compiled in.
     */
    if (ret == 0) {
        if (info->algo_type == WC_ALGO_TYPE_PK) {
#if !defined(NO_RSA) || defined(HAVE_ECC)
            switch (info->pk.type) {
    #ifndef NO_RSA
                case WC_PK_TYPE_RSA:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11Rsa(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
        #ifdef WOLFSSL_KEY_GEN
                case WC_PK_TYPE_RSA_KEYGEN:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11RsaKeyGen(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
        #endif
                case WC_PK_TYPE_RSA_CHECK_PRIV_KEY:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11RsaCheckPrivKey(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
                case WC_PK_TYPE_RSA_GET_SIZE:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11RsaGetSize(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
    #endif
    #ifdef HAVE_ECC
        #ifndef NO_PKCS11_EC_KEYGEN
                case WC_PK_TYPE_EC_KEYGEN:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11EcKeyGen(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
        #endif
        #ifndef NO_PKCS11_ECDH
                case WC_PK_TYPE_ECDH:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11ECDH(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
        #endif
                case WC_PK_TYPE_ECDSA_SIGN:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11ECDSA_Sign(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
                case WC_PK_TYPE_ECDSA_VERIFY:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11ECDSA_Verify(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
                case WC_PK_TYPE_EC_CHECK_PRIV_KEY:
                    ret = Pkcs11OpenSession(token, &session, readWrite);
                    if (ret == 0) {
                        ret = Pkcs11EccCheckPrivKey(&session, info);
                        Pkcs11CloseSession(token, &session);
                    }
                    break;
    #endif
                default:
                    ret = NOT_COMPILED_IN;
                    break;
            }
#else
            ret = NOT_COMPILED_IN;
#endif /* !NO_RSA || HAVE_ECC */
        }
        else if (info->algo_type == WC_ALGO_TYPE_CIPHER) {
    #ifndef NO_AES
            switch (info->cipher.type) {
        #ifdef HAVE_AESGCM
                case WC_CIPHER_AES_GCM:
                    if (info->cipher.enc) {
                        ret = Pkcs11OpenSession(token, &session, readWrite);
                        if (ret == 0) {
                            ret = Pkcs11AesGcmEncrypt(&session, info);
                            Pkcs11CloseSession(token, &session);
                        }
                    }
                    else {
                        ret = Pkcs11OpenSession(token, &session, readWrite);
                        if (ret == 0) {
                            ret = Pkcs11AesGcmDecrypt(&session, info);
                            Pkcs11CloseSession(token, &session);
                        }
                    }
                    break;
        #endif
        #ifdef HAVE_AES_CBC
                case WC_CIPHER_AES_CBC:
                    if (info->cipher.enc) {
                        ret = Pkcs11OpenSession(token, &session, readWrite);
                        if (ret == 0) {
                            ret = Pkcs11AesCbcEncrypt(&session, info);
                            Pkcs11CloseSession(token, &session);
                        }
                    }
                    else {
                        ret = Pkcs11OpenSession(token, &session, readWrite);
                        if (ret == 0) {
                            ret = Pkcs11AesCbcDecrypt(&session, info);
                            Pkcs11CloseSession(token, &session);
                        }
                    }
                    break;
        #endif
                }
    #else
            ret = NOT_COMPILED_IN;
    #endif
        }
        else if (info->algo_type == WC_ALGO_TYPE_HMAC) {
    #ifndef NO_HMAC
            ret = Pkcs11OpenSession(token, &session, readWrite);
            if (ret == 0) {
                ret = Pkcs11Hmac(&session, info);
                Pkcs11CloseSession(token, &session);
            }
    #else
            ret = NOT_COMPILED_IN;
    #endif
        }
        else if (info->algo_type == WC_ALGO_TYPE_RNG) {
    #if !defined(WC_NO_RNG) && !defined(HAVE_HASHDRBG)
            ret = Pkcs11OpenSession(token, &session, readWrite);
            if (ret == 0) {
                ret = Pkcs11RandomBlock(&session, info);
                Pkcs11CloseSession(token, &session);
            }
    #else
            ret = NOT_COMPILED_IN;
    #endif
        }
        else if (info->algo_type == WC_ALGO_TYPE_SEED) {
    #ifndef WC_NO_RNG
            ret = Pkcs11OpenSession(token, &session, readWrite);
            if (ret == 0) {
                ret = Pkcs11RandomSeed(&session, info);
                Pkcs11CloseSession(token, &session);
            }
    #else
            ret = NOT_COMPILED_IN;
    #endif
        }
        else
            ret = NOT_COMPILED_IN;
    }

    return ret;
}

#endif /* HAVE_PKCS11 */
