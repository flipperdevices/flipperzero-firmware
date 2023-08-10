/* x509.c
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

#if !defined(WOLFSSL_X509_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning x509.c does not need to be compiled separately from ssl.c
    #endif
#else

#ifndef WOLFCRYPT_ONLY

#ifndef NO_CERTS

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    #include <wolfssl/openssl/x509v3.h>
#endif

#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA)
unsigned int wolfSSL_X509_get_extension_flags(WOLFSSL_X509* x509)
{
    unsigned int flags = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_extension_flags");

    if (x509 != NULL) {
        if (x509->keyUsageSet) {
            flags |= EXFLAG_KUSAGE;
        }
        if (x509->extKeyUsageSrc != NULL) {
            flags |= EXFLAG_XKUSAGE;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_X509_get_extension_flags", flags);

    return flags;
}

unsigned int wolfSSL_X509_get_key_usage(WOLFSSL_X509* x509)
{
    unsigned int ret = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_key_usage");

    if (x509 == NULL) {
        WOLFSSL_MSG("x509 is NULL");
    }
    else {
        if (x509->keyUsageSet) {
            ret = wolfSSL_X509_get_keyUsage(x509);
        }
        else {
            ret = (unsigned int)-1;
        }
    }

    WOLFSSL_LEAVE("wolfSSL_X509_get_key_usage", ret);

    return ret;
}

unsigned int wolfSSL_X509_get_extended_key_usage(WOLFSSL_X509* x509)
{
    int ret = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_extended_key_usage");

    if (x509 != NULL) {
        if (x509->extKeyUsage & EXTKEYUSE_OCSP_SIGN)
            ret |= XKU_OCSP_SIGN;
        if (x509->extKeyUsage & EXTKEYUSE_TIMESTAMP)
            ret |= XKU_TIMESTAMP;
        if (x509->extKeyUsage & EXTKEYUSE_EMAILPROT)
            ret |= XKU_SMIME;
        if (x509->extKeyUsage & EXTKEYUSE_CODESIGN)
            ret |= XKU_CODE_SIGN;
        if (x509->extKeyUsage & EXTKEYUSE_CLIENT_AUTH)
            ret |= XKU_SSL_CLIENT;
        if (x509->extKeyUsage & EXTKEYUSE_SERVER_AUTH)
            ret |= XKU_SSL_SERVER;
        if (x509->extKeyUsage & EXTKEYUSE_ANY)
            ret |= XKU_ANYEKU;
    }

    WOLFSSL_LEAVE("wolfSSL_X509_get_extended_key_usage", ret);

    return (unsigned int)ret;
}

/* Returns the number of X509V3 extensions in X509 object, or 0 on failure */
int wolfSSL_X509_get_ext_count(const WOLFSSL_X509* passedCert)
{
    int extCount = 0;
    int length = 0;
    int outSz = 0;
    const byte* rawCert;
    int sz = 0;
    word32 idx = 0;
    const byte* input;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert *cert;
#else
    DecodedCert cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_X509_get_ext_count");
    if (passedCert == NULL) {
        WOLFSSL_MSG("\tNot passed a certificate");
        return WOLFSSL_FAILURE;
    }

    rawCert = wolfSSL_X509_get_der((WOLFSSL_X509*)passedCert, &outSz);
    if (rawCert == NULL) {
        WOLFSSL_MSG("\tpassedCert has no internal DerBuffer set.");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert *)XMALLOC(sizeof(*cert), NULL, DYNAMIC_TYPE_DCERT);
    if (cert == NULL) {
        WOLFSSL_MSG("out of memory");
        return WOLFSSL_FAILURE;
    }
#endif

    InitDecodedCert(cert, rawCert, (word32)outSz, 0);

    if (ParseCert(cert,
#ifdef WOLFSSL_CERT_REQ
            passedCert->isCSR ? CERTREQ_TYPE :
#endif
                    CA_TYPE,
            NO_VERIFY, NULL) < 0) {
        WOLFSSL_MSG("\tCertificate parsing failed");
        goto out;
    }

    input = cert->extensions;
    sz = cert->extensionsSz;

    if (input == NULL || sz == 0) {
        WOLFSSL_MSG("\tsz or input NULL error");
        goto out;
    }

#ifdef WOLFSSL_CERT_REQ
    if (!passedCert->isCSR)
#endif
    {
        if (input[idx++] != ASN_EXTENSIONS) {
            WOLFSSL_MSG("\tfail: should be an EXTENSIONS");
            goto out;
        }

        if (GetLength(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: invalid length");
            goto out;
        }
    }

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE (1)");
        goto out;
    }

    while (idx < (word32)sz) {
        if (GetSequence(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: should be a SEQUENCE");
            FreeDecodedCert(cert);
            return WOLFSSL_FAILURE;
        }
        idx += length;
        extCount++;
    }

out:

    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif
    return extCount;
}

/* Creates and returns pointer to a new X509_EXTENSION object in memory */
WOLFSSL_X509_EXTENSION* wolfSSL_X509_EXTENSION_new(void)
{
    WOLFSSL_X509_EXTENSION* newExt;

    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_new");

    newExt = (WOLFSSL_X509_EXTENSION*)XMALLOC(sizeof(WOLFSSL_X509_EXTENSION),
              NULL, DYNAMIC_TYPE_X509_EXT);
    if (newExt == NULL)
        return NULL;
    XMEMSET(newExt, 0, sizeof(WOLFSSL_X509_EXTENSION));

    return newExt;
}


/* Clear out and free internal pointers of ASN.1 STRING object.
 *
 * @param [in] asn1  ASN.1 STRING object.
 */
static void wolfSSL_ASN1_STRING_clear(WOLFSSL_ASN1_STRING* asn1)
{
    /* Check we have an object to free. */
    if (asn1 != NULL) {
        /* Dispose of dynamic data. */
        if ((asn1->length > 0) && asn1->isDynamic) {
            XFREE(asn1->data, NULL, DYNAMIC_TYPE_OPENSSL);
        }
        XMEMSET(asn1, 0, sizeof(WOLFSSL_ASN1_STRING));
    }
}


void wolfSSL_X509_EXTENSION_free(WOLFSSL_X509_EXTENSION* x)
{
    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_free");
    if (x == NULL)
        return;

    if (x->obj != NULL) {
        wolfSSL_ASN1_OBJECT_free(x->obj);
    }

    wolfSSL_ASN1_STRING_clear(&x->value);
    wolfSSL_sk_pop_free(x->ext_sk, NULL);

    XFREE(x, NULL, DYNAMIC_TYPE_X509_EXT);
}

WOLFSSL_X509_EXTENSION* wolfSSL_X509_EXTENSION_dup(WOLFSSL_X509_EXTENSION* src)
{
    WOLFSSL_X509_EXTENSION* ret = NULL;
    int err = 0;

    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_dup");

    if (src == NULL) {
        err = 1;
    }

    if (err == 0) {
        ret = wolfSSL_X509_EXTENSION_new();
        if (ret == NULL) {
            err = 1;
        }
    }
    if (err == 0 && src->obj != NULL) {
        ret->obj = wolfSSL_ASN1_OBJECT_dup(src->obj);
        if (ret->obj == NULL) {
            err = 1;
        }
    }
    if (err == 0) {
        ret->crit = src->crit;
        if (wolfSSL_ASN1_STRING_copy(&ret->value, &src->value) !=
                WOLFSSL_SUCCESS) {
            err = 1;
        }
    }

    if (err == 1 && ret != NULL) {
        wolfSSL_X509_EXTENSION_free(ret);
        ret = NULL;
    }

    return ret;
}

WOLFSSL_X509_EXTENSION* wolfSSL_X509_EXTENSION_create_by_OBJ(
    WOLFSSL_X509_EXTENSION* ex, WOLFSSL_ASN1_OBJECT *obj, int crit,
    WOLFSSL_ASN1_STRING *data)
{
    int err = 0;
    WOLFSSL_X509_EXTENSION *ret = ex;

    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_create_by_OBJ");

    if ((obj == NULL) || (data == NULL)) {
       return NULL;
    }

    if (ret == NULL) {
        ret = wolfSSL_X509_EXTENSION_new();
        if (ret == NULL) {
            err = 1;
        }
    } else {
        /* Prevent potential memory leaks and dangling pointers. */
        wolfSSL_ASN1_OBJECT_free(ret->obj);
        ret->obj = NULL;
        wolfSSL_ASN1_STRING_clear(&ret->value);
    }

    if (err == 0) {
        ret->crit = crit;
        ret->obj = wolfSSL_ASN1_OBJECT_dup(obj);
        if (ret->obj == NULL) {
            err = 1;
        }
    }

    if (err == 0) {
        if (wolfSSL_ASN1_STRING_copy(&ret->value, data) != WOLFSSL_SUCCESS) {
            err = 1;
        }
    }

    if (err == 1) {
        if (ret != ex) {
            wolfSSL_X509_EXTENSION_free(ret);
        }
        ret = NULL;
    }
    return ret;
}

/* Creates and returns a new WOLFSSL_X509_EXTENSION stack. */
WOLFSSL_STACK* wolfSSL_sk_new_x509_ext(void)
{
    WOLFSSL_STACK* sk;
    WOLFSSL_ENTER("wolfSSL_sk_new_x509_ext");

    sk = wolfSSL_sk_new_null();
    if (sk) {
        sk->type = STACK_TYPE_X509_EXT;
    }
    return sk;
}

/* This function does NOT return 1 on success. It returns 0 on fail, and the
 * number of items in the stack upon success. This is for compatibility with
 * OpenSSL. */
int wolfSSL_sk_X509_EXTENSION_push(WOLFSSL_STACK* sk,WOLFSSL_X509_EXTENSION* ext)
{
    WOLFSSL_STACK* node;

    WOLFSSL_ENTER("wolfSSL_sk_X509_EXTENSION_push");

    if (sk == NULL || ext == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* no previous values in stack */
    if (sk->data.ext == NULL) {
        sk->data.ext = ext;
        sk->num += 1;
        return (int)sk->num;
    }

    /* stack already has value(s) create a new node and add more */
    node = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                                             DYNAMIC_TYPE_X509);
    if (node == NULL) {
        WOLFSSL_MSG("Memory error");
        return WOLFSSL_FAILURE;
    }
    XMEMSET(node, 0, sizeof(WOLFSSL_STACK));

    /* push new obj onto head of stack */
    node->data.ext  = sk->data.ext;
    node->next      = sk->next;
    node->type      = sk->type;
    sk->next        = node;
    sk->data.ext    = ext;
    sk->num        += 1;

    return (int)sk->num;
}

/* Free the structure for X509_EXTENSION stack
 *
 * sk  stack to free nodes in
 */
void wolfSSL_sk_X509_EXTENSION_free(WOLFSSL_STACK* sk)
{
    WOLFSSL_STACK* node;

    WOLFSSL_ENTER("wolfSSL_sk_X509_EXTENSION_free");

    if (sk == NULL) {
        return;
    }

    /* parse through stack freeing each node */
    node = sk->next;
    while ((node != NULL) && (sk->num > 1)) {
        WOLFSSL_STACK* tmp = node;
        node = node->next;

        wolfSSL_X509_EXTENSION_free(tmp->data.ext);
        XFREE(tmp, NULL, DYNAMIC_TYPE_X509);
        sk->num -= 1;
    }

    /* free head of stack */
    if (sk->num == 1) {
        wolfSSL_X509_EXTENSION_free(sk->data.ext);
    }
    XFREE(sk, NULL, DYNAMIC_TYPE_X509);
}

static WOLFSSL_STACK* generateExtStack(const WOLFSSL_X509 *x)
{
    int numOfExt, i;
    WOLFSSL_X509 *x509 = (WOLFSSL_X509*)x;
    WOLFSSL_STACK* ret;
    WOLFSSL_STACK* tmp;

    if (!x509) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    /* Save x509->ext_sk */
    tmp = x509->ext_sk;
    x509->ext_sk = NULL;
    numOfExt = wolfSSL_X509_get_ext_count(x509);

    for (i = 0; i < numOfExt; i++) {
        /* Build the extension stack */
        (void)wolfSSL_X509_set_ext(x509, i);
    }

    /* Restore */
    ret = x509->ext_sk;
    x509->ext_sk = tmp;
    return ret;
}

/**
 * @param x Certificate to extract extensions from
 * @return STACK_OF(X509_EXTENSION)*
 */
const WOLFSSL_STACK *wolfSSL_X509_get0_extensions(const WOLFSSL_X509 *x)
{
    int numOfExt;
    WOLFSSL_X509 *x509 = (WOLFSSL_X509*)x;
    WOLFSSL_ENTER("wolfSSL_X509_get0_extensions");

    if (!x509) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    numOfExt = wolfSSL_X509_get_ext_count(x509);

    if (numOfExt != wolfSSL_sk_num(x509->ext_sk_full)) {
        wolfSSL_sk_pop_free(x509->ext_sk_full, NULL);
        x509->ext_sk_full = generateExtStack(x);
    }

    return x509->ext_sk_full;
}

/**
 * Caller is responsible for freeing the returned stack.
 */
const WOLFSSL_STACK *wolfSSL_X509_REQ_get_extensions(const WOLFSSL_X509 *x)
{
    return generateExtStack(x);
}

/* Gets the X509_EXTENSION* ext based on it's location in WOLFSSL_X509* x509.
 *
 * x509   : The X509 structure to look for the extension.
 * loc    : Location of the extension. If the extension is found at the given
 * location, a new X509_EXTENSION structure is populated with extension-specific
 * data based on the extension type.

 * Returns NULL on error or pointer to X509_EXTENSION structure containing the
 * extension. The returned X509_EXTENSION should not be free'd by caller.
 * The returned X509_EXTENSION is pushed onto a stack inside the x509 argument.
 * This is later free'd when x509 is free'd.
 *
 * NOTE: for unknown extension NIDs, a X509_EXTENSION is populated with the
 * extension oid as the ASN1_OBJECT (QT compatibility)
 */
WOLFSSL_X509_EXTENSION* wolfSSL_X509_get_ext(const WOLFSSL_X509* x509, int loc)
{
    WOLFSSL_X509_EXTENSION* ext = NULL;
    WOLFSSL_ENTER("wolfSSL_X509_get_ext");
    if (x509 == NULL)
        return NULL;

   ext = wolfSSL_X509_set_ext((WOLFSSL_X509*) x509, loc);
   return ext;
}

int wolfSSL_X509_get_ext_by_OBJ(const WOLFSSL_X509 *x,
        const WOLFSSL_ASN1_OBJECT *obj, int lastpos)
{
    const WOLF_STACK_OF(WOLFSSL_X509_EXTENSION) *sk;

    if (!x || !obj) {
        WOLFSSL_MSG("Bad parameter");
        return -1;
    }

    sk = wolfSSL_X509_get0_extensions(x);
    if (!sk) {
        WOLFSSL_MSG("No extensions");
        return -1;
    }
    lastpos++;
    if (lastpos < 0)
        lastpos = 0;
    for (; lastpos < wolfSSL_sk_num(sk); lastpos++)
        if (wolfSSL_OBJ_cmp((WOLFSSL_ASN1_OBJECT*)wolfSSL_sk_value(sk,
                        lastpos), obj) == 0)
            return lastpos;
    return -1;
}

#endif /* OPENSSL_ALL || OPENSSL_EXTRA */

#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
    defined(WOLFSSL_WPAS_SMALL)
/* Set a general name from the DNS entry data.
 *
 * @param [in]      dns  DNS entry.
 * @param [in, out] gn   General name to place data in.
 * @return  1 on success.
 * @return  0 on failure.
 */
static int wolfssl_dns_entry_othername_to_gn(DNS_entry* dns,
    WOLFSSL_GENERAL_NAME* gn)
{
    int ret = 0;
    WOLFSSL_ASN1_OBJECT* obj;
    WOLFSSL_ASN1_TYPE* type;
    WOLFSSL_ASN1_STRING* str;
    byte tag;
    unsigned char* p = (unsigned char *)dns->name;
    long len = dns->len;

#ifdef WOLFSSL_FPKI
    if (dns->oidSum != 0) {
        /* UPN OID: 1.3.6.1.4.1.311.20.2.3 */
        static const unsigned char upn_oid[] = {
            0x2B, 0x06, 0x01, 0x04, 0x01, 0x82, 0x37, 0x14, 0x02, 0x03
        };
        const unsigned char* oid;
        word32 oidSz;

        if ((oid = OidFromId(dns->oidSum, oidCertAltNameType, &oidSz)) ==
                NULL) {
            if (dns->oidSum == UPN_OID) {
                oid = upn_oid;
                oidSz = (word32)sizeof(upn_oid);
            }
            else {
                goto err;
            }
        }
        if ((obj = wolfSSL_c2i_ASN1_OBJECT(NULL, &oid, oidSz)) == NULL) {
            goto err;
        }

        tag = ASN_UTF8STRING;
    }
    else
#endif
    {
        word32 idx = 0;
        int nameLen;

        /* Create an object id for general name from DER encoding. */
        obj = wolfSSL_d2i_ASN1_OBJECT(NULL, (const unsigned char**)&p, len);
        if (obj == NULL) {
            goto err;
        }
        /* Pointer moved on and now update length of remaining data. */
        len -= (long)((size_t)p - (size_t)dns->name);

        /* Next is: [0]. Check tag and length. */
        if (GetASNTag(p, &idx, &tag, (word32)len) < 0) {
            wolfSSL_ASN1_OBJECT_free(obj);
            goto err;
        }
        if (tag != (ASN_CONTEXT_SPECIFIC | ASN_CONSTRUCTED | 0)) {
            wolfSSL_ASN1_OBJECT_free(obj);
            goto err;
        }
        if (GetLength(p, &idx, &nameLen, (word32)len) <= 1) {
            wolfSSL_ASN1_OBJECT_free(obj);
            goto err;
        }

        /* Next is a string of some type. */
        if (GetASNTag(p, &idx, &tag, (word32)len) < 0) {
            wolfSSL_ASN1_OBJECT_free(obj);
            goto err;
        }
        if (GetLength(p, &idx, &nameLen, (word32)len) <= 0) {
            wolfSSL_ASN1_OBJECT_free(obj);
            goto err;
        }
        p += idx;
        len -= idx;
    }

    /* Create a WOLFSSL_ASN1_STRING from the DER. */
    str = wolfSSL_ASN1_STRING_type_new(tag);
    if (str == NULL) {
        wolfSSL_ASN1_OBJECT_free(obj);
        goto err;
    }
    wolfSSL_ASN1_STRING_set(str, p, (word32)len);

    /* Wrap string in a WOLFSSL_ASN1_TYPE. */
    type = wolfSSL_ASN1_TYPE_new();
    if (type == NULL) {
        wolfSSL_ASN1_OBJECT_free(obj);
        wolfSSL_ASN1_STRING_free(str);
        goto err;
    }
    wolfSSL_ASN1_TYPE_set(type, tag, str);

    /* Store the object and string in general name. */
    gn->d.otherName->type_id = obj;
    gn->d.otherName->value = type;

    ret = 1;
err:
    return ret;
}
#endif /* OPENSSL_ALL || WOLFSSL_WPAS_SMALL */

#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA)
static int wolfssl_x509_alt_names_to_gn(WOLFSSL_X509* x509,
    WOLFSSL_X509_EXTENSION* ext)
{
    int ret = 0;
    WOLFSSL_GENERAL_NAME* gn = NULL;
    DNS_entry* dns = NULL;
    WOLFSSL_STACK* sk;

#ifdef OPENSSL_ALL
    ret = wolfSSL_ASN1_STRING_set(&ext->value, x509->subjAltNameSrc,
              x509->subjAltNameSz);
    if (ret != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("ASN1_STRING_set() failed");
        goto err;
    }
#endif

    sk = (WOLFSSL_GENERAL_NAMES*)XMALLOC(sizeof(WOLFSSL_GENERAL_NAMES), NULL,
        DYNAMIC_TYPE_ASN1);
    if (sk == NULL) {
        goto err;
    }
    XMEMSET(sk, 0, sizeof(WOLFSSL_GENERAL_NAMES));
    sk->type = STACK_TYPE_GEN_NAME;

    if (x509->subjAltNameSet && x509->altNames != NULL) {
        /* alt names are DNS_entry structs */
        dns = x509->altNames;
        /* Currently only support GEN_DNS type */
        while (dns != NULL) {
            gn = wolfSSL_GENERAL_NAME_new();
            if (gn == NULL) {
                WOLFSSL_MSG("Error creating GENERAL_NAME");
                wolfSSL_sk_pop_free(sk, NULL);
                goto err;
            }

            gn->type = dns->type;
            if (gn->type == GEN_OTHERNAME) {
                if (!wolfssl_dns_entry_othername_to_gn(dns, gn)) {
                    WOLFSSL_MSG("OTHERNAME set failed");
                    wolfSSL_GENERAL_NAME_free(gn);
                    wolfSSL_sk_pop_free(sk, NULL);
                    goto err;
                }
            }
            else {
                gn->d.ia5->length = dns->len;
                if (wolfSSL_ASN1_STRING_set(gn->d.ia5, dns->name,
                        gn->d.ia5->length) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("ASN1_STRING_set failed");
                    wolfSSL_GENERAL_NAME_free(gn);
                    wolfSSL_sk_pop_free(sk, NULL);
                    goto err;
                }
            }

            if (wolfSSL_sk_GENERAL_NAME_push(sk, gn) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Error pushing onto stack");
                wolfSSL_GENERAL_NAME_free(gn);
                wolfSSL_sk_pop_free(sk, NULL);
                goto err;
            }

            dns = dns->next;
        }
    }
    ext->ext_sk = sk;
    ext->crit = x509->subjAltNameCrit;

    ret = 1;
err:
    return ret;
}

/* Pushes a new X509_EXTENSION* ext onto the stack inside WOLFSSL_X509* x509.
 * This is currently a helper function for wolfSSL_X509_get_ext
 * Caller does not free the returned WOLFSSL_X509_EXTENSION*
 */
WOLFSSL_X509_EXTENSION* wolfSSL_X509_set_ext(WOLFSSL_X509* x509, int loc)
{
    int extCount = 0, length = 0, outSz = 0, sz = 0, ret = 0;
    int objSz = 0, isSet = 0;
    const byte* rawCert;
    const byte* input;
    byte* oidBuf;
    word32 oid, idx = 0, tmpIdx = 0, nid;
    WOLFSSL_X509_EXTENSION* ext = NULL;
    WOLFSSL_ASN1_INTEGER* a;
    WOLFSSL_STACK* sk;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_X509_set_ext");

    if(x509 == NULL){
        WOLFSSL_MSG("\tNot passed a certificate");
        return NULL;
    }

    if(loc <0 || (loc > wolfSSL_X509_get_ext_count(x509))){
        WOLFSSL_MSG("\tBad location argument");
        return NULL;
    }

    ext = wolfSSL_X509_EXTENSION_new();
    if (ext == NULL) {
        WOLFSSL_MSG("\tX509_EXTENSION_new() failed");
        return NULL;
    }

    rawCert = wolfSSL_X509_get_der((WOLFSSL_X509*)x509, &outSz);
    if (rawCert == NULL) {
        WOLFSSL_MSG("\tX509_get_der() failed");
        wolfSSL_X509_EXTENSION_free(ext);
        return NULL;
    }

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL, DYNAMIC_TYPE_DCERT);
    if (cert == NULL) {
        WOLFSSL_MSG("Failed to allocate memory for DecodedCert");
        wolfSSL_X509_EXTENSION_free(ext);
        return NULL;
    }
#endif

    InitDecodedCert(cert, rawCert, (word32)outSz, 0);

    if (ParseCert(cert,
#ifdef WOLFSSL_CERT_REQ
            x509->isCSR ? CERTREQ_TYPE :
#endif
                    CA_TYPE,
            NO_VERIFY, NULL) < 0) {
        WOLFSSL_MSG("\tCertificate parsing failed");
        wolfSSL_X509_EXTENSION_free(ext);
        FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
        return NULL;
    }

    input = cert->extensions;
    sz = cert->extensionsSz;

    if (input == NULL || sz == 0) {
        WOLFSSL_MSG("\tfail: should be an EXTENSIONS");
        wolfSSL_X509_EXTENSION_free(ext);
        FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif
        return NULL;
    }

#ifdef WOLFSSL_CERT_REQ
    if (!x509->isCSR)
#endif
    {
        if (input[idx++] != ASN_EXTENSIONS) {
            WOLFSSL_MSG("\tfail: should be an EXTENSIONS");
            wolfSSL_X509_EXTENSION_free(ext);
            FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
            return NULL;
        }

        if (GetLength(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: invalid length");
            wolfSSL_X509_EXTENSION_free(ext);
            FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
            return NULL;
        }
    }

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE (1)");
        wolfSSL_X509_EXTENSION_free(ext);
        FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif
        return NULL;
    }

    while (idx < (word32)sz) {
        oid = 0;

        if (GetSequence(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: should be a SEQUENCE");
            wolfSSL_X509_EXTENSION_free(ext);
            FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
            return NULL;
        }

        tmpIdx = idx;
        ret = GetObjectId(input, &idx, &oid, oidCertExtType, sz);
        if (ret < 0) {
            WOLFSSL_MSG("\tfail: OBJECT ID");
            wolfSSL_X509_EXTENSION_free(ext);
            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
        #endif
            return NULL;
        }
        idx = tmpIdx;
        nid = (word32)oid2nid(oid, oidCertExtType);

        /* Continue while loop until extCount == loc or idx > sz */
        if (extCount != loc) {
            idx += length;
            extCount++;
            continue;
        }
        /* extCount == loc. Now get the extension. */
        /* Check if extension has been set */
        isSet = wolfSSL_X509_ext_isSet_by_NID((WOLFSSL_X509*)x509, nid);

        if (wolfSSL_OBJ_nid2ln(nid) != NULL) {
            /* This is NOT an unknown OID. */
            ext->obj = wolfSSL_OBJ_nid2obj(nid);
            if (ext->obj == NULL) {
                WOLFSSL_MSG("\tfail: Invalid OBJECT");
                wolfSSL_X509_EXTENSION_free(ext);
                FreeDecodedCert(cert);
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
            #endif
                return NULL;
            }
        }

        if (ext->obj) {
            ext->obj->nid = nid;
        }

        switch (oid) {
            case BASIC_CA_OID:
                if (!isSet)
                    break;
                /* Set pathlength */
                a = wolfSSL_ASN1_INTEGER_new();
                if (a == NULL) {
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                a->length = x509->pathLength;

                /* Save ASN1_INTEGER in x509 extension */
                ext->obj->pathlen = a;

                ext->obj->ca = x509->isCa;
                ext->crit = x509->basicConstCrit;
                break;

            case AUTH_INFO_OID:
                if (!isSet)
                    break;

                /* Create a stack to hold both the caIssuer and ocsp objects
                    in X509_EXTENSION structure */
                sk = wolfSSL_sk_new_asn1_obj();
                if (sk == NULL) {
                    WOLFSSL_MSG("Failed to malloc stack");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }

                /* Add CaIssuers object to stack */
                if (x509->authInfoCaIssuer != NULL &&
                    x509->authInfoCaIssuerSz > 0)
                {
                    WOLFSSL_ASN1_OBJECT* obj;
                    obj = wolfSSL_ASN1_OBJECT_new();
                    if (obj == NULL) {
                        WOLFSSL_MSG("Error creating ASN1 object");
                        wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                        wolfSSL_X509_EXTENSION_free(ext);
                        FreeDecodedCert(cert);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                    #endif
                        return NULL;
                    }
                    obj->obj = (byte*)x509->authInfoCaIssuer;
                    obj->objSz = x509->authInfoCaIssuerSz;
                    obj->grp = oidCertAuthInfoType;
                    obj->nid = NID_ad_ca_issuers;

                    ret = wolfSSL_sk_ASN1_OBJECT_push(sk, obj);
                    if (ret != WOLFSSL_SUCCESS) {
                        WOLFSSL_MSG("Error pushing ASN1 object onto stack");
                        wolfSSL_ASN1_OBJECT_free(obj);
                        wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                        wolfSSL_X509_EXTENSION_free(ext);
                        FreeDecodedCert(cert);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                    #endif
                        return NULL;
                    }
                }

                /* Add OCSP object to stack */
                if (x509->authInfo != NULL &&
                    x509->authInfoSz > 0)
                {
                    WOLFSSL_ASN1_OBJECT* obj;
                    obj = wolfSSL_ASN1_OBJECT_new();
                    if (obj == NULL) {
                        WOLFSSL_MSG("Error creating ASN1 object");
                        wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                        wolfSSL_X509_EXTENSION_free(ext);
                        FreeDecodedCert(cert);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                    #endif
                        return NULL;
                    }
                    obj->obj = x509->authInfo;
                    obj->objSz = x509->authInfoSz;
                    obj->grp = oidCertAuthInfoType;
                    obj->nid = NID_ad_OCSP;

                    ret = wolfSSL_sk_ASN1_OBJECT_push(sk, obj);
                    if (ret != WOLFSSL_SUCCESS) {
                        WOLFSSL_MSG("Error pushing ASN1 object onto stack");
                        wolfSSL_ASN1_OBJECT_free(obj);
                        wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                        wolfSSL_X509_EXTENSION_free(ext);
                        FreeDecodedCert(cert);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                    #endif
                        return NULL;
                    }
                }
                ext->ext_sk = sk;
                ext->crit = x509->authInfoCrit;
                break;

            case AUTH_KEY_OID:
                if (!isSet)
                    break;

                ret = wolfSSL_ASN1_STRING_set(&ext->value, x509->authKeyId,
                                        x509->authKeyIdSz);
                if (ret != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("ASN1_STRING_set() failed");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                ext->crit = x509->authKeyIdCrit;
                break;

            case SUBJ_KEY_OID:
                if (!isSet)
                    break;

                ret = wolfSSL_ASN1_STRING_set(&ext->value, x509->subjKeyId,
                                        x509->subjKeyIdSz);
                if (ret != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("ASN1_STRING_set() failed");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                ext->crit = x509->subjKeyIdCrit;
                break;

            case CERT_POLICY_OID:
                if (!isSet)
                    break;
                ext->crit = x509->certPolicyCrit;
                break;

            case KEY_USAGE_OID:
                if (!isSet)
                    break;

                ret = wolfSSL_ASN1_STRING_set(&ext->value,
                                  (byte*)&(x509->keyUsage), sizeof(word16));
                if (ret != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("ASN1_STRING_set() failed");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                ext->crit = x509->keyUsageCrit;
                break;

            case EXT_KEY_USAGE_OID:
                if (!isSet)
                    break;

                ret = wolfSSL_ASN1_STRING_set(&ext->value, x509->extKeyUsageSrc,
                                              x509->extKeyUsageSz);
                if (ret != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("ASN1_STRING_set() failed");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                ext->crit = x509->extKeyUsageCrit;
                break;

            case CRL_DIST_OID:
                if (!isSet)
                    break;
                ext->crit = x509->CRLdistCrit;
                break;

            case ALT_NAMES_OID:
                if (!isSet)
                    break;
                if (!wolfssl_x509_alt_names_to_gn(x509, ext)) {
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                break;

            default:
                WOLFSSL_MSG("Unknown extension type found, parsing OID");
                /* If the extension type is not recognized/supported,
                 *  set the ASN1_OBJECT in the extension with the
                 *  parsed oid for access in later function calls */

                /* Get OID from input */
                if (GetASNObjectId(input, &idx, &length, sz) != 0) {
                    WOLFSSL_MSG("Failed to Get ASN Object Id");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                oidBuf = (byte*)XMALLOC(length+1+MAX_LENGTH_SZ, NULL,
                                    DYNAMIC_TYPE_TMP_BUFFER);
                if (oidBuf == NULL) {
                    WOLFSSL_MSG("Failed to malloc tmp buffer");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                oidBuf[0] = ASN_OBJECT_ID;
                objSz++;
                objSz += SetLength(length, oidBuf + 1);
                objSz += length;

                /* Set object size and reallocate space in object buffer */
                if (ext->obj == NULL) {
                    ext->obj = wolfSSL_ASN1_OBJECT_new();
                    if (ext->obj == NULL) {
                        XFREE(oidBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                        wolfSSL_X509_EXTENSION_free(ext);
                        FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                        return NULL;
                    }
                }

                ext->obj->objSz = objSz;
                if(((ext->obj->dynamic & WOLFSSL_ASN1_DYNAMIC_DATA) != 0) ||
                   (ext->obj->obj == NULL)) {
                        ext->obj->obj =(byte*)XREALLOC((byte*)ext->obj->obj,
                                             ext->obj->objSz,
                                             NULL,DYNAMIC_TYPE_ASN1);
                    if (ext->obj->obj == NULL) {
                        wolfSSL_X509_EXTENSION_free(ext);
                        FreeDecodedCert(cert);
                        XFREE(oidBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                    #endif
                        return NULL;
                    }
                    ext->obj->dynamic |= WOLFSSL_ASN1_DYNAMIC_DATA;
                } else {
                    ext->obj->dynamic &= ~WOLFSSL_ASN1_DYNAMIC_DATA;
                }
                /* Get OID from input and copy to ASN1_OBJECT buffer */
                XMEMCPY(oidBuf+2, input+idx, length);
                XMEMCPY((byte*)ext->obj->obj, oidBuf, ext->obj->objSz);
                XFREE(oidBuf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                oidBuf = NULL;
                ext->obj->grp = oidCertExtType;
                ext->crit = 0;

                /* Get extension data and copy as ASN1_STRING */
                tmpIdx = idx + length;
                if ((tmpIdx >= (word32)sz) ||
                    (input[tmpIdx] != ASN_OCTET_STRING))
                {
                    WOLFSSL_MSG("Error decoding unknown extension data");
                    wolfSSL_ASN1_OBJECT_free(ext->obj);
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }

                tmpIdx++;

                if (GetLength(input, &tmpIdx, &length, sz) <= 0) {
                    WOLFSSL_MSG("Error: Invalid Input Length.");
                    wolfSSL_ASN1_OBJECT_free(ext->obj);
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                ext->value.data = (char*)XMALLOC(length, NULL, DYNAMIC_TYPE_ASN1);
                ext->value.isDynamic = 1;
                if (ext->value.data == NULL) {
                    WOLFSSL_MSG("Failed to malloc ASN1_STRING data");
                    wolfSSL_X509_EXTENSION_free(ext);
                    FreeDecodedCert(cert);
                #ifdef WOLFSSL_SMALL_STACK
                    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
                #endif
                    return NULL;
                }
                XMEMCPY(ext->value.data,input+tmpIdx,length);
                ext->value.length = length;
        } /* switch(oid) */

        break; /* Got the Extension. Now exit while loop. */

    } /* while(idx < sz) */

    /* Store the new extension in a stack inside x509
     * The extensions on the stack are free'd internally when FreeX509 is called
     */
    if (x509->ext_sk == NULL)
        x509->ext_sk = wolfSSL_sk_new_x509_ext();
    if (wolfSSL_sk_X509_EXTENSION_push(x509->ext_sk, ext) == WOLFSSL_FAILURE) {
        wolfSSL_X509_EXTENSION_free(ext);
        ext = NULL;
    }

    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif
    return ext;
}

/**
 * @param str String to copy
 * @param buf Output buffer. If this contains a pointer then it is free'd
 *            with the DYNAMIC_TYPE_X509_EXT hint.
 * @param len Output length
 * @return WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on error
 */
static int asn1_string_copy_to_buffer(WOLFSSL_ASN1_STRING* str, byte** buf,
        word32* len, void* heap) {
    if (!str || !buf || !len) {
        return WOLFSSL_FAILURE;
    }
    if (str->data && str->length > 0) {
        if (*buf)
            XFREE(*buf, heap, DYNAMIC_TYPE_X509_EXT);
        *len = 0;
        *buf = (byte*)XMALLOC(str->length, heap,
                DYNAMIC_TYPE_X509_EXT);
        if (!*buf) {
            WOLFSSL_MSG("malloc error");
            return WOLFSSL_FAILURE;
        }
        *len = str->length;
        XMEMCPY(*buf, str->data, str->length);
    }

    (void)heap;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_add_ext(WOLFSSL_X509 *x509, WOLFSSL_X509_EXTENSION *ext, int loc)
{
    int nid;

    WOLFSSL_ENTER("wolfSSL_X509_add_ext");

    if (!x509 || !ext || loc >= 0) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }
    nid = (ext->obj != NULL) ? ext->obj->type : ext->value.nid;

    switch (nid) {
    case NID_authority_key_identifier:
        if (x509->authKeyIdSrc != NULL) {
            /* If authKeyId points into authKeyIdSrc then free it and
             * revert to old functionality */
            XFREE(x509->authKeyIdSrc, x509->heap, DYNAMIC_TYPE_X509_EXT);
            x509->authKeyIdSrc = NULL;
            x509->authKeyId = NULL;
        }
        if (asn1_string_copy_to_buffer(&ext->value, &x509->authKeyId,
                &x509->authKeyIdSz, x509->heap) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("asn1_string_copy_to_buffer error");
            return WOLFSSL_FAILURE;
        }
        x509->authKeyIdCrit = (byte)ext->crit;
        break;
    case NID_subject_key_identifier:
        if (asn1_string_copy_to_buffer(&ext->value, &x509->subjKeyId,
                &x509->subjKeyIdSz, x509->heap) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("asn1_string_copy_to_buffer error");
            return WOLFSSL_FAILURE;
        }
        x509->subjKeyIdCrit = (byte)ext->crit;
        break;
    case NID_subject_alt_name:
    {
        WOLFSSL_GENERAL_NAMES* gns = ext->ext_sk;
        while (gns) {
            WOLFSSL_GENERAL_NAME* gn = gns->data.gn;
            if ((gn != NULL) && (gn->type == ASN_OTHER_TYPE)) {
                char *buf = NULL;
                int ret = 0;
                word32 len = 0;

                len = SetOthername(gn->d.otherName, NULL);
                if (len == WOLFSSL_FAILURE) {
                    return WOLFSSL_FAILURE;
                }

                buf = (char*)XMALLOC(len, x509->heap, DYNAMIC_TYPE_X509_EXT);
                if (buf == NULL) {
                    WOLFSSL_MSG("Couldn't allocate memory for othername");
                    return WOLFSSL_FAILURE;
                }

                /* SetOthername() cannot fail; already passed above. */
                SetOthername(gn->d.otherName, (byte*)buf);

                ret = wolfSSL_X509_add_altname_ex(x509, buf, len,
                                                  ASN_OTHER_TYPE);
                XFREE(buf, x509->heap, DYNAMIC_TYPE_X509_EXT);
                if (ret == WOLFSSL_FAILURE) {
                     WOLFSSL_MSG("wolfSSL_X509_add_altname_ex() failed");
                     return WOLFSSL_FAILURE;
                }
            }
            else if (!gn || !gn->d.ia5 ||
                wolfSSL_X509_add_altname_ex(x509, gn->d.ia5->data,
                    gn->d.ia5->length, gn->type) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Subject alternative name missing extension");
                return WOLFSSL_FAILURE;
            }
            gns = gns->next;
        }
        x509->subjAltNameSet = 1;
        x509->subjAltNameCrit = (byte)ext->crit;
        break;
    }
    case NID_key_usage:
        if (ext && ext->value.data) {
            if (ext->value.length == sizeof(word16)) {
                /* if ext->value is already word16, set directly */
                x509->keyUsage = *(word16*)ext->value.data;
                x509->keyUsageCrit = (byte)ext->crit;
                x509->keyUsageSet = 1;
            }
            else if (ext->value.length > 0) {
                /* ext->value is comma-delimited string, convert to word16 */
                if (ParseKeyUsageStr(ext->value.data, &x509->keyUsage,
                                     x509->heap) != 0) {
                    return WOLFSSL_FAILURE;
                }
                x509->keyUsageCrit = (byte)ext->crit;
                x509->keyUsageSet = 1;
            }
            else {
                return WOLFSSL_FAILURE;
            }
        }
        break;
    case NID_ext_key_usage:
        if (ext && ext->value.data) {
            if (ext->value.length == sizeof(byte)) {
                /* if ext->value is already word16, set directly */
                x509->extKeyUsage = *(byte*)ext->value.data;
                x509->extKeyUsageCrit = (byte)ext->crit;
            }
            else if (ext->value.length > 0) {
                /* ext->value is comma-delimited string, convert to word16 */
                if (ParseExtKeyUsageStr(ext->value.data, &x509->extKeyUsage,
                                        x509->heap) != 0) {
                    return WOLFSSL_FAILURE;
                }
                x509->extKeyUsageCrit = (byte)ext->crit;
            }
            else {
                return WOLFSSL_FAILURE;
            }
        }
        break;
    case NID_basic_constraints:
        if (ext->obj) {
            x509->isCa = (byte)ext->obj->ca;
            x509->basicConstCrit = (byte)ext->crit;
            if (ext->obj->pathlen)
                x509->pathLength = ext->obj->pathlen->length;
            x509->basicConstSet = 1;
        }
        break;
    default:
#ifdef WOLFSSL_CUSTOM_OID
        if ((ext->obj == NULL) || (ext->value.length == 0)) {
            WOLFSSL_MSG("Extension has insufficient information.");
            return WOLFSSL_FAILURE;
        }

        if ((x509->customExtCount < 0) ||
            (x509->customExtCount >= NUM_CUSTOM_EXT)) {
            WOLFSSL_MSG("Bad value for customExtCount.");
            return WOLFSSL_FAILURE;
        }

        /* This is a viable custom extension. */
        char *oid = XMALLOC(MAX_OID_STRING_SZ, x509->heap,
                            DYNAMIC_TYPE_X509_EXT);
        byte *val = XMALLOC(ext->value.length, x509->heap,
                            DYNAMIC_TYPE_X509_EXT);
        int err = 0;

        if ((oid == NULL) || (val == NULL)) {
            WOLFSSL_MSG("Memory allocation failure.\n");
            err = 1;
        }

        if (err == 0) {
            XMEMCPY(val, ext->value.data, ext->value.length);
            if (wolfSSL_OBJ_obj2txt(oid, MAX_OID_STRING_SZ, ext->obj, 1) < 0) {
                err = 1;
            }
        }

        if (err == 1) {
            XFREE(val, x509->heap, DYNAMIC_TYPE_X509_EXT);
            XFREE(oid, x509->heap, DYNAMIC_TYPE_X509_EXT);
            return WOLFSSL_FAILURE;
        }

        /* x509->custom_exts now owns the buffers and they must be managed. */
        x509->custom_exts[x509->customExtCount].oid = oid;
        x509->custom_exts[x509->customExtCount].crit = ext->crit;
        x509->custom_exts[x509->customExtCount].val = val;
        x509->custom_exts[x509->customExtCount].valSz = ext->value.length;
        x509->customExtCount++;
#else
        WOLFSSL_MSG("Unsupported extension to add");
        return WOLFSSL_FAILURE;
#endif /* WOLFSSL_CUSTOM_OID */
        break;
    }

    return WOLFSSL_SUCCESS;
}

#ifndef NO_BIO
/* Return 0 on success and 1 on failure. Copies ext data to bio, using indent
 *  to pad the output. flag is ignored. */
int wolfSSL_X509V3_EXT_print(WOLFSSL_BIO *out, WOLFSSL_X509_EXTENSION *ext,
        unsigned long flag, int indent)
{
    ASN1_OBJECT* obj;
    ASN1_STRING* str;
    int nid;
    int rc = WOLFSSL_FAILURE;
    char tmp[CTC_NAME_SIZE*2 + 1];
    const int tmpSz = sizeof(tmp);
    int tmpLen = 0;
    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_print");

    if ((out == NULL) || (ext == NULL)) {
        WOLFSSL_MSG("NULL parameter error");
        return rc;
    }

    obj = wolfSSL_X509_EXTENSION_get_object(ext);
    if (obj == NULL) {
        WOLFSSL_MSG("Error getting ASN1_OBJECT from X509_EXTENSION");
        return rc;
    }

    str = wolfSSL_X509_EXTENSION_get_data(ext);
    if (str == NULL) {
        WOLFSSL_MSG("Error getting ASN1_STRING from X509_EXTENSION");
        return rc;
    }

    /* Print extension based on the type */
    nid = wolfSSL_OBJ_obj2nid(obj);
    switch (nid) {
        case BASIC_CA_OID:
        {
            char isCa[] = "TRUE";
            char notCa[] = "FALSE";
            if ((tmpLen = XSNPRINTF(tmp, tmpSz, "%*sCA:%s", indent, "",
                                     obj->ca ? isCa : notCa))
                >= tmpSz)
                return rc;
            break;
        }
        case ALT_NAMES_OID:
        {
            WOLFSSL_STACK* sk;
            char* val;
            int valLen;
            int len;

            sk = ext->ext_sk;
            while (sk != NULL) {
                if (sk->type == STACK_TYPE_GEN_NAME && sk->data.gn) {
                    /* str is GENERAL_NAME for subject alternative name ext */
                    str = sk->data.gn->d.ia5;
                    len = str->length + 2; /* + 2 for NULL char and "," */
                    if (len > tmpSz) {
                        WOLFSSL_MSG("len greater than buffer size");
                        return rc;
                    }

                    val = (char*)XMALLOC(len + indent, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
                    if (val == NULL) {
                        WOLFSSL_MSG("Memory error");
                        return rc;
                    }
                    if (sk->next) {
                        if ((valLen = XSNPRINTF(val, len, "%*s%s,",
                                      indent, "", str->strData))
                            >= len)
                            return rc;
                    } else {
                        if ((valLen = XSNPRINTF(val, len, "%*s%s",
                                      indent, "", str->strData))
                            >= len)
                            return rc;
                    }
                    if (tmpLen + valLen > tmpSz) {
                        XFREE(val, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                        return rc;
                    }
                    XMEMCPY(tmp + tmpLen, val, valLen);
                    tmpLen += valLen;
                    XFREE(val, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                }
                sk = sk->next;
            }
            break;
        }
        case AUTH_KEY_OID:
        case SUBJ_KEY_OID:
        {
            char* asn1str;
            asn1str = wolfSSL_i2s_ASN1_STRING(NULL, str);
            if ((tmpLen = XSNPRINTF(
                     tmp, tmpSz, "%*s%s", indent, "", asn1str))
                >= tmpSz)
                return rc;
            XFREE(asn1str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            break;
        }
        case AUTH_INFO_OID:
        case CERT_POLICY_OID:
        case CRL_DIST_OID:
        case KEY_USAGE_OID:
            WOLFSSL_MSG("X509V3_EXT_print not yet implemented for ext type");
            break;

        default:
            if ((tmpLen = XSNPRINTF(
                     tmp, tmpSz, "%*s%s", indent, "", str->strData))
                >= tmpSz)
                return rc;
    }

    if (wolfSSL_BIO_write(out, tmp, tmpLen) == tmpLen) {
        rc = WOLFSSL_SUCCESS;
    }
    (void) flag;

    return rc;
}
#endif /* !NO_BIO */

#ifndef NO_WOLFSSL_STUB
int wolfSSL_X509V3_EXT_add_nconf(WOLFSSL_CONF *conf, WOLFSSL_X509V3_CTX *ctx,
        const char *section, WOLFSSL_X509 *cert)
{
    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_add_nconf");
    WOLFSSL_STUB("wolfSSL_X509V3_EXT_add_nconf");
    (void)conf;
    (void)ctx;
    (void)section;
    (void)cert;
    return WOLFSSL_SUCCESS;
}
#endif

/* Returns crit flag in X509_EXTENSION object */
int wolfSSL_X509_EXTENSION_get_critical(const WOLFSSL_X509_EXTENSION* ex)
{
    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_get_critical");
    if (ex == NULL)
        return BAD_FUNC_ARG;
    return ex->crit;
}

/* Sets if the extension is critical
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_X509_EXTENSION_set_critical(WOLFSSL_X509_EXTENSION* ex, int crit)
{
    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_set_critical");
    if (ex == NULL)
        return WOLFSSL_FAILURE;
    ex->crit = crit;
    return WOLFSSL_SUCCESS;
}

/* Creates v3_ext_method for a given X509v3 extension
 *
 * ex   : The X509_EXTENSION used to create v3_ext_method. If the extension is
 * not NULL, get the NID of the extension object and populate the
 * extension type-specific X509V3_EXT_* function(s) in v3_ext_method.
 *
 * Returns NULL on error or pointer to the v3_ext_method populated with extension
 * type-specific X509V3_EXT_* function(s).
 *
 * NOTE: NID_subject_key_identifier is currently the only extension implementing
 * the X509V3_EXT_* functions, as it is the only type called directly by QT. The
 * other extension types return a pointer to a v3_ext_method struct that contains
 * only the NID.
 */
#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
const WOLFSSL_v3_ext_method* wolfSSL_X509V3_EXT_get(WOLFSSL_X509_EXTENSION* ex)
#else
WOLFSSL_v3_ext_method* wolfSSL_X509V3_EXT_get(WOLFSSL_X509_EXTENSION* ex)
#endif
{
    int nid;
    WOLFSSL_v3_ext_method method;

    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_get");
    if ((ex == NULL) || (ex->obj == NULL)) {
        WOLFSSL_MSG("Passed an invalid X509_EXTENSION*");
        return NULL;
    }
    /* Initialize method to 0 */
    XMEMSET(&method, 0, sizeof(struct WOLFSSL_v3_ext_method));

    nid = ex->obj->nid;
    if (nid <= 0) {
        WOLFSSL_MSG("Failed to get nid from passed extension object");
        return NULL;
    }
    XMEMSET(&method, 0, sizeof(WOLFSSL_v3_ext_method));
    switch (nid) {
        case NID_basic_constraints:
            break;
        case NID_subject_key_identifier:
            method.i2s = (X509V3_EXT_I2S)wolfSSL_i2s_ASN1_STRING;
            break;
        case NID_subject_alt_name:
            WOLFSSL_MSG("i2v function not yet implemented for Subject Alternative Name");
            break;
        case NID_key_usage:
            WOLFSSL_MSG("i2v function not yet implemented for Key Usage");
            break;
        case NID_authority_key_identifier:
            WOLFSSL_MSG("i2v function not yet implemented for Auth Key Id");
            break;
        case NID_info_access:
            WOLFSSL_MSG("i2v function not yet implemented for Info Access");
            break;
        case NID_ext_key_usage:
            WOLFSSL_MSG("i2v function not yet implemented for Ext Key Usage");
            break;
        case NID_certificate_policies:
            WOLFSSL_MSG("r2i function not yet implemented for Cert Policies");
            break;
        case NID_crl_distribution_points:
            WOLFSSL_MSG("r2i function not yet implemented for CRL Dist Points");
            break;
        default:
            /* If extension type is unknown, return NULL -- QT makes call to
                X509_EXTENSION_get_data() if there is no v3_ext_method */
            WOLFSSL_MSG("X509V3_EXT_get(): Unknown extension type found");
            return NULL;
    }

    method.ext_nid = nid;
    ex->ext_method = method;

#if defined(OPENSSL_VERSION_NUMBER) && OPENSSL_VERSION_NUMBER >= 0x10100000L
    return (const WOLFSSL_v3_ext_method*)&ex->ext_method;
#else
    return (WOLFSSL_v3_ext_method*)&ex->ext_method;
#endif
}

/* Create an Authority Info Access (AIA) from the contents of the extension.
 *
 * AIA is a stack of Access Descriptions.
 *
 * RFC 5280: 4.2.2.1
 *
 * @param [in] ext  X509v3 extension.
 * @return  Stack of Access Descriptions as an AIA on success.
 * @return  NULL on error.
 */
static WOLFSSL_AUTHORITY_INFO_ACCESS* wolfssl_x509v3_ext_aia_d2i(
    WOLFSSL_X509_EXTENSION* ext)
{
    int err = 0;
    int ret;
    WOLFSSL_AUTHORITY_INFO_ACCESS* aia = NULL;
    WOLFSSL_STACK* sk;
    WOLFSSL_ACCESS_DESCRIPTION* ad = NULL;

    /* Get the type specific data of this extension. */
    sk = ext->ext_sk;
    if (sk == NULL) {
        WOLFSSL_MSG("ACCESS_DESCRIPTION stack NULL");
        err = 1;
    }

    if (!err) {
        /* AUTHORITY_INFO_ACCESS is a stack of ACCESS_DESCRIPTION entries. */
        aia = wolfSSL_sk_new_null();
        if (aia == NULL) {
            WOLFSSL_MSG("Failed to malloc AUTHORITY_INFO_ACCESS");
            err = 1;
        }
    }
    if (!err) {
        /* AIA is a stack of Access Descriptions. */
        aia->type = STACK_TYPE_ACCESS_DESCRIPTION;
    }

    while ((!err) && (sk != NULL)) {
        WOLFSSL_ASN1_OBJECT* aiaEntry;

        /* Looking for objects in extension's data. */
        if (sk->type != STACK_TYPE_OBJ) {
            sk = sk->next;
            continue;
        }

        /* Get ASN.1 Object from the stack entry's data. */
        aiaEntry = sk->data.obj;

        /* ACCESS_DESCRIPTION has two members: method and location.
         *  method: ASN1_OBJECT as either AIA_OCSP_OID or AIA_CA_ISSUER_OID
         *  location: GENERAL_NAME structure containing the URI.
         */

        /* Allocate a new Access Description. */
        ad = (WOLFSSL_ACCESS_DESCRIPTION*)XMALLOC(
            sizeof(WOLFSSL_ACCESS_DESCRIPTION), NULL, DYNAMIC_TYPE_X509_EXT);
        if (ad == NULL) {
            WOLFSSL_MSG("Failed to malloc ACCESS_DESCRIPTION");
            err = 1;
            break;
        }
        XMEMSET(ad, 0, sizeof(WOLFSSL_ACCESS_DESCRIPTION));

        /* Create new ASN1_OBJECT from NID. */
        ad->method = wolfSSL_OBJ_nid2obj(aiaEntry->nid);
        if (ad->method == NULL) {
            WOLFSSL_MSG("OBJ_nid2obj() failed");
            err = 1;
            break;
        }

        /* Allocate memory for GENERAL NAME. */
        ad->location = wolfSSL_GENERAL_NAME_new();
        if (ad->location == NULL) {
            WOLFSSL_MSG("Failed to malloc GENERAL_NAME");
            err = 1;
            break;
        }

        /* Set the type of general name to URI (only type supported). */
        ret = wolfSSL_GENERAL_NAME_set_type(ad->location, GEN_URI);
        if (ret != WOLFSSL_SUCCESS) {
            err = 1;
            break;
        }

        /* Set the URI into GENERAL_NAME. */
        ret = wolfSSL_ASN1_STRING_set(ad->location->d.uniformResourceIdentifier,
            aiaEntry->obj, aiaEntry->objSz);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("ASN1_STRING_set() failed");
            err = 1;
            break;
        }
        /* Push onto AUTHORITY_INFO_ACCESS stack. */
        ret = wolfSSL_sk_ACCESS_DESCRIPTION_push(aia, ad);
        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Error pushing ASN1 AD onto stack");
            err = 1;
            break;
        }
        /* Set to NULL so that it doesn't get freed now it is in AIA stack. */
        ad = NULL;

        sk = sk->next;
    }

    if (err) {
        /* Dispose of Access Description if not put in stack. */
        if (ad != NULL) {
            wolfSSL_ASN1_OBJECT_free(ad->method);
            wolfSSL_GENERAL_NAME_free(ad->location);
            XFREE(ad, NULL, DYNAMIC_TYPE_X509_EXT);
        }
        /* Dispose of incomplete Access Description stack. */
        wolfSSL_sk_ACCESS_DESCRIPTION_pop_free(aia, NULL);
        aia = NULL;
    }
    return aia;
}

/* Parses and returns an x509v3 extension internal structure.
 *
 * ext   : The X509_EXTENSION for parsing internal structure. If extension is
 * not NULL, get the NID of the extension object and create a new
 * extension-specific internal structure based on the extension type.
 *
 * Returns NULL on error or if NID is not found, otherwise returns a pointer to
 * the extension type-specific X509_EXTENSION internal structure.
 * Return is expected to be free'd by caller.
 */
void* wolfSSL_X509V3_EXT_d2i(WOLFSSL_X509_EXTENSION* ext)
{
    const WOLFSSL_v3_ext_method* method;
    int ret;
    WOLFSSL_ASN1_OBJECT* object;
    WOLFSSL_BASIC_CONSTRAINTS* bc;
    WOLFSSL_AUTHORITY_KEYID* akey;
    WOLFSSL_ASN1_STRING* asn1String, *newString;
    WOLFSSL_STACK* sk;

    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_d2i");

    if(ext == NULL) {
        WOLFSSL_MSG("Bad function Argument");
        return NULL;
    }

    /* extract extension info */
    method = wolfSSL_X509V3_EXT_get(ext);
    if (method == NULL) {
        WOLFSSL_MSG("wolfSSL_X509V3_EXT_get error");
        return NULL;
    }
    object = wolfSSL_X509_EXTENSION_get_object(ext);
    if (object == NULL) {
        WOLFSSL_MSG("X509_EXTENSION_get_object failed");
        return NULL;
    }

    /* Return pointer to proper internal structure based on NID */
    switch (object->type) {
        /* basicConstraints */
        case (NID_basic_constraints):
            WOLFSSL_MSG("basicConstraints");
            /* Allocate new BASIC_CONSTRAINTS structure */
            bc = wolfSSL_BASIC_CONSTRAINTS_new();
            if (bc == NULL) {
                WOLFSSL_MSG("Failed to malloc basic constraints");
                return NULL;
            }
            /* Copy pathlen and CA into BASIC_CONSTRAINTS from object */
            bc->ca = object->ca;
            if (object->pathlen->length > 0) {
                bc->pathlen = wolfSSL_ASN1_INTEGER_dup(object->pathlen);
                if (bc->pathlen == NULL) {
                    WOLFSSL_MSG("Failed to duplicate ASN1_INTEGER");
                    wolfSSL_BASIC_CONSTRAINTS_free(bc);
                    return NULL;
                }
            }
            else
                bc->pathlen = NULL;
            return bc;

        /* subjectKeyIdentifier */
        case (NID_subject_key_identifier):
            WOLFSSL_MSG("subjectKeyIdentifier");
            asn1String = wolfSSL_X509_EXTENSION_get_data(ext);
            if (asn1String == NULL) {
                WOLFSSL_MSG("X509_EXTENSION_get_data() failed");
                return NULL;
            }
            newString = wolfSSL_ASN1_STRING_new();
            if (newString == NULL) {
                WOLFSSL_MSG("Failed to malloc ASN1_STRING");
                return NULL;
            }
            ret = wolfSSL_ASN1_STRING_set(newString, asn1String->data,
                                                            asn1String->length);
            if (ret != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("ASN1_STRING_set() failed");
                wolfSSL_ASN1_STRING_free(newString);
                return NULL;
            };
            newString->type = asn1String->type;
            return newString;

        /* authorityKeyIdentifier */
        case (NID_authority_key_identifier):
            WOLFSSL_MSG("AuthorityKeyIdentifier");

            akey = (WOLFSSL_AUTHORITY_KEYID*)
                    XMALLOC(sizeof(WOLFSSL_AUTHORITY_KEYID), NULL,
                    DYNAMIC_TYPE_X509_EXT);
            if (akey == NULL) {
                WOLFSSL_MSG("Failed to malloc authority key id");
                return NULL;
            }

            XMEMSET(akey, 0, sizeof(WOLFSSL_AUTHORITY_KEYID));

            akey->keyid = wolfSSL_ASN1_STRING_new();
            if (akey->keyid == NULL) {
                WOLFSSL_MSG("ASN1_STRING_new() failed");
                wolfSSL_AUTHORITY_KEYID_free(akey);
                return NULL;
            }

            asn1String = wolfSSL_X509_EXTENSION_get_data(ext);
            if (asn1String == NULL) {
                WOLFSSL_MSG("X509_EXTENSION_get_data() failed");
                wolfSSL_AUTHORITY_KEYID_free(akey);
                return NULL;
            }

            ret = wolfSSL_ASN1_STRING_set(akey->keyid, asn1String->data,
                                                            asn1String->length);
            if (ret != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("ASN1_STRING_set() failed");
                wolfSSL_AUTHORITY_KEYID_free(akey);
                return NULL;
            };
            akey->keyid->type   = asn1String->type;

            /* For now, set issuer and serial to NULL. This may need to be
                updated for future use */
            akey->issuer = NULL;
            akey->serial = NULL;
            return akey;

        /* keyUsage */
        case (NID_key_usage):
            WOLFSSL_MSG("keyUsage");
            /* This may need to be updated for future use. The i2v method for
                keyUsage is not currently set. For now, return the ASN1_STRING
                representation of KeyUsage bit string */
            asn1String = wolfSSL_X509_EXTENSION_get_data(ext);
            if (asn1String == NULL) {
                WOLFSSL_MSG("X509_EXTENSION_get_data() failed");
                return NULL;
            }
            newString = wolfSSL_ASN1_STRING_new();
            if (newString == NULL) {
                WOLFSSL_MSG("Failed to malloc ASN1_STRING");
                return NULL;
            }
            ret = wolfSSL_ASN1_STRING_set(newString, asn1String->data,
                                                            asn1String->length);
            if (ret != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("ASN1_STRING_set() failed");
                wolfSSL_ASN1_STRING_free(newString);
                return NULL;
            };
            newString->type = asn1String->type;
            return newString;

        /* extKeyUsage */
        case (NID_ext_key_usage):
            WOLFSSL_MSG("extKeyUsage not supported yet");
            return NULL;

        /* certificatePolicies */
        case (NID_certificate_policies):
            WOLFSSL_MSG("certificatePolicies not supported yet");
            return NULL;

        /* cRLDistributionPoints */
        case (NID_crl_distribution_points):
            WOLFSSL_MSG("cRLDistributionPoints not supported yet");
            return NULL;

        case NID_subject_alt_name:
            if (ext->ext_sk == NULL) {
                WOLFSSL_MSG("Subject alt name stack NULL");
                return NULL;
            }
            sk = wolfSSL_sk_dup(ext->ext_sk);
            if (sk == NULL) {
                WOLFSSL_MSG("Failed to duplicate subject alt names stack.");
                return NULL;
            }
            return sk;

        /* authorityInfoAccess */
        case NID_info_access:
            WOLFSSL_MSG("AuthorityInfoAccess");
            return wolfssl_x509v3_ext_aia_d2i(ext);

        default:
            WOLFSSL_MSG("Extension NID not in table, returning NULL");
            break;
    }
    return NULL;
}

/* Looks for the extension matching the passed in nid
 *
 * x509 : certificate to get parse through for extension.
 * nid : Extension OID to be found.
 * lastPos : Start search from extension after lastPos.
 *           Set to -1 to search from index 0.
 * return >= 0 If successful the extension index is returned.
 * return -1 If extension is not found or error is encountered.
 */
int wolfSSL_X509_get_ext_by_NID(const WOLFSSL_X509* x509, int nid, int lastPos)
{
    int extCount = 0, length = 0, outSz = 0, sz = 0, ret = 0;
    int isSet = 0, found = 0, loc;
    const byte* rawCert;
    const byte* input;
    word32 oid, idx = 0, tmpIdx = 0, foundNID;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert *cert;
#else
    DecodedCert cert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_X509_get_ext_by_NID");

    if(x509 == NULL){
        WOLFSSL_MSG("\tNot passed a certificate");
        return WOLFSSL_FATAL_ERROR;
    }

    if(lastPos < -1 || (lastPos > (wolfSSL_X509_get_ext_count(x509) - 1))){
        WOLFSSL_MSG("\tBad location argument");
        return WOLFSSL_FATAL_ERROR;
    }

    loc = lastPos + 1;

    rawCert = wolfSSL_X509_get_der((WOLFSSL_X509*)x509, &outSz);
    if (rawCert == NULL) {
        WOLFSSL_MSG("\tX509_get_der() failed");
        return WOLFSSL_FATAL_ERROR;
    }

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert *)XMALLOC(sizeof(*cert), x509->heap,
                                  DYNAMIC_TYPE_DCERT);
    if (cert == NULL) {
        WOLFSSL_MSG("\tout of memory");
        return WOLFSSL_FATAL_ERROR;
    }
#endif

    InitDecodedCert( cert, rawCert, (word32)outSz, 0);

    if (ParseCert(cert,
#ifdef WOLFSSL_CERT_REQ
            x509->isCSR ? CERTREQ_TYPE :
#endif
            CA_TYPE,
            NO_VERIFY, NULL) < 0) {
        WOLFSSL_MSG("\tCertificate parsing failed");
        goto out;
    }

    input = cert->extensions;
    sz = cert->extensionsSz;

    if (input == NULL || sz == 0) {
        WOLFSSL_MSG("\tfail: should be an EXTENSIONS");
        goto out;
    }

#ifdef WOLFSSL_CERT_REQ
    if (!x509->isCSR)
#endif
    {
        if (input[idx++] != ASN_EXTENSIONS) {
            WOLFSSL_MSG("\tfail: should be an EXTENSIONS");
            goto out;
        }

        if (GetLength(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: invalid length");
            goto out;
        }
    }

    if (GetSequence(input, &idx, &length, sz) < 0) {
        WOLFSSL_MSG("\tfail: should be a SEQUENCE (1)");
        goto out;
    }

    while (idx < (word32)sz) {
        oid = 0;

        if (GetSequence(input, &idx, &length, sz) < 0) {
            WOLFSSL_MSG("\tfail: should be a SEQUENCE");
            goto out;
        }

        tmpIdx = idx;
        ret = GetObjectId(input, &idx, &oid, oidCertExtType, sz);
        if (ret < 0) {
            WOLFSSL_MSG("\tfail: OBJECT ID");
            goto out;
        }
        idx = tmpIdx;
        foundNID = (word32)oid2nid(oid, oidCertExtType);

        if (extCount >= loc) {
            /* extCount >= loc. Now check if extension has been set */
            isSet = wolfSSL_X509_ext_isSet_by_NID((WOLFSSL_X509*)x509, foundNID);

            if (isSet && ((word32)nid == foundNID)) {
                found = 1;
                break;
            }
        }

        idx += length;
        extCount++;
    } /* while(idx < sz) */

out:

    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, x509->heap, DYNAMIC_TYPE_DCERT);
#endif

    return found ? extCount : WOLFSSL_FATAL_ERROR;
}

#endif /* OPENSSL_ALL || OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
/* Looks for the extension matching the passed in nid
 *
 * c   : if not null then is set to status value -2 if multiple occurrences
 *       of the extension are found, -1 if not found, 0 if found and not
 *       critical, and 1 if found and critical.
 * nid : Extension OID to be found.
 * idx : if NULL return first extension found match, otherwise start search at
 *       idx location and set idx to the location of extension returned.
 * returns NULL or a pointer to an WOLFSSL_ASN1_BIT_STRING (for KEY_USAGE_OID)
 * or WOLFSSL_STACK (for other)
 * holding extension structure
 *
 * NOTE code for decoding extensions is in asn.c DecodeCertExtensions --
 * use already decoded extension in this function to avoid decoding twice.
 * Currently we do not make use of idx since getting pre decoded extensions.
 */
void* wolfSSL_X509_get_ext_d2i(const WOLFSSL_X509* x509, int nid, int* c,
    int* idx)
{
    void* ret = NULL;
    WOLFSSL_STACK* sk = NULL;
    WOLFSSL_ASN1_OBJECT* obj = NULL;
    WOLFSSL_GENERAL_NAME* gn = NULL;
#ifdef OPENSSL_EXTRA
    WOLFSSL_DIST_POINT* dp = NULL;
#endif
    WOLFSSL_BASIC_CONSTRAINTS* bc = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_get_ext_d2i");

    if (x509 == NULL) {
        return NULL;
    }

    if (c != NULL) {
        *c = -1; /* default to not found */
    }

    switch (nid) {
        case BASIC_CA_OID:
            if (x509->basicConstSet) {
                WOLFSSL_ASN1_INTEGER* a;

                bc = wolfSSL_BASIC_CONSTRAINTS_new();
                if (!bc) {
                    WOLFSSL_MSG("wolfSSL_BASIC_CONSTRAINTS_new error");
                    return NULL;
                }

                a = wolfSSL_ASN1_INTEGER_new();
                if (!a) {
                    WOLFSSL_MSG("wolfSSL_ASN1_INTEGER_new error");
                    wolfSSL_BASIC_CONSTRAINTS_free(bc);
                    return NULL;
                }
                a->length = x509->pathLength;

#if defined(OPENSSL_ALL) || defined(WOLFSSL_QT) || \
        defined(WOLFSSL_APACHE_HTTPD)
                bc->ca = x509->isCa;
#endif
                bc->pathlen = a;
                if (c != NULL) {
                    *c = x509->basicConstCrit;
                }
            }
            else {
                WOLFSSL_MSG("No Basic Constraint set");
            }
            return bc;

        case ALT_NAMES_OID:
        {
            DNS_entry* dns = NULL;

            if (x509->subjAltNameSet && x509->altNames != NULL) {
                /* Malloc GENERAL_NAME stack */
                sk = wolfSSL_sk_new_null();
                if (sk == NULL)
                    return NULL;
                sk->type = STACK_TYPE_GEN_NAME;

                /* alt names are DNS_entry structs */
                if (c != NULL) {
                    if (x509->altNames->next != NULL) {
                        *c = -2; /* more then one found */
                    }
                    else {
                        *c = x509->subjAltNameCrit;
                    }
                }

                dns = x509->altNames;
                /* Currently only support GEN_DNS type */
                while (dns != NULL) {
                    gn = wolfSSL_GENERAL_NAME_new();
                    if (gn == NULL) {
                        WOLFSSL_MSG("Error creating GENERAL_NAME");
                        goto err;
                    }

                    gn->type = dns->type;
                    switch (gn->type) {
                        case ASN_DIR_TYPE:
                            {
                                int localIdx = 0;
                                unsigned char* n = (unsigned char*)XMALLOC(
                                        dns->len + MAX_SEQ_SZ, x509->heap,
                                        DYNAMIC_TYPE_TMP_BUFFER);
                                if (n == NULL) {
                                    goto err;
                                }

                                localIdx += SetSequence(dns->len, n);
                                XMEMCPY(n + localIdx, dns->name, dns->len);
                                gn->d.dirn =  wolfSSL_d2i_X509_NAME(NULL, &n,
                                        dns->len + localIdx);
                                XFREE(n, x509->heap, DYNAMIC_TYPE_TMP_BUFFER);
                                if (gn->d.dirn == NULL) {
                                    WOLFSSL_MSG("Convert altDirName to X509 "
                                            "NAME failed");
                                    goto err;
                                }
                            }
                            break;

                        case ASN_OTHER_TYPE:
                            if (!wolfssl_dns_entry_othername_to_gn(dns, gn)) {
                                goto err;
                            }
                            break;

                        default:
                            if (wolfSSL_ASN1_STRING_set(gn->d.dNSName,
                                    dns->name, dns->len) != WOLFSSL_SUCCESS) {
                                WOLFSSL_MSG("ASN1_STRING_set failed");
                                goto err;
                            }
                            gn->d.dNSName->type = V_ASN1_IA5STRING;
                    }

                    dns = dns->next;
                    if (wolfSSL_sk_GENERAL_NAME_push(sk, gn) !=
                                                      WOLFSSL_SUCCESS) {
                        WOLFSSL_MSG("Error pushing ASN1 object onto stack");
                        goto err;
                    }
                    /* null so that it doesn't get pushed again after switch */
                    gn = NULL;
                }
            }
            else {
                WOLFSSL_MSG("No Alt Names set");
            }

            break;
        }

        case CRL_DIST_OID:
    #if defined(OPENSSL_EXTRA)
            if (x509->CRLdistSet && x509->CRLInfo != NULL) {
                if (c != NULL) {
                    *c = x509->CRLdistCrit;
                }

                sk = wolfSSL_sk_new_null();
                if (sk == NULL) {
                    return NULL;
                }
                sk->type = STACK_TYPE_DIST_POINT;

                gn = wolfSSL_GENERAL_NAME_new();
                if (gn == NULL) {
                    WOLFSSL_MSG("Error creating GENERAL_NAME");
                    goto err;
                }

                if (wolfSSL_GENERAL_NAME_set_type(gn, GEN_URI) !=
                        WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Error setting GENERAL_NAME type");
                    goto err;
                }

                if (wolfSSL_ASN1_STRING_set(gn->d.uniformResourceIdentifier,
                        x509->CRLInfo, x509->CRLInfoSz) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("ASN1_STRING_set failed");
                    goto err;
                }

                /* wolfSSL only decodes one dist point */
                dp = wolfSSL_DIST_POINT_new();
                if (dp == NULL) {
                    WOLFSSL_MSG("Error creating DIST_POINT");
                    goto err;
                }

                /* push GENERAL_NAME onto fullname stack */
                if (wolfSSL_sk_GENERAL_NAME_push(dp->distpoint->name.fullname,
                                                 gn) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("wolfSSL_sk_GENERAL_NAME_push error");
                    goto err;
                }

                /* push DIST_POINT onto stack */
                if (wolfSSL_sk_DIST_POINT_push(sk, dp) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Error pushing DIST_POINT onto stack");
                    goto err;
                }

                gn = NULL;
                dp = NULL;

            }
            else {
                WOLFSSL_MSG("No CRL dist set");
            }
    #endif /* OPENSSL_EXTRA */
            break;

        case AUTH_INFO_OID:
            if (x509->authInfoSet && x509->authInfo != NULL) {
                if (c != NULL) {
                    *c = x509->authInfoCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                if (obj == NULL) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                    return NULL;
                }
                obj->type  = AUTH_INFO_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->authInfo;
                obj->objSz = x509->authInfoSz;
            }
            else {
                WOLFSSL_MSG("No Auth Info set");
            }
            break;

        case AUTH_KEY_OID:
            if (x509->authKeyIdSet) {
                WOLFSSL_AUTHORITY_KEYID* akey = wolfSSL_AUTHORITY_KEYID_new();
                if (!akey) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_AUTHORITY_KEYID struct");
                    return NULL;
                }

                if (c != NULL) {
                    *c = x509->authKeyIdCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                if (obj == NULL) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                    wolfSSL_AUTHORITY_KEYID_free(akey);
                    return NULL;
                }
                obj->type  = AUTH_KEY_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->authKeyId;
                obj->objSz = x509->authKeyIdSz;
                akey->issuer = obj;
                return akey;
            }
            else {
                WOLFSSL_MSG("No Auth Key set");
            }
            break;

        case SUBJ_KEY_OID:
            if (x509->subjKeyIdSet) {
                if (c != NULL) {
                    *c = x509->subjKeyIdCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                if (obj == NULL) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                    return NULL;
                }
                obj->type  = SUBJ_KEY_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->subjKeyId;
                obj->objSz = x509->subjKeyIdSz;
            }
            else {
                WOLFSSL_MSG("No Subject Key set");
            }
            break;

        case CERT_POLICY_OID:
        {
        #ifdef WOLFSSL_CERT_EXT
            int i;

            if (x509->certPoliciesNb > 0) {
                if (c != NULL) {
                    if (x509->certPoliciesNb > 1) {
                        *c = -2;
                    }
                    else {
                        *c = 0;
                    }
                }

                sk = wolfSSL_sk_new_asn1_obj();
                if (sk == NULL) {
                    return NULL;
                }

                for (i = 0; i < x509->certPoliciesNb - 1; i++) {
                    obj = wolfSSL_ASN1_OBJECT_new();
                    if (obj == NULL) {
                        WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                        wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                        return NULL;
                    }
                    obj->type  = CERT_POLICY_OID;
                    obj->grp   = oidCertExtType;
                    obj->obj   = (byte*)(x509->certPolicies[i]);
                    obj->objSz = MAX_CERTPOL_SZ;
                    if (wolfSSL_sk_ASN1_OBJECT_push(sk, obj)
                                                           != WOLFSSL_SUCCESS) {
                        WOLFSSL_MSG("Error pushing ASN1 object onto stack");
                        wolfSSL_ASN1_OBJECT_free(obj);
                        wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                        sk = NULL;
                    }
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                if (obj == NULL) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                    wolfSSL_sk_ASN1_OBJECT_pop_free(sk, NULL);
                    return NULL;
                }
                obj->type  = CERT_POLICY_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = (byte*)(x509->certPolicies[i]);
                obj->objSz = MAX_CERTPOL_SZ;
            }
            else {
                WOLFSSL_MSG("No Cert Policy set");
            }
        #elif defined(WOLFSSL_SEP)
            if (x509->certPolicySet) {
                if (c != NULL) {
                    *c = x509->certPolicyCrit;
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                if (obj == NULL) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                    return NULL;
                }
                obj->type  = CERT_POLICY_OID;
                obj->grp   = oidCertExtType;
            }
            else {
                WOLFSSL_MSG("No Cert Policy set");
            }
        #else
            WOLFSSL_MSG("wolfSSL not built with WOLFSSL_SEP or WOLFSSL_CERT_EXT");
        #endif
            break;
        }
        case KEY_USAGE_OID:
        {
            WOLFSSL_ASN1_STRING* asn1str = NULL;
            if (x509->keyUsageSet) {
                if (c != NULL) {
                    *c = x509->keyUsageCrit;
                }

                asn1str = wolfSSL_ASN1_STRING_new();
                if (asn1str == NULL) {
                    WOLFSSL_MSG("Failed to malloc ASN1_STRING");
                    return NULL;
                }

                if (wolfSSL_ASN1_STRING_set(asn1str, &x509->keyUsage,
                        sizeof(word16)) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("wolfSSL_ASN1_STRING_set error");
                    wolfSSL_ASN1_STRING_free(asn1str);
                    return NULL;
                }

                asn1str->type = KEY_USAGE_OID;
            }
            else {
                WOLFSSL_MSG("No Key Usage set");
            }
            /* don't add stack of and return bit string directly */
            return asn1str;
        }
        case INHIBIT_ANY_OID:
            WOLFSSL_MSG("INHIBIT ANY extension not supported");
            break;

        case EXT_KEY_USAGE_OID:
            if (x509->extKeyUsageSrc != NULL) {
                if (c != NULL) {
                    if (x509->extKeyUsageCount > 1) {
                        *c = -2;
                    }
                    else {
                        *c = x509->extKeyUsageCrit;
                    }
                }
                obj = wolfSSL_ASN1_OBJECT_new();
                if (obj == NULL) {
                    WOLFSSL_MSG("Issue creating WOLFSSL_ASN1_OBJECT struct");
                    return NULL;
                }
                obj->type  = EXT_KEY_USAGE_OID;
                obj->grp   = oidCertExtType;
                obj->obj   = x509->extKeyUsageSrc;
                obj->objSz = x509->extKeyUsageSz;
            }
            else {
                WOLFSSL_MSG("No Extended Key Usage set");
            }
            break;

        case NAME_CONS_OID:
            WOLFSSL_MSG("Name Constraint OID extension not supported");
            break;

        case PRIV_KEY_USAGE_PERIOD_OID:
            WOLFSSL_MSG("Private Key Usage Period extension not supported");
            break;

        case SUBJ_INFO_ACC_OID:
            WOLFSSL_MSG("Subject Info Access extension not supported");
            break;

        case POLICY_MAP_OID:
            WOLFSSL_MSG("Policy Map extension not supported");
            break;

        case POLICY_CONST_OID:
            WOLFSSL_MSG("Policy Constraint extension not supported");
            break;

        case ISSUE_ALT_NAMES_OID:
            WOLFSSL_MSG("Issue Alt Names extension not supported");
            break;

        case TLS_FEATURE_OID:
            WOLFSSL_MSG("TLS Feature extension not supported");
            break;

        default:
            WOLFSSL_MSG("Unsupported/Unknown extension OID");
    }

    /* make sure stack of is allocated */
    if ((obj || gn) && sk == NULL) {
        sk = wolfSSL_sk_new_asn1_obj();
        if (sk == NULL) {
            goto err;
        }
    }
    if (obj) {
        if (wolfSSL_sk_ASN1_OBJECT_push(sk, obj) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Error pushing ASN1_OBJECT object onto "
                        "stack.");
            goto err;
        }
    }

    ret = sk;

    (void)idx;

    return ret;

err:
    if (obj) {
        wolfSSL_ASN1_OBJECT_free(obj);
    }
    if (gn) {
        wolfSSL_GENERAL_NAME_free(gn);
    }
    #ifdef OPENSSL_EXTRA
    if (dp) {
        wolfSSL_DIST_POINT_free(dp);
    }
    #endif
    if (sk) {
        wolfSSL_sk_pop_free(sk, NULL);
    }
    return NULL;
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA
int wolfSSL_X509_add_altname_ex(WOLFSSL_X509* x509, const char* name,
        word32 nameSz, int type)
{
    DNS_entry* newAltName = NULL;
    char* nameCopy = NULL;

    if (x509 == NULL)
        return WOLFSSL_FAILURE;

    if ((name == NULL) || (nameSz == 0))
        return WOLFSSL_SUCCESS;

    newAltName = AltNameNew(x509->heap);
    if (newAltName == NULL)
        return WOLFSSL_FAILURE;

    nameCopy = (char*)XMALLOC(nameSz + 1, x509->heap, DYNAMIC_TYPE_ALTNAME);
    if (nameCopy == NULL) {
        XFREE(newAltName, x509->heap, DYNAMIC_TYPE_ALTNAME);
        return WOLFSSL_FAILURE;
    }

    XMEMCPY(nameCopy, name, nameSz);

    nameCopy[nameSz] = '\0';

    newAltName->next = x509->altNames;
    newAltName->type = type;
    newAltName->len = nameSz;
    newAltName->name = nameCopy;
    x509->altNames = newAltName;

    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_add_altname(WOLFSSL_X509* x509, const char* name, int type)
{
    word32 nameSz;

    if (name == NULL)
        return WOLFSSL_SUCCESS;

    nameSz = (word32)XSTRLEN(name);
    if (nameSz == 0)
        return WOLFSSL_SUCCESS;

    if (type == ASN_IP_TYPE) {
        WOLFSSL_MSG("Type not supported, use wolfSSL_X509_add_altname_ex");
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_X509_add_altname_ex(x509, name, nameSz, type);
}

#ifndef NO_WOLFSSL_STUB
WOLFSSL_X509_EXTENSION *wolfSSL_X509_delete_ext(WOLFSSL_X509 *x509, int loc)
{
    WOLFSSL_STUB("wolfSSL_X509_delete_ext");
    (void)x509;
    (void)loc;
    return NULL;
}

/* currently LHASH is not implemented (and not needed for Apache port) */
WOLFSSL_X509_EXTENSION* wolfSSL_X509V3_EXT_conf_nid(
        WOLF_LHASH_OF(CONF_VALUE)* conf, WOLFSSL_X509V3_CTX* ctx, int nid,
        char* value)
{
    WOLFSSL_STUB("wolfSSL_X509V3_EXT_conf_nid");

    if (conf != NULL) {
        WOLFSSL_MSG("Handling LHASH not implemented yet");
        return NULL;
    }

    (void)conf;
    (void)ctx;
    (void)nid;
    (void)value;
    return NULL;
}

void wolfSSL_X509V3_set_ctx_nodb(WOLFSSL_X509V3_CTX* ctx)
{
    WOLFSSL_STUB("wolfSSL_X509V3_set_ctx_nodb");
    (void)ctx;
}
#endif /* !NO_WOLFSSL_STUB */

#ifdef OPENSSL_EXTRA
static WOLFSSL_X509_EXTENSION* createExtFromStr(int nid, const char *value)
{
    WOLFSSL_X509_EXTENSION* ext;

    if (value == NULL)
        return NULL;

    ext = wolfSSL_X509_EXTENSION_new();
    if (ext == NULL) {
        WOLFSSL_MSG("memory error");
        return NULL;
    }
    ext->value.nid = nid;

    switch (nid) {
        case NID_subject_key_identifier:
        case NID_authority_key_identifier:
            if (wolfSSL_ASN1_STRING_set(&ext->value, value, -1)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_ASN1_STRING_set error");
                goto err_cleanup;
            }
            ext->value.type = CTC_UTF8;
            break;
        case NID_subject_alt_name:
        {
            WOLFSSL_GENERAL_NAMES* gns;
            WOLFSSL_GENERAL_NAME* gn;

            if (wolfSSL_ASN1_STRING_set(&ext->value, value, -1)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_ASN1_STRING_set error");
                goto err_cleanup;
            }
            ext->value.type = ASN_DNS_TYPE;

            /* add stack of general names */
            gns = wolfSSL_sk_new_null();
            if (gns == NULL) {
                WOLFSSL_MSG("wolfSSL_sk_new_null error");
                goto err_cleanup;
            }
            ext->ext_sk = gns; /* wolfSSL_X509_EXTENSION_free will handle
                                * free'ing gns */
            gns->type = STACK_TYPE_GEN_NAME;
            gn = wolfSSL_GENERAL_NAME_new();
            if (gn == NULL) {
                WOLFSSL_MSG("wolfSSL_GENERAL_NAME_new error");
                goto err_cleanup;
            }
            if (wolfSSL_sk_GENERAL_NAME_push(gns, gn) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_sk_GENERAL_NAME_push error");
                wolfSSL_GENERAL_NAME_free(gn);
                goto err_cleanup;
            }
            if (wolfSSL_ASN1_STRING_set(gn->d.ia5, value, -1)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_ASN1_STRING_set failed");
                goto err_cleanup;
            }
            gn->type = ASN_DNS_TYPE;
            break;
        }
        case NID_key_usage:
            if (wolfSSL_ASN1_STRING_set(&ext->value, value, -1)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_ASN1_STRING_set error");
                goto err_cleanup;
            }
            ext->value.type = KEY_USAGE_OID;
            break;
        case NID_ext_key_usage:
            if (wolfSSL_ASN1_STRING_set(&ext->value, value, -1)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_ASN1_STRING_set error");
                goto err_cleanup;
            }
            ext->value.type = EXT_KEY_USAGE_OID;
            break;
        default:
            WOLFSSL_MSG("invalid or unsupported NID");
            goto err_cleanup;
    }
    return ext;
err_cleanup:
    wolfSSL_X509_EXTENSION_free(ext);
    return NULL;
}

/**
 * Create a WOLFSSL_X509_EXTENSION from the input arguments.
 * @param conf  Not used
 * @param ctx   Not used
 * @param nid   Interprets the value parameter as the x509 extension that
 *              corresponds to this NID.
 * @param value A NULL terminated string that is taken as the value of the
 *              newly created extension object.
 * @return WOLFSSL_X509_EXTENSION* on success or NULL on failure.
 */
WOLFSSL_X509_EXTENSION* wolfSSL_X509V3_EXT_nconf_nid(WOLFSSL_CONF* conf,
        WOLFSSL_X509V3_CTX *ctx, int nid, const char *value)
{
    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_nconf_nid");

    if (value == NULL) {
        WOLFSSL_MSG("value NULL parameter");
        return NULL;
    }

    if (conf != NULL || ctx != NULL) {
        WOLFSSL_MSG("wolfSSL_X509V3_EXT_nconf_nid does not handle either "
                    "conf or ctx parameters");
    }

    return createExtFromStr(nid, value);
}

/**
 * Create a WOLFSSL_X509_EXTENSION from the input arguments.
 * @param conf  Not used
 * @param ctx   Not used
 * @param sName The textual representation of the NID that the value parameter
 *              should be interpreted as.
 * @param value A NULL terminated string that is taken as the value of the
 *              newly created extension object.
 * @return WOLFSSL_X509_EXTENSION* on success or NULL on failure.
 */
WOLFSSL_X509_EXTENSION* wolfSSL_X509V3_EXT_nconf(WOLFSSL_CONF *conf,
        WOLFSSL_X509V3_CTX *ctx, const char *sName, const char *value)
{
    const WOLFSSL_ObjectInfo* info = wolfssl_object_info;
    size_t i;

    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_nconf");

    if (value == NULL) {
        WOLFSSL_MSG("value NULL parameter");
        return NULL;
    }

    if (conf != NULL || ctx != NULL) {
        WOLFSSL_MSG("wolfSSL_X509V3_EXT_nconf does not handle either "
                    "conf or ctx parameters");
    }

    for (i = 0; i < wolfssl_object_info_sz; i++, info++) {
        if (XSTRCMP(info->sName, sName) == 0)
            return createExtFromStr(info->nid, value);
    }

    WOLFSSL_MSG("value didn't match any known NID");
    return NULL;
}

static void wolfSSL_X509V3_EXT_METHOD_populate(WOLFSSL_v3_ext_method *method,
                                               int nid)
{
    if (!method)
        return;

    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_METHOD_populate");
    switch (nid) {
    case NID_subject_key_identifier:
        method->i2s = (X509V3_EXT_I2S)wolfSSL_i2s_ASN1_STRING;
        FALL_THROUGH;
    case NID_authority_key_identifier:
    case NID_key_usage:
    case NID_certificate_policies:
    case NID_policy_mappings:
    case NID_subject_alt_name:
    case NID_issuer_alt_name:
    case NID_basic_constraints:
    case NID_name_constraints:
    case NID_policy_constraints:
    case NID_ext_key_usage:
    case NID_crl_distribution_points:
    case NID_inhibit_any_policy:
    case NID_info_access:
        WOLFSSL_MSG("Nothing to populate for current NID");
        break;
    default:
        WOLFSSL_MSG("Unknown or unsupported NID");
        break;
    }

    return;
}

/**
 * @param nid One of the NID_* constants defined in asn.h
 * @param crit
 * @param data This data is copied to the returned extension.
 * @return
 */
WOLFSSL_X509_EXTENSION *wolfSSL_X509V3_EXT_i2d(int nid, int crit,
                                               void *data)
{
    WOLFSSL_X509_EXTENSION *ext = NULL;
    WOLFSSL_ASN1_STRING* asn1str = NULL;

    WOLFSSL_ENTER("wolfSSL_X509V3_EXT_i2d");

    if (!data) {
        return NULL;
    }

    if (!(ext = wolfSSL_X509_EXTENSION_new())) {
        return NULL;
    }

    wolfSSL_X509V3_EXT_METHOD_populate(&ext->ext_method, nid);

    switch (nid) {
    case NID_subject_key_identifier:
        /* WOLFSSL_ASN1_STRING */
    case NID_key_usage:
        /* WOLFSSL_ASN1_STRING */
    {
        asn1str = (WOLFSSL_ASN1_STRING*)data;
        ext->value = *asn1str;
        if (asn1str->isDynamic) {
            ext->value.data = (char*)XMALLOC(asn1str->length, NULL,
                                             DYNAMIC_TYPE_OPENSSL);
            if (!ext->value.data) {
                WOLFSSL_MSG("malloc failed");
                /* Zero so that no existing memory is freed */
                XMEMSET(&ext->value, 0, sizeof(WOLFSSL_ASN1_STRING));
                goto err_cleanup;
            }
            XMEMCPY(ext->value.data, asn1str->data, asn1str->length);
        }
        else {
            ext->value.data = ext->value.strData;
        }

        if (!(ext->obj = wolfSSL_OBJ_nid2obj(nid))) {
            WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new failed");
            goto err_cleanup;
        }

        break;
    }
    case NID_subject_alt_name:
        /* typedef STACK_OF(GENERAL_NAME) GENERAL_NAMES */
    case NID_issuer_alt_name:
        /* typedef STACK_OF(GENERAL_NAME) GENERAL_NAMES */
    case NID_ext_key_usage:
        /* typedef STACK_OF(ASN1_OBJECT) EXTENDED_KEY_USAGE */
    case NID_info_access:
        /* typedef STACK_OF(ACCESS_DESCRIPTION) AUTHORITY_INFO_ACCESS */
    {
        WOLFSSL_STACK* sk = (WOLFSSL_STACK*)data;

        if (ext->ext_sk) {
            wolfSSL_sk_pop_free(ext->ext_sk, NULL);
        }

        if (!(ext->ext_sk = wolfSSL_sk_dup(sk))) {
            WOLFSSL_MSG("wolfSSL_sk_dup failed");
            goto err_cleanup;
        }

        if (!(ext->obj = wolfSSL_OBJ_nid2obj(nid))) {
            WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new failed");
            goto err_cleanup;
        }

        break;
    }
    case NID_basic_constraints:
    {
        /* WOLFSSL_BASIC_CONSTRAINTS */
        WOLFSSL_BASIC_CONSTRAINTS* bc = (WOLFSSL_BASIC_CONSTRAINTS*)data;

        if (!(ext->obj = wolfSSL_ASN1_OBJECT_new())) {
            WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new failed");
            goto err_cleanup;
        }

        ext->obj->ca = bc->ca;
        if (bc->pathlen) {
            ext->obj->pathlen = wolfSSL_ASN1_INTEGER_dup(bc->pathlen);
            if (!ext->obj->pathlen) {
                WOLFSSL_MSG("wolfSSL_ASN1_INTEGER_dup failed");
                goto err_cleanup;
            }
        }
        break;
    }
    case NID_authority_key_identifier:
    {
        /* AUTHORITY_KEYID */
        WOLFSSL_AUTHORITY_KEYID* akey = (WOLFSSL_AUTHORITY_KEYID*)data;

        if (akey->keyid) {
            if (wolfSSL_ASN1_STRING_set(&ext->value, akey->keyid->data,
                                    akey->keyid->length) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("wolfSSL_ASN1_STRING_set failed");
                goto err_cleanup;
            }
            ext->value.type = akey->keyid->type;

            if (!(ext->obj = wolfSSL_OBJ_nid2obj(nid))) {
                WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new failed");
                goto err_cleanup;
            }

        }
        else if (akey->issuer) {
            ext->obj = wolfSSL_ASN1_OBJECT_dup(akey->issuer);
            if (!ext->obj) {
                WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_dup failed");
                goto err_cleanup;
            }
        }
        else {
            WOLFSSL_MSG("NID_authority_key_identifier empty data");
            goto err_cleanup;
        }
        break;
    }
    case NID_inhibit_any_policy:
        /* ASN1_INTEGER */
    case NID_certificate_policies:
        /* STACK_OF(POLICYINFO) */
    case NID_policy_mappings:
        /* STACK_OF(POLICY_MAPPING) */
    case NID_name_constraints:
        /* NAME_CONSTRAINTS */
    case NID_policy_constraints:
        /* POLICY_CONSTRAINTS */
    case NID_crl_distribution_points:
        /* typedef STACK_OF(DIST_POINT) CRL_DIST_POINTS */
    default:
        WOLFSSL_MSG("Unknown or unsupported NID");
        break;
    }

    ext->crit = crit;

    return ext;
err_cleanup:
    if (ext) {
        wolfSSL_X509_EXTENSION_free(ext);
    }
    return NULL;
}

/* Returns pointer to ASN1_OBJECT from an X509_EXTENSION object */
WOLFSSL_ASN1_OBJECT* wolfSSL_X509_EXTENSION_get_object \
    (WOLFSSL_X509_EXTENSION* ext)
{
    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_get_object");
    if(ext == NULL)
        return NULL;
    return ext->obj;
}


/**
 * duplicates the 'obj' input and sets it into the 'ext' structure
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_X509_EXTENSION_set_object(WOLFSSL_X509_EXTENSION* ext,
        const WOLFSSL_ASN1_OBJECT* obj)
{
    WOLFSSL_ASN1_OBJECT *current;

    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_set_object");
    if (ext == NULL)
        return WOLFSSL_FAILURE;

    current = wolfSSL_X509_EXTENSION_get_object(ext);
    if (current != NULL) {
        wolfSSL_ASN1_OBJECT_free(current);
    }
    ext->obj = wolfSSL_ASN1_OBJECT_dup((WOLFSSL_ASN1_OBJECT*)obj);
    return WOLFSSL_SUCCESS;
}
#endif /* OPENSSL_ALL */

/* Returns pointer to ASN1_STRING in X509_EXTENSION object */
WOLFSSL_ASN1_STRING* wolfSSL_X509_EXTENSION_get_data(WOLFSSL_X509_EXTENSION* ext)
{
    WOLFSSL_ENTER("wolfSSL_X509_EXTENSION_get_data");
    if (ext == NULL)
        return NULL;
    return &ext->value;
}


/**
 * Creates a duplicate of input 'data' and sets it into 'ext' structure
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_X509_EXTENSION_set_data(WOLFSSL_X509_EXTENSION* ext,
        WOLFSSL_ASN1_STRING* data)
{
    WOLFSSL_ASN1_STRING* current;

    if (ext == NULL || data == NULL)
        return WOLFSSL_FAILURE;

    current = wolfSSL_X509_EXTENSION_get_data(ext);
    if (current->length > 0 && current->data != NULL && current->isDynamic) {
        XFREE(current->data, NULL, DYNAMIC_TYPE_OPENSSL);
    }

    return wolfSSL_ASN1_STRING_copy(&ext->value, data);
}

#if !defined(NO_PWDBASED)
int wolfSSL_X509_digest(const WOLFSSL_X509* x509, const WOLFSSL_EVP_MD* digest,
        unsigned char* buf, unsigned int* len)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_X509_digest");

    if (x509 == NULL || digest == NULL) {
        WOLFSSL_MSG("Null argument found");
        return WOLFSSL_FAILURE;
    }

    if (x509->derCert == NULL) {
        WOLFSSL_MSG("No DER certificate stored in X509");
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_EVP_Digest(x509->derCert->buffer, x509->derCert->length, buf,
                              len, digest, NULL);
    WOLFSSL_LEAVE("wolfSSL_X509_digest", ret);
    return ret;
}

int wolfSSL_X509_pubkey_digest(const WOLFSSL_X509 *x509,
        const WOLFSSL_EVP_MD *digest, unsigned char* buf, unsigned int* len)
{
    int ret;

    WOLFSSL_ENTER("wolfSSL_X509_pubkey_digest");

    if (x509 == NULL || digest == NULL) {
        WOLFSSL_MSG("Null argument found");
        return WOLFSSL_FAILURE;
    }

    if (x509->pubKey.buffer == NULL || x509->pubKey.length == 0) {
        WOLFSSL_MSG("No DER public key stored in X509");
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_EVP_Digest(x509->pubKey.buffer, x509->pubKey.length, buf,
                              len, digest, NULL);
    WOLFSSL_LEAVE("wolfSSL_X509_pubkey_digest", ret);
    return ret;
}
#endif

#endif /* OPENSSL_EXTRA */

#ifdef OPENSSL_EXTRA

    #ifndef NO_WOLFSSL_STUB
    const char* wolfSSL_X509_get_default_cert_file_env(void)
    {
        WOLFSSL_STUB("X509_get_default_cert_file_env");
        return NULL;
    }

    const char* wolfSSL_X509_get_default_cert_file(void)
    {
        WOLFSSL_STUB("X509_get_default_cert_file");
        return NULL;
    }

    const char* wolfSSL_X509_get_default_cert_dir_env(void)
    {
        WOLFSSL_STUB("X509_get_default_cert_dir_env");
        return NULL;
    }

    const char* wolfSSL_X509_get_default_cert_dir(void)
    {
        WOLFSSL_STUB("X509_get_default_cert_dir");
        return NULL;
    }
    #endif

#endif /* OPENSSL_EXTRA */

#if defined(KEEP_PEER_CERT) || defined(SESSION_CERTS) || \
    defined(OPENSSL_EXTRA)  || defined(OPENSSL_EXTRA_X509_SMALL)

/* user externally called free X509, if dynamic go ahead with free, otherwise
 * don't */
static void ExternalFreeX509(WOLFSSL_X509* x509)
{
#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA)
    int doFree = 0;
#endif

    WOLFSSL_ENTER("ExternalFreeX509");
    if (x509) {
#ifdef HAVE_EX_DATA_CLEANUP_HOOKS
        wolfSSL_CRYPTO_cleanup_ex_data(&x509->ex_data);
#endif
        if (x509->dynamicMemory) {
        #if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA)
            int ret;
            wolfSSL_RefDec(&x509->ref, &doFree, &ret);
            if (ret != 0) {
                WOLFSSL_MSG("Couldn't lock x509 mutex");
            }
        #endif /* OPENSSL_EXTRA_X509_SMALL || OPENSSL_EXTRA */

        #if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA)
            if (doFree)
        #endif /* OPENSSL_EXTRA_X509_SMALL || OPENSSL_EXTRA */
            {
                FreeX509(x509);
                XFREE(x509, x509->heap, DYNAMIC_TYPE_X509);
            }
        } else {
            WOLFSSL_MSG("free called on non dynamic object, not freeing");
        }
    }
}

/* Frees an external WOLFSSL_X509 structure */
WOLFSSL_ABI
void wolfSSL_X509_free(WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_FreeX509");
    ExternalFreeX509(x509);
}


/* copy name into in buffer, at most sz bytes, if buffer is null will
   malloc buffer, call responsible for freeing                     */
WOLFSSL_ABI
char* wolfSSL_X509_NAME_oneline(WOLFSSL_X509_NAME* name, char* in, int sz)
{
    int copySz;

    if (name == NULL) {
        WOLFSSL_MSG("WOLFSSL_X509_NAME pointer was NULL");
        return NULL;
    }

    copySz = min(sz, name->sz);

    WOLFSSL_ENTER("wolfSSL_X509_NAME_oneline");
    if (!name->sz) return in;

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (char*)XMALLOC(name->sz, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!in ) return in;
        copySz = name->sz;
    #endif
    }

    if (copySz <= 0)
        return in;

    XMEMCPY(in, name->name, copySz - 1);
    in[copySz - 1] = 0;

    return in;
}

#ifdef OPENSSL_EXTRA
/* Given an X509_NAME, convert it to canonical form and then hash
 * with the provided hash type. Returns the first 4 bytes of the hash
 * as unsigned long on success, and 0 otherwise. */
static unsigned long X509NameHash(WOLFSSL_X509_NAME* name,
    enum wc_HashType hashType)
{
    unsigned long  hash = 0;
    unsigned char* canonName = NULL;
    byte           digest[WC_MAX_DIGEST_SIZE];
    int            size = 0;
    int            rc;

    WOLFSSL_ENTER("X509NameHash");

    if (name == NULL) {
        WOLFSSL_ERROR_MSG("WOLFSSL_X509_NAME pointer was NULL");
        return 0;
    }

    if (name->sz == 0) {
        WOLFSSL_ERROR_MSG("Nothing to hash in WOLFSSL_X509_NAME");
        return 0;
    }

    size = wolfSSL_i2d_X509_NAME_canon(name, &canonName);

    if (size <= 0 || canonName == NULL) {
        WOLFSSL_ERROR_MSG("wolfSSL_i2d_X509_NAME_canon error");
        return 0;
    }

    rc = wc_Hash(hashType, (const byte*)canonName,(word32)size, digest,
        sizeof(digest));

    if (rc == 0) {
        hash = (((unsigned long)digest[3] << 24) |
                ((unsigned long)digest[2] << 16) |
                ((unsigned long)digest[1] <<  8) |
                ((unsigned long)digest[0]));
    }
    else if (rc == HASH_TYPE_E) {
        WOLFSSL_ERROR_MSG("Hash function not compiled in");
    }
    else {
        WOLFSSL_ERROR_MSG("Error hashing name");
    }

    XFREE(canonName, NULL, DYNAMIC_TYPE_OPENSSL);
    return hash;
}

unsigned long wolfSSL_X509_NAME_hash(WOLFSSL_X509_NAME* name)
{
    return X509NameHash(name, WC_HASH_TYPE_SHA);
}

/******************************************************************************
* wolfSSL_X509_subject_name_hash
* wolfSSL_X509_issuer_name_hash
* Compute the hash digest of the subject / issuer name.
* These functions prefer SHA-1 (if available) for compatibility. Otherwise
* they use SHA-256.
*
* RETURNS:
* The first 4 bytes of SHA-1 (or SHA-256) hash in little endian order as
* unsigned long.
* Otherwise, returns zero.
*
* Note:
* Returns the same hash value as OpenSSL's X509_X_name_hash() API
* if SHA-1 support is compiled in. SHA-256 will be used if SHA-1 is
* not available.
*/
unsigned long wolfSSL_X509_subject_name_hash(const WOLFSSL_X509* x509)
{
    if (x509 == NULL) {
        WOLFSSL_ERROR_MSG("WOLFSSL_X509 pointer was NULL");
        return 0;
    }

    #ifndef NO_SHA
    return X509NameHash((WOLFSSL_X509_NAME*) &x509->subject, WC_HASH_TYPE_SHA);
    #elif !defined(NO_SHA256)
    return X509NameHash((WOLFSSL_X509_NAME*) &x509->subject,
                        WC_HASH_TYPE_SHA256);
    #else
    WOLFSSL_ERROR_MSG("Hash function not compiled in");
    return 0;
    #endif
}

unsigned long wolfSSL_X509_issuer_name_hash(const WOLFSSL_X509* x509)
{
    if (x509 == NULL) {
        WOLFSSL_ERROR_MSG("WOLFSSL_X509 pointer was NULL");
        return 0;
    }

    #ifndef NO_SHA
    return X509NameHash((WOLFSSL_X509_NAME*) &x509->issuer, WC_HASH_TYPE_SHA);
    #elif !defined(NO_SHA256)
    return X509NameHash((WOLFSSL_X509_NAME*) &x509->issuer,
                        WC_HASH_TYPE_SHA256);
    #else
    WOLFSSL_ERROR_MSG("Hash function not compiled in");
    return 0;
    #endif
}
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) && defined(XSNPRINTF)
/* Copies X509 subject name into a buffer, with comma-separated name entries
 *   (matching OpenSSL v1.0.0 format)
 * Example Output for Issuer:
 *
 * C=US, ST=Montana, L=Bozeman, O=Sawtooth, OU=Consulting,
 *  CN=www.wolfssl.com, emailAddress=info@wolfssl.com
 */
char* wolfSSL_X509_get_name_oneline(WOLFSSL_X509_NAME* name, char* in, int sz)
{
    int count, i;
    int totalLen = 0;
    char tmpBuf[256];
    WOLFSSL_ENTER("wolfSSL_X509_get_name_oneline");

    if (name == NULL) {
        WOLFSSL_MSG("wolfSSL_X509_get_name_oneline failed");
        return NULL;
    }
    #ifdef WOLFSSL_STATIC_MEMORY
    if (!in) {
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    }
    #endif

    /* Loop through X509 name entries and copy new format to buffer */
    count = wolfSSL_X509_NAME_entry_count(name);
    for (i = 0; i < count; i++) {
        WOLFSSL_X509_NAME_ENTRY* entry;
        int nameSz;
        int strSz;
        int strLen;
        char *str;
        const int tmpBufSz = sizeof(tmpBuf);
        char buf[80];
        const char* sn;

        /* Get name entry and size */
        entry = wolfSSL_X509_NAME_get_entry(name, i);
        if (entry == NULL) {
            WOLFSSL_MSG("wolfSSL_X509_NAME_get_entry failed");
            return NULL;
        }
        nameSz = wolfSSL_X509_NAME_get_text_by_NID(name, entry->nid, buf,
                                                                   sizeof(buf));
        if (nameSz < 0) {
            WOLFSSL_MSG("wolfSSL_X509_NAME_get_text_by_NID failed");
            return NULL;
        }

        /* Get short name */
        sn = wolfSSL_OBJ_nid2sn(entry->nid);
        if (sn == NULL) {
            WOLFSSL_MSG("OBJ_nid2sn failed");
            return NULL;
        }

        /* Copy sn and name text to buffer
         * Add extra strSz for '=', ',', ' ' and '\0' characters in XSNPRINTF.
         */
        if (i != count - 1) {
            strSz = (int)XSTRLEN(sn) + nameSz + 4;
            str = (char*)XMALLOC(strSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (str == NULL) {
                WOLFSSL_MSG("Memory error");
                return NULL;
            }
            if ((strLen = XSNPRINTF(str, strSz, "%s=%s, ", sn, buf))
                >= strSz)
            {
                WOLFSSL_MSG("buffer overrun");
                XFREE(str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                return NULL;
            }
        }
        else {
            /* Copy last name entry
            * Add extra strSz for '=' and '\0' characters in XSNPRINTF.
            */
            strSz = (int)XSTRLEN(sn) + nameSz + 2;
            str = (char*)XMALLOC(strSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            if (str == NULL) {
                WOLFSSL_MSG("Memory error");
                return NULL;
            }
            if ((strLen = XSNPRINTF(str, strSz, "%s=%s", sn, buf)) >= strSz) {
                WOLFSSL_MSG("buffer overrun");
                XFREE(str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                return NULL;
            }
        }
        /* Copy string to tmpBuf */
        if (totalLen + strLen > tmpBufSz) {
            WOLFSSL_MSG("buffer overrun");
            XFREE(str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return NULL;
        }
        XMEMCPY(tmpBuf + totalLen, str, strLen);
        totalLen += strLen;
        XFREE(str, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    /* Allocate space based on total string size if no buffer was provided */
    if (!in) {
        in = (char*)XMALLOC(totalLen+1, NULL, DYNAMIC_TYPE_OPENSSL);
        if (in == NULL) {
            WOLFSSL_MSG("Memory error");
            return in;
        }
    }
    else {
        if (totalLen + 1 > sz) {
            WOLFSSL_MSG("buffer overrun");
            return NULL;
        }
    }

    XMEMCPY(in, tmpBuf, totalLen);
    in[totalLen] = '\0';

    return in;
}
#endif


/* Wraps wolfSSL_X509_d2i
 *
 * returns a WOLFSSL_X509 structure pointer on success and NULL on fail
 */
WOLFSSL_X509* wolfSSL_d2i_X509(WOLFSSL_X509** x509, const unsigned char** in,
        int len)
{
    WOLFSSL_X509* newX509 = NULL;
    WOLFSSL_ENTER("wolfSSL_d2i_X509");

    if (in == NULL) {
        WOLFSSL_MSG("NULL input for wolfSSL_d2i_X509");
        return NULL;
    }

    newX509 = wolfSSL_X509_d2i(x509, *in, len);
    if (newX509 != NULL) {
        *in += newX509->derCert->length;
    }
    return newX509;
}

static WOLFSSL_X509* d2i_X509orX509REQ(WOLFSSL_X509** x509,
                                        const byte* in, int len, int req)
{
    WOLFSSL_X509 *newX509 = NULL;
    int type = req ? CERTREQ_TYPE : CERT_TYPE;

    WOLFSSL_ENTER("wolfSSL_X509_d2i");

    if (in != NULL && len != 0
    #ifndef WOLFSSL_CERT_REQ
            && req == 0
    #else
            && (req == 0 || req == 1)
    #endif
            ) {
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert;
    #else
        DecodedCert  cert[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                     DYNAMIC_TYPE_DCERT);
        if (cert == NULL)
            return NULL;
    #endif

        InitDecodedCert(cert, (byte*)in, len, NULL);
    #ifdef WOLFSSL_CERT_REQ
        cert->isCSR = (byte)req;
    #endif
        if (ParseCertRelative(cert, type, 0, NULL) == 0) {
            newX509 = wolfSSL_X509_new();
            if (newX509 != NULL) {
                if (CopyDecodedToX509(newX509, cert) != 0) {
                    wolfSSL_X509_free(newX509);
                    newX509 = NULL;
                }
            }
        }
        FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
    }

    if (x509 != NULL)
        *x509 = newX509;

    return newX509;
}

int wolfSSL_X509_get_isCA(WOLFSSL_X509* x509)
{
    int isCA = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_isCA");

    if (x509 != NULL)
        isCA = x509->isCa;

    WOLFSSL_LEAVE("wolfSSL_X509_get_isCA", isCA);

    return isCA;
}

WOLFSSL_X509* wolfSSL_X509_d2i(WOLFSSL_X509** x509, const byte* in, int len)
{
    return d2i_X509orX509REQ(x509, in, len, 0);
}

#ifdef WOLFSSL_CERT_REQ
WOLFSSL_X509* wolfSSL_X509_REQ_d2i(WOLFSSL_X509** x509,
        const unsigned char* in, int len)
{
    return d2i_X509orX509REQ(x509, in, len, 1);
}
#endif

#endif /* KEEP_PEER_CERT || SESSION_CERTS || OPENSSL_EXTRA ||
          OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* returns the number of entries in the WOLFSSL_X509_NAME */
int wolfSSL_X509_NAME_entry_count(WOLFSSL_X509_NAME* name)
{
    int count = 0;

    WOLFSSL_ENTER("wolfSSL_X509_NAME_entry_count");

    if (name != NULL)
        count = name->entrySz;

    WOLFSSL_LEAVE("wolfSSL_X509_NAME_entry_count", count);
    return count;
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA) || \
    defined(KEEP_OUR_CERT) || defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)

/* return the next, if any, altname from the peer cert */
WOLFSSL_ABI
char* wolfSSL_X509_get_next_altname(WOLFSSL_X509* cert)
{
    char* ret = NULL;
    WOLFSSL_ENTER("wolfSSL_X509_get_next_altname");

    /* don't have any to work with */
    if (cert == NULL || cert->altNames == NULL)
        return NULL;

    /* already went through them */
    if (cert->altNamesNext == NULL) {
#ifdef WOLFSSL_MULTICIRCULATE_ALTNAMELIST
        /* Reset altNames List to head
         * so that caller can circulate the list again
         */
        cert->altNamesNext = cert->altNames;
#endif
        return NULL;
    }

    ret = cert->altNamesNext->name;
#if defined(OPENSSL_ALL) || defined(WOLFSSL_IP_ALT_NAME)
    /* return the IP address as a string */
    if (cert->altNamesNext->type == ASN_IP_TYPE) {
        ret = cert->altNamesNext->ipString;
    }
#endif
    cert->altNamesNext = cert->altNamesNext->next;

    return ret;
}

int wolfSSL_X509_get_signature(WOLFSSL_X509* x509,
                                                unsigned char* buf, int* bufSz)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_signature");
    if (x509 == NULL || bufSz == NULL || (*bufSz < (int)x509->sig.length &&
                buf != NULL))
        return WOLFSSL_FATAL_ERROR;

    if (buf != NULL)
        XMEMCPY(buf, x509->sig.buffer, x509->sig.length);
    *bufSz = x509->sig.length;

    return WOLFSSL_SUCCESS;
}


/* Getter function that copies over the DER public key buffer to "buf" and
    * sets the size in bufSz. If "buf" is NULL then just bufSz is set to needed
    * buffer size. "bufSz" passed in should initially be set by the user to be
    * the size of "buf". This gets checked to make sure the buffer is large
    * enough to hold the public key.
    *
    * Note: this is the X.509 form of key with "header" info.
    * return WOLFSSL_SUCCESS on success
    */
int wolfSSL_X509_get_pubkey_buffer(WOLFSSL_X509* x509,
                                            unsigned char* buf, int* bufSz)
{
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert;
#else
    DecodedCert cert[1];
#endif
    const byte*  der;
    int length = 0;
    int    ret = 0, derSz = 0;
    int badDate = 0;
    const byte* pubKeyX509 = NULL;
    int   pubKeyX509Sz = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_pubkey_buffer");
    if (x509 == NULL || bufSz == NULL) {
        WOLFSSL_LEAVE("wolfSSL_X509_get_pubkey_buffer", BAD_FUNC_ARG);
        return WOLFSSL_FATAL_ERROR;
    }


#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert),
                                    x509->heap, DYNAMIC_TYPE_TMP_BUFFER);
    if (cert == NULL) {
        WOLFSSL_LEAVE("wolfSSL_X509_get_pubkey_buffer", MEMORY_E);
        return WOLFSSL_FATAL_ERROR;
    }
#endif

    der = wolfSSL_X509_get_der(x509, &derSz);
    if (der != NULL) {
        InitDecodedCert(cert, der, derSz, NULL);
        ret = wc_GetPubX509(cert, 0, &badDate);
        if (ret >= 0) {
            word32 idx = cert->srcIdx;
            pubKeyX509 = cert->source + cert->srcIdx;
            ret = GetSequence(cert->source, &cert->srcIdx, &length,
                    cert->maxIdx);
            pubKeyX509Sz = length + (cert->srcIdx - idx);
        }
        FreeDecodedCert(cert);
    }
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, x509->heap, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    if (ret < 0) {
        WOLFSSL_LEAVE("wolfSSL_X509_get_pubkey_buffer", ret);
        return WOLFSSL_FATAL_ERROR;
    }

    if (buf != NULL && pubKeyX509 != NULL) {
        if (pubKeyX509Sz > *bufSz) {
            WOLFSSL_LEAVE("wolfSSL_X509_get_pubkey_buffer", BUFFER_E);
            return WOLFSSL_FATAL_ERROR;
        }
        XMEMCPY(buf, pubKeyX509, pubKeyX509Sz);
    }
    *bufSz = pubKeyX509Sz;

    return WOLFSSL_SUCCESS;
}


/* Getter function for the public key OID value
    * return public key OID stored in WOLFSSL_X509 structure */
int wolfSSL_X509_get_pubkey_type(WOLFSSL_X509* x509)
{
    if (x509 == NULL)
        return WOLFSSL_FAILURE;
    return x509->pubKeyOID;
}

#endif /* OPENSSL_EXTRA || KEEP_OUR_CERT || KEEP_PEER_CERT || SESSION_CERTS */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL) || \
    defined(KEEP_OUR_CERT) || defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)

/* write X509 serial number in unsigned binary to buffer
    buffer needs to be at least EXTERNAL_SERIAL_SIZE (32) for all cases
    return WOLFSSL_SUCCESS on success */
int wolfSSL_X509_get_serial_number(WOLFSSL_X509* x509,
                                    byte* in, int* inOutSz)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_serial_number");
    if (x509 == NULL || inOutSz == NULL) {
        WOLFSSL_MSG("Null argument passed in");
        return BAD_FUNC_ARG;
    }

    if (in != NULL) {
        if (*inOutSz < x509->serialSz) {
            WOLFSSL_MSG("Serial buffer too small");
            return BUFFER_E;
        }
        XMEMCPY(in, x509->serial, x509->serialSz);
    }
    *inOutSz = x509->serialSz;

    return WOLFSSL_SUCCESS;
}

/* not an openssl compatibility function - getting for derCert */
const byte* wolfSSL_X509_get_der(WOLFSSL_X509* x509, int* outSz)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_der");

    if (x509 == NULL || x509->derCert == NULL || outSz == NULL)
        return NULL;

    *outSz = (int)x509->derCert->length;
    return x509->derCert->buffer;
}

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL || KEEP_OUR_CERT || KEEP_PEER_CERT || SESSION_CERTS */

#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_ALL) || defined(KEEP_OUR_CERT) || \
    defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)

/* used by JSSE (not a standard compatibility function) */
WOLFSSL_ABI
const byte* wolfSSL_X509_notBefore(WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_notBefore");

    if (x509 == NULL)
        return NULL;

    XMEMSET(x509->notBeforeData, 0, sizeof(x509->notBeforeData));
    x509->notBeforeData[0] = (byte)x509->notBefore.type;
    x509->notBeforeData[1] = (byte)x509->notBefore.length;
    XMEMCPY(&x509->notBeforeData[2], x509->notBefore.data, x509->notBefore.length);

    return x509->notBeforeData;
}

/* used by JSSE (not a standard compatibility function) */
WOLFSSL_ABI
const byte* wolfSSL_X509_notAfter(WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_notAfter");

    if (x509 == NULL)
        return NULL;

    XMEMSET(x509->notAfterData, 0, sizeof(x509->notAfterData));
    x509->notAfterData[0] = (byte)x509->notAfter.type;
    x509->notAfterData[1] = (byte)x509->notAfter.length;
    XMEMCPY(&x509->notAfterData[2], x509->notAfter.data, x509->notAfter.length);

    return x509->notAfterData;
}

int wolfSSL_X509_version(WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_version");

    if (x509 == NULL)
        return 0;

    return x509->version;
}
#endif

#ifdef OPENSSL_EXTRA

/* get the buffer to be signed (tbs) from the WOLFSSL_X509 certificate
    *
    * outSz : gets set to the size of the buffer
    * returns a pointer to the internal buffer at the location of TBS on
    *         on success and NULL on failure.
    */
const unsigned char* wolfSSL_X509_get_tbs(WOLFSSL_X509* x509, int* outSz)
{
    int sz = 0, len;
    unsigned int idx = 0, tmpIdx;
    const unsigned char* der = NULL;
    const unsigned char* tbs = NULL;

    if (x509 == NULL || outSz == NULL) {
        return NULL;
    }

    der = wolfSSL_X509_get_der(x509, &sz);
    if (der == NULL) {
        return NULL;
    }

    if (GetSequence(der, &idx, &len, sz) < 0) {
        return NULL;
    }
    tbs = der + idx;
    tmpIdx = idx;
    if (GetSequence(der, &idx, &len, sz) < 0) {
        return NULL;
    }
    *outSz = len + (idx - tmpIdx);
    return tbs;
}

#ifdef WOLFSSL_SEP

/* copy oid into in buffer, at most *inOutSz bytes, if buffer is null will
   malloc buffer, call responsible for freeing. Actual size returned in
   *inOutSz. Requires inOutSz be non-null */
byte* wolfSSL_X509_get_device_type(WOLFSSL_X509* x509, byte* in, int *inOutSz)
{
    int copySz;

    WOLFSSL_ENTER("wolfSSL_X509_get_dev_type");
    if (inOutSz == NULL) return NULL;
    if (!x509->deviceTypeSz) return in;

    copySz = min(*inOutSz, x509->deviceTypeSz);

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (byte*)XMALLOC(x509->deviceTypeSz, 0, DYNAMIC_TYPE_OPENSSL);
        if (!in) return in;
        copySz = x509->deviceTypeSz;
    #endif
    }

    XMEMCPY(in, x509->deviceType, copySz);
    *inOutSz = copySz;

    return in;
}


byte* wolfSSL_X509_get_hw_type(WOLFSSL_X509* x509, byte* in, int* inOutSz)
{
    int copySz;

    WOLFSSL_ENTER("wolfSSL_X509_get_hw_type");
    if (inOutSz == NULL) return NULL;
    if (!x509->hwTypeSz) return in;

    copySz = min(*inOutSz, x509->hwTypeSz);

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (byte*)XMALLOC(x509->hwTypeSz, 0, DYNAMIC_TYPE_OPENSSL);
        if (!in) return in;
        copySz = x509->hwTypeSz;
    #endif
    }

    XMEMCPY(in, x509->hwType, copySz);
    *inOutSz = copySz;

    return in;
}


byte* wolfSSL_X509_get_hw_serial_number(WOLFSSL_X509* x509,byte* in,
                                        int* inOutSz)
{
    int copySz;

    WOLFSSL_ENTER("wolfSSL_X509_get_hw_serial_number");
    if (inOutSz == NULL) return NULL;
    if (!x509->hwTypeSz) return in;

    copySz = min(*inOutSz, x509->hwSerialNumSz);

    if (!in) {
    #ifdef WOLFSSL_STATIC_MEMORY
        WOLFSSL_MSG("Using static memory -- please pass in a buffer");
        return NULL;
    #else
        in = (byte*)XMALLOC(x509->hwSerialNumSz, 0, DYNAMIC_TYPE_OPENSSL);
        if (!in) return in;
        copySz = x509->hwSerialNumSz;
    #endif
    }

    XMEMCPY(in, x509->hwSerialNum, copySz);
    *inOutSz = copySz;

    return in;
}

#endif /* WOLFSSL_SEP */
#endif /* OPENSSL_EXTRA */

/* require OPENSSL_EXTRA since wolfSSL_X509_free is wrapped by OPENSSL_EXTRA */
#if defined(OPENSSL_EXTRA)

WOLFSSL_ASN1_TIME* wolfSSL_X509_get_notBefore(const WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_notBefore");

    if (x509 == NULL)
        return NULL;

    return (WOLFSSL_ASN1_TIME*)&x509->notBefore;
}


WOLFSSL_ASN1_TIME* wolfSSL_X509_get_notAfter(const WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_notAfter");

    if (x509 == NULL)
        return NULL;

    return (WOLFSSL_ASN1_TIME*)&x509->notAfter;
}


/* return 1 on success 0 on fail */
int wolfSSL_sk_X509_push(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk, WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_push");

    if (sk == NULL || x509 == NULL) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_sk_push(sk, x509);
}


/* Return and remove the last x509 pushed on stack */
WOLFSSL_X509* wolfSSL_sk_X509_pop(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk)
{
    WOLFSSL_STACK* node;
    WOLFSSL_X509*  x509;

    if (sk == NULL) {
        return NULL;
    }

    node = sk->next;
    x509 = sk->data.x509;

    if (node != NULL) { /* update sk and remove node from stack */
        sk->data.x509 = node->data.x509;
        sk->next = node->next;
        XFREE(node, NULL, DYNAMIC_TYPE_X509);
    }
    else { /* last x509 in stack */
        sk->data.x509 = NULL;
    }

    if (sk->num > 0) {
        sk->num -= 1;
    }

    return x509;
}

/* Getter function for WOLFSSL_X509 pointer
 *
 * sk is the stack to retrieve pointer from
 * i  is the index value in stack
 *
 * returns a pointer to a WOLFSSL_X509 structure on success and NULL on
 *         fail
 */
WOLFSSL_X509* wolfSSL_sk_X509_value(STACK_OF(WOLFSSL_X509)* sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_value");

    for (; sk != NULL && i > 0; i--)
        sk = sk->next;

    if (i != 0 || sk == NULL)
        return NULL;
    return sk->data.x509;
}


/* Return and remove the first x509 pushed on stack */
WOLFSSL_X509* wolfSSL_sk_X509_shift(WOLF_STACK_OF(WOLFSSL_X509)* sk)
{
    WOLFSSL_STACK* node;
    WOLFSSL_X509*  x509;

    if (sk == NULL) {
        return NULL;
    }

    node = sk->next;
    x509 = sk->data.x509;

    if (node != NULL) {
        /* walk to end of stack to first node pushed, and remove it */
        WOLFSSL_STACK* prevNode = sk;

        while (node->next != NULL) {
            prevNode = node;
            node = node->next;
        }

        x509 = node->data.x509;
        prevNode->next = NULL;
        XFREE(node, NULL, DYNAMIC_TYPE_X509);
    }
    else { /* only one x509 in stack */
        sk->data.x509 = NULL;
    }

    if (sk->num > 0) {
        sk->num -= 1;
    }

    return x509;
}

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
/* Free's all nodes in X509 stack. This is different then wolfSSL_sk_X509_free
 * in that it free's the underlying objects pushed to the stack.
 *
 * sk  stack to free nodes in
 * f   X509 free function
 */
void wolfSSL_sk_X509_pop_free(STACK_OF(WOLFSSL_X509)* sk,
    void (*f) (WOLFSSL_X509*))
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}


/* free just the stack structure */
void wolfSSL_sk_X509_free(WOLF_STACK_OF(WOLFSSL_X509)* sk)
{
    wolfSSL_sk_free(sk);
}

#ifdef HAVE_CRL
WOLFSSL_STACK* wolfSSL_sk_X509_CRL_new(void)
{
    WOLFSSL_STACK* s = wolfSSL_sk_new_node(NULL);
    if (s != NULL)
        s->type = STACK_TYPE_X509_CRL;
    return s;
}

void wolfSSL_sk_X509_CRL_pop_free(WOLF_STACK_OF(WOLFSSL_X509_CRL)* sk,
    void (*f) (WOLFSSL_X509_CRL*))
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_CRL_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

void wolfSSL_sk_X509_CRL_free(WOLF_STACK_OF(WOLFSSL_X509_CRL)* sk)
{
    wolfSSL_sk_X509_CRL_pop_free(sk, NULL);
}

/* return 1 on success 0 on fail */
int wolfSSL_sk_X509_CRL_push(WOLF_STACK_OF(WOLFSSL_X509_CRL)* sk, WOLFSSL_X509_CRL* crl)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_CRL_push");

    if (sk == NULL || crl == NULL) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_sk_push(sk, crl);
}

WOLFSSL_X509_CRL* wolfSSL_sk_X509_CRL_value(WOLF_STACK_OF(WOLFSSL_X509)* sk,
                                            int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_CRL_value");
    if (sk)
        return (WOLFSSL_X509_CRL*)wolfSSL_sk_value(sk, i);
    return NULL;
}

int wolfSSL_sk_X509_CRL_num(WOLF_STACK_OF(WOLFSSL_X509)* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_CRL_num");
    if (sk)
        return wolfSSL_sk_num(sk);
    return 0;
}
#endif /* HAVE_CRL */

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_QT)
/* return 1 on success 0 on fail */
int wolfSSL_sk_ACCESS_DESCRIPTION_push(WOLF_STACK_OF(ACCESS_DESCRIPTION)* sk,
                                              WOLFSSL_ACCESS_DESCRIPTION* a)
{
    WOLFSSL_ENTER("wolfSSL_sk_ACCESS_DESCRIPTION_push");

    return wolfSSL_sk_push(sk, a);
}

/* Frees all nodes in ACCESS_DESCRIPTION stack
*
* sk stack of nodes to free
* f  free function to use
*/
void wolfSSL_sk_ACCESS_DESCRIPTION_pop_free(WOLFSSL_STACK* sk,
    void (*f) (WOLFSSL_ACCESS_DESCRIPTION*))
{
   WOLFSSL_ENTER("wolfSSL_sk_ACCESS_DESCRIPTION_pop_free");
   wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

void wolfSSL_sk_ACCESS_DESCRIPTION_free(WOLFSSL_STACK* sk)
{
    wolfSSL_sk_free(sk);
}


/* AUTHORITY_INFO_ACCESS object is a stack of ACCESS_DESCRIPTION objects,
 * to free the stack the WOLFSSL_ACCESS_DESCRIPTION stack free function is
 * used */
void wolfSSL_AUTHORITY_INFO_ACCESS_free(
        WOLF_STACK_OF(WOLFSSL_ACCESS_DESCRIPTION)* sk)
{
    WOLFSSL_ENTER("wolfSSL_AUTHORITY_INFO_ACCESS_free");
    wolfSSL_sk_ACCESS_DESCRIPTION_free(sk);
}

void wolfSSL_AUTHORITY_INFO_ACCESS_pop_free(
        WOLF_STACK_OF(WOLFSSL_ACCESS_DESCRIPTION)* sk,
        void (*f) (WOLFSSL_ACCESS_DESCRIPTION*))
{
    WOLFSSL_ENTER("wolfSSL_AUTHORITY_INFO_ACCESS_free");
    wolfSSL_sk_ACCESS_DESCRIPTION_pop_free(sk, f);
}


void wolfSSL_ACCESS_DESCRIPTION_free(WOLFSSL_ACCESS_DESCRIPTION* a)
{
    WOLFSSL_ENTER("wolfSSL_ACCESS_DESCRIPTION_free");
    if (a == NULL)
        return;

    if (a->method)
        wolfSSL_ASN1_OBJECT_free(a->method);
    if (a->location)
        wolfSSL_GENERAL_NAME_free(a->location);
    XFREE(a, NULL, DYNAMIC_TYPE_X509_EXT);

    /* a = NULL, don't try to a or double free it */
}
#endif /* OPENSSL_EXTRA || WOLFSSL_QT */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)

/* Creates and returns new GENERAL_NAME structure */
WOLFSSL_GENERAL_NAME* wolfSSL_GENERAL_NAME_new(void)
{
    WOLFSSL_GENERAL_NAME* gn;
    WOLFSSL_ENTER("GENERAL_NAME_new");

    gn = (WOLFSSL_GENERAL_NAME*)XMALLOC(sizeof(WOLFSSL_GENERAL_NAME), NULL,
                                                             DYNAMIC_TYPE_ASN1);
    if (gn == NULL) {
        return NULL;
    }
    XMEMSET(gn, 0, sizeof(WOLFSSL_GENERAL_NAME));

    gn->d.ia5 = wolfSSL_ASN1_STRING_new();
    if (gn->d.ia5 == NULL) {
        WOLFSSL_MSG("Issue creating ASN1_STRING struct");
        wolfSSL_GENERAL_NAME_free(gn);
        return NULL;
    }
    gn->type = GEN_IA5;
    return gn;
}

WOLFSSL_GENERAL_NAME* wolfSSL_GENERAL_NAME_dup(WOLFSSL_GENERAL_NAME* gn)
{
    WOLFSSL_GENERAL_NAME* dupl = NULL;

    WOLFSSL_ENTER("wolfSSL_GENERAL_NAME_dup");

    if (!gn) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    if (!(dupl = wolfSSL_GENERAL_NAME_new())) {
        WOLFSSL_MSG("wolfSSL_GENERAL_NAME_new error");
        return NULL;
    }

    wolfSSL_ASN1_STRING_free(dupl->d.ia5);
    dupl->d.ia5 = NULL;
    switch (gn->type) {
    /* WOLFSSL_ASN1_STRING types */
    case GEN_DNS:
        if (!(dupl->d.dNSName = wolfSSL_ASN1_STRING_dup(gn->d.dNSName))) {
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_dup error");
            goto error;
        }
        break;
    case GEN_IPADD:
        if (!(dupl->d.iPAddress = wolfSSL_ASN1_STRING_dup(gn->d.iPAddress))) {
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_dup error");
            goto error;
        }
        break;
    case GEN_EMAIL:
        if (!(dupl->d.rfc822Name = wolfSSL_ASN1_STRING_dup(gn->d.rfc822Name))) {
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_dup error");
            goto error;
        }
        break;
    case GEN_URI:
        if (!(dupl->d.uniformResourceIdentifier =
                wolfSSL_ASN1_STRING_dup(gn->d.uniformResourceIdentifier))) {
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_dup error");
            goto error;
        }
        break;
    case GEN_OTHERNAME:
        if (gn->d.otherName->value->type != V_ASN1_UTF8STRING) {
            WOLFSSL_MSG("Unsupported othername value type");
            goto error;
        }
        dupl->d.otherName = (WOLFSSL_ASN1_OTHERNAME*)XMALLOC(
            sizeof(WOLFSSL_ASN1_OTHERNAME), NULL, DYNAMIC_TYPE_ASN1);
        if (dupl->d.otherName == NULL) {
            WOLFSSL_MSG("XMALLOC error");
            goto error;
        }
        dupl->d.otherName->type_id = wolfSSL_ASN1_OBJECT_dup(
            gn->d.otherName->type_id);
        dupl->d.otherName->value = (WOLFSSL_ASN1_TYPE*)XMALLOC(
            sizeof(WOLFSSL_ASN1_TYPE), NULL, DYNAMIC_TYPE_ASN1);
        if (dupl->d.otherName->value != NULL) {
            dupl->d.otherName->value->type = gn->d.otherName->value->type;
            dupl->d.otherName->value->value.utf8string =
                wolfSSL_ASN1_STRING_dup(
                                      gn->d.otherName->value->value.utf8string);
        }
        if ((dupl->d.otherName->type_id == NULL) ||
            (dupl->d.otherName->value == NULL) ||
            (dupl->d.otherName->value->value.utf8string == NULL)) {
            wolfSSL_ASN1_OBJECT_free(dupl->d.otherName->type_id);
            wolfSSL_ASN1_TYPE_free(dupl->d.otherName->value);
            XFREE(dupl->d.otherName, NULL, DYNAMIC_TYPE_ASN1);
            dupl->d.otherName = NULL;
            WOLFSSL_MSG("error duping othername");
            goto error;
        }
        break;
    case GEN_X400:
    case GEN_DIRNAME:
    case GEN_EDIPARTY:
    case GEN_RID:
    default:
        WOLFSSL_MSG("Unrecognized or unsupported GENERAL_NAME type");
        goto error;
    }
    dupl->type = gn->type;

    return dupl;
error:
    if (dupl) {
        wolfSSL_GENERAL_NAME_free(dupl);
    }
    return NULL;
}

/* Set an Othername in a general name.
 *
 * @param [out] gen     Pointer to the GENERAL_NAME where the othername is set.
 * @param [in]  oid     Object ID (ie UPN).
 * @param [in]  name    The actual name.
 * @return  WOLFSSL_FAILURE on invalid parameter or memory error,
 *          WOLFSSL_SUCCESS otherwise.
 */
int wolfSSL_GENERAL_NAME_set0_othername(WOLFSSL_GENERAL_NAME* gen,
                                        ASN1_OBJECT* oid, ASN1_TYPE* value)
{
    WOLFSSL_ASN1_OBJECT *x = NULL;

    if ((gen == NULL) || (oid == NULL) || (value == NULL)) {
        return WOLFSSL_FAILURE;
    }

    x = wolfSSL_ASN1_OBJECT_dup(oid);
    if (x == NULL) {
        WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_dup() failed");
        return WOLFSSL_FAILURE;
    }

    gen->type = GEN_OTHERNAME;
    gen->d.otherName->type_id = x;
    gen->d.otherName->value = value;
    return WOLFSSL_SUCCESS;
}

/* return 1 on success 0 on fail */
int wolfSSL_sk_GENERAL_NAME_push(WOLFSSL_GENERAL_NAMES* sk,
                                 WOLFSSL_GENERAL_NAME* gn)
{
    WOLFSSL_STACK* node;
    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_push");

    if (sk == NULL || gn == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* no previous values in stack */
    if (sk->data.gn == NULL) {
        sk->data.gn = gn;
        sk->num += 1;

        return WOLFSSL_SUCCESS;
    }

    /* stack already has value(s) create a new node and add more */
    node = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
                                                             DYNAMIC_TYPE_ASN1);
    if (node == NULL) {
        WOLFSSL_MSG("Memory error");
        return WOLFSSL_FAILURE;
    }
    XMEMSET(node, 0, sizeof(WOLFSSL_STACK));

    /* push new obj onto head of stack */
    node->type    = STACK_TYPE_GEN_NAME;
    node->data.gn = sk->data.gn;
    node->next    = sk->next;
    sk->next      = node;
    sk->data.gn   = gn;
    sk->num      += 1;

    return WOLFSSL_SUCCESS;
}

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA

/* Returns the general name at index i from the stack
 *
 * sk  stack to get general name from
 * idx index to get
 *
 * return a pointer to the internal node of the stack
 */
WOLFSSL_GENERAL_NAME* wolfSSL_sk_GENERAL_NAME_value(WOLFSSL_STACK* sk, int idx)
{
    WOLFSSL_STACK* ret;

    if (sk == NULL) {
        return NULL;
    }

    ret = wolfSSL_sk_get_node(sk, idx);
    if (ret != NULL) {
        return ret->data.gn;
    }
    return NULL;
}

/* Gets the number of nodes in the stack
 *
 * sk  stack to get the number of nodes from
 *
 * returns the number of nodes, -1 if no nodes
 */
int wolfSSL_sk_GENERAL_NAME_num(WOLFSSL_STACK* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_num");

    if (sk == NULL) {
        return -1;
    }

    return (int)sk->num;
}

/* Allocates an empty GENERAL NAME stack */
WOLFSSL_STACK* wolfSSL_sk_GENERAL_NAME_new(void *cmpFunc) {
    WOLFSSL_STACK* sk = NULL;
    (void)cmpFunc;
    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_new");

    sk = wolfSSL_sk_new_null();
    if (sk != NULL) {
        sk->type = STACK_TYPE_GEN_NAME;
    }

    return sk;
}
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)

/* Frees all nodes in a GENERAL NAME stack
 *
 * sk stack of nodes to free
 * f  free function to use, not called with wolfSSL
 */
void wolfSSL_sk_GENERAL_NAME_pop_free(WOLFSSL_STACK* sk,
        void (*f) (WOLFSSL_GENERAL_NAME*))
{
    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

void wolfSSL_sk_GENERAL_NAME_free(WOLFSSL_STACK* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_GENERAL_NAME_free");
    wolfSSL_sk_X509_pop_free(sk, NULL);
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA
static void wolfSSL_DIST_POINT_NAME_free(WOLFSSL_DIST_POINT_NAME* dpn)
{
    if (dpn != NULL) {
        if (dpn->name.fullname != NULL) {
            wolfSSL_sk_X509_pop_free(dpn->name.fullname, NULL);
        }
        XFREE(dpn, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}


/* returns new pointer on success and NULL on fail */
static WOLFSSL_DIST_POINT_NAME* wolfSSL_DIST_POINT_NAME_new(void)
{
    WOLFSSL_DIST_POINT_NAME* dpn = NULL;
    WOLFSSL_GENERAL_NAMES* gns = NULL;

    dpn = (WOLFSSL_DIST_POINT_NAME*)XMALLOC(sizeof(WOLFSSL_DIST_POINT_NAME),
                                            NULL, DYNAMIC_TYPE_OPENSSL);
    if (dpn == NULL) {
        return NULL;
    }
    XMEMSET(dpn, 0, sizeof(WOLFSSL_DIST_POINT_NAME));

    gns = wolfSSL_sk_new_null();
    if (gns == NULL) {
        WOLFSSL_MSG("wolfSSL_sk_new_null error");
        XFREE(dpn, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }
    gns->type = STACK_TYPE_GEN_NAME;

    /* DIST_POINT_NAME type may be 0 or 1, indicating whether fullname or
     * relativename is used. See: RFC 5280 section 4.2.1.13 */
    dpn->name.fullname = gns;
    dpn->type = 0;

    return dpn;
}


/* Creates and returns new DIST_POINT structure */
WOLFSSL_DIST_POINT* wolfSSL_DIST_POINT_new(void)
{
    WOLFSSL_DIST_POINT* dp = NULL;
    WOLFSSL_DIST_POINT_NAME* dpn = NULL;

    WOLFSSL_ENTER("wolfSSL_DIST_POINT_new");

    dp = (WOLFSSL_DIST_POINT*)XMALLOC(sizeof(WOLFSSL_DIST_POINT), NULL,
                                      DYNAMIC_TYPE_OPENSSL);
    if (dp == NULL) {
        return NULL;
    }
    XMEMSET(dp, 0, sizeof(WOLFSSL_DIST_POINT));

    dpn = wolfSSL_DIST_POINT_NAME_new();
    if (dpn == NULL) {
        XFREE(dp, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }
    dp->distpoint = dpn;

    return dp;
}


/* Frees DIST_POINT objects.
*/
void wolfSSL_DIST_POINT_free(WOLFSSL_DIST_POINT* dp)
{
    WOLFSSL_ENTER("wolfSSL_DIST_POINT_free");
    if (dp != NULL) {
        wolfSSL_DIST_POINT_NAME_free(dp->distpoint);
        XFREE(dp, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

void wolfSSL_DIST_POINTS_free(WOLFSSL_DIST_POINTS *dps)
{
    WOLFSSL_ENTER("wolfSSL_DIST_POINTS_free");

    if (dps == NULL) {
        return;
    }

    wolfSSL_sk_free(dps);
}

/* return 1 on success 0 on fail */
int wolfSSL_sk_DIST_POINT_push(WOLFSSL_DIST_POINTS* sk, WOLFSSL_DIST_POINT* dp)
{
    WOLFSSL_ENTER("wolfSSL_sk_DIST_POINT_push");

    if (sk == NULL || dp == NULL) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_sk_push(sk, dp);
}

/* Returns the CRL dist point at index i from the stack
 *
 * sk  stack to get general name from
 * idx index to get
 *
 * return a pointer to the internal node of the stack
 */
WOLFSSL_DIST_POINT* wolfSSL_sk_DIST_POINT_value(WOLFSSL_STACK* sk, int idx)
{
    if (sk == NULL) {
        return NULL;
    }

    return (WOLFSSL_DIST_POINT*)wolfSSL_sk_value(sk, idx);
}

/* Gets the number of nodes in the stack
 *
 * sk  stack to get the number of nodes from
 *
 * returns the number of nodes, -1 if no nodes
 */
int wolfSSL_sk_DIST_POINT_num(WOLFSSL_STACK* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_DIST_POINT_num");

    if (sk == NULL) {
        return -1;
    }

    return wolfSSL_sk_num(sk);
}

/* Frees all nodes in a DIST_POINT stack
 *
 * sk stack of nodes to free
 * f  free function to use
 */
void wolfSSL_sk_DIST_POINT_pop_free(WOLFSSL_STACK* sk,
        void (*f) (WOLFSSL_DIST_POINT*))
{
    WOLFSSL_ENTER("wolfSSL_sk_DIST_POINT_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

void wolfSSL_sk_DIST_POINT_free(WOLFSSL_STACK* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_DIST_POINT_free");
    wolfSSL_sk_free(sk);
}

/* returns the number of nodes in stack on success and WOLFSSL_FATAL_ERROR
 * on fail */
int wolfSSL_sk_ACCESS_DESCRIPTION_num(WOLFSSL_STACK* sk)
{
    if (sk == NULL) {
        return WOLFSSL_FATAL_ERROR;
    }

    return (int)sk->num;
}

/* returns NULL on fail and pointer to internal data on success */
WOLFSSL_ACCESS_DESCRIPTION* wolfSSL_sk_ACCESS_DESCRIPTION_value(
        WOLFSSL_STACK* sk, int idx)
{
    WOLFSSL_STACK* ret;

    if (sk == NULL) {
        return NULL;
    }

    ret = wolfSSL_sk_get_node(sk, idx);
    if (ret != NULL) {
        return ret->data.access;
    }
    return NULL;
}
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
/* free's the internal type for the general name */
static void wolfSSL_GENERAL_NAME_type_free(WOLFSSL_GENERAL_NAME* name)
{
    if (name != NULL) {
        switch (name->type) {
        case GEN_IA5:
            wolfSSL_ASN1_STRING_free(name->d.ia5);
            name->d.ia5 = NULL;
            break;
        case GEN_EMAIL:
            wolfSSL_ASN1_STRING_free(name->d.rfc822Name);
            name->d.rfc822Name = NULL;
            break;
        case GEN_DNS:
            wolfSSL_ASN1_STRING_free(name->d.dNSName);
            name->d.dNSName = NULL;
            break;
        case GEN_DIRNAME:
            wolfSSL_X509_NAME_free(name->d.dirn);
            name->d.dirn = NULL;
            break;
        case GEN_URI:
            wolfSSL_ASN1_STRING_free(name->d.uniformResourceIdentifier);
            name->d.uniformResourceIdentifier = NULL;
            break;
        case GEN_IPADD:
            wolfSSL_ASN1_STRING_free(name->d.iPAddress);
            name->d.iPAddress = NULL;
            break;
        case GEN_RID:
            wolfSSL_ASN1_OBJECT_free(name->d.registeredID);
            name->d.registeredID = NULL;
            break;
        case GEN_OTHERNAME:
            if (name->d.otherName != NULL) {
                wolfSSL_ASN1_OBJECT_free(name->d.otherName->type_id);
                wolfSSL_ASN1_TYPE_free(name->d.otherName->value);
                XFREE(name->d.otherName, NULL, DYNAMIC_TYPE_ASN1);
                name->d.otherName = NULL;
            }
            break;
        case GEN_X400:
            /* Unsupported: fall through */
        case GEN_EDIPARTY:
            /* Unsupported: fall through */
        default:
            WOLFSSL_MSG("wolfSSL_GENERAL_NAME_type_free: possible leak");
            break;
        }
    }
}

/* sets the general name type and free's the existing one
 * can fail with a memory error if malloc fails or bad arg error
 * otherwise return WOLFSSL_SUCCESS */
int wolfSSL_GENERAL_NAME_set_type(WOLFSSL_GENERAL_NAME* name, int typ)
{
    int ret = WOLFSSL_SUCCESS;

    if (name != NULL) {
        wolfSSL_GENERAL_NAME_type_free(name);
        name->type = typ;

        switch (typ) {
            case GEN_URI:
                name->d.uniformResourceIdentifier = wolfSSL_ASN1_STRING_new();
                if (name->d.uniformResourceIdentifier == NULL)
                    ret = MEMORY_E;
                break;
            default:
                name->type = GEN_IA5;
                name->d.ia5 = wolfSSL_ASN1_STRING_new();
                if (name->d.ia5 == NULL)
                    ret = MEMORY_E;
        }
    }
    else {
        ret = BAD_FUNC_ARG;
    }

    return ret;
}

/* Set the value in a general name. This is a compat layer API.
 *
 * @param [out] a       Pointer to the GENERAL_NAME where the othername is set.
 * @param [in]  type    The type of this general name.
 * @param [in]  value   The ASN.1 string that is the value.
 * @return none
 * @note the set0 indicates we take ownership so the user does NOT free value.
 */
void wolfSSL_GENERAL_NAME_set0_value(WOLFSSL_GENERAL_NAME *a, int type,
                                     void *value)
{
    WOLFSSL_ASN1_STRING *val = (WOLFSSL_ASN1_STRING *)value;
    if (a == NULL) {
        WOLFSSL_MSG("a is NULL");
        return;
    }

    if (val == NULL) {
        WOLFSSL_MSG("value is NULL");
        return;
    }

    if (type != GEN_DNS) {
        WOLFSSL_MSG("Only GEN_DNS is supported");
        return;
    }

    wolfSSL_GENERAL_NAME_type_free(a);
    a->type = type;
    if (type == GEN_DNS) {
        a->d.dNSName = val;
    }
}

/* Frees GENERAL_NAME objects.
*/
void wolfSSL_GENERAL_NAME_free(WOLFSSL_GENERAL_NAME* name)
{
    WOLFSSL_ENTER("wolfSSL_GENERAL_NAME_Free");
    if (name != NULL) {
        wolfSSL_GENERAL_NAME_type_free(name);
        XFREE(name, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL*/

#ifdef OPENSSL_EXTRA
void wolfSSL_GENERAL_NAMES_free(WOLFSSL_GENERAL_NAMES *gens)
{
    WOLFSSL_ENTER("wolfSSL_GENERAL_NAMES_free");

    if (gens == NULL) {
        return;
    }

    wolfSSL_sk_GENERAL_NAME_free(gens);
}

#if defined(OPENSSL_ALL) && !defined(NO_BIO)
/* Outputs name string of the given WOLFSSL_GENERAL_NAME_OBJECT to WOLFSSL_BIO.
 * Can handle following GENERAL_NAME_OBJECT types:
 *  - GEN_OTHERNAME #
 *  - GEN_EMAIL
 *  - GEN_DNS
 *  - GEN_X400  #
 *  - GEN_DIRNAME
 *  - GEN_EDIPARTY #
 *  - GEN_URI
 *  - GEN_RID
 * The each name string to be output has "typename:namestring" format.
 * For instance, email name string will be output as "email:info@wolfssl.com".
 * However,some types above marked with "#" will be output with
 * "typename:<unsupported>".
 *
 * Parameters:
 *  - out: WOLFSSL_BIO object which is the output destination
 *  - gen: WOLFSSL_GENERAL_NAME object to be output its name
 *
 * Returns WOLFSSL_SUCCESS on success, WOLFSSL_FAILURE on failure.
 */
int wolfSSL_GENERAL_NAME_print(WOLFSSL_BIO* out, WOLFSSL_GENERAL_NAME* gen)
{
    int ret, i;
    unsigned int wd;
    unsigned char* p;
    (void)wd;
    (void)p;
    (void)i;
    WOLFSSL_ENTER("wolfSSL_GENERAL_NAME_print");

    if (out == NULL || gen == NULL)
        return WOLFSSL_FAILURE;

    ret = WOLFSSL_FAILURE;
    switch (gen->type)
    {
    case GEN_OTHERNAME:
        ret = wolfSSL_BIO_printf(out, "othername:<unsupported>");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        break;

    case GEN_EMAIL:
        ret = wolfSSL_BIO_printf(out, "email:");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS)
        {
            ret = wolfSSL_ASN1_STRING_print(out, gen->d.rfc822Name);
        }
        break;

    case GEN_DNS:
        ret = wolfSSL_BIO_printf(out, "DNS:");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS) {
            ret = wolfSSL_BIO_printf(out, "%s", gen->d.dNSName->strData);
            ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        }
        break;

    case GEN_X400:
        ret = wolfSSL_BIO_printf(out, "X400Name:<unsupported>");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        break;

    case GEN_DIRNAME:
        ret = wolfSSL_BIO_printf(out, "DirName:");
        if (ret == WOLFSSL_SUCCESS) {
            ret = wolfSSL_X509_NAME_print_ex(out, gen->d.directoryName, 0,
                                                         XN_FLAG_ONELINE);
        }
        break;

    case GEN_EDIPARTY:
        ret = wolfSSL_BIO_printf(out, "EdiPartyName:<unsupported>");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        break;

    case GEN_URI:
        ret = wolfSSL_BIO_printf(out, "URI:");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS) {
            ret = wolfSSL_ASN1_STRING_print(out,
                                    gen->d.uniformResourceIdentifier);
        }
        break;

    case GEN_IPADD:
        ret = wolfSSL_BIO_printf(out, "IP Address");
        ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS) {

            if (!gen->d.iPAddress->length) {
                ret = WOLFSSL_FAILURE;
                break;
            }
            p = (unsigned char*)gen->d.iPAddress->strData;

            if (gen->d.iPAddress->length == 4) {
                ret = wolfSSL_BIO_printf(out, ":%d.%d.%d.%d",
                                  p[0],p[1],p[2],p[3]);
            }
            else if (gen->d.iPAddress->length == 16) {

                for (i = 0; i < 16 && ret == WOLFSSL_SUCCESS;) {
                    wd = p[i] << 8 | p[i+1];

                    i += 2;
                    ret = wolfSSL_BIO_printf(out, ":%X", wd);
                    ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
                }
            }
            else {
                ret = wolfSSL_BIO_printf(out, "<unsupported>");
            }
            ret = (ret > 0) ? WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
        }
        break;

    case GEN_RID:
        ret = wolfSSL_BIO_printf(out, "Registered ID:");
        if (ret == WOLFSSL_SUCCESS) {
            ret = wolfSSL_i2a_ASN1_OBJECT(out, gen->d.registeredID);
        }
        break;

    default:
        /* unsupported type */
        break;
    }

    if (ret == WOLFSSL_FAILURE)
        return WOLFSSL_FAILURE;
    else
        return WOLFSSL_SUCCESS;
}
#endif /* OPENSSL_ALL */

WOLF_STACK_OF(WOLFSSL_X509_EXTENSION)* wolfSSL_sk_X509_EXTENSION_new_null(void)
{
    WOLFSSL_STACK* sk = wolfSSL_sk_new_node(NULL);
    if (sk) {
        sk->type = STACK_TYPE_X509_EXT;
    }

    return (WOLF_STACK_OF(WOLFSSL_X509_EXTENSION)*)sk;;
}

/* returns the number of nodes on the stack */
int wolfSSL_sk_X509_EXTENSION_num(WOLF_STACK_OF(WOLFSSL_X509_EXTENSION)* sk)
{
    if (sk != NULL) {
        return (int)sk->num;
    }
    return WOLFSSL_FATAL_ERROR;
}


/* returns null on failure and pointer to internal value on success */
WOLFSSL_X509_EXTENSION* wolfSSL_sk_X509_EXTENSION_value(
        WOLF_STACK_OF(WOLFSSL_X509_EXTENSION)* sk, int idx)
{
    WOLFSSL_STACK* ret;

    if (sk == NULL) {
        return NULL;
    }

    ret = wolfSSL_sk_get_node(sk, idx);
    if (ret != NULL) {
        return ret->data.ext;
    }
    return NULL;
}

/* frees all of the nodes and the values in stack */
void wolfSSL_sk_X509_EXTENSION_pop_free(
        WOLF_STACK_OF(WOLFSSL_X509_EXTENSION)* sk,
        void (*f) (WOLFSSL_X509_EXTENSION*))
{
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) && !defined(NO_FILESYSTEM) && !defined(NO_STDIO_FILESYSTEM)

WOLFSSL_X509* wolfSSL_X509_d2i_fp(WOLFSSL_X509** x509, XFILE file)
{
    WOLFSSL_X509* newX509 = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_d2i_fp");

    if (file != XBADFILE) {
        byte* fileBuffer = NULL;
        long sz = 0;

        if (XFSEEK(file, 0, XSEEK_END) != 0)
            return NULL;
        sz = XFTELL(file);
        if (XFSEEK(file, 0, XSEEK_SET) != 0)
            return NULL;

        if (sz > MAX_WOLFSSL_FILE_SIZE || sz < 0) {
            WOLFSSL_MSG("X509_d2i file size error");
            return NULL;
        }

        fileBuffer = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE);
        if (fileBuffer != NULL) {
            int ret = (int)XFREAD(fileBuffer, 1, sz, file);
            if (ret == sz) {
                newX509 = wolfSSL_X509_d2i(NULL, fileBuffer, (int)sz);
            }
            XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);
        }
    }

    if (x509 != NULL)
        *x509 = newX509;

    return newX509;
}

#endif /* OPENSSL_EXTRA && !NO_FILESYSTEM && !NO_STDIO_FILESYSTEM */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL) || \
    defined(KEEP_PEER_CERT) || defined(SESSION_CERTS)

#ifndef NO_FILESYSTEM
WOLFSSL_ABI
WOLFSSL_X509* wolfSSL_X509_load_certificate_file(const char* fname, int format)
{
#ifdef WOLFSSL_SMALL_STACK
    byte  staticBuffer[1]; /* force heap usage */
#else
    byte  staticBuffer[FILE_BUFFER_SIZE];
#endif
    byte* fileBuffer = staticBuffer;
    int   dynamic = 0;
    int   ret;
    long  sz = 0;
    XFILE file;

    WOLFSSL_X509* x509 = NULL;

    /* Check the inputs */
    if ((fname == NULL) ||
        (format != WOLFSSL_FILETYPE_ASN1 && format != WOLFSSL_FILETYPE_PEM))
        return NULL;

    file = XFOPEN(fname, "rb");
    if (file == XBADFILE)
        return NULL;

    if (XFSEEK(file, 0, XSEEK_END) != 0){
        XFCLOSE(file);
        return NULL;
    }
    sz = XFTELL(file);
    if (XFSEEK(file, 0, XSEEK_SET) != 0){
        XFCLOSE(file);
        return NULL;
    }

    if (sz > MAX_WOLFSSL_FILE_SIZE || sz < 0) {
        WOLFSSL_MSG("X509_load_certificate_file size error");
        XFCLOSE(file);
        return NULL;
    }

    if (sz > (long)sizeof(staticBuffer)) {
        fileBuffer = (byte*)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE);
        if (fileBuffer == NULL) {
            XFCLOSE(file);
            return NULL;
        }
        dynamic = 1;
    }

    ret = (int)XFREAD(fileBuffer, 1, sz, file);
    if (ret != sz) {
        XFCLOSE(file);
        if (dynamic)
            XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);
        return NULL;
    }

    XFCLOSE(file);

    x509 = wolfSSL_X509_load_certificate_buffer(fileBuffer, (int)sz, format);

    if (dynamic)
        XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);

    return x509;
}
#endif /* !NO_FILESYSTEM */

static WOLFSSL_X509* loadX509orX509REQFromBuffer(
    const unsigned char* buf, int sz, int format, int type)
{

    int ret;
    WOLFSSL_X509* x509 = NULL;
    DerBuffer* der = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_load_certificate_ex");

    if (format == WOLFSSL_FILETYPE_PEM) {
    #ifdef WOLFSSL_PEM_TO_DER
        if (PemToDer(buf, sz, type, &der, NULL, NULL, NULL) != 0) {
            FreeDer(&der);
        }
    #else
        ret = NOT_COMPILED_IN;
    #endif
    }
    else {
        ret = AllocDer(&der, (word32)sz, type, NULL);
        if (ret == 0) {
            XMEMCPY(der->buffer, buf, sz);
        }
    }

    /* At this point we want `der` to have the certificate in DER format */
    /* ready to be decoded. */
    if (der != NULL && der->buffer != NULL) {
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert;
    #else
        DecodedCert  cert[1];
    #endif

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                     DYNAMIC_TYPE_DCERT);
        if (cert != NULL)
    #endif
        {
            InitDecodedCert(cert, der->buffer, der->length, NULL);
            if (ParseCertRelative(cert, type, 0, NULL) == 0) {
                x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
                                                             DYNAMIC_TYPE_X509);
                if (x509 != NULL) {
                    InitX509(x509, 1, NULL);
                    if (CopyDecodedToX509(x509, cert) != 0) {
                        wolfSSL_X509_free(x509);
                        x509 = NULL;
                    }
                }
            }

            FreeDecodedCert(cert);
        #ifdef WOLFSSL_SMALL_STACK
            XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
        #endif
        }

        FreeDer(&der);
    }

    return x509;
}

WOLFSSL_X509* wolfSSL_X509_load_certificate_buffer(
    const unsigned char* buf, int sz, int format)
{
    return loadX509orX509REQFromBuffer(buf, sz,
            format, CERT_TYPE);
}

#ifdef WOLFSSL_CERT_REQ
WOLFSSL_X509* wolfSSL_X509_REQ_load_certificate_buffer(
    const unsigned char* buf, int sz, int format)
{
    return loadX509orX509REQFromBuffer(buf, sz,
            format, CERTREQ_TYPE);
}
#endif

#endif /* KEEP_PEER_CERT || SESSION_CERTS */

#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(KEEP_PEER_CERT) || \
    defined(SESSION_CERTS)
/* Smaller subset of X509 compatibility functions. Avoid increasing the size of
 * this subset and its memory usage */

/* returns a pointer to a new WOLFSSL_X509 structure on success and NULL on
 * fail
 */
WOLFSSL_X509* wolfSSL_X509_new(void)
{
    WOLFSSL_X509* x509;

    x509 = (WOLFSSL_X509*)XMALLOC(sizeof(WOLFSSL_X509), NULL,
            DYNAMIC_TYPE_X509);
    if (x509 != NULL) {
        InitX509(x509, 1, NULL);
    }

    return x509;
}

WOLFSSL_ABI
WOLFSSL_X509_NAME* wolfSSL_X509_get_subject_name(WOLFSSL_X509* cert)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_subject_name");
    if (cert)
        return &cert->subject;
    return NULL;
}

WOLFSSL_ABI
WOLFSSL_X509_NAME* wolfSSL_X509_get_issuer_name(WOLFSSL_X509* cert)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_issuer_name");
    if (cert)
        return &cert->issuer;
    return NULL;
}


int wolfSSL_X509_get_signature_type(WOLFSSL_X509* x509)
{
    int type = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_signature_type");

    if (x509 != NULL)
        type = x509->sigOID;

    return type;
}

#if defined(OPENSSL_EXTRA_X509_SMALL)

int wolfSSL_X509_NAME_get_sz(WOLFSSL_X509_NAME* name)
{
    WOLFSSL_ENTER("wolfSSL_X509_NAME_get_sz");
    if (!name)
        return -1;
    return name->sz;
}

/* Searches for the first ENTRY of type NID
 * idx is the location to start searching from, the value at when the entry was
 *     found is stored into idx
 * returns a pointer to the entry on success and null on fail */
static WOLFSSL_X509_NAME_ENTRY* GetEntryByNID(WOLFSSL_X509_NAME* name, int nid,
        int* idx)
{
    int i;
    WOLFSSL_X509_NAME_ENTRY* ret = NULL;

    /* and index of less than 0 is assumed to be starting from 0 */
    if (*idx < 0) {
        *idx = 0;
    }

    for (i = *idx; i < MAX_NAME_ENTRIES; i++) {
        if (name->entry[i].nid == nid) {
            ret = &name->entry[i];
            *idx = i;
            break;
        }
    }
    return ret;
}


/* Used to get a string from the WOLFSSL_X509_NAME structure that
 * corresponds with the NID value passed in. This finds the first entry with
 * matching NID value, if searching for the case where there is multiple
 * entries with the same NID value than other functions should be used
 * (i.e. wolfSSL_X509_NAME_get_index_by_NID, wolfSSL_X509_NAME_get_entry)
 *
 * name structure to get string from
 * nid  NID value to search for
 * buf  [out] buffer to hold results. If NULL then the buffer size minus the
 *      null char is returned.
 * len  size of "buf" passed in
 *
 * returns the length of string found, not including the NULL terminator.
 *         It's possible the function could return a negative value in the
 *         case that len is less than or equal to 0. A negative value is
 *         considered an error case.
 */
int wolfSSL_X509_NAME_get_text_by_NID(WOLFSSL_X509_NAME* name,
                                      int nid, char* buf, int len)
{
    WOLFSSL_X509_NAME_ENTRY* e;
    unsigned char *text = NULL;
    int textSz = 0;
    int idx    = 0;

    WOLFSSL_ENTER("wolfSSL_X509_NAME_get_text_by_NID");

    if (name == NULL) {
        WOLFSSL_MSG("NULL argument passed in");
        return WOLFSSL_FATAL_ERROR;
    }

    e = GetEntryByNID(name, nid, &idx);
    if (e == NULL) {
        WOLFSSL_MSG("Entry type not found");
        return WOLFSSL_FATAL_ERROR;
    }
    text   = wolfSSL_ASN1_STRING_data(e->value);
    textSz = wolfSSL_ASN1_STRING_length(e->value);

    if (text == NULL) {
        WOLFSSL_MSG("Unable to get entry text");
        return WOLFSSL_FATAL_ERROR;
    }

    /* if buf is NULL return size of buffer needed (minus null char) */
    if (buf == NULL) {
        WOLFSSL_MSG("Buffer is NULL, returning buffer size only");
        return textSz;
    }

    /* buf is not NULL from above */
    if (text != NULL) {
        textSz = min(textSz + 1, len); /* + 1 to account for null char */
        if (textSz > 0) {
            XMEMCPY(buf, text, textSz - 1);
            buf[textSz - 1] = '\0';
        }
    }

    WOLFSSL_LEAVE("wolfSSL_X509_NAME_get_text_by_NID", textSz);
    return (textSz - 1); /* do not include null character in size */
}

/* Creates a new WOLFSSL_EVP_PKEY structure that has the public key from x509
 *
 * returns a pointer to the created WOLFSSL_EVP_PKEY on success and NULL on fail
 */
WOLFSSL_EVP_PKEY* wolfSSL_X509_get_pubkey(WOLFSSL_X509* x509)
{
    WOLFSSL_EVP_PKEY* key = NULL;
    int ret;

    (void)ret;

    WOLFSSL_ENTER("wolfSSL_X509_get_pubkey");
    if (x509 != NULL) {
        key = wolfSSL_EVP_PKEY_new_ex(x509->heap);
        if (key != NULL) {
            if (x509->pubKeyOID == RSAk) {
                key->type = EVP_PKEY_RSA;
            }
            else if (x509->pubKeyOID == DSAk) {
                key->type = EVP_PKEY_DSA;
            }
            else {
                key->type = EVP_PKEY_EC;
            }
            key->save_type = 0;
            key->pkey.ptr = (char*)XMALLOC(
                        x509->pubKey.length, x509->heap,
                                                       DYNAMIC_TYPE_PUBLIC_KEY);
            if (key->pkey.ptr == NULL) {
                wolfSSL_EVP_PKEY_free(key);
                return NULL;
            }
            XMEMCPY(key->pkey.ptr, x509->pubKey.buffer, x509->pubKey.length);
            key->pkey_sz = x509->pubKey.length;

            #ifdef HAVE_ECC
                key->pkey_curve = (int)x509->pkCurveOID;
            #endif /* HAVE_ECC */

            /* decode RSA key */
            #ifndef NO_RSA
            if (key->type == EVP_PKEY_RSA) {
                key->ownRsa = 1;
                key->rsa = wolfSSL_RSA_new();
                if (key->rsa == NULL) {
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }

                if (wolfSSL_RSA_LoadDer_ex(key->rsa,
                            (const unsigned char*)key->pkey.ptr, key->pkey_sz,
                            WOLFSSL_RSA_LOAD_PUBLIC) != WOLFSSL_SUCCESS) {
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }
            }
            #endif /* NO_RSA */

            /* decode ECC key */
            #if defined(HAVE_ECC) && defined(OPENSSL_EXTRA)
            if (key->type == EVP_PKEY_EC) {
                word32 idx = 0;

                key->ownEcc = 1;
                key->ecc = wolfSSL_EC_KEY_new();
                if (key->ecc == NULL || key->ecc->internal == NULL) {
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }

                /* not using wolfSSL_EC_KEY_LoadDer because public key in x509
                 * is in the format of x963 (no sequence at start of buffer) */
                ret = wc_EccPublicKeyDecode((const unsigned char*)key->pkey.ptr,
                                            &idx, (ecc_key*)key->ecc->internal,
                                            key->pkey_sz);
                if (ret < 0) {
                    WOLFSSL_ERROR_VERBOSE(ret);
                    WOLFSSL_MSG("wc_EccPublicKeyDecode failed");
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }

                if (SetECKeyExternal(key->ecc) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("SetECKeyExternal failed");
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }

                key->ecc->inSet = 1;
            }
            #endif /* HAVE_ECC && OPENSSL_EXTRA */

            #ifndef NO_DSA
            if (key->type == EVP_PKEY_DSA) {
                key->ownDsa = 1;
                key->dsa = wolfSSL_DSA_new();
                if (key->dsa == NULL) {
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }

                if (wolfSSL_DSA_LoadDer_ex(key->dsa,
                            (const unsigned char*)key->pkey.ptr, key->pkey_sz, \
                            WOLFSSL_DSA_LOAD_PUBLIC) != WOLFSSL_SUCCESS) {
                    wolfSSL_DSA_free(key->dsa);
                    key->dsa = NULL;
                    wolfSSL_EVP_PKEY_free(key);
                    return NULL;
                }
            }
            #endif /* NO_DSA */
        }
    }
    return key;
}
#endif /* OPENSSL_EXTRA_X509_SMALL */

/* End of smaller subset of X509 compatibility functions. Avoid increasing the
 * size of this subset and its memory usage */
#endif /* OPENSSL_EXTRA_X509_SMALL || KEEP_PEER_CERT || SESSION_CERTS */

#if defined(OPENSSL_ALL)
/*
 * Converts a and b to DER and then does an XMEMCMP to check if they match.
 * Returns 0 when certificates match and WOLFSSL_FATAL_ERROR when they don't.
 */
int wolfSSL_X509_cmp(const WOLFSSL_X509 *a, const WOLFSSL_X509 *b)
{
        const byte* derA;
        const byte* derB;
        int outSzA = 0;
        int outSzB = 0;

        if (a == NULL || b == NULL){
            return BAD_FUNC_ARG;
        }

        derA = wolfSSL_X509_get_der((WOLFSSL_X509*)a, &outSzA);
        if (derA == NULL){
            WOLFSSL_MSG("wolfSSL_X509_get_der - certificate A has failed");
            return WOLFSSL_FATAL_ERROR;
        }
        derB = wolfSSL_X509_get_der((WOLFSSL_X509*)b, &outSzB);
        if (derB == NULL){
            WOLFSSL_MSG("wolfSSL_X509_get_der - certificate B has failed");
            return WOLFSSL_FATAL_ERROR;
        }

        if (outSzA != outSzB || XMEMCMP(derA, derB, outSzA) != 0) {
            WOLFSSL_LEAVE("wolfSSL_X509_cmp", WOLFSSL_FATAL_ERROR);
            return WOLFSSL_FATAL_ERROR;
        }

        WOLFSSL_LEAVE("wolfSSL_X509_cmp", 0);

        return 0;
    }
#endif /* OPENSSL_ALL */

#if defined(OPENSSL_EXTRA)
    int wolfSSL_X509_ext_isSet_by_NID(WOLFSSL_X509* x509, int nid)
    {
        int isSet = 0;

        WOLFSSL_ENTER("wolfSSL_X509_ext_isSet_by_NID");

        if (x509 != NULL) {
            switch (nid) {
                case NID_basic_constraints: isSet = x509->basicConstSet; break;
                case NID_subject_alt_name: isSet = x509->subjAltNameSet; break;
                case NID_authority_key_identifier: isSet = x509->authKeyIdSet; break;
                case NID_subject_key_identifier: isSet = x509->subjKeyIdSet; break;
                case NID_key_usage: isSet = x509->keyUsageSet; break;
                case NID_crl_distribution_points: isSet = x509->CRLdistSet; break;
                case NID_ext_key_usage: isSet = ((x509->extKeyUsageSrc) ? 1 : 0);
                    break;
                case NID_info_access: isSet = x509->authInfoSet; break;
                #if defined(WOLFSSL_SEP) || defined(WOLFSSL_QT)
                    case NID_certificate_policies: isSet = x509->certPolicySet; break;
                #endif /* WOLFSSL_SEP || WOLFSSL_QT */
                default:
                    WOLFSSL_MSG("NID not in table");
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_ext_isSet_by_NID", isSet);

        return isSet;
    }


    int wolfSSL_X509_ext_get_critical_by_NID(WOLFSSL_X509* x509, int nid)
    {
        int crit = 0;

        WOLFSSL_ENTER("wolfSSL_X509_ext_get_critical_by_NID");

        if (x509 != NULL) {
            switch (nid) {
                case NID_basic_constraints: crit = x509->basicConstCrit; break;
                case NID_subject_alt_name: crit = x509->subjAltNameCrit; break;
                case NID_authority_key_identifier: crit = x509->authKeyIdCrit; break;
                case NID_subject_key_identifier: crit = x509->subjKeyIdCrit; break;
                case NID_key_usage: crit = x509->keyUsageCrit; break;
                case NID_crl_distribution_points: crit= x509->CRLdistCrit; break;
                case NID_ext_key_usage: crit= x509->extKeyUsageCrit; break;
                #if defined(WOLFSSL_SEP) || defined(WOLFSSL_QT)
                    case NID_certificate_policies: crit = x509->certPolicyCrit; break;
                #endif /* WOLFSSL_SEP || WOLFSSL_QT */
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_ext_get_critical_by_NID", crit);

        return crit;
    }


    int wolfSSL_X509_get_isSet_pathLength(WOLFSSL_X509* x509)
    {
        int isSet = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_isSet_pathLength");

        if (x509 != NULL)
            isSet = x509->basicConstPlSet;

        WOLFSSL_LEAVE("wolfSSL_X509_get_isSet_pathLength", isSet);

        return isSet;
    }


    word32 wolfSSL_X509_get_pathLength(WOLFSSL_X509* x509)
    {
        word32 pathLength = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_pathLength");

        if (x509 != NULL)
            pathLength = x509->pathLength;

        WOLFSSL_LEAVE("wolfSSL_X509_get_pathLength", pathLength);

        return pathLength;
    }


    unsigned int wolfSSL_X509_get_keyUsage(WOLFSSL_X509* x509)
    {
        word16 usage = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_keyUsage");

        if (x509 != NULL)
            usage = x509->keyUsage;

        WOLFSSL_LEAVE("wolfSSL_X509_get_keyUsage", usage);

        return usage;
    }


    byte* wolfSSL_X509_get_authorityKeyID(WOLFSSL_X509* x509,
                                          byte* dst, int* dstLen)
    {
        byte *id = NULL;
        int copySz = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_authorityKeyID");

        if (x509 != NULL) {
            if (x509->authKeyIdSet) {
                copySz = min(dstLen != NULL ? *dstLen : 0,
                             (int)x509->authKeyIdSz);
                id = x509->authKeyId;
            }

            if (dst != NULL && dstLen != NULL && id != NULL && copySz > 0) {
                XMEMCPY(dst, id, copySz);
                id = dst;
                *dstLen = copySz;
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_get_authorityKeyID", copySz);

        return id;
    }

    byte* wolfSSL_X509_get_subjectKeyID(WOLFSSL_X509* x509,
                                        byte* dst, int* dstLen)
    {
        byte *id = NULL;
        int copySz = 0;

        WOLFSSL_ENTER("wolfSSL_X509_get_subjectKeyID");

        if (x509 != NULL) {
            if (x509->subjKeyIdSet) {
                copySz = min(dstLen != NULL ? *dstLen : 0,
                                                        (int)x509->subjKeyIdSz);
                id = x509->subjKeyId;
            }

            if (dst != NULL && dstLen != NULL && id != NULL && copySz > 0) {
                XMEMCPY(dst, id, copySz);
                id = dst;
                *dstLen = copySz;
            }
        }

        WOLFSSL_LEAVE("wolfSSL_X509_get_subjectKeyID", copySz);

        return id;
    }
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL) || \
    defined(OPENSSL_EXTRA_X509_SMALL)

    /* Looks up the index of the first entry encountered with matching NID
     * The search starts from index 'pos'
     * returns a negative value on failure and positive index value on success*/
    int wolfSSL_X509_NAME_get_index_by_NID(WOLFSSL_X509_NAME* name,
                                          int nid, int pos)
    {
        int value = nid, i;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_get_index_by_NID");

        if (name == NULL) {
            return BAD_FUNC_ARG;
        }

        i = pos + 1; /* start search after index passed in */
        if (i < 0) {
            i = 0;
        }

        for (;i < name->entrySz && i < MAX_NAME_ENTRIES; i++) {
            if (name->entry[i].nid == value) {
                return i;
            }
        }
        return WOLFSSL_FATAL_ERROR;
    }


    WOLFSSL_ASN1_STRING*  wolfSSL_X509_NAME_ENTRY_get_data(
                                                    WOLFSSL_X509_NAME_ENTRY* in)
    {
        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_get_data");
        if (in == NULL)
            return NULL;

        return in->value;
    }

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA
#ifndef NO_BIO

#ifndef MAX_WIDTH
    #define MAX_WIDTH 80
#endif

static int X509PrintSubjAltName(WOLFSSL_BIO* bio, WOLFSSL_X509* x509,
        int indent)
{
    int ret = WOLFSSL_SUCCESS;
    DNS_entry* entry;

    if (bio == NULL || x509 == NULL) {
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS && x509->subjAltNameSet &&
            x509->altNames != NULL) {
        char scratch[MAX_WIDTH];
        int len;

        len = XSNPRINTF(scratch, MAX_WIDTH, "%*s", indent, "");
        if (len >= MAX_WIDTH)
            ret = WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS) {
            if (wolfSSL_BIO_write(bio, scratch, (int)XSTRLEN(scratch)) <= 0) {
                ret = WOLFSSL_FAILURE;
            }
        }
        if (ret == WOLFSSL_SUCCESS) {
            int nameCount = 0;

            entry = x509->altNames;
            while (entry != NULL) {
                ++nameCount;
                if (nameCount > 1) {
                    if (wolfSSL_BIO_write(bio, ", ", 2) <= 0) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }

                if (entry->type == ASN_DNS_TYPE) {
                    len = XSNPRINTF(scratch, MAX_WIDTH, "DNS:%s", entry->name);
                    if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
            #if defined(OPENSSL_ALL) || defined(WOLFSSL_IP_ALT_NAME)
                else if (entry->type == ASN_IP_TYPE) {
                    len = XSNPRINTF(scratch, MAX_WIDTH, "IP Address:%s",
                            entry->ipString);
                    if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
            #endif /* OPENSSL_ALL || WOLFSSL_IP_ALT_NAME */
                else if (entry->type == ASN_RFC822_TYPE) {
                    len = XSNPRINTF(scratch, MAX_WIDTH, "email:%s",
                            entry->name);
                    if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
                else if (entry->type == ASN_DIR_TYPE) {
                    /* @TODO entry->name in ASN1 syntax */
                    len = XSNPRINTF(scratch, MAX_WIDTH,
                        "DirName:<print out not supported yet>");
                    if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
                else if (entry->type == ASN_URI_TYPE) {
                    len = XSNPRINTF(scratch, MAX_WIDTH, "URI:%s",
                        entry->name);
                     if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
            #if defined(OPENSSL_ALL)
                else if (entry->type == ASN_RID_TYPE) {
                    len = XSNPRINTF(scratch, MAX_WIDTH, "Registered ID:%s",
                        entry->ridString);
                    if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
            #endif
                else if (entry->type == ASN_OTHER_TYPE) {
                    len = XSNPRINTF(scratch, MAX_WIDTH,
                        "othername <unsupported>");
                    if (len >= MAX_WIDTH) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }
                else {
                    WOLFSSL_MSG("Bad alt name type.");
                    ret = WOLFSSL_FAILURE;
                    break;
                }

                if (wolfSSL_BIO_write(bio, scratch, (int)XSTRLEN(scratch))
                        <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }

                entry = entry->next;
            }
        }

        if (ret == WOLFSSL_SUCCESS && wolfSSL_BIO_write(bio, "\n", 1) <= 0) {
            ret = WOLFSSL_FAILURE;
        }
    }

    return ret;
}

#ifdef XSNPRINTF
static int X509PrintKeyUsage(WOLFSSL_BIO* bio, WOLFSSL_X509* x509, int indent)
{
    int ret = WOLFSSL_SUCCESS;
    const int usages[] = {
        KEYUSE_DIGITAL_SIG,
        KEYUSE_CONTENT_COMMIT,
        KEYUSE_KEY_ENCIPHER,
        KEYUSE_DATA_ENCIPHER,
        KEYUSE_KEY_AGREE,
        KEYUSE_KEY_CERT_SIGN,
        KEYUSE_CRL_SIGN,
        KEYUSE_ENCIPHER_ONLY,
        KEYUSE_DECIPHER_ONLY
    };
    const char* usageStrs[] = {
        "Digital Signature",
        "Non Repudiation",
        "Key Encipherment",
        "Data Encipherment",
        "Key Agreement",
        "Certificate Sign",
        "CRL Sign",
        "Encipher Only",
        "Decipher Only"
    };

    if (bio == NULL || x509 == NULL) {
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS && x509->keyUsageSet && x509->keyUsage != 0) {
        char scratch[MAX_WIDTH];
        int len;
        word32 i = 0;
        int usageCount = 0;

        len = XSNPRINTF(scratch, MAX_WIDTH, "%*s", indent, "");
        if (len >= MAX_WIDTH)
            ret = WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS) {
            if (wolfSSL_BIO_write(bio, scratch, (int)XSTRLEN(scratch)) <= 0) {
                ret = WOLFSSL_FAILURE;
            }
        }
        for (; ret == WOLFSSL_SUCCESS && i < sizeof(usages) / sizeof(usages[i]);
             i++) {
            if (x509->keyUsage & usages[i]) {
                ++usageCount;
                if (usageCount > 1 && wolfSSL_BIO_write(bio, ", ", 2) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                if (wolfSSL_BIO_write(bio, usageStrs[i],
                                      (int)XSTRLEN(usageStrs[i])) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
        }
        if (ret == WOLFSSL_SUCCESS && wolfSSL_BIO_write(bio, "\n", 1) <= 0) {
            ret = WOLFSSL_FAILURE;
        }
    }

    return ret;
}

static int X509PrintExtendedKeyUsage(WOLFSSL_BIO* bio, WOLFSSL_X509* x509,
        int indent)
{
    int ret = WOLFSSL_SUCCESS;
    const int usages[] = {
        EXTKEYUSE_OCSP_SIGN,
        EXTKEYUSE_TIMESTAMP,
        EXTKEYUSE_EMAILPROT,
        EXTKEYUSE_CODESIGN,
        EXTKEYUSE_CLIENT_AUTH,
        EXTKEYUSE_SERVER_AUTH
    };
    const char* usageStrs[] = {
        "OCSP Signing",
        "Time Stamping",
        "E-mail Protection",
        "Code Signing",
        "TLS Web Client Authentication",
        "TLS Web Server Authentication"
    };

    if (bio == NULL || x509 == NULL) {
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS && x509->extKeyUsageCount > 0
            && x509->extKeyUsage != 0) {
        char scratch[MAX_WIDTH];
        int len;
        word32 i = 0;
        int usageCount = 0;

        len = XSNPRINTF(scratch, MAX_WIDTH, "%*s", indent, "");
        if (len >= MAX_WIDTH)
            ret = WOLFSSL_FAILURE;
        if (ret == WOLFSSL_SUCCESS) {
            if (wolfSSL_BIO_write(bio, scratch, (int)XSTRLEN(scratch)) <= 0) {
                ret = WOLFSSL_FAILURE;
            }
        }
        for (; ret == WOLFSSL_SUCCESS && i < sizeof(usages) / sizeof(usages[i]);
             i++) {
            if (x509->extKeyUsage & usages[i]) {
                ++usageCount;
                if (usageCount > 1 && wolfSSL_BIO_write(bio, ", ", 2) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                if (wolfSSL_BIO_write(bio, usageStrs[i],
                                      (int)XSTRLEN(usageStrs[i])) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
        }
        if (ret == WOLFSSL_SUCCESS && wolfSSL_BIO_write(bio, "\n", 1) <= 0) {
            ret = WOLFSSL_FAILURE;
        }
    }

    return ret;
}


/* print serial number out
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintSerial_ex(WOLFSSL_BIO* bio, byte* serial, int sz,
        int delimiter, int indent)
{
    char scratch[MAX_WIDTH];
    const int scratchSz = sizeof(scratch);
    int scratchLen;

    if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH, "%*sSerial Number:",
                                 indent, "")) >= MAX_WIDTH) {
        WOLFSSL_MSG("buffer overrun");
        return WOLFSSL_FAILURE;
    }
    if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (sz > (int)sizeof(byte)) {
        int i;

        /* serial is larger than int size so print off hex values */
        if ((scratchLen = XSNPRINTF(
                 scratch, MAX_WIDTH, "\n%*s", indent + 4, ""))
                >= MAX_WIDTH) {
            WOLFSSL_MSG("buffer overrun");
            return WOLFSSL_FAILURE;
        }
        for (i = 0; i < sz; i++) {
            int valLen;

            if ((valLen = XSNPRINTF(
                     scratch + scratchLen, scratchSz - scratchLen,
                     "%02x%s", serial[i], (i < sz - 1) ?
                     (delimiter ? ":" : "") : "\n"))
                >= scratchSz - scratchLen)
            {
                WOLFSSL_MSG("buffer overrun");
                return WOLFSSL_FAILURE;
            }
            scratchLen += valLen;
        }
        if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
            return WOLFSSL_FAILURE;
        }
    }

    /* if serial can fit into byte then print on the same line */
    else  {
        if ((scratchLen = XSNPRINTF(
                 scratch, MAX_WIDTH, " %d (0x%x)\n", serial[0], serial[0]))
            >= MAX_WIDTH)
        {
            WOLFSSL_MSG("buffer overrun");
            return WOLFSSL_FAILURE;
        }
        if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
            return WOLFSSL_FAILURE;
        }
    }
    return WOLFSSL_SUCCESS;
}

static int X509PrintSerial(WOLFSSL_BIO* bio, WOLFSSL_X509* x509, int indent)
{
    unsigned char serial[32];
    int  sz = sizeof(serial);

    XMEMSET(serial, 0, sz);
    if (wolfSSL_X509_get_serial_number(x509, serial, &sz) == WOLFSSL_SUCCESS) {
        X509PrintSerial_ex(bio, serial, sz, 1, indent);
    }
    return WOLFSSL_SUCCESS;
}

/* iterate through certificate extensions printing them out in human readable
 * form
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintExtensions(WOLFSSL_BIO* bio, WOLFSSL_X509* x509, int indent)
{
    int  ret = WOLFSSL_SUCCESS;
    char scratch[MAX_WIDTH];
    const int scratchSz = sizeof(scratch);
    int scratchLen;
    int  count, i;
    char* buf = NULL;

    count = wolfSSL_X509_get_ext_count(x509);
    if (count <= 0)
        return WOLFSSL_SUCCESS;

#ifdef WOLFSSL_CERT_REQ
    if (x509->isCSR) {
        if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH, "%*s%s\n", indent, "",
                      "Requested extensions:")) >= MAX_WIDTH) {
            return WOLFSSL_FAILURE;
        }
    }
    else
#endif
    {
        if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH, "%*s%s\n", indent, "",
                                     "X509v3 extensions:")) >= MAX_WIDTH) {
            return WOLFSSL_FAILURE;
        }
    }
    if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
        return WOLFSSL_FAILURE;
    }

    buf = (char*)XMALLOC(MAX_WIDTH-4-indent, x509->heap,
                         DYNAMIC_TYPE_TMP_BUFFER);
    if (buf == NULL) {
        return WOLFSSL_FAILURE;
    }

    for (i = 0; (i < count) && (ret != WOLFSSL_FAILURE); i++) {
        WOLFSSL_X509_EXTENSION* ext;

        ext = wolfSSL_X509_get_ext(x509, i);
        if (ext != NULL) {
            WOLFSSL_ASN1_OBJECT* obj;
            int nid;
            char val[6];
            int valLen;
            word32 j;

            obj = wolfSSL_X509_EXTENSION_get_object(ext);
            if (obj == NULL) {
                ret = WOLFSSL_FAILURE;
                break;
            }
            if (wolfSSL_OBJ_obj2txt(buf, MAX_WIDTH, obj, 0)
                == WOLFSSL_FAILURE)
            {
                ret = WOLFSSL_FAILURE;
                break;
            }
            if ((scratchLen = XSNPRINTF(
                     scratch, MAX_WIDTH, "%*s%s%s\n", indent + 4, "",
                     buf,
                     (wolfSSL_X509_EXTENSION_get_critical(ext)
                      ? ": critical"
                      : ": ")))
                >= MAX_WIDTH)
            {
                ret = WOLFSSL_FAILURE;
                break;
            }

            if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
                ret = WOLFSSL_FAILURE;
                break;
            }
            nid = wolfSSL_OBJ_obj2nid(obj);
            switch (nid) {
            case NID_subject_alt_name:
                ret = X509PrintSubjAltName(bio, x509, indent + 8);
                break;

            case NID_subject_key_identifier:
                if (!x509->subjKeyIdSet || x509->subjKeyId == NULL ||
                    x509->subjKeyIdSz == 0)
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }

                if ((scratchLen = XSNPRINTF(
                         scratch, scratchSz,
                         "%*s", indent + 8, "")) >= scratchSz)
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                for (j = 0; j < x509->subjKeyIdSz; j++) {
                    if ((valLen = XSNPRINTF(
                             val, sizeof(val), "%02X%s",
                             x509->subjKeyId[j],
                             (j < x509->subjKeyIdSz - 1) ? ":" : "\n"))
                        >= (int)sizeof(val))
                    {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                    if (scratchLen + valLen >= scratchSz) {
                        if (wolfSSL_BIO_write(bio, scratch,
                                              scratchLen) <= 0) {
                            ret = WOLFSSL_FAILURE;
                            break;
                        }
                        scratchLen = 0;
                    }
                    XMEMCPY(scratch + scratchLen, val, valLen);
                    scratchLen += valLen;
                }
                if (ret == WOLFSSL_FAILURE)
                    break;
                if (wolfSSL_BIO_write(bio, scratch,
                                      scratchLen) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                break;

            case NID_authority_key_identifier:
                if (!x509->authKeyIdSet || x509->authKeyId == NULL ||
                    x509->authKeyIdSz == 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }

                if ((scratchLen = XSNPRINTF(
                         scratch, scratchSz, "%*s%s",
                         indent + 8, "", "keyid:")) >= scratchSz)
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                for (j = 0; j < x509->authKeyIdSz; j++) {
                    if ((valLen = XSNPRINTF(
                             val, sizeof(val), "%02X%s",
                             x509->authKeyId[j],
                             (j < x509->authKeyIdSz - 1) ? ":" : "\n\n"))
                        >= (int)sizeof(val))
                    {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                    if (scratchLen >= scratchSz - valLen) {
                        if (wolfSSL_BIO_write(bio, scratch,
                                              scratchLen) <= 0)
                        {
                            ret = WOLFSSL_FAILURE;
                            break;
                        }
                        scratchLen = 0;
                    }
                    if (scratchLen + valLen >= scratchSz) {
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                    XMEMCPY(scratch + scratchLen, val, valLen);
                    scratchLen += valLen;
                }
                if (ret == WOLFSSL_FAILURE)
                    break;
                if (wolfSSL_BIO_write(bio, scratch,
                                      scratchLen) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                break;

            case NID_basic_constraints:
                if (!x509->basicConstSet) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                if ((scratchLen = XSNPRINTF(
                         scratch, scratchSz,
                         "%*sCA:%s\n",
                         indent + 8, "", (x509->isCa)? "TRUE": "FALSE"))
                    >= scratchSz)
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                if (wolfSSL_BIO_write(bio, scratch,
                                      scratchLen) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                break;

            case NID_key_usage:
                ret = X509PrintKeyUsage(bio, x509, indent + 8);
                break;

            case NID_ext_key_usage:
                ret = X509PrintExtendedKeyUsage(bio, x509, indent + 8);
                break;

            default:
                /* extension nid not yet supported */
                if ((scratchLen = XSNPRINTF(
                         scratch, MAX_WIDTH,
                         "%*sNID %d print not yet supported\n",
                         indent + 8, "", nid)) >= MAX_WIDTH)
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }

                if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
        }
    }

    if (buf != NULL) {
        XFREE(buf, x509->heap, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return ret;
}


/* print out the signature in human readable format for use with
 * wolfSSL_X509_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintSignature_ex(WOLFSSL_BIO* bio, byte* sig,
        int sigSz, int sigNid, int algOnly, int indent)
{
    char scratch[MAX_WIDTH];
    int scratchLen;
    WOLFSSL_ASN1_OBJECT* obj = NULL;
    int ret = WOLFSSL_SUCCESS;
    char tmp[100];
    int tmpLen = 0;

    if (sigSz <= 0) {
        return WOLFSSL_SUCCESS;
    }

    if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH, "%*s%s", indent, "",
                                "Signature Algorithm: ")) >= MAX_WIDTH) {
        ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS) {
        if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0)
            ret = WOLFSSL_FAILURE;
    }

    if (ret == WOLFSSL_SUCCESS) {
        obj = wolfSSL_OBJ_nid2obj(sigNid);
        if (obj == NULL)
            ret = WOLFSSL_FAILURE;
    }
    if (ret == WOLFSSL_SUCCESS) {
        if (wolfSSL_OBJ_obj2txt(scratch, MAX_WIDTH, obj, 0)
            == WOLFSSL_FAILURE)
        {
            ret = WOLFSSL_FAILURE;
        }
    }

    if (ret == WOLFSSL_SUCCESS) {
        if ((tmpLen = XSNPRINTF(tmp, sizeof(tmp),"%s\n", scratch))
            >= (int)sizeof(tmp))
        {
            ret = WOLFSSL_FAILURE;
        }
    }
    if (ret == WOLFSSL_SUCCESS) {
        if (wolfSSL_BIO_write(bio, tmp, tmpLen) <= 0)
            ret = WOLFSSL_FAILURE;
    }

    /* Leave function if the desired content to print
     * is only the signature algorithm */
    if (algOnly) {
        if (obj != NULL)
            wolfSSL_ASN1_OBJECT_free(obj);

        return ret;
    }

    if (ret == WOLFSSL_SUCCESS) {
        if ((tmpLen = XSNPRINTF(tmp, sizeof(tmp), "%*s", indent + 5, ""))
            >= (int)sizeof(tmp))
        {
            ret = WOLFSSL_FAILURE;
        }
    }

    if (ret == WOLFSSL_SUCCESS) {
        int i;

        for (i = 0; i < sigSz; i++) {
            char val[6];
            int valLen;

            if (i == 0) {
                if ((valLen = XSNPRINTF(val, sizeof(val), "%02x", sig[i]))
                    >= (int)sizeof(val))
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
            else if (((i % 18) == 0)) {
                if (wolfSSL_BIO_write(bio, tmp, tmpLen)
                    <= 0) {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                if ((tmpLen = XSNPRINTF(tmp, sizeof(tmp), ":\n%*s",
                                        indent + 5, ""))
                    >= (int)sizeof(tmp))
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
                if ((valLen = XSNPRINTF(val, sizeof(val), "%02x", sig[i]))
                    >= (int)sizeof(val))
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
            else {
                if ((valLen = XSNPRINTF(val, sizeof(val), ":%02x", sig[i]))
                    >= (int)sizeof(val))
                {
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
            if (valLen >= (int)sizeof(tmp) - tmpLen - 1) {
                ret = WOLFSSL_FAILURE;
                break;
            }
            XMEMCPY(tmp + tmpLen, val, valLen);
            tmpLen += valLen;
            tmp[tmpLen] = 0;
        }
    }

    /* print out remaining sig values */
    if (ret == WOLFSSL_SUCCESS) {
        if (tmpLen > 0) {
            if (wolfSSL_BIO_write(bio, tmp, tmpLen)
                <= 0)
            {
                ret = WOLFSSL_FAILURE;
            }
        }
    }

    if (obj != NULL)
        wolfSSL_ASN1_OBJECT_free(obj);

    return ret;
}

static int X509PrintSignature(WOLFSSL_BIO* bio, WOLFSSL_X509* x509,
        int algOnly, int indent)
{
    int sigSz = 0;
    if (wolfSSL_X509_get_signature(x509, NULL, &sigSz) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (sigSz > 0) {
        unsigned char* sig;
        int sigNid;

        sigNid = wolfSSL_X509_get_signature_nid(x509);
        if (sigNid <= 0) {
            return WOLFSSL_FAILURE;
        }

        sig = (unsigned char*)XMALLOC(sigSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sig == NULL) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_X509_get_signature(x509, sig, &sigSz) <= 0) {
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return WOLFSSL_FAILURE;
        }

        if (X509PrintSignature_ex(bio, sig, sigSz, sigNid, algOnly, indent)
                != WOLFSSL_SUCCESS) {
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return WOLFSSL_FAILURE;
        }

        if (sig != NULL) {
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }

    }

    return WOLFSSL_SUCCESS;
}


/* print out the public key in human readable format for use with
 * wolfSSL_X509_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintPubKey(WOLFSSL_BIO* bio, WOLFSSL_X509* x509, int indent)
{
    char scratch[MAX_WIDTH];
    WOLFSSL_EVP_PKEY* pubKey;
    int len;
    int ret = WOLFSSL_SUCCESS;

    if (bio == NULL || x509 == NULL)
        return BAD_FUNC_ARG;

    len = XSNPRINTF(scratch, MAX_WIDTH, "%*sSubject Public Key Info:\n", indent, "");
    if (len >= MAX_WIDTH)
        return WOLFSSL_FAILURE;
    if (wolfSSL_BIO_write(bio, scratch, len) <= 0)
        return WOLFSSL_FAILURE;

    switch (x509->pubKeyOID) {
    #ifndef NO_RSA
        case RSAk:
            len = XSNPRINTF(scratch, MAX_WIDTH,
                    "%*sPublic Key Algorithm: rsaEncryption\n", indent + 4, "");
            if (len >= MAX_WIDTH)
                return WOLFSSL_FAILURE;
            if (wolfSSL_BIO_write(bio, scratch, len) <= 0)
                return WOLFSSL_FAILURE;
            break;
    #endif
    #ifdef HAVE_ECC
        case ECDSAk:
            len = XSNPRINTF(scratch, MAX_WIDTH,
                    "%*sPublic Key Algorithm: EC\n", indent + 4, "");
            if (len >= MAX_WIDTH)
                return WOLFSSL_FAILURE;
            if (wolfSSL_BIO_write(bio, scratch, len) <= 0)
                return WOLFSSL_FAILURE;
            break;
    #endif
        default:
                WOLFSSL_MSG("Unknown key type");
                return WOLFSSL_FAILURE;
    }

    pubKey = wolfSSL_X509_get_pubkey(x509);
    if (pubKey == NULL)
        return WOLFSSL_FAILURE;

    ret = wolfSSL_EVP_PKEY_print_public(bio, pubKey, indent + 8, NULL);

    wolfSSL_EVP_PKEY_free(pubKey);

    return ret;
}


/* human readable print out of x509 name formatted for use with
 * wolfSSL_X509_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintName(WOLFSSL_BIO* bio, WOLFSSL_X509_NAME* name,
        char* type, int indent)
{
    if (name != NULL) {
        char scratch[MAX_WIDTH];
        int scratchLen;

        if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH,
                                     "%*s%s", indent, "", type))
            >= MAX_WIDTH)
        {
            return WOLFSSL_FAILURE;
        }
        if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
            return WOLFSSL_FAILURE;
        }
        if (wolfSSL_X509_NAME_print_ex(bio, name, 1, 0) <= 0) {
            return WOLFSSL_FAILURE;
        }
        if (wolfSSL_BIO_write(bio, "\n", (int)XSTRLEN("\n")) <= 0) {
            return WOLFSSL_FAILURE;
        }
    }
    return WOLFSSL_SUCCESS;
}


/* human readable print out of x509 version
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintVersion(WOLFSSL_BIO* bio, int version, int indent)
{
    char scratch[MAX_WIDTH];
    int scratchLen;

    if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH,
                                 "%*s%s", indent, "", "Version:"))
        >= MAX_WIDTH)
    {
        return WOLFSSL_FAILURE;
    }
    if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH,
                                 " %d (0x%x)\n", version, (byte)version-1))
        >= MAX_WIDTH)
    {
        return WOLFSSL_FAILURE;
    }
    if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
        return WOLFSSL_FAILURE;
    }
    return WOLFSSL_SUCCESS;
}

#ifdef WOLFSSL_CERT_REQ
/* Print out of REQ attributes
 * return WOLFSSL_SUCCESS on success
 */
static int X509PrintReqAttributes(WOLFSSL_BIO* bio, WOLFSSL_X509* x509,
        int indent)
{
    WOLFSSL_X509_ATTRIBUTE* attr;
    char scratch[MAX_WIDTH];
    int scratchLen;
    int i = 0;

    if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH,
                                 "%*s%s", indent, "", "Attributes: \n"))
        >= MAX_WIDTH)
    {
        return WOLFSSL_FAILURE;
    }
    if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
        return WOLFSSL_FAILURE;
    }
    do {
        attr = wolfSSL_X509_REQ_get_attr(x509, i);
        if (attr != NULL) {
            char lName[NAME_SZ/4]; /* NAME_SZ default is 80 */
            int lNameSz = NAME_SZ/4;
            const byte* data;

            if (wolfSSL_OBJ_obj2txt(lName, lNameSz, attr->object, 0)
                == WOLFSSL_FAILURE)
            {
                return WOLFSSL_FAILURE;
            }
            lNameSz = (int)XSTRLEN(lName);
            data = wolfSSL_ASN1_STRING_get0_data(
                    attr->value->value.asn1_string);
            if (data == NULL) {
                WOLFSSL_MSG("No REQ attribute found when expected");
                return WOLFSSL_FAILURE;
            }
            if ((scratchLen = XSNPRINTF(scratch, MAX_WIDTH,
                          "%*s%s%*s:%s\n", indent+4, "",
                          lName, (NAME_SZ/4)-lNameSz, "", data))
                >= MAX_WIDTH)
            {
                return WOLFSSL_FAILURE;
            }
            if (wolfSSL_BIO_write(bio, scratch, scratchLen) <= 0) {
                WOLFSSL_MSG("Error writing REQ attribute");
                return WOLFSSL_FAILURE;
            }
        }
        i++;
    } while (attr != NULL);

    return WOLFSSL_SUCCESS;
}


/*
 * return WOLFSSL_SUCCESS on success
 */
int wolfSSL_X509_REQ_print(WOLFSSL_BIO* bio, WOLFSSL_X509* x509)
{
    char subjType[] = "Subject: ";

    if (bio == NULL || x509 == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "Certificate Request:\n",
                  (int)XSTRLEN("Certificate Request:\n")) <= 0) {
            return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "    Data:\n",
                  (int)XSTRLEN("    Data:\n")) <= 0) {
            return WOLFSSL_FAILURE;
    }

    /* print version of cert */
    if (X509PrintVersion(bio, wolfSSL_X509_version(x509), 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    if (X509PrintSerial(bio, x509, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print subject */
    if (X509PrintName(bio, wolfSSL_X509_get_subject_name(x509), subjType, 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* get and print public key */
    if (X509PrintPubKey(bio, x509, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print out extensions */
    if (X509PrintExtensions(bio, x509, 4) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print out req attributes */
    if (X509PrintReqAttributes(bio, x509, 4) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print out signature */
    if (X509PrintSignature(bio, x509, 0, 4) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* done with print out */
    if (wolfSSL_BIO_write(bio, "\n\0", (int)XSTRLEN("\n\0")) <= 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_CERT_REQ */


/* Writes the human readable form of x509 to bio.
 *
 * bio  WOLFSSL_BIO to write to.
 * x509 Certificate to write.
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_X509_print_ex(WOLFSSL_BIO* bio, WOLFSSL_X509* x509,
    unsigned long nmflags, unsigned long cflag)
{
    char issuType[] = "Issuer:";
    char subjType[] = "Subject:";

    WOLFSSL_ENTER("wolfSSL_X509_print_ex");

    /* flags currently not supported */
    (void)nmflags;
    (void)cflag;

    if (bio == NULL || x509 == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "Certificate:\n",
                  (int)XSTRLEN("Certificate:\n")) <= 0) {
            return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "    Data:\n",
                  (int)XSTRLEN("    Data:\n")) <= 0) {
            return WOLFSSL_FAILURE;
    }

    /* print version of cert */
    if (X509PrintVersion(bio, wolfSSL_X509_version(x509), 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print serial number out */
    if (X509PrintSerial(bio, x509, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print out signature algo*/
    if (X509PrintSignature(bio, x509, 1, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print issuer */
    if (X509PrintName(bio, wolfSSL_X509_get_issuer_name(x509), issuType, 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

#ifndef NO_ASN_TIME
    /* print validity */
    {
        char tmp[80];

        if (wolfSSL_BIO_write(bio, "        Validity\n",
                      (int)XSTRLEN("        Validity\n")) <= 0) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, "            Not Before: ",
                      (int)XSTRLEN("            Not Before: ")) <= 0) {
            return WOLFSSL_FAILURE;
        }
        if (x509->notBefore.length > 0) {
            if (GetTimeString(x509->notBefore.data, ASN_UTC_TIME,
                tmp, sizeof(tmp)) != WOLFSSL_SUCCESS) {
                if (GetTimeString(x509->notBefore.data, ASN_GENERALIZED_TIME,
                tmp, sizeof(tmp)) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Error getting not before date");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        else {
            XSTRNCPY(tmp, "Not Set", sizeof(tmp)-1);
        }
        tmp[sizeof(tmp) - 1] = '\0'; /* make sure null terminated */
        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, "\n            Not After : ",
                      (int)XSTRLEN("\n            Not After : ")) <= 0) {
            return WOLFSSL_FAILURE;
        }
        if (x509->notAfter.length > 0) {
            if (GetTimeString(x509->notAfter.data, ASN_UTC_TIME,
                tmp, sizeof(tmp)) != WOLFSSL_SUCCESS) {
                if (GetTimeString(x509->notAfter.data, ASN_GENERALIZED_TIME,
                    tmp, sizeof(tmp)) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Error getting not after date");
                    return WOLFSSL_FAILURE;
                }
            }
        }
        else {
            XSTRNCPY(tmp, "Not Set", sizeof(tmp)-1);
        }
        tmp[sizeof(tmp) - 1] = '\0'; /* make sure null terminated */
        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, "\n\0", (int)XSTRLEN("\n\0")) <= 0) {
            return WOLFSSL_FAILURE;
        }
    }
    #endif

    /* print subject */
    if (X509PrintName(bio, wolfSSL_X509_get_subject_name(x509), subjType, 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* get and print public key */
    if (X509PrintPubKey(bio, x509, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print out extensions */
    if (X509PrintExtensions(bio, x509, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print out signature */
    if (X509PrintSignature(bio, x509, 0, 4) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* done with print out */
    if (wolfSSL_BIO_write(bio, "\n\0", (int)XSTRLEN("\n\0")) <= 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
int wolfSSL_X509_print(WOLFSSL_BIO* bio, WOLFSSL_X509* x509)
{
    return wolfSSL_X509_print_ex(bio, x509, 0, 0);
}

#ifndef NO_FILESYSTEM
int wolfSSL_X509_print_fp(XFILE fp, WOLFSSL_X509 *x509)
{
    WOLFSSL_BIO* bio;
    int ret;

    WOLFSSL_ENTER("wolfSSL_X509_print_fp");

    if (!fp || !x509) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if (!(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file()))) {
        WOLFSSL_MSG("wolfSSL_BIO_new wolfSSL_BIO_s_file error");
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_set_fp(bio, fp, BIO_NOCLOSE) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_BIO_set_fp error");
        wolfSSL_BIO_free(bio);
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_X509_print(bio, x509);

    wolfSSL_BIO_free(bio);

    return ret;
}
#endif /* NO_FILESYSTEM */

#endif /* XSNPRINTF */

int wolfSSL_X509_signature_print(WOLFSSL_BIO *bp,
        const WOLFSSL_X509_ALGOR *sigalg, const WOLFSSL_ASN1_STRING *sig)
{
    int length = 0;
    word32 idx = 0;
    int i;

    (void)sig;

    WOLFSSL_ENTER("wolfSSL_X509_signature_print");

    if (!bp || !sigalg) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if ((sigalg->algorithm->obj == NULL) ||
        (sigalg->algorithm->obj[idx] != ASN_OBJECT_ID)) {
        WOLFSSL_MSG("Bad ASN1 Object");
        return WOLFSSL_FAILURE;
    }
    idx++; /* skip object id */

    if (GetLength((const byte*)sigalg->algorithm->obj, &idx, &length,
                  sigalg->algorithm->objSz) < 0 || length < 0) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_puts(bp, "    Raw Signature Algorithm:") <= 0) {
        WOLFSSL_MSG("wolfSSL_BIO_puts error");
        return WOLFSSL_FAILURE;
    }

    for (i = 0; i < length; ++i) {
        char hex_digits[4];
#ifdef XSNPRINTF
        if (XSNPRINTF(hex_digits, sizeof(hex_digits), "%c%02X", i>0 ? ':' : ' ',
                  (unsigned int)sigalg->algorithm->obj[idx+i])
            >= (int)sizeof(hex_digits))
        {
            WOLFSSL_MSG("buffer overrun");
            return WOLFSSL_FAILURE;
        }
#else
        XSPRINTF(hex_digits, "%c%02X", i>0 ? ':' : ' ',
                 (unsigned int)sigalg->algorithm->obj[idx+i]);
#endif
        if (wolfSSL_BIO_puts(bp, hex_digits) <= 0)
            return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_puts(bp, "\n") <= 0)
        return WOLFSSL_FAILURE;

    return WOLFSSL_SUCCESS;
}
#endif /* !NO_BIO */

#ifndef NO_WOLFSSL_STUB
void wolfSSL_X509_get0_signature(const WOLFSSL_ASN1_BIT_STRING **psig,
        const WOLFSSL_X509_ALGOR **palg, const WOLFSSL_X509 *x509)
{
    (void)psig;
    (void)palg;
    (void)x509;
    WOLFSSL_STUB("wolfSSL_X509_get0_signature");
}
#endif

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
const char* wolfSSL_X509_verify_cert_error_string(long err)
{
    return wolfSSL_ERR_reason_error_string(err);
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA

/* Add directory path that will be used for loading certs and CRLs
 * which have the <hash>.rn name format.
 * type may be WOLFSSL_FILETYPE_PEM or WOLFSSL_FILETYPE_ASN1.
 * returns WOLFSSL_SUCCESS on successful, otherwise negative or zero. */
int wolfSSL_X509_LOOKUP_add_dir(WOLFSSL_X509_LOOKUP* lookup, const char* dir,
                               long type)
{
    return wolfSSL_X509_LOOKUP_ctrl(lookup, WOLFSSL_X509_L_ADD_DIR, dir, type,
                                    NULL);
}

int wolfSSL_X509_LOOKUP_load_file(WOLFSSL_X509_LOOKUP* lookup,
                                 const char* file, long type)
{
#if !defined(NO_FILESYSTEM) && \
    (defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM))
    int           ret = WOLFSSL_FAILURE;
    XFILE         fp;
    long          sz;
    byte*         pem = NULL;
    byte*         curr = NULL;
    byte*         prev = NULL;
    WOLFSSL_X509* x509;
    const char* header = NULL;
    const char* footer = NULL;

    if (type != WOLFSSL_FILETYPE_PEM)
        return WS_RETURN_CODE(BAD_FUNC_ARG, (int)WOLFSSL_FAILURE);

    fp = XFOPEN(file, "rb");
    if (fp == XBADFILE)
        return WS_RETURN_CODE(BAD_FUNC_ARG, (int)WOLFSSL_FAILURE);

    if(XFSEEK(fp, 0, XSEEK_END) != 0) {
        XFCLOSE(fp);
        return WS_RETURN_CODE(WOLFSSL_BAD_FILE,WOLFSSL_FAILURE);
    }
    sz = XFTELL(fp);
    if(XFSEEK(fp, 0, XSEEK_SET) != 0) {
        XFCLOSE(fp);
        return WS_RETURN_CODE(WOLFSSL_BAD_FILE,WOLFSSL_FAILURE);
    }

    if (sz > MAX_WOLFSSL_FILE_SIZE || sz <= 0) {
        WOLFSSL_MSG("X509_LOOKUP_load_file size error");
        goto end;
    }

    pem = (byte*)XMALLOC(sz, 0, DYNAMIC_TYPE_PEM);
    if (pem == NULL) {
        ret = MEMORY_ERROR;
        goto end;
    }

    /* Read in file which may be CRLs or certificates. */
    if (XFREAD(pem, (size_t)sz, 1, fp) != 1)
        goto end;

    prev = curr = pem;
    do {
        /* get PEM header and footer based on type */
        if (wc_PemGetHeaderFooter(CRL_TYPE, &header, &footer) == 0 &&
                XSTRNSTR((char*)curr, header, (unsigned int)sz) != NULL) {
#ifdef HAVE_CRL
            WOLFSSL_CERT_MANAGER* cm = lookup->store->cm;

            if (cm->crl == NULL) {
                if (wolfSSL_CertManagerEnableCRL(cm, WOLFSSL_CRL_CHECK)
                    != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Enable CRL failed");
                    goto end;
                }
            }

            ret = BufferLoadCRL(cm->crl, curr, sz, WOLFSSL_FILETYPE_PEM,
                NO_VERIFY);
            if (ret != WOLFSSL_SUCCESS)
                goto end;
#endif
            curr = (byte*)XSTRNSTR((char*)curr, footer, (unsigned int)sz);
        }
        else if (wc_PemGetHeaderFooter(CERT_TYPE, &header, &footer) == 0 &&
                XSTRNSTR((char*)curr, header, (unsigned int)sz) != NULL) {
            x509 = wolfSSL_X509_load_certificate_buffer(curr, (int)sz,
                                                        WOLFSSL_FILETYPE_PEM);
            if (x509 == NULL)
                 goto end;
            ret = wolfSSL_X509_STORE_add_cert(lookup->store, x509);
            wolfSSL_X509_free(x509);
            if (ret != WOLFSSL_SUCCESS)
                goto end;
            curr = (byte*)XSTRNSTR((char*)curr, footer, (unsigned int)sz);
        }
        else
            goto end;

        if (curr == NULL)
            goto end;

        curr++;
        sz -= (long)(curr - prev);
        prev = curr;
    }
    while (ret == WOLFSSL_SUCCESS);

end:
    if (pem != NULL)
        XFREE(pem, 0, DYNAMIC_TYPE_PEM);
    XFCLOSE(fp);
    return WS_RETURN_CODE(ret, (int)WOLFSSL_FAILURE);
#else
    (void)lookup;
    (void)file;
    (void)type;
    return WS_RETURN_CODE(WOLFSSL_FAILURE,WOLFSSL_FAILURE);
#endif
}

WOLFSSL_X509_LOOKUP_METHOD* wolfSSL_X509_LOOKUP_hash_dir(void)
{
    /* Method implementation in functions. */
    static WOLFSSL_X509_LOOKUP_METHOD meth = { 1 };
    return &meth;
}

WOLFSSL_X509_LOOKUP_METHOD* wolfSSL_X509_LOOKUP_file(void)
{
    /* Method implementation in functions. */
    static WOLFSSL_X509_LOOKUP_METHOD meth = { 0 };
    return &meth;
}

/* set directory path to load certificate or CRL which have the hash.N form */
/* for late use                                                             */
/* @param ctx    a pointer to WOLFSSL_BY_DIR structure                      */
/* @param argc   directory path                                             */
/* @param argl   file type, either WOLFSSL_FILETYPE_PEM or                  */
/*                                          WOLFSSL_FILETYPE_ASN1           */
/* @return WOLFSSL_SUCCESS on successful, otherwise negative or zero        */
static int x509AddCertDir(WOLFSSL_BY_DIR *ctx, const char *argc, long argl)
{
#if defined(OPENSSL_ALL) && !defined(NO_FILESYSTEM) && !defined(NO_WOLFSSL_DIR)
    WOLFSSL_BY_DIR_entry *entry;
    size_t pathLen;
    int i, num;
    const char* c;
#ifdef WOLFSSL_SMALL_STACK
    char *buf;
#else
    char  buf[MAX_FILENAME_SZ];
#endif

    WOLFSSL_ENTER("x509AddCertDir");

    pathLen = 0;
    c = argc;
    /* sanity check, zero length */
    if (ctx == NULL || c == NULL || *c == '\0')
        return WOLFSSL_FAILURE;

#ifdef WOLFSSL_SMALL_STACK
    buf = (char*)XMALLOC(MAX_FILENAME_SZ, NULL, DYNAMIC_TYPE_OPENSSL);
    if (buf == NULL) {
        WOLFSSL_LEAVE("x509AddCertDir", MEMORY_E);
        return MEMORY_E;
    }
#endif

    XMEMSET(buf, 0, MAX_FILENAME_SZ);

    do {
        if (*c == SEPARATOR_CHAR || *c == '\0') {

            num = wolfSSL_sk_BY_DIR_entry_num(ctx->dir_entry);

            for (i=0; i<num; i++) {

                entry = wolfSSL_sk_BY_DIR_entry_value(ctx->dir_entry, i);

                if (XSTRLEN(entry->dir_name) == pathLen &&
                    XSTRNCMP(entry->dir_name, buf, pathLen) == 0) {
                    WOLFSSL_MSG("dir entry found");
                    break;
                }
            }

            if (num == -1 || i == num) {
                WOLFSSL_MSG("no entry found");

                if (ctx->dir_entry == NULL) {
                    ctx->dir_entry = wolfSSL_sk_BY_DIR_entry_new_null();

                    if (ctx->dir_entry == NULL) {
                        WOLFSSL_MSG("failed to allocate dir_entry");
                        #ifdef WOLFSSL_SMALL_STACK
                            XFREE(buf, 0, DYNAMIC_TYPE_OPENSSL);
                        #endif
                        return 0;
                    }
                }

                entry = wolfSSL_BY_DIR_entry_new();
                if (entry == NULL) {
                    WOLFSSL_MSG("failed to allocate dir entry");
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(buf, 0, DYNAMIC_TYPE_OPENSSL);
                    #endif
                    return 0;
                }
                entry->dir_type = (int)argl;
                entry->dir_name = (char*)XMALLOC(pathLen + 1/* \0 termination*/
                                                , NULL, DYNAMIC_TYPE_OPENSSL);
                entry->hashes = wolfSSL_sk_BY_DIR_HASH_new_null();
                if (entry->dir_name == NULL || entry->hashes == NULL) {
                    WOLFSSL_MSG("failed to allocate dir name");
                    wolfSSL_BY_DIR_entry_free(entry);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(buf, 0, DYNAMIC_TYPE_OPENSSL);
                    #endif
                    return 0;
                }

                XSTRNCPY(entry->dir_name, buf, pathLen);
                entry->dir_name[pathLen] = '\0';

                if (wolfSSL_sk_BY_DIR_entry_push(ctx->dir_entry, entry)
                                                    != WOLFSSL_SUCCESS) {
                    wolfSSL_BY_DIR_entry_free(entry);
                    #ifdef WOLFSSL_SMALL_STACK
                        XFREE(buf, 0, DYNAMIC_TYPE_OPENSSL);
                    #endif
                    return 0;
                }
            }
            /* skip separator */
            if (*c == SEPARATOR_CHAR) c++;

            pathLen = 0;
            XMEMSET(buf, 0, MAX_FILENAME_SZ);
        }
        buf[pathLen++] = *c;

    } while(*c++ != '\0');

#ifdef WOLFSSL_SMALL_STACK
    XFREE(buf, 0, DYNAMIC_TYPE_OPENSSL);
#endif

    return WOLFSSL_SUCCESS;
#else
    (void)ctx;
    (void)argc;
    (void)argl;
    return WOLFSSL_NOT_IMPLEMENTED;
#endif
}

/* set additional data to X509_LOOKUP                                   */
/* @param ctx    a pointer to X509_LOOKUP structure                     */
/* @param cmd    control command :                                      */
/*               X509_L_FILE_LOAD, X509_L_ADD_DIR X509_L_ADD_STORE or   */
/*               X509_L_LOAD_STORE                                      */
/* @param argc   arguments for the control command                      */
/* @param argl   arguments for the control command                      */
/* @param **ret  return value of the control command                    */
/* @return WOLFSSL_SUCCESS on successful, otherwise WOLFSSL_FAILURE     */
/* note: WOLFSSL_X509_L_ADD_STORE and WOLFSSL_X509_L_LOAD_STORE have not*/
/*       yet implemented. It returns WOLFSSL_NOT_IMPLEMENTED            */
/*       when those control commands are passed.                        */
int wolfSSL_X509_LOOKUP_ctrl(WOLFSSL_X509_LOOKUP *ctx, int cmd,
        const char *argc, long argl, char **ret)
{
    int lret = WOLFSSL_FAILURE;

    WOLFSSL_ENTER("wolfSSL_X509_LOOKUP_ctrl");
#if !defined(NO_FILESYSTEM)
    if (ctx != NULL) {
        switch (cmd) {
        case WOLFSSL_X509_L_FILE_LOAD:
            /* expects to return a number of processed cert or crl file */
            lret = wolfSSL_X509_load_cert_crl_file(ctx, argc, (int)argl) > 0 ?
                            WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
            break;
        case WOLFSSL_X509_L_ADD_DIR:
            /* store directory location to use it later */
#if !defined(NO_WOLFSSL_DIR)
            lret = x509AddCertDir(ctx->dirs, argc, argl);
#else
            (void)x509AddCertDir;
            lret = WOLFSSL_NOT_IMPLEMENTED;
#endif
            break;
        case WOLFSSL_X509_L_ADD_STORE:
        case WOLFSSL_X509_L_LOAD_STORE:
            return WOLFSSL_NOT_IMPLEMENTED;

        default:
            break;
        }
    }
    (void)ret;
#else
    (void)ctx;
    (void)argc;
    (void)argl;
    (void)ret;
    (void)cmd;
    (void)x509AddCertDir;
    lret = WOLFSSL_NOT_IMPLEMENTED;
#endif
    return lret;
}


#if defined(WOLFSSL_CERT_GEN)
static int wolfssl_x509_make_der(WOLFSSL_X509* x509, int req,
        unsigned char* der, int* derSz, int includeSig);
#endif

#ifdef WOLFSSL_CERT_GEN
#ifndef NO_BIO
/* Converts the X509 to DER format and outputs it into bio.
 *
 * bio  is the structure to hold output DER
 * x509 certificate to create DER from
 * req  if set then a CSR is generated
 *
 * returns WOLFSSL_SUCCESS on success
 */
static int loadX509orX509REQFromBio(WOLFSSL_BIO* bio, WOLFSSL_X509* x509, int req)
{
    int ret = WOLFSSL_FAILURE;
    /* Get large buffer to hold cert der */
    int derSz = X509_BUFFER_SZ;
#ifdef WOLFSSL_SMALL_STACK
    byte* der;
#else
    byte der[X509_BUFFER_SZ];
#endif
    WOLFSSL_ENTER("wolfSSL_i2d_X509_bio");

    if (bio == NULL || x509 == NULL) {
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    der = (byte*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (!der) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }
#endif

    if (wolfssl_x509_make_der(x509, req, der, &derSz, 1) != WOLFSSL_SUCCESS) {
        goto cleanup;
    }

    if (wolfSSL_BIO_write(bio, der, derSz) != derSz) {
        goto cleanup;
    }

    ret = WOLFSSL_SUCCESS;
cleanup:
    #ifdef WOLFSSL_SMALL_STACK
    XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    #endif

    return ret;
}

/* Converts the X509 to DER format and outputs it into bio.
 *
 * bio  is the structure to hold output DER
 * x509 certificate to create DER from
 *
 * returns WOLFSSL_SUCCESS on success
 */
int wolfSSL_i2d_X509_bio(WOLFSSL_BIO* bio, WOLFSSL_X509* x509)
{
    return loadX509orX509REQFromBio(bio, x509, 0);
}

#ifdef WOLFSSL_CERT_REQ
int wolfSSL_i2d_X509_REQ_bio(WOLFSSL_BIO* bio, WOLFSSL_X509* x509)
{
    return loadX509orX509REQFromBio(bio, x509, 1);
}
#endif /* WOLFSSL_CERT_REQ */
#endif /* !NO_BIO */
#endif /* WOLFSSL_CERT_GEN */

/* Converts an internal structure to a DER buffer
 *
 * x509 structure to get DER buffer from
 * out  buffer to hold result. If NULL then *out is NULL then a new buffer is
 *      created.
 *
 * returns the size of the DER result on success
 */
int wolfSSL_i2d_X509(WOLFSSL_X509* x509, unsigned char** out)
{
    const unsigned char* der;
    int derSz = 0;

    WOLFSSL_ENTER("wolfSSL_i2d_X509");

    if (x509 == NULL) {
        WOLFSSL_LEAVE("wolfSSL_i2d_X509", BAD_FUNC_ARG);
        return BAD_FUNC_ARG;
    }

    der = wolfSSL_X509_get_der(x509, &derSz);
    if (der == NULL) {
        WOLFSSL_LEAVE("wolfSSL_i2d_X509", MEMORY_E);
        return MEMORY_E;
    }

    if (out != NULL && *out == NULL) {
        *out = (unsigned char*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_OPENSSL);
        if (*out == NULL) {
            WOLFSSL_LEAVE("wolfSSL_i2d_X509", MEMORY_E);
            return MEMORY_E;
        }
    }

    if (out != NULL)
        XMEMCPY(*out, der, derSz);

    WOLFSSL_LEAVE("wolfSSL_i2d_X509", derSz);
    return derSz;
}

#ifndef NO_BIO
/**
 * Converts the DER from bio and creates a WOLFSSL_X509 structure from it.
 * @param bio  is the structure holding DER
 * @param x509 certificate to create from DER. Can be NULL
 * @param req  1 for a CSR and 0 for a x509 cert
 * @return pointer to WOLFSSL_X509 structure on success and NULL on fail
 */
static WOLFSSL_X509* d2i_X509orX509REQ_bio(WOLFSSL_BIO* bio,
                                            WOLFSSL_X509** x509, int req)
{
    WOLFSSL_X509* localX509 = NULL;
    byte* mem  = NULL;
    int    size;

    WOLFSSL_ENTER("wolfSSL_d2i_X509_bio");

    if (bio == NULL) {
        WOLFSSL_MSG("Bad Function Argument bio is NULL");
        return NULL;
    }

    size = wolfSSL_BIO_get_len(bio);
    if (size <= 0) {
        WOLFSSL_MSG("wolfSSL_BIO_get_len error. Possibly no pending data.");
        WOLFSSL_ERROR(ASN1_R_HEADER_TOO_LONG);
        return NULL;
    }

    if (!(mem = (byte*)XMALLOC(size, NULL, DYNAMIC_TYPE_OPENSSL))) {
        WOLFSSL_MSG("malloc error");
        return NULL;
    }

    if ((size = wolfSSL_BIO_read(bio, mem, size)) == 0) {
        WOLFSSL_MSG("wolfSSL_BIO_read error");
        XFREE(mem, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }

    if (req) {
#ifdef WOLFSSL_CERT_REQ
        localX509 = wolfSSL_X509_REQ_d2i(NULL, mem, size);
#else
        WOLFSSL_MSG("CSR not compiled in");
#endif
    }
    else {
        localX509 = wolfSSL_X509_d2i(NULL, mem, size);
    }
    if (localX509 == NULL) {
        WOLFSSL_MSG("wolfSSL_X509_d2i error");
        XFREE(mem, NULL, DYNAMIC_TYPE_OPENSSL);
        return NULL;
    }

    if (x509 != NULL) {
        *x509 = localX509;
    }

    XFREE(mem, NULL, DYNAMIC_TYPE_OPENSSL);
    return localX509;
}

WOLFSSL_X509* wolfSSL_d2i_X509_bio(WOLFSSL_BIO* bio, WOLFSSL_X509** x509)
{
    return d2i_X509orX509REQ_bio(bio, x509, 0);
}

#ifdef WOLFSSL_CERT_REQ
WOLFSSL_X509* wolfSSL_d2i_X509_REQ_bio(WOLFSSL_BIO* bio, WOLFSSL_X509** x509)
{
    return d2i_X509orX509REQ_bio(bio, x509, 1);
}
#endif
#endif /* !NO_BIO */

#endif /* OPENSSL_EXTRA */

#ifdef OPENSSL_EXTRA
/* Use the public key to verify the signature. Note: this only verifies
 * the certificate signature.
 * returns WOLFSSL_SUCCESS on successful signature verification */
static int verifyX509orX509REQ(WOLFSSL_X509* x509, WOLFSSL_EVP_PKEY* pkey, int req)
{
    int ret;
    const byte* der;
    int derSz = 0;
    int type;

    (void)req;

    if (x509 == NULL || pkey == NULL) {
        return WOLFSSL_FATAL_ERROR;
    }

    der = wolfSSL_X509_get_der(x509, &derSz);
    if (der == NULL) {
        WOLFSSL_MSG("Error getting WOLFSSL_X509 DER");
        return WOLFSSL_FATAL_ERROR;
    }

    switch (pkey->type) {
        case EVP_PKEY_RSA:
            type = RSAk;
            break;

        case EVP_PKEY_EC:
            type = ECDSAk;
            break;

        case EVP_PKEY_DSA:
            type = DSAk;
            break;

        default:
            WOLFSSL_MSG("Unknown pkey key type");
            return WOLFSSL_FATAL_ERROR;
    }

#ifdef WOLFSSL_CERT_REQ
    if (req)
        ret = CheckCSRSignaturePubKey(der, derSz, x509->heap,
                (unsigned char*)pkey->pkey.ptr, pkey->pkey_sz, type);
    else
#endif
        ret = CheckCertSignaturePubKey(der, derSz, x509->heap,
                (unsigned char*)pkey->pkey.ptr, pkey->pkey_sz, type);
    if (ret == 0) {
        return WOLFSSL_SUCCESS;
    }
    return WOLFSSL_FAILURE;
}

int wolfSSL_X509_verify(WOLFSSL_X509* x509, WOLFSSL_EVP_PKEY* pkey)
{
    return verifyX509orX509REQ(x509, pkey, 0);
}

#ifdef WOLFSSL_CERT_REQ
int wolfSSL_X509_REQ_verify(WOLFSSL_X509* x509, WOLFSSL_EVP_PKEY* pkey)
{
    return verifyX509orX509REQ(x509, pkey, 1);
}
#endif /* WOLFSSL_CERT_REQ */

#if !defined(NO_FILESYSTEM)
static void *wolfSSL_d2i_X509_fp_ex(XFILE file, void **x509, int type)
{
    void *newx509 = NULL;
    byte *fileBuffer = NULL;
    long sz = 0;

    /* init variable */
    if (x509)
        *x509 = NULL;

    /* argument check */
    if (file == XBADFILE) {
        return NULL;
    }

    /* determine file size */
    if (XFSEEK(file, 0, XSEEK_END) != 0) {
        return NULL;
    }
    sz = XFTELL(file);
    if (XFSEEK(file, 0, XSEEK_SET) != 0) {
        return NULL;
    }

    if (sz > MAX_WOLFSSL_FILE_SIZE || sz <= 0) {
        WOLFSSL_MSG("d2i_X509_fp_ex file size error");
        return NULL;
    }

    fileBuffer = (byte *)XMALLOC(sz, NULL, DYNAMIC_TYPE_FILE);
    if (fileBuffer != NULL) {
        if ((long)XFREAD(fileBuffer, 1, sz, file) != sz) {
            WOLFSSL_MSG("File read failed");
            goto err_exit;
        }
        if (type == CERT_TYPE) {
            newx509 = (void *)wolfSSL_X509_d2i(NULL, fileBuffer, (int)sz);
        }
    #ifdef HAVE_CRL
        else if (type == CRL_TYPE) {
            newx509 = (void *)wolfSSL_d2i_X509_CRL(NULL, fileBuffer, (int)sz);
        }
    #endif
    #ifdef WOLFSSL_CERT_REQ
        else if (type == CERTREQ_TYPE) {
             newx509 = (void *)wolfSSL_X509_REQ_d2i(NULL, fileBuffer, (int)sz);
        }
    #endif
    #if !defined(NO_ASN) && !defined(NO_PWDBASED) && defined(HAVE_PKCS12)
        else if (type == PKCS12_TYPE) {
            if ((newx509 = wc_PKCS12_new()) == NULL) {
                goto err_exit;
            }
            if (wc_d2i_PKCS12(fileBuffer, (int)sz, (WC_PKCS12*)newx509) < 0) {
                goto err_exit;
            }
        }
    #endif
        else {
            goto err_exit;
        }
        if (newx509 == NULL) {
            WOLFSSL_MSG("X509 failed");
            goto err_exit;
        }
    }

    if (x509)
        *x509 = newx509;

    goto _exit;

err_exit:
#if !defined(NO_ASN) && !defined(NO_PWDBASED) && defined(HAVE_PKCS12)
    if ((newx509 != NULL) && (type == PKCS12_TYPE)) {
        wc_PKCS12_free((WC_PKCS12*)newx509);
        newx509 = NULL;
    }
#endif
_exit:
    if (fileBuffer != NULL)
        XFREE(fileBuffer, NULL, DYNAMIC_TYPE_FILE);

    return newx509;
}

#ifdef WOLFSSL_CERT_REQ
WOLFSSL_X509* wolfSSL_d2i_X509_REQ_fp(XFILE fp, WOLFSSL_X509 **req)
{
    return (WOLFSSL_X509 *)wolfSSL_d2i_X509_fp_ex(fp, (void **)req,
                                                  CERTREQ_TYPE);
}
#endif /* WOLFSSL_CERT_REQ */

WOLFSSL_X509 *wolfSSL_d2i_X509_fp(XFILE fp, WOLFSSL_X509 **x509)
{
    WOLFSSL_ENTER("wolfSSL_d2i_X509_fp");
    return (WOLFSSL_X509 *)wolfSSL_d2i_X509_fp_ex(fp, (void **)x509, CERT_TYPE);
}

/* load certificate or CRL file, and add it to the STORE           */
/* @param ctx    a pointer to X509_LOOKUP structure                */
/* @param file   file name to load                                 */
/* @param type   WOLFSSL_FILETYPE_PEM or WOLFSSL_FILETYPE_ASN1     */
/* @return a number of loading CRL or certificate, otherwise zero  */
WOLFSSL_API int wolfSSL_X509_load_cert_crl_file(WOLFSSL_X509_LOOKUP *ctx,
    const char *file, int type)
{
    WOLFSSL_X509 *x509 = NULL;

    int cnt = 0;

    WOLFSSL_ENTER("wolfSSL_X509_load_cert_crl_file");

    /* stanity check */
    if (ctx == NULL || file == NULL) {
        WOLFSSL_MSG("bad arguments");
        return 0;
    }

    if (type != WOLFSSL_FILETYPE_PEM) {
        x509 = wolfSSL_X509_load_certificate_file(file, type);
        if (x509 != NULL) {
            if (wolfSSL_X509_STORE_add_cert(ctx->store, x509)
                                    == WOLFSSL_SUCCESS) {
                cnt++;
            } else {
                WOLFSSL_MSG("wolfSSL_X509_STORE_add_cert error");
            }
            wolfSSL_X509_free(x509);
            x509 = NULL;
        } else {
            WOLFSSL_MSG("wolfSSL_X509_load_certificate_file error");
        }

    } else {
#if defined(OPENSSL_ALL)
    #if !defined(NO_BIO)
        STACK_OF(WOLFSSL_X509_INFO) *info;
        WOLFSSL_X509_INFO *info_tmp;
        int i;
        int num = 0;
        WOLFSSL_BIO *bio = wolfSSL_BIO_new_file(file, "rb");
        if(!bio) {
            WOLFSSL_MSG("wolfSSL_BIO_new error");
            return cnt;
        }

        info = wolfSSL_PEM_X509_INFO_read_bio(bio, NULL, NULL, NULL);

        wolfSSL_BIO_free(bio);

        if (!info) {
            WOLFSSL_MSG("wolfSSL_PEM_X509_INFO_read_bio error");
            return cnt;
        }
        num = wolfSSL_sk_X509_INFO_num(info);
        for (i=0; i < num; i++) {
            info_tmp = wolfSSL_sk_X509_INFO_value(info, i);

            if (info_tmp->x509) {
                if(wolfSSL_X509_STORE_add_cert(ctx->store, info_tmp->x509) ==
                    WOLFSSL_SUCCESS) {
                    cnt ++;
                } else {
                    WOLFSSL_MSG("wolfSSL_X509_STORE_add_cert failed");
                }
            }
#ifdef HAVE_CRL
            if (info_tmp->crl) {
                if(wolfSSL_X509_STORE_add_crl(ctx->store, info_tmp->crl) ==
                    WOLFSSL_SUCCESS) {
                    cnt ++;
                } else {
                    WOLFSSL_MSG("wolfSSL_X509_STORE_add_crl failed");
                }
            }
#endif
        }
        wolfSSL_sk_X509_INFO_pop_free(info, wolfSSL_X509_INFO_free);
    #elif defined(HAVE_CRL)
        /* Only supports one certificate or CRL in the file. */
        WOLFSSL_X509_CRL* crl = NULL;
        XFILE fp = XFOPEN(file, "rb");
        if (fp == XBADFILE) {
            WOLFSSL_MSG("XFOPEN error");
            return cnt;
        }

        x509 = wolfSSL_PEM_read_X509(fp, NULL, NULL, NULL);
        if (x509 != NULL) {
            if (wolfSSL_X509_STORE_add_cert(ctx->store, x509) ==
                WOLFSSL_SUCCESS) {
                cnt++;
            }
            else {
                WOLFSSL_MSG("wolfSSL_X509_STORE_add_cert failed");
            }
        }
        else {
            if (XFSEEK(fp, 0, XSEEK_SET) != 0) {
                WOLFSSL_MSG("XFSEEK error");
                return cnt;
            }
            crl = wolfSSL_PEM_read_X509_CRL(fp, NULL, NULL, NULL);
            if (crl != NULL) {
                if (wolfSSL_X509_STORE_add_crl(ctx->store, crl) ==
                    WOLFSSL_SUCCESS) {
                    cnt++;
                }
                else {
                    WOLFSSL_MSG("wolfSSL_X509_STORE_add_crl failed");
                }
            }
            else {
                WOLFSSL_MSG("Certificate and CRL not recognized");
                return cnt;
            }
        }

        wolfSSL_X509_free(x509);
        wolfSSL_X509_CRL_free(crl);
    #endif
#else
    (void)cnt;
#endif /* OPENSSL_ALL && !NO_BIO */
    }

    WOLFSSL_LEAVE("wolfSSL_X509_load_ceretificate_crl_file", cnt);
    return cnt;
}
#endif /* !NO_FILESYSTEM */


#ifdef HAVE_CRL

#ifndef NO_BIO
WOLFSSL_API WOLFSSL_X509_CRL *wolfSSL_d2i_X509_CRL_bio(WOLFSSL_BIO *bp,
                                                    WOLFSSL_X509_CRL **x)
{
    int derSz;
    byte* der = NULL;
    WOLFSSL_X509_CRL* crl = NULL;

    if (bp == NULL)
        return NULL;

    if ((derSz = wolfSSL_BIO_get_len(bp)) > 0) {
        der = (byte*)XMALLOC(derSz, 0, DYNAMIC_TYPE_DER);
        if (der != NULL) {
            if (wolfSSL_BIO_read(bp, der, derSz) == derSz) {
                crl = wolfSSL_d2i_X509_CRL(x, der, derSz);
            }
        }
    }

    if (der != NULL) {
        XFREE(der, 0, DYNAMIC_TYPE_DER);
    }

    return crl;
}
#endif

#ifndef NO_FILESYSTEM
WOLFSSL_X509_CRL *wolfSSL_d2i_X509_CRL_fp(XFILE fp, WOLFSSL_X509_CRL **crl)
{
    WOLFSSL_ENTER("wolfSSL_d2i_X509_CRL_fp");
    return (WOLFSSL_X509_CRL *)wolfSSL_d2i_X509_fp_ex(fp, (void **)crl, CRL_TYPE);
}

/* Read CRL file, and add it to store and corresponding cert manager     */
/* @param ctx   a pointer of X509_LOOKUP back to the X509_STORE          */
/* @param file  a file to read                                           */
/* @param type  WOLFSSL_FILETYPE_PEM or WOLFSSL_FILETYPE_ASN1            */
/* @return WOLFSSL_SUCCESS(1) on successful, otherwise WOLFSSL_FAILURE(0)*/
WOLFSSL_API int wolfSSL_X509_load_crl_file(WOLFSSL_X509_LOOKUP *ctx,
                                             const char *file, int type)
{
#ifndef NO_BIO
    int ret = WOLFSSL_FAILURE;
    int count = 0;
    WOLFSSL_BIO *bio = NULL;
    WOLFSSL_X509_CRL *crl = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_load_crl_file");

    if (ctx == NULL || file == NULL)
        return ret;

    if ((bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file())) == NULL)
        return ret;

    if (wolfSSL_BIO_read_filename(bio, file) <= 0) {
        wolfSSL_BIO_free(bio);
        return ret;
    }

    if (wolfSSL_BIO_read_filename(bio, file) <= 0) {
        wolfSSL_BIO_free(bio);
        return ret;
    }

    if (type == WOLFSSL_FILETYPE_PEM) {
        do {
            crl = wolfSSL_PEM_read_bio_X509_CRL(bio, NULL, NULL, NULL);
            if (crl == NULL) {
                if (count <= 0) {
                    WOLFSSL_MSG("Load crl failed");
                }
                break;
            }

            ret = wolfSSL_X509_STORE_add_crl(ctx->store, crl);
            if (ret == WOLFSSL_FAILURE) {
                WOLFSSL_MSG("Adding crl failed");
                break;
            }
            count++;
            wolfSSL_X509_CRL_free(crl);
            crl = NULL;
        }   while(crl == NULL);

        ret = count;
    }
    else if (type == WOLFSSL_FILETYPE_ASN1) {
        crl = wolfSSL_d2i_X509_CRL_bio(bio, NULL);
        if (crl == NULL) {
            WOLFSSL_MSG("Load crl failed");
        } else {
            ret = wolfSSL_X509_STORE_add_crl(ctx->store, crl);
            if (ret == WOLFSSL_FAILURE) {
                WOLFSSL_MSG("Adding crl failed");
            } else {
                ret = 1;/* handled a file */
            }
        }
    } else {
        WOLFSSL_MSG("Invalid file type");
    }

    wolfSSL_X509_CRL_free(crl);
    wolfSSL_BIO_free(bio);

    WOLFSSL_LEAVE("wolfSSL_X509_load_crl_file", ret);
    return ret;
#else
    int ret = WOLFSSL_FAILURE;
    int count = 0;
    XFILE fp;
    WOLFSSL_X509_CRL *crl = NULL;

    WOLFSSL_ENTER("wolfSSL_X509_load_crl_file");

    if (ctx == NULL || file == NULL)
        return ret;

    if ((fp = XFOPEN(file, "rb")) == XBADFILE)
        return ret;

    if (type == WOLFSSL_FILETYPE_PEM) {
        do {
            crl = wolfSSL_PEM_read_X509_CRL(fp, NULL, NULL, NULL);
            if (crl == NULL) {
                if (count <= 0) {
                    WOLFSSL_MSG("Load crl failed");
                }
                break;
            }

            ret = wolfSSL_X509_STORE_add_crl(ctx->store, crl);
            if (ret == WOLFSSL_FAILURE) {
                WOLFSSL_MSG("Adding crl failed");
                break;
            }
            count++;
            wolfSSL_X509_CRL_free(crl);
            crl = NULL;
        }
        while(crl == NULL);

        ret = count;
    }
    else if (type == WOLFSSL_FILETYPE_ASN1) {
        crl = wolfSSL_d2i_X509_CRL_fp(fp, NULL);
        if (crl == NULL) {
            WOLFSSL_MSG("Load crl failed");
        }
        else {
            ret = wolfSSL_X509_STORE_add_crl(ctx->store, crl);
            if (ret == WOLFSSL_FAILURE) {
                WOLFSSL_MSG("Adding crl failed");
            }
            else {
                ret = 1;/* handled a file */
            }
        }
    }
    else {
        WOLFSSL_MSG("Invalid file type");
    }

    wolfSSL_X509_CRL_free(crl);
    XFCLOSE(fp);

    WOLFSSL_LEAVE("wolfSSL_X509_load_crl_file", ret);
    return ret;
#endif /* !NO_BIO */
}
#endif /* !NO_FILESYSTEM */


WOLFSSL_X509_CRL* wolfSSL_d2i_X509_CRL(WOLFSSL_X509_CRL** crl,
        const unsigned char* in, int len)
{
    WOLFSSL_X509_CRL *newcrl = NULL;
    int ret = WOLFSSL_SUCCESS;

    WOLFSSL_ENTER("wolfSSL_d2i_X509_CRL");

    if (in == NULL) {
        WOLFSSL_MSG("Bad argument value");
    } else {
        newcrl =(WOLFSSL_X509_CRL*)XMALLOC(sizeof(WOLFSSL_X509_CRL), NULL,
                DYNAMIC_TYPE_CRL);
        if (newcrl == NULL){
            WOLFSSL_MSG("New CRL allocation failed");
        } else {
            ret = InitCRL(newcrl, NULL);
            if (ret < 0) {
                WOLFSSL_MSG("Init tmp CRL failed");
            } else {
                ret = BufferLoadCRL(newcrl, in, len, WOLFSSL_FILETYPE_ASN1,
                    NO_VERIFY);
                if (ret != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Buffer Load CRL failed");
                } else {
                    if (crl) {
                        *crl = newcrl;
                    }
                }
            }
        }
    }

    if((ret != WOLFSSL_SUCCESS) && (newcrl != NULL)) {
        wolfSSL_X509_CRL_free(newcrl);
        newcrl = NULL;
    }

    return newcrl;
}

/* Retrieve issuer X509_NAME from CRL
 * return X509_NAME*  on success
 * return NULL on failure
 */
WOLFSSL_X509_NAME* wolfSSL_X509_CRL_get_issuer_name(WOLFSSL_X509_CRL* crl)
{
    if (crl == NULL || crl->crlList == NULL)
        return NULL;

    return crl->crlList->issuer;
}

/* Retrieve version from CRL
 * return version on success
 * return 0 on failure
 */
int wolfSSL_X509_CRL_version(WOLFSSL_X509_CRL* crl)
{
    if (crl == NULL || crl->crlList == NULL)
        return 0;

    return crl->crlList->version;
}

/* Retrieve sig OID from CRL
 * return OID on success
 * return 0 on failure
 */
int wolfSSL_X509_CRL_get_signature_type(WOLFSSL_X509_CRL* crl)
{
    if (crl == NULL || crl->crlList == NULL)
        return 0;

    return crl->crlList->signatureOID;
}

/* Retrieve sig NID from CRL
 * return NID on success
 * return 0 on failure
 */
int wolfSSL_X509_CRL_get_signature_nid(const WOLFSSL_X509_CRL* crl)
{
    if (crl == NULL || crl->crlList == NULL)
        return 0;

    return oid2nid(crl->crlList->signatureOID, oidSigType);
}

/* Retrieve signature from CRL
 * return WOLFSSL_SUCCESS on success and negative values on failure
 */
int wolfSSL_X509_CRL_get_signature(WOLFSSL_X509_CRL* crl,
    unsigned char* buf, int* bufSz)
{
    WOLFSSL_ENTER("wolfSSL_X509_CRL_get_signature");

    if (crl == NULL || crl->crlList == NULL || bufSz == NULL)
        return BAD_FUNC_ARG;

    if (buf != NULL)
        XMEMCPY(buf, crl->crlList->signature, *bufSz);
    *bufSz = (int)crl->crlList->signatureSz;

    return WOLFSSL_SUCCESS;
}

/* Retrieve serial number from RevokedCert
 * return WOLFSSL_SUCCESS on success and negative values on failure
 */
int wolfSSL_X509_REVOKED_get_serial_number(RevokedCert* rev,
    byte* in, int* inOutSz)
{
    WOLFSSL_ENTER("wolfSSL_X509_REVOKED_get_serial_number");
    if (rev == NULL || inOutSz == NULL) {
        return BAD_FUNC_ARG;
    }

    if (in != NULL) {
        if (*inOutSz < rev->serialSz) {
            WOLFSSL_MSG("Serial buffer too small");
            return BUFFER_E;
        }
        XMEMCPY(in, rev->serialNumber, rev->serialSz);
    }
    *inOutSz = rev->serialSz;

    return WOLFSSL_SUCCESS;
}

const WOLFSSL_ASN1_INTEGER* wolfSSL_X509_REVOKED_get0_serial_number(const
                                                      WOLFSSL_X509_REVOKED *rev)
{
    WOLFSSL_ENTER("wolfSSL_X509_REVOKED_get0_serial_number");

    if (rev != NULL) {
        return rev->serialNumber;
    }
    else
        return NULL;
}

#ifndef NO_WOLFSSL_STUB
const WOLFSSL_ASN1_TIME* wolfSSL_X509_REVOKED_get0_revocation_date(const
                                                      WOLFSSL_X509_REVOKED *rev)
{
    WOLFSSL_STUB("wolfSSL_X509_REVOKED_get0_revocation_date");

    (void) rev;
    return NULL;
}
#endif


#ifndef NO_BIO
/* print serial number out
*  return WOLFSSL_SUCCESS on success
*/
static int X509RevokedPrintSerial(WOLFSSL_BIO* bio, RevokedCert* rev,
    int indent)
{
    unsigned char serial[32];
    int  sz = sizeof(serial);

    XMEMSET(serial, 0, sz);
    if (wolfSSL_X509_REVOKED_get_serial_number(rev, serial, &sz)
            == WOLFSSL_SUCCESS) {
        X509PrintSerial_ex(bio, serial, sz, 0, indent);
    }
    return WOLFSSL_SUCCESS;
}


/* print out the signature in human readable format for use with
* wolfSSL_X509_CRL_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509CRLPrintSignature(WOLFSSL_BIO* bio, WOLFSSL_X509_CRL* crl,
        int algOnly, int indent)
{
    int sigSz = 0;

    if (wolfSSL_X509_CRL_get_signature(crl, NULL, &sigSz) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (sigSz > 0) {
        unsigned char* sig;
        int sigNid = wolfSSL_X509_CRL_get_signature_nid(crl);

        sig = (unsigned char*)XMALLOC(sigSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (sig == NULL) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_X509_CRL_get_signature(crl, sig, &sigSz) <= 0) {
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return WOLFSSL_FAILURE;
        }

        if (X509PrintSignature_ex(bio, sig, sigSz, sigNid, algOnly, indent)
                != WOLFSSL_SUCCESS) {
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return WOLFSSL_FAILURE;
        }

        if (sig != NULL) {
            XFREE(sig, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        }

    }

    return WOLFSSL_SUCCESS;
}
#endif /* !NO_BIO */

#if !defined(NO_BIO) && defined(XSNPRINTF)
/* print out the extensions in human readable format for use with
 * wolfSSL_X509_CRL_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509CRLPrintExtensions(WOLFSSL_BIO* bio, WOLFSSL_X509_CRL* crl,
        int indent)
{
    char tmp[MAX_WIDTH]; /* buffer for XSNPRINTF */

    if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s\n", indent, "",
                "CRL extensions:") >= MAX_WIDTH) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
    }

    if (crl->crlList->crlNumber) {
        if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s\n", indent + 4, "",
                    "X509v3 CRL Number:") >= MAX_WIDTH) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }

        if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%d\n", indent + 8, "",
            crl->crlList->crlNumber) >= MAX_WIDTH)
        {
            return WOLFSSL_FAILURE;
        }
        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }
        XMEMSET(tmp, 0, sizeof(tmp));
    }

#if !defined(NO_SKID)
    if (crl->crlList->extAuthKeyIdSet && crl->crlList->extAuthKeyId[0] != 0) {
        word32 i;
        char val[5];
        int valSz = 5;

        if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s", indent + 4, "",
                    "X509v3 Authority Key Identifier:") >= MAX_WIDTH) {
            return WOLFSSL_FAILURE;
        }

        XSTRNCAT(tmp, "\n", MAX_WIDTH - XSTRLEN(tmp) - 1);

        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }
        XMEMSET(tmp, 0, MAX_WIDTH);

        if (XSNPRINTF(tmp, MAX_WIDTH - 1, "%*s%s",
                    indent + 8, "", "keyid") >= MAX_WIDTH) {
            return WOLFSSL_FAILURE;
        }


        for (i = 0; i < XSTRLEN((char*)crl->crlList->extAuthKeyId); i++) {
            /* check if buffer is almost full */
            if (XSTRLEN(tmp) >= sizeof(tmp) - valSz) {
                if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                    return WOLFSSL_FAILURE;
                }
                tmp[0] = '\0';
            }
            if (XSNPRINTF(val, valSz, ":%02X", crl->crlList->extAuthKeyId[i])
                >= valSz)
            {
                WOLFSSL_MSG("buffer overrun");
                return WOLFSSL_FAILURE;
            }
            XSTRNCAT(tmp, val, valSz);
        }
        XSTRNCAT(tmp, "\n", XSTRLEN("\n") + 1);
        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }
    }
#endif

    return WOLFSSL_SUCCESS;
}

/* iterate through a CRL's Revoked Certs and print out in human
 * readable format for use with wolfSSL_X509_CRL_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509CRLPrintRevoked(WOLFSSL_BIO* bio, WOLFSSL_X509_CRL* crl,
        int indent)
{
    char tmp[MAX_WIDTH]; /* buffer for XSNPRINTF */
    int i;

    if (crl->crlList->totalCerts > 0) {
        RevokedCert* revoked = crl->crlList->certs;

        if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s\n", indent, "",
                    "Revoked Certificates:") >= MAX_WIDTH) {
            return WOLFSSL_FAILURE;
        }

        if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
            return WOLFSSL_FAILURE;
        }
        XMEMSET(tmp, 0, MAX_WIDTH);

        for (i = 0; i < crl->crlList->totalCerts; i++) {
            if (revoked->serialSz > 0) {
                if (X509RevokedPrintSerial(bio, revoked, indent + 4)
                        != WOLFSSL_SUCCESS) {
                    return WOLFSSL_FAILURE;
                }
            }
        #ifndef NO_ASN_TIME
             if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s", indent + 8, "",
                         "Revocation Date: ") >= MAX_WIDTH) {
                return WOLFSSL_FAILURE;
            }

            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }

            if (revoked->revDate[0] != 0) {
                if (GetTimeString(revoked->revDate, ASN_UTC_TIME,
                    tmp, MAX_WIDTH) != WOLFSSL_SUCCESS) {
                    if (GetTimeString(revoked->revDate, ASN_GENERALIZED_TIME,
                    tmp, MAX_WIDTH) != WOLFSSL_SUCCESS) {
                        WOLFSSL_MSG("Error getting revocation date");
                        return WOLFSSL_FAILURE;
                    }
                }
            }
            else {
                XSTRNCPY(tmp, "Not Set", MAX_WIDTH-1);
            }
            tmp[MAX_WIDTH - 1] = '\0'; /* make sure null terminated */
            if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
                return WOLFSSL_FAILURE;
            }

            if (wolfSSL_BIO_write(bio, "\n", (int)XSTRLEN("\n")) <= 0) {
                return WOLFSSL_FAILURE;
            }
        #endif
            revoked = revoked->next;
        }
    }
    else {
        if (wolfSSL_BIO_write(bio, "No Revoked Certificates.\n",
                       (int)XSTRLEN("No Revoked Certificates.\n")) <= 0) {
            return WOLFSSL_FAILURE;
        }
    }

    return WOLFSSL_SUCCESS;
}

#ifndef NO_ASN_TIME
/* print out the last/next update times in human readable
 * format for use with wolfSSL_X509_CRL_print()
 * return WOLFSSL_SUCCESS on success
 */
static int X509CRLPrintDates(WOLFSSL_BIO* bio, WOLFSSL_X509_CRL* crl,
        int indent)
{
    char tmp[MAX_WIDTH]; /* buffer for XSNPRINTF */

    if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s", indent, "",
                "Last Update: ") >= MAX_WIDTH) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (crl->crlList->lastDate[0] != 0) {
        if (GetTimeString(crl->crlList->lastDate, ASN_UTC_TIME,
            tmp, MAX_WIDTH) != WOLFSSL_SUCCESS) {
            if (GetTimeString(crl->crlList->lastDate, ASN_GENERALIZED_TIME,
            tmp, MAX_WIDTH) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Error getting last update date");
                return WOLFSSL_FAILURE;
            }
        }
    }
    else {
        XSTRNCPY(tmp, "Not Set", sizeof(tmp)-1);
    }
    tmp[sizeof(tmp) - 1] = '\0'; /* make sure null terminated */
    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "\n", (int)XSTRLEN("\n")) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (XSNPRINTF(tmp, MAX_WIDTH, "%*s%s", indent, "",
                "Next Update: ") >= MAX_WIDTH) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (crl->crlList->nextDate[0] != 0) {
        if (GetTimeString(crl->crlList->nextDate, ASN_UTC_TIME,
            tmp, MAX_WIDTH) != WOLFSSL_SUCCESS) {
            if (GetTimeString(crl->crlList->nextDate, ASN_GENERALIZED_TIME,
            tmp, MAX_WIDTH) != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Error getting next update date");
                return WOLFSSL_FAILURE;
            }
        }
    }
    else {
        XSTRNCPY(tmp, "Not Set", sizeof(tmp)-1);
    }
    tmp[sizeof(tmp) - 1] = '\0'; /* make sure null terminated */
    if (wolfSSL_BIO_write(bio, tmp, (int)XSTRLEN(tmp)) <= 0) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "\n", (int)XSTRLEN("\n")) <= 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif

/* Writes the human readable form of x509 to bio.
 *
 * bio  WOLFSSL_BIO to write to.
 * crl Certificate revocation list to write.
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on failure
 */
int wolfSSL_X509_CRL_print(WOLFSSL_BIO* bio, WOLFSSL_X509_CRL* crl)
{
    char issuType[] = "Issuer: ";

    if (bio == NULL || crl == NULL || crl->crlList == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "Certificate Revocation List (CRL):\n",
                  (int)XSTRLEN("Certificate Revocation List (CRL):\n")) <= 0) {
            return WOLFSSL_FAILURE;
    }

    /* print version */
    if (X509PrintVersion(bio, wolfSSL_X509_CRL_version(crl), 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print signature algo */
    if (X509CRLPrintSignature(bio, crl, 1, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print issuer name */
    if (X509PrintName(bio, wolfSSL_X509_CRL_get_issuer_name(crl), issuType, 8)
            != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

#ifndef NO_ASN_TIME
    /* print last and next update times */
    if (X509CRLPrintDates(bio, crl, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }
#endif

    /* print CRL extensions */
    if (X509CRLPrintExtensions(bio, crl, 8) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    /* print CRL Revoked Certs */
    if (X509CRLPrintRevoked(bio, crl, 0) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    if (X509CRLPrintSignature(bio, crl, 0, 4) != WOLFSSL_SUCCESS) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_BIO_write(bio, "\n\0", (int)XSTRLEN("\n\0")) <= 0) {
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}
#endif /* !NO_BIO && XSNPRINTF */
#endif /* HAVE_CRL */
#endif /* OPENSSL_EXTRA */

#if defined(HAVE_CRL) && (defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL))
void wolfSSL_X509_CRL_free(WOLFSSL_X509_CRL *crl)
{
    WOLFSSL_ENTER("wolfSSL_X509_CRL_free");

    if (crl)
        FreeCRL(crl, 1);
}
#endif /* HAVE_CRL && (OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL) */

#ifdef OPENSSL_EXTRA
WOLFSSL_ASN1_TIME* wolfSSL_X509_CRL_get_lastUpdate(WOLFSSL_X509_CRL* crl)
{
    if ((crl != NULL) && (crl->crlList != NULL) &&
        (crl->crlList->lastDateAsn1.data[0] != 0)) {
        return &crl->crlList->lastDateAsn1;
    }
    else
        return NULL;
}

WOLFSSL_ASN1_TIME* wolfSSL_X509_CRL_get_nextUpdate(WOLFSSL_X509_CRL* crl)
{
    if ((crl != NULL) && (crl->crlList != NULL) &&
        (crl->crlList->nextDateAsn1.data[0] != 0)) {
        return &crl->crlList->nextDateAsn1;
    }
    else
        return NULL;
}

#ifndef NO_WOLFSSL_STUB
int wolfSSL_X509_CRL_verify(WOLFSSL_X509_CRL* crl, WOLFSSL_EVP_PKEY* key)
{
    (void)crl;
    (void)key;
    WOLFSSL_STUB("X509_CRL_verify");
    return 0;
}
#endif
#endif /* OPENSSL_EXTRA */

#ifdef OPENSSL_EXTRA


WOLFSSL_X509_VERIFY_PARAM* wolfSSL_X509_VERIFY_PARAM_new(void)
{
    WOLFSSL_X509_VERIFY_PARAM *param = NULL;
    param = (WOLFSSL_X509_VERIFY_PARAM*)XMALLOC(
            sizeof(WOLFSSL_X509_VERIFY_PARAM), NULL, DYNAMIC_TYPE_OPENSSL);
    if (param != NULL)
        XMEMSET(param, 0, sizeof(WOLFSSL_X509_VERIFY_PARAM ));

    return(param);
}


void wolfSSL_X509_VERIFY_PARAM_free(WOLFSSL_X509_VERIFY_PARAM *param)
{
    if (param != NULL)
        XFREE(param, NULL, DYNAMIC_TYPE_OPENSSL);
}


/* Sets flags by OR'ing with existing value. */
int wolfSSL_X509_VERIFY_PARAM_set_flags(WOLFSSL_X509_VERIFY_PARAM *param,
        unsigned long flags)
{
    int ret = WOLFSSL_FAILURE;

    if (param != NULL) {
        param->flags |= flags;
        ret = WOLFSSL_SUCCESS;
    }

    return ret;
}


int wolfSSL_X509_VERIFY_PARAM_get_flags(WOLFSSL_X509_VERIFY_PARAM *param)
{
    int ret = 0;

    if (param != NULL) {
        ret = (int)param->flags;
    }

    return ret;
}


int wolfSSL_X509_VERIFY_PARAM_clear_flags(WOLFSSL_X509_VERIFY_PARAM *param,
        unsigned long flags)
{
    int ret = WOLFSSL_FAILURE;

    if (param != NULL) {
        param->flags &= ~flags;
        ret = WOLFSSL_SUCCESS;
    }

    return ret;
}


/* inherits properties of param "to" to param "from"
*
* WOLFSSL_VPARAM_DEFAULT          any values in "src" is copied
*                                 if "src" value is new for "to".
* WOLFSSL_VPARAM_OVERWRITE        all values of "form" are copied to "to"
* WOLFSSL_VPARAM_RESET_FLAGS      the flag values are copied, not Ored
* WOLFSSL_VPARAM_LOCKED           don't copy any values
* WOLFSSL_VPARAM_ONCE             the current inherit_flags is zerroed
*/
static int wolfSSL_X509_VERIFY_PARAM_inherit(WOLFSSL_X509_VERIFY_PARAM *to,
                                         const WOLFSSL_X509_VERIFY_PARAM *from)
{
    int ret = WOLFSSL_FAILURE;
    int isOverWrite = 0;
    int isDefault = 0;
    unsigned int flags;

    /* sanity check */
    if (!to || !from) {
        /* be compatible to openssl return value */
        return WOLFSSL_SUCCESS;
    }
    flags = to->inherit_flags | from->inherit_flags;

    if (flags & WOLFSSL_VPARAM_LOCKED) {
        return WOLFSSL_SUCCESS;
    }

    if (flags & WOLFSSL_VPARAM_ONCE) {
        to->inherit_flags = 0;
    }

    isOverWrite = (flags & WOLFSSL_VPARAM_OVERWRITE);
    isDefault = (flags & WOLFSSL_VPARAM_DEFAULT);

    /* copy check_time if check time is not set */
    if ((to->flags & WOLFSSL_USE_CHECK_TIME) == 0 || isOverWrite) {
           to->check_time = from->check_time;
           to->flags &= ~WOLFSSL_USE_CHECK_TIME;
    }
    /* host name */
    if (isOverWrite ||
        (from->hostName[0] != 0 && (to->hostName[0] == 0 || isDefault))) {
            if (!(ret = wolfSSL_X509_VERIFY_PARAM_set1_host(to, from->hostName,
                (int)XSTRLEN(from->hostName))))
                return ret;
        to->hostFlags = from->hostFlags;
    }
    /* ip ascii */
    if (isOverWrite ||
        (from->ipasc[0] != 0 && (to->ipasc[0] == 0 || isDefault))) {

            if (!(ret = wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(to, from->ipasc)))
                return ret;
    }

    if (flags & WOLFSSL_VPARAM_RESET_FLAGS)
        to->flags = 0;

    to->flags |= from->flags;

    return ret;
}

/******************************************************************************
* wolfSSL_X509_VERIFY_PARAM_set1_host - sets the DNS hostname to name
* hostnames is cleared if name is NULL or empty.
*
* RETURNS:
*
*/
int wolfSSL_X509_VERIFY_PARAM_set1_host(WOLFSSL_X509_VERIFY_PARAM* pParam,
                                         const char* name,
                                         unsigned int nameSz)
{
    WOLFSSL_ENTER("wolfSSL_X509_VERIFY_PARAM_set1_host");

    if (pParam == NULL)
        return WOLFSSL_FAILURE;

    /* If name is NULL, clear hostname. */
    if (name == NULL) {
        XMEMSET(pParam->hostName, 0, WOLFSSL_HOST_NAME_MAX);
        return WOLFSSL_SUCCESS;
    }

    /* If name is NULL-terminated, namelen can be set to zero. */
    if (nameSz == 0) {
        nameSz = (unsigned int)XSTRLEN(name);
    }

    if (nameSz > 0 && name[nameSz - 1] == '\0')
        nameSz--;

    if (nameSz > WOLFSSL_HOST_NAME_MAX-1) {
        WOLFSSL_MSG("Truncating name");
        nameSz = WOLFSSL_HOST_NAME_MAX-1;
    }

    if (nameSz > 0) {
        XMEMCPY(pParam->hostName, name, nameSz);
        XMEMSET(pParam->hostName + nameSz, 0,
                WOLFSSL_HOST_NAME_MAX - nameSz);
    }

    pParam->hostName[nameSz] = '\0';

    return WOLFSSL_SUCCESS;
}

/* Set VERIFY PARAM from "from" pointer to "to" pointer */
int wolfSSL_X509_VERIFY_PARAM_set1(WOLFSSL_X509_VERIFY_PARAM *to,
                                   const WOLFSSL_X509_VERIFY_PARAM *from)
{
    int ret = WOLFSSL_FAILURE;
    unsigned int _inherit_flags;

    if (!to) {
        return ret;
    }
    /* keeps the inherit flags for save */
    _inherit_flags = to->inherit_flags;

    /* Ored DEFAULT inherit flag property to copy "from" contents to "to"
    *  contents
    */
    to->inherit_flags |= WOLFSSL_VPARAM_DEFAULT;

    ret = wolfSSL_X509_VERIFY_PARAM_inherit(to, from);

    /* restore inherit flag */
    to->inherit_flags = _inherit_flags;

    return ret;
}

/* Set the host flag in the X509_VERIFY_PARAM structure */
void wolfSSL_X509_VERIFY_PARAM_set_hostflags(WOLFSSL_X509_VERIFY_PARAM* param,
                                             unsigned int flags)
{
    if (param != NULL) {
        param->hostFlags = flags;
    }
}

/* Sets the expected IP address to ipasc.
 *
 * param is a pointer to the X509_VERIFY_PARAM structure
 * ipasc is a NULL-terminated string with N.N.N.N for IPv4 and
 *       HH:HH ... HH:HH for IPv6. There is no validation performed on the
 *       parameter, and it must be an exact match with the IP in the cert.
 *
 * return 1 for success and 0 for failure*/
int wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(WOLFSSL_X509_VERIFY_PARAM *param,
        const char *ipasc)
{
    int ret = WOLFSSL_FAILURE;

    if (param != NULL) {
        if (ipasc == NULL) {
            param->ipasc[0] = '\0';
        }
        else {
            XSTRLCPY(param->ipasc, ipasc, WOLFSSL_MAX_IPSTR);
            param->ipasc[WOLFSSL_MAX_IPSTR-1] = '\0';
        }
        ret = WOLFSSL_SUCCESS;
    }

    return ret;
}
/* Sets the expected IP address to ip(asc)
 *          by re-constructing IP address in ascii
 * @param  param is a pointer to the X509_VERIFY_PARAM structure
 * @param  ip    in binary format of ip address
 * @param  iplen size of ip, 4 for ipv4, 16 for ipv6
 * @return 1 for success and 0 for failure
 */
int wolfSSL_X509_VERIFY_PARAM_set1_ip(WOLFSSL_X509_VERIFY_PARAM* param,
    const unsigned char* ip, size_t iplen)
{
    int ret = WOLFSSL_FAILURE;
#ifndef NO_FILESYSTEM
    char* buf = NULL;
    char* p = NULL;
    word32 val = 0;
    int i;
    const size_t max_ipv6_len = 40;
    byte write_zero = 0;
#endif

    /* sanity check */
    if (param == NULL || (iplen != 0 && iplen != 4 && iplen != 16)) {
        WOLFSSL_MSG("bad function arg");
        return ret;
    }
#ifndef NO_FILESYSTEM
    if (iplen == 4) {
        /* ipv4 www.xxx.yyy.zzz max 15 length + Null termination */
        buf = (char*)XMALLOC(16, NULL, DYNAMIC_TYPE_TMP_BUFFER);

        if (!buf) {
            WOLFSSL_MSG("failed malloc");
            return ret;
        }

        XSPRINTF(buf, "%d.%d.%d.%d", ip[0], ip[1], ip[2], ip[3]);
        buf[15] = '\0';
    }
    else if (iplen == 16) {
        /* ipv6 normal address scheme
        *   y1:y2:y3:y4:y5:y6:y7:y8, len(yx):4, len(y1-y8):32. len(":"):7
        *   Max len is 32 + 7 + 1(Termination) = 40 bytes
        *
        *   ipv6 dual address
        *   Or y1:y2:y3:y4:y:y6:x.x.x.x yx is 4, y1-y6 is 24, ":" is 6
        *   x.x.x.x is 15.
        *   Max len is 24 + 6 + 15 + 1(Termination) = 46 bytes
        *
        *   Expect data in ip[16]
        *   e.g (aaaa):(bbbb):(cccc):....(hhhh)
        *   (aaaa) = (ip[0<<8)|ip[1]
        *   ......
        *   (hhhh) = (ip[14]<<8)|(ip[15])
        *
        *   e.g ::(gggg):(hhhh)
        *   ip[0]-[11] = 0
        *   (gggg) = (ip[12]<<8) |(ip[13])
        *   (hhhh) = (ip[14]<<8) |(ip[15])
        *
        *   Because it is not able to know which ivp6 scheme uses from data to
        *   reconstruct IP address, this function assumes
        *   ivp6 normal address scheme, not dual address scheme,
        *   to re-construct IP address in ascii.
        */
        buf = (char*)XMALLOC(max_ipv6_len, NULL, DYNAMIC_TYPE_TMP_BUFFER);

        if (!buf) {
            WOLFSSL_MSG("failed malloc");
            return ret;
        }
        p = buf;
        for (i = 0; i < 16; i += 2) {
           val = (((word32)(ip[i]<<8)) | (ip[i+1])) & 0xFFFF;
           if (val == 0){
               if (!write_zero) {
                    *p = ':';
               }
               p++;
               *p = '\0';
               write_zero = 1;
           }
           else {
               if (i != 0)
                *p++ = ':';
               XSPRINTF(p, "%x", val);
           }
           /* sanity check */
           if (XSTRLEN(buf) > max_ipv6_len) {
               WOLFSSL_MSG("The target ip address exceeds buffer length(40)");
               XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
               buf = NULL;
               break;
           }
           /* move the pointer to the last */
           /* XSTRLEN includes NULL because of XSPRINTF use */
           p = buf + (XSTRLEN(buf));
        }
        /* termination */
        if(i == 16 && buf) {
            p--;
            if ((*p) == ':') {
            /* when the last character is :, the following segments are zero
             * Therefore, adding : and null termination
             */
                 p++;
                 *p++ = ':';
                *p = '\0';
            }
        }
    }
    else {
        WOLFSSL_MSG("iplen is zero, do nothing");
        return WOLFSSL_SUCCESS;
    }

    if (buf) {
         /* set address to ip asc */
        ret = wolfSSL_X509_VERIFY_PARAM_set1_ip_asc(param, buf);
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
#else
    (void)param;
    (void)ip;
    (void)iplen;
#endif

    return ret;
}

#ifndef NO_WOLFSSL_STUB
void wolfSSL_X509_OBJECT_free_contents(WOLFSSL_X509_OBJECT* obj)
{
    (void)obj;
    WOLFSSL_STUB("X509_OBJECT_free_contents");
}
#endif

#ifndef NO_ASN_TIME
int wolfSSL_X509_cmp_current_time(const WOLFSSL_ASN1_TIME* asnTime)
{
    return wolfSSL_X509_cmp_time(asnTime, NULL);
}

/* return -1 if asnTime is earlier than or equal to cmpTime, and 1 otherwise
 * return 0 on error
 */
int wolfSSL_X509_cmp_time(const WOLFSSL_ASN1_TIME* asnTime, time_t* cmpTime)
{
    int ret = WOLFSSL_FAILURE;
    time_t tmpTime, *pTime = &tmpTime;
    struct tm ts, *tmpTs, *ct;
#if defined(NEED_TMP_TIME)
    /* for use with gmtime_r */
    struct tm tmpTimeStorage;

    tmpTs = &tmpTimeStorage;
#else
    tmpTs = NULL;
#endif
    (void)tmpTs;

    if (asnTime == NULL) {
        return WOLFSSL_FAILURE;
    }

    if (cmpTime == NULL) {
        /* Use current time */
        *pTime = wc_Time(0);
    }
    else {
        pTime = cmpTime;
    }

    if (wolfSSL_ASN1_TIME_to_tm((WOLFSSL_ASN1_TIME*)asnTime, &ts) !=
                                                              WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("Failed to convert WOLFSSL_ASN1_TIME to struct tm.");
        return WOLFSSL_FAILURE;
    }

    /* Convert to time struct*/
    ct = XGMTIME(pTime, tmpTs);

    if (ct == NULL)
        return GETTIME_ERROR;

    /* DateGreaterThan returns 1 for >; 0 for <= */
    ret = DateGreaterThan(&ts, ct) ? 1 : -1;

    return ret;
}
#endif /* !NO_ASN_TIME */

#if (defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)) && \
    !defined(NO_ASN_TIME) && !defined(USER_TIME) && !defined(TIME_OVERRIDES)
WOLFSSL_ASN1_TIME *wolfSSL_X509_time_adj_ex(WOLFSSL_ASN1_TIME *asnTime,
    int offset_day, long offset_sec, time_t *in_tm)
{
    /* get current time if in_tm is null */
    time_t t = in_tm ? *in_tm : wc_Time(0);
    return wolfSSL_ASN1_TIME_adj(asnTime, t, offset_day, offset_sec);
}

WOLFSSL_ASN1_TIME *wolfSSL_X509_time_adj(WOLFSSL_ASN1_TIME *asnTime,
    long offset_sec, time_t *in_tm)
{
    return wolfSSL_X509_time_adj_ex(asnTime, 0, offset_sec, in_tm);
}

WOLFSSL_ASN1_TIME* wolfSSL_X509_gmtime_adj(WOLFSSL_ASN1_TIME *s, long adj)
{
    return wolfSSL_X509_time_adj(s, adj, NULL);
}
#endif

#ifndef NO_WOLFSSL_STUB
int wolfSSL_sk_X509_REVOKED_num(WOLFSSL_X509_REVOKED* revoked)
{
    (void)revoked;
    WOLFSSL_STUB("sk_X509_REVOKED_num");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
WOLFSSL_X509_REVOKED* wolfSSL_X509_CRL_get_REVOKED(WOLFSSL_X509_CRL* crl)
{
    (void)crl;
    WOLFSSL_STUB("X509_CRL_get_REVOKED");
    return 0;
}
#endif

#ifndef NO_WOLFSSL_STUB
WOLFSSL_X509_REVOKED* wolfSSL_sk_X509_REVOKED_value(
                                    WOLFSSL_X509_REVOKED* revoked, int value)
{
    (void)revoked;
    (void)value;
    WOLFSSL_STUB("sk_X509_REVOKED_value");
    return 0;
}
#endif

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)

WOLFSSL_ASN1_INTEGER* wolfSSL_X509_get_serialNumber(WOLFSSL_X509* x509)
{
    WOLFSSL_ASN1_INTEGER* a;
    int i = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_serialNumber");

    if (x509 == NULL) {
        WOLFSSL_MSG("NULL function argument");
        return NULL;
    }

    if (x509->serialNumber != NULL)
       return x509->serialNumber;

    a = wolfSSL_ASN1_INTEGER_new();
    if (a == NULL)
        return NULL;

    /* Make sure there is space for the data, ASN.1 type and length. */
    if (x509->serialSz > (WOLFSSL_ASN1_INTEGER_MAX - 2)) {
        /* dynamically create data buffer, +2 for type and length */
        a->data = (unsigned char*)XMALLOC(x509->serialSz + 2, NULL,
                DYNAMIC_TYPE_OPENSSL);
        if (a->data == NULL) {
            wolfSSL_ASN1_INTEGER_free(a);
            return NULL;
        }
        a->dataMax   = x509->serialSz + 2;
        a->isDynamic = 1;
    } else {
        /* Use array instead of dynamic memory */
        a->data    = a->intData;
        a->dataMax = WOLFSSL_ASN1_INTEGER_MAX;
    }

    #if defined(WOLFSSL_QT) || defined(WOLFSSL_HAPROXY)
        XMEMCPY(&a->data[i], x509->serial, x509->serialSz);
        a->length = x509->serialSz;
    #else
        a->data[i++] = ASN_INTEGER;
        i += SetLength(x509->serialSz, a->data + i);
        XMEMCPY(&a->data[i], x509->serial, x509->serialSz);
        a->length = x509->serialSz + 2;
    #endif

    x509->serialNumber = a;

    return a;
}

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA

#if defined(OPENSSL_ALL) || defined(WOLFSSL_APACHE_HTTPD) \
    || defined(WOLFSSL_HAPROXY) || defined(WOLFSSL_WPAS)
WOLFSSL_X509_ALGOR* wolfSSL_X509_ALGOR_new(void)
{
    WOLFSSL_X509_ALGOR* ret;
    ret = (WOLFSSL_X509_ALGOR*)XMALLOC(sizeof(WOLFSSL_X509_ALGOR), NULL,
                                       DYNAMIC_TYPE_OPENSSL);
    if (ret) {
        XMEMSET(ret, 0, sizeof(WOLFSSL_X509_ALGOR));
    }
    return ret;
}

void wolfSSL_X509_ALGOR_free(WOLFSSL_X509_ALGOR *alg)
{
    if (alg) {
        wolfSSL_ASN1_OBJECT_free(alg->algorithm);
        wolfSSL_ASN1_TYPE_free(alg->parameter);
        XFREE(alg, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

/* Returns X509_ALGOR struct with signature algorithm */
const WOLFSSL_X509_ALGOR* wolfSSL_X509_get0_tbs_sigalg(const WOLFSSL_X509 *x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_get0_tbs_sigalg");

    if (x509 == NULL) {
        WOLFSSL_MSG("x509 struct NULL error");
        return NULL;
    }

    return &x509->algor;
}

/* Sets paobj pointer to X509_ALGOR signature algorithm */
void wolfSSL_X509_ALGOR_get0(const WOLFSSL_ASN1_OBJECT **paobj, int *pptype,
                            const void **ppval, const WOLFSSL_X509_ALGOR *algor)
{
    WOLFSSL_ENTER("wolfSSL_X509_ALGOR_get0");

    if (!algor) {
        WOLFSSL_MSG("algor object is NULL");
        return;
    }

    if (paobj)
        *paobj = algor->algorithm;
    if (ppval && algor->parameter)
        *ppval = algor->parameter->value.ptr;
    if (pptype) {
        if (algor->parameter) {
            *pptype = algor->parameter->type;
        }
        else {
            /* Default to V_ASN1_OBJECT */
            *pptype = V_ASN1_OBJECT;
        }
    }
}

/**
 * Populate algor members.
 *
 * @param algor The object to be set
 * @param aobj The value to be set in algor->algorithm
 * @param ptype The type of algor->parameter
 * @param pval The value of algor->parameter
 * @return WOLFSSL_SUCCESS on success
 *         WOLFSSL_FAILURE on missing parameters or bad malloc
 */
int wolfSSL_X509_ALGOR_set0(WOLFSSL_X509_ALGOR *algor, WOLFSSL_ASN1_OBJECT *aobj,
                            int ptype, void *pval)
{
    if (!algor) {
        return WOLFSSL_FAILURE;
    }

    if (!algor->parameter) {
        algor->parameter = wolfSSL_ASN1_TYPE_new();
        if (!algor->parameter) {
            return WOLFSSL_FAILURE;
        }
    }

    if (aobj) {
        algor->algorithm = aobj;
    }
    wolfSSL_ASN1_TYPE_set(algor->parameter, ptype, pval);

    return WOLFSSL_SUCCESS;
}

/**
 * Allocate a new WOLFSSL_X509_PUBKEY object.
 *
 * @return New zero'ed WOLFSSL_X509_PUBKEY object
 */
WOLFSSL_X509_PUBKEY *wolfSSL_X509_PUBKEY_new(void)
{
    WOLFSSL_X509_PUBKEY *ret;
    ret = (WOLFSSL_X509_PUBKEY*)XMALLOC(sizeof(WOLFSSL_X509_PUBKEY), NULL,
                                        DYNAMIC_TYPE_OPENSSL);
    if (!ret) {
        return NULL;
    }
    XMEMSET(ret, 0, sizeof(WOLFSSL_X509_PUBKEY));
    ret->algor = wolfSSL_X509_ALGOR_new();
    if (!ret->algor) {
        wolfSSL_X509_PUBKEY_free(ret);
        return NULL;
    }
    return ret;
}

/**
 * Free WOLFSSL_X509_PUBKEY and all its members.
 *
 * @param at Object to free
 */
void wolfSSL_X509_PUBKEY_free(WOLFSSL_X509_PUBKEY *x)
{
    if (x) {
        if (x->algor) {
            wolfSSL_X509_ALGOR_free(x->algor);
        }
        if (x->pkey) {
            wolfSSL_EVP_PKEY_free(x->pkey);
        }
        XFREE(x, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

/* Returns X509_PUBKEY structure containing X509_ALGOR and EVP_PKEY */
WOLFSSL_X509_PUBKEY* wolfSSL_X509_get_X509_PUBKEY(const WOLFSSL_X509* x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_X509_PUBKEY");

    if (x509 == NULL) {
        WOLFSSL_MSG("x509 struct NULL error");
        return NULL;
    }

    return (WOLFSSL_X509_PUBKEY*)&x509->key;
}

/* Sets ppkalg pointer to X509_PUBKEY algorithm. Returns WOLFSSL_SUCCESS on
    success or WOLFSSL_FAILURE on error. */
int wolfSSL_X509_PUBKEY_get0_param(WOLFSSL_ASN1_OBJECT **ppkalg,
     const unsigned char **pk, int *ppklen, WOLFSSL_X509_ALGOR **pa,
     WOLFSSL_X509_PUBKEY *pub)
{
    WOLFSSL_ENTER("wolfSSL_X509_PUBKEY_get0_param");

    if (!pub || !pub->pubKeyOID) {
        WOLFSSL_MSG("X509_PUBKEY struct not populated");
        return WOLFSSL_FAILURE;
    }

    if (!pub->algor) {
        if (!(pub->algor = wolfSSL_X509_ALGOR_new())) {
            return WOLFSSL_FAILURE;
        }
        pub->algor->algorithm = wolfSSL_OBJ_nid2obj(pub->pubKeyOID);
        if (pub->algor->algorithm == NULL) {
            WOLFSSL_MSG("Failed to create object from NID");
            return WOLFSSL_FAILURE;
        }
    }

    if (pa)
        *pa = pub->algor;
    if (ppkalg)
        *ppkalg = pub->algor->algorithm;
    if (pk)
        *pk = (unsigned char*)pub->pkey->pkey.ptr;
    if (ppklen)
        *ppklen = pub->pkey->pkey_sz;

    return WOLFSSL_SUCCESS;
}

/* Returns a pointer to the pkey when passed a key */
WOLFSSL_EVP_PKEY* wolfSSL_X509_PUBKEY_get(WOLFSSL_X509_PUBKEY* key)
{
    WOLFSSL_ENTER("wolfSSL_X509_PUBKEY_get");
    if (key == NULL || key->pkey == NULL) {
        WOLFSSL_LEAVE("wolfSSL_X509_PUBKEY_get", BAD_FUNC_ARG);
        return NULL;
    }
    if (wolfSSL_EVP_PKEY_up_ref(key->pkey) != WOLFSSL_SUCCESS) {
        WOLFSSL_LEAVE("wolfSSL_X509_PUBKEY_get", BAD_MUTEX_E);
        return NULL;
    }
    WOLFSSL_LEAVE("wolfSSL_X509_PUBKEY_get", WOLFSSL_SUCCESS);
    return key->pkey;
}

int wolfSSL_X509_PUBKEY_set(WOLFSSL_X509_PUBKEY **x, WOLFSSL_EVP_PKEY *key)
{
    WOLFSSL_X509_PUBKEY *pk = NULL;
    int ptype;
    void *pval;
#ifndef NO_DSA
    WOLFSSL_ASN1_STRING *str;
#endif
#ifdef HAVE_ECC
    int nid;
    const WOLFSSL_EC_GROUP *group;
#endif
    WOLFSSL_ASN1_OBJECT *keyTypeObj;

    WOLFSSL_ENTER("wolfSSL_X509_PUBKEY_set");

    if (!x || !key) {
        return WOLFSSL_FAILURE;
    }

    if (!(pk = wolfSSL_X509_PUBKEY_new())) {
        return WOLFSSL_FAILURE;
    }

    switch (key->type) {
#ifndef NO_RSA
    case EVP_PKEY_RSA:
        pval = NULL;
        ptype = V_ASN1_NULL;
        pk->pubKeyOID = RSAk;
        break;
#endif
#ifndef NO_DSA
    case EVP_PKEY_DSA:
        if (!key->dsa->p || !key->dsa->q || !key->dsa->g)
            goto error;

        str = wolfSSL_ASN1_STRING_new();
        if (str == NULL)
            goto error;

        str->length = wolfSSL_i2d_DSAparams(key->dsa,
             (unsigned char **)&str->data);
        if (str->length <= 0) {
            wolfSSL_ASN1_STRING_free(str);
            goto error;
        }
        str->isDynamic = 1;

        pval = str;
        ptype = V_ASN1_SEQUENCE;
        pk->pubKeyOID = DSAk;
        break;
#endif
#ifdef HAVE_ECC
    case EVP_PKEY_EC:
        group = wolfSSL_EC_KEY_get0_group(key->ecc);
        if (!group)
            goto error;

        nid = wolfSSL_EC_GROUP_get_curve_name(group);
        if (nid <= 0) {
            /* TODO: Add support for no nid case */
            WOLFSSL_MSG("nid not found");
            goto error;
        }

        pval = wolfSSL_OBJ_nid2obj(nid);
        if (!pval)
            goto error;

        ptype = V_ASN1_OBJECT;
        pk->pubKeyOID = ECDSAk;
        break;
#endif
    default:
        WOLFSSL_MSG("Unknown key type");
        goto error;
    }

    keyTypeObj = wolfSSL_OBJ_nid2obj(key->type);
    if (keyTypeObj == NULL) {
        if (ptype == V_ASN1_OBJECT)
            ASN1_OBJECT_free((WOLFSSL_ASN1_OBJECT *)pval);
        else
            ASN1_STRING_free((WOLFSSL_ASN1_STRING *)pval);
        goto error;
    }
    if (!wolfSSL_X509_ALGOR_set0(pk->algor, keyTypeObj, ptype, pval)) {
        WOLFSSL_MSG("Failed to create algorithm object");
        ASN1_OBJECT_free(keyTypeObj);
        if (ptype == V_ASN1_OBJECT)
            ASN1_OBJECT_free((WOLFSSL_ASN1_OBJECT *)pval);
        else
            ASN1_STRING_free((WOLFSSL_ASN1_STRING *)pval);
        goto error;
    }

    if (!wolfSSL_EVP_PKEY_up_ref(key)) {
        WOLFSSL_MSG("Failed to up key reference");
        goto error;
    }
    pk->pkey = key;

    wolfSSL_X509_PUBKEY_free(*x);
    *x = pk;
    return WOLFSSL_SUCCESS;
error:
    if (pk) {
        wolfSSL_X509_PUBKEY_free(pk);
    }
    return WOLFSSL_FAILURE;
}

#endif /* OPENSSL_ALL || WOLFSSL_APACHE_HTTPD || WOLFSSL_HAPROXY || WOLFSSL_WPAS */

#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
WOLFSSL_BASIC_CONSTRAINTS* wolfSSL_BASIC_CONSTRAINTS_new(void)
{
    WOLFSSL_BASIC_CONSTRAINTS* bc;
    bc = (WOLFSSL_BASIC_CONSTRAINTS*)
          XMALLOC(sizeof(WOLFSSL_BASIC_CONSTRAINTS), NULL,
          DYNAMIC_TYPE_X509_EXT);
    if (bc == NULL) {
        WOLFSSL_MSG("Failed to malloc basic constraints");
        return NULL;
    }
    XMEMSET(bc, 0, sizeof(WOLFSSL_BASIC_CONSTRAINTS));
    return bc;
}

/* frees the wolfSSL_BASIC_CONSTRAINTS object */
void wolfSSL_BASIC_CONSTRAINTS_free(WOLFSSL_BASIC_CONSTRAINTS *bc)
{
    WOLFSSL_ENTER("wolfSSL_BASIC_CONSTRAINTS_free");
    if (bc == NULL) {
        WOLFSSL_MSG("Argument is NULL");
        return;
    }
    if (bc->pathlen) {
        wolfSSL_ASN1_INTEGER_free(bc->pathlen);
    }
    XFREE(bc, NULL, DYNAMIC_TYPE_OPENSSL);
}

WOLFSSL_AUTHORITY_KEYID* wolfSSL_AUTHORITY_KEYID_new(void)
{
    WOLFSSL_AUTHORITY_KEYID* akey = (WOLFSSL_AUTHORITY_KEYID*)XMALLOC(
          sizeof(WOLFSSL_AUTHORITY_KEYID), NULL, DYNAMIC_TYPE_OPENSSL);
    if (!akey) {
        WOLFSSL_MSG("Issue creating WOLFSSL_AUTHORITY_KEYID struct");
        return NULL;
    }
    XMEMSET(akey, 0, sizeof(WOLFSSL_AUTHORITY_KEYID));
    return akey;
}

/* frees the wolfSSL_AUTHORITY_KEYID object */
void wolfSSL_AUTHORITY_KEYID_free(WOLFSSL_AUTHORITY_KEYID *id)
{
    WOLFSSL_ENTER("wolfSSL_AUTHORITY_KEYID_free");
    if(id == NULL) {
        WOLFSSL_MSG("Argument is NULL");
        return;
    }
    if (id->keyid) {
        wolfSSL_ASN1_STRING_free(id->keyid);
    }
    if (id->issuer) {
        wolfSSL_ASN1_OBJECT_free(id->issuer);
    }
    if (id->serial) {
        wolfSSL_ASN1_INTEGER_free(id->serial);
    }
    XFREE(id, NULL, DYNAMIC_TYPE_OPENSSL);
}

#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef KEEP_PEER_CERT
char*  wolfSSL_X509_get_subjectCN(WOLFSSL_X509* x509)
{
    if (x509 == NULL)
        return NULL;

    return x509->subjectCN;
}
#endif /* KEEP_PEER_CERT */

#if defined(OPENSSL_EXTRA_X509_SMALL) || defined(OPENSSL_EXTRA)
/* increments ref count of WOLFSSL_X509. Return 1 on success, 0 on error */
int wolfSSL_X509_up_ref(WOLFSSL_X509* x509)
{
    if (x509) {
        int ret;
        wolfSSL_RefInc(&x509->ref, &ret);
        if (ret != 0) {
            WOLFSSL_MSG("Failed to lock x509 mutex");
            return WOLFSSL_FAILURE;
        }

        return WOLFSSL_SUCCESS;
    }

    return WOLFSSL_FAILURE;
}
#endif /* OPENSSL_EXTRA_X509_SMALL || OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL)
WOLF_STACK_OF(WOLFSSL_X509)* wolfSSL_X509_chain_up_ref(
        WOLF_STACK_OF(WOLFSSL_X509)* chain)
{
    /* wolfSSL_sk_dup takes care of doing a deep copy */
    return wolfSSL_sk_dup(chain);
}
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    void wolfSSL_X509_NAME_free(WOLFSSL_X509_NAME *name)
    {
        WOLFSSL_ENTER("wolfSSL_X509_NAME_free");
        FreeX509Name(name);
        if (name != NULL) {
            XFREE(name, name->heap, DYNAMIC_TYPE_X509);
        }
    }


    /* Malloc's a new WOLFSSL_X509_NAME structure
     *
     * returns NULL on failure, otherwise returns a new structure.
     */
    WOLFSSL_X509_NAME* wolfSSL_X509_NAME_new_ex(void *heap)
    {
        WOLFSSL_X509_NAME* name;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_new_ex");

        name = (WOLFSSL_X509_NAME*)XMALLOC(sizeof(WOLFSSL_X509_NAME), heap,
                DYNAMIC_TYPE_X509);
        if (name != NULL) {
            InitX509Name(name, 1, heap);
        }
        return name;
    }

    WOLFSSL_X509_NAME* wolfSSL_X509_NAME_new(void) {
        return wolfSSL_X509_NAME_new_ex(NULL);
    }

    /* Creates a duplicate of a WOLFSSL_X509_NAME structure.
       Returns a new WOLFSSL_X509_NAME structure or NULL on failure */
    WOLFSSL_X509_NAME* wolfSSL_X509_NAME_dup(WOLFSSL_X509_NAME *name)
    {
        WOLFSSL_X509_NAME* copy = NULL;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_dup");

        if (name == NULL) {
            WOLFSSL_MSG("NULL parameter");
            return NULL;
        }

        if (!(copy = wolfSSL_X509_NAME_new_ex(name->heap))) {
            return NULL;
        }

        /* copy contents */
        InitX509Name(copy, 1, name->heap);
        if (wolfSSL_X509_NAME_copy(name, copy) != WOLFSSL_SUCCESS) {
            wolfSSL_X509_NAME_free(copy);
            return NULL;
        }

        return copy;
    }

#ifdef WOLFSSL_CERT_GEN

#if defined(WOLFSSL_CERT_REQ) || defined(WOLFSSL_CERT_EXT) || defined(OPENSSL_EXTRA)
    /* Helper function to copy cert name from a WOLFSSL_X509_NAME structure to
    * a Cert structure.
    *
    * returns length of DER on success and a negative error value on failure
    */
    static int CopyX509NameToCert(WOLFSSL_X509_NAME* n, byte* out)
    {
        unsigned char* der = NULL;
        int length = BAD_FUNC_ARG, ret;
        word32 idx = 0;

        ret = wolfSSL_i2d_X509_NAME(n, &der);
        if (ret > (int)sizeof(CertName) || ret < 0) {
            WOLFSSL_MSG("Name conversion error");
            ret = MEMORY_E;
        }

        if (ret > 0) {
            /* strip off sequence, this gets added on certificate creation */
            ret = GetSequence(der, &idx, &length, ret);
        }

        if (ret > 0) {
            XMEMCPY(out, der + idx, length);
        }

        if (der != NULL)
            XFREE(der, NULL, DYNAMIC_TYPE_OPENSSL);

        return length;
    }
#endif

#ifdef WOLFSSL_CERT_REQ
    static int ReqCertFromX509(Cert* cert, WOLFSSL_X509* req)
    {
        int ret;

        if (wc_InitCert(cert) != 0)
            return WOLFSSL_FAILURE;


        ret = CopyX509NameToCert(&req->subject, cert->sbjRaw);
        if (ret < 0) {
            WOLFSSL_MSG("REQ subject conversion error");
            ret = MEMORY_E;
        }
        else {
            ret = WOLFSSL_SUCCESS;
        }

        if (ret == WOLFSSL_SUCCESS) {
        #if defined(OPENSSL_ALL)
            int idx;
        #endif

            cert->version = req->version;
            cert->isCA = req->isCa;
            cert->basicConstSet = req->basicConstSet;
    #ifdef WOLFSSL_CERT_EXT
            if (req->subjKeyIdSz != 0) {
                XMEMCPY(cert->skid, req->subjKeyId, req->subjKeyIdSz);
                cert->skidSz = req->subjKeyIdSz;
            }
            if (req->keyUsageSet)
                cert->keyUsage = req->keyUsage;
            /* Extended Key Usage not supported. */
    #endif

            XMEMCPY(cert->challengePw, req->challengePw, CTC_NAME_SIZE);
            cert->challengePwPrintableString = req->challengePw[0] != 0;

        #if defined(OPENSSL_ALL)
            idx = wolfSSL_X509_REQ_get_attr_by_NID(req,
                    NID_pkcs9_unstructuredName, -1);
            if (idx != WOLFSSL_FATAL_ERROR) {
                WOLFSSL_X509_ATTRIBUTE *attr;
                attr = wolfSSL_X509_REQ_get_attr(req, idx);
                if (attr != NULL) {
                    const unsigned char *attrData;
                    int attrDataSz;

                    attrData = wolfSSL_ASN1_STRING_get0_data(
                            attr->value->value.asn1_string);
                    attrDataSz = wolfSSL_ASN1_STRING_length(
                            attr->value->value.asn1_string);

                    /* +1 to make sure is terminated string */
                    if (attrDataSz + 1 > CTC_NAME_SIZE) {
                        WOLFSSL_MSG("attribute size was too large to copy");
                        ret = REQ_ATTRIBUTE_E;
                    }
                    else {
                        XMEMCPY(cert->unstructuredName, attrData, attrDataSz);
                        cert->unstructuredName[attrDataSz] = '\0';
                    }
                }
            }

        #ifdef WOLFSSL_CUSTOM_OID
            if (ret == WOLFSSL_SUCCESS) {
                if ((req->customExtCount < 0) ||
                    (req->customExtCount >= NUM_CUSTOM_EXT)) {
                    WOLFSSL_MSG("Bad value for customExtCount.");
                    ret = WOLFSSL_FAILURE;
                }

                if (ret == WOLFSSL_SUCCESS) {
                    for (idx = 0; idx < req->customExtCount; idx++) {
                        /* Note that ownership is NOT transferred.
                         * req->custom_exts buffers still need to be cleaned
                         * up. */
                        cert->customCertExt[idx] = req->custom_exts[idx];
                    }
                    cert->customCertExtCount = req->customExtCount;
                }
            }
        #endif /* WOLFSSL_CUSTOM_OID */
        #endif /* OPENSSL_ALL */

    #ifdef WOLFSSL_ALT_NAMES
            if (ret == WOLFSSL_SUCCESS) {
                cert->altNamesSz = FlattenAltNames(cert->altNames,
                        sizeof(cert->altNames), req->altNames);
            }
    #endif /* WOLFSSL_ALT_NAMES */
        }

        return ret;
    }
#endif /* WOLFSSL_CERT_REQ */

    /* converts WOLFSSL_AN1_TIME to Cert form, returns positive size on
     * success */
    static int CertDateFromX509(byte* out, int outSz, WOLFSSL_ASN1_TIME* t)
    {
        int sz, i;

        if (t->length + 1 >= outSz) {
            return BUFFER_E;
        }

        out[0] = (byte) t->type;
        sz = SetLength(t->length, out + 1) + 1;  /* gen tag */
        for (i = 0; i < t->length; i++) {
            out[sz + i] = t->data[i];
        }
        return t->length + sz;
    }

    /* convert a WOLFSSL_X509 to a Cert structure for writing out */
    static int CertFromX509(Cert* cert, WOLFSSL_X509* x509)
    {
        int ret;
        #ifdef WOLFSSL_CERT_EXT
        int i;
        #endif

        WOLFSSL_ENTER("wolfSSL_X509_to_Cert");

        if (x509 == NULL || cert == NULL) {
            return BAD_FUNC_ARG;
        }

        wc_InitCert(cert);

        cert->version = (int)wolfSSL_X509_get_version(x509);

        if (x509->notBefore.length > 0) {
            cert->beforeDateSz = CertDateFromX509(cert->beforeDate,
                        CTC_DATE_SIZE, &x509->notBefore);
            if (cert->beforeDateSz <= 0){
                WOLFSSL_MSG("Error converting WOLFSSL_X509 not before date");
                return WOLFSSL_FAILURE;
            }
        }
        else {
            cert->beforeDateSz = 0;
        }

        if (x509->notAfter.length > 0) {
            cert->afterDateSz = CertDateFromX509(cert->afterDate,
                        CTC_DATE_SIZE, &x509->notAfter);
            if (cert->afterDateSz <= 0){
                WOLFSSL_MSG("Error converting WOLFSSL_X509 not after date");
                return WOLFSSL_FAILURE;
            }
        }
        else {
            cert->afterDateSz = 0;
        }

    #ifdef WOLFSSL_ALT_NAMES
        cert->altNamesSz = FlattenAltNames(cert->altNames,
                sizeof(cert->altNames), x509->altNames);
    #endif /* WOLFSSL_ALT_NAMES */

        cert->sigType = wolfSSL_X509_get_signature_type(x509);
        cert->keyType = x509->pubKeyOID;
        cert->isCA    = wolfSSL_X509_get_isCA(x509);
        cert->basicConstSet = x509->basicConstSet;

    #ifdef WOLFSSL_CERT_EXT
        if (x509->subjKeyIdSz <= CTC_MAX_SKID_SIZE) {
            if (x509->subjKeyId) {
                XMEMCPY(cert->skid, x509->subjKeyId, x509->subjKeyIdSz);
            }
            cert->skidSz = (int)x509->subjKeyIdSz;
        }
        else {
            WOLFSSL_MSG("Subject Key ID too large");
            WOLFSSL_ERROR_VERBOSE(BUFFER_E);
            return WOLFSSL_FAILURE;
        }

        if (x509->authKeyIdSz < sizeof(cert->akid)) {
        #ifdef WOLFSSL_AKID_NAME
            cert->rawAkid = 0;
            if (x509->authKeyIdSrc) {
                XMEMCPY(cert->akid, x509->authKeyIdSrc, x509->authKeyIdSrcSz);
                cert->akidSz = (int)x509->authKeyIdSrcSz;
                cert->rawAkid = 1;
            }
            else
        #endif
            if (x509->authKeyId) {
                XMEMCPY(cert->akid, x509->authKeyId, x509->authKeyIdSz);
                cert->akidSz = (int)x509->authKeyIdSz;
            }
        }
        else {
            WOLFSSL_MSG("Auth Key ID too large");
            WOLFSSL_ERROR_VERBOSE(BUFFER_E);
            return WOLFSSL_FAILURE;
        }

        for (i = 0; i < x509->certPoliciesNb; i++) {
            /* copy the smaller of MAX macros, by default they are currently equal*/
            if ((int)CTC_MAX_CERTPOL_SZ <= (int)MAX_CERTPOL_SZ) {
                XMEMCPY(cert->certPolicies[i], x509->certPolicies[i],
                        CTC_MAX_CERTPOL_SZ);
            }
            else {
                XMEMCPY(cert->certPolicies[i], x509->certPolicies[i],
                        MAX_CERTPOL_SZ);
            }
        }
        cert->certPoliciesNb = (word16)x509->certPoliciesNb;

        cert->keyUsage = x509->keyUsage;
        cert->extKeyUsage = x509->extKeyUsage;
        cert->nsCertType = x509->nsCertType;

        if (x509->rawCRLInfo != NULL) {
            if (x509->rawCRLInfoSz > CTC_MAX_CRLINFO_SZ) {
                WOLFSSL_MSG("CRL Info too large");
                WOLFSSL_ERROR_VERBOSE(BUFFER_E);
                return WOLFSSL_FAILURE;
            }
            XMEMCPY(cert->crlInfo, x509->rawCRLInfo, x509->rawCRLInfoSz);
            cert->crlInfoSz = x509->rawCRLInfoSz;
        }
    #endif /* WOLFSSL_CERT_EXT */

    #ifdef WOLFSSL_CERT_REQ
        /* copy over challenge password for REQ certs */
        XMEMCPY(cert->challengePw, x509->challengePw, CTC_NAME_SIZE);
    #endif

        /* Only makes sense to do this for OPENSSL_EXTRA because without
         * this define the function will error out below */
        #ifdef OPENSSL_EXTRA
        if (x509->serialSz == 0 && x509->serialNumber != NULL &&
                /* Check if the buffer contains more than just the
                 * ASN tag and length */
                x509->serialNumber->length > 2) {
            if (wolfSSL_X509_set_serialNumber(x509, x509->serialNumber)
                    != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Failed to set serial number");
                return WOLFSSL_FAILURE;
            }
        }
        #endif

        /* set serial number */
        if (x509->serialSz > 0) {
        #if defined(OPENSSL_EXTRA)
            byte serial[EXTERNAL_SERIAL_SIZE];
            int  serialSz = EXTERNAL_SERIAL_SIZE;

            ret = wolfSSL_X509_get_serial_number(x509, serial, &serialSz);
            if (ret != WOLFSSL_SUCCESS) {
                WOLFSSL_MSG("Serial size error");
                return WOLFSSL_FAILURE;
            }

            if (serialSz > EXTERNAL_SERIAL_SIZE ||
                    serialSz > CTC_SERIAL_SIZE) {
                WOLFSSL_MSG("Serial size too large error");
                WOLFSSL_ERROR_VERBOSE(BUFFER_E);
                return WOLFSSL_FAILURE;
            }
            XMEMCPY(cert->serial, serial, serialSz);
            cert->serialSz = serialSz;
        #else
            WOLFSSL_MSG("Getting X509 serial number not supported");
            return WOLFSSL_FAILURE;
        #endif
        }

        /* copy over Name structures */
        if (x509->issuerSet)
            cert->selfSigned = 0;

    #if defined(WOLFSSL_CERT_EXT) || defined(OPENSSL_EXTRA)
        ret = CopyX509NameToCert(&x509->subject, cert->sbjRaw);
        if (ret < 0) {
            WOLFSSL_MSG("Subject conversion error");
            return MEMORY_E;
        }
        if (cert->selfSigned) {
            XMEMCPY(cert->issRaw, cert->sbjRaw, sizeof(CertName));
        }
        else {
            ret = CopyX509NameToCert(&x509->issuer, cert->issRaw);
            if (ret < 0) {
                WOLFSSL_MSG("Issuer conversion error");
                return MEMORY_E;
            }
        }
    #endif

        cert->heap = x509->heap;

        (void)ret;
        return WOLFSSL_SUCCESS;
    }


    /* returns the sig type to use on success i.e CTC_SHAwRSA and WOLFSSL_FALURE
     * on fail case */
    static int wolfSSL_sigTypeFromPKEY(WOLFSSL_EVP_MD* md,
            WOLFSSL_EVP_PKEY* pkey)
    {
    #if !defined(NO_PWDBASED) && defined(OPENSSL_EXTRA)
        int hashType;
        int sigType = WOLFSSL_FAILURE;

        /* Convert key type and hash algorithm to a signature algorithm */
        if (wolfSSL_EVP_get_hashinfo(md, &hashType, NULL) == WOLFSSL_FAILURE) {
            return WOLFSSL_FAILURE;
        }

        if (pkey->type == EVP_PKEY_RSA) {
            switch (hashType) {
                case WC_HASH_TYPE_SHA:
                    sigType = CTC_SHAwRSA;
                    break;
                case WC_HASH_TYPE_SHA224:
                    sigType = CTC_SHA224wRSA;
                    break;
                case WC_HASH_TYPE_SHA256:
                    sigType = CTC_SHA256wRSA;
                    break;
                case WC_HASH_TYPE_SHA384:
                    sigType = CTC_SHA384wRSA;
                    break;
                case WC_HASH_TYPE_SHA512:
                    sigType = CTC_SHA512wRSA;
                    break;
            #ifdef WOLFSSL_SHA3
                case WC_HASH_TYPE_SHA3_224:
                    sigType = CTC_SHA3_224wRSA;
                    break;
                case WC_HASH_TYPE_SHA3_256:
                    sigType = CTC_SHA3_256wRSA;
                    break;
                case WC_HASH_TYPE_SHA3_384:
                    sigType = CTC_SHA3_384wRSA;
                    break;
                case WC_HASH_TYPE_SHA3_512:
                    sigType = CTC_SHA3_512wRSA;
                    break;
            #endif
                default:
                    return WOLFSSL_FAILURE;
            }
        }
        else if (pkey->type == EVP_PKEY_EC) {
            switch (hashType) {
                case WC_HASH_TYPE_SHA:
                    sigType = CTC_SHAwECDSA;
                    break;
                case WC_HASH_TYPE_SHA224:
                    sigType = CTC_SHA224wECDSA;
                    break;
                case WC_HASH_TYPE_SHA256:
                    sigType = CTC_SHA256wECDSA;
                    break;
                case WC_HASH_TYPE_SHA384:
                    sigType = CTC_SHA384wECDSA;
                    break;
                case WC_HASH_TYPE_SHA512:
                    sigType = CTC_SHA512wECDSA;
                    break;
            #ifdef WOLFSSL_SHA3
                case WC_HASH_TYPE_SHA3_224:
                    sigType = CTC_SHA3_224wECDSA;
                    break;
                case WC_HASH_TYPE_SHA3_256:
                    sigType = CTC_SHA3_256wECDSA;
                    break;
                case WC_HASH_TYPE_SHA3_384:
                    sigType = CTC_SHA3_384wECDSA;
                    break;
                case WC_HASH_TYPE_SHA3_512:
                    sigType = CTC_SHA3_512wECDSA;
                    break;
            #endif
                default:
                    return WOLFSSL_FAILURE;
            }
        }
        else
            return WOLFSSL_FAILURE;
        return sigType;
#else
        (void)md;
        (void)pkey;
        WOLFSSL_MSG("Cannot get hashinfo when NO_PWDBASED is defined");
        return WOLFSSL_FAILURE;
#endif /* !NO_PWDBASED && OPENSSL_EXTRA */
    }


    /* generates DER buffer from WOLFSSL_X509
     * If req == 1 then creates a request DER buffer
     *
     * updates derSz with certificate body size on success
     * return WOLFSSL_SUCCESS on success
     */
    static int wolfssl_x509_make_der(WOLFSSL_X509* x509, int req,
            unsigned char* der, int* derSz, int includeSig)
    {
        int ret = WOLFSSL_FAILURE;
        int totalLen;
        Cert* cert = NULL;
        void* key = NULL;
        int type = -1;
    #ifndef NO_RSA
        RsaKey* rsa = NULL;
    #endif
    #ifdef HAVE_ECC
        ecc_key* ecc = NULL;
    #endif
    #ifndef NO_DSA
        DsaKey* dsa = NULL;
    #endif
        WC_RNG rng;
        word32 idx = 0;

        if (x509 == NULL || der == NULL || derSz == NULL)
            return BAD_FUNC_ARG;

    #ifndef WOLFSSL_CERT_REQ
        if (req) {
            WOLFSSL_MSG("WOLFSSL_CERT_REQ needed for certificate request");
            return WOLFSSL_FAILURE;
        }
    #endif

        /* allocate Cert struct on heap since it is large */
        cert = (Cert*)XMALLOC(sizeof(Cert), NULL, DYNAMIC_TYPE_CERT);
        if (cert == NULL) {
            WOLFSSL_MSG("Failed to allocate memory for Cert struct");
            return WOLFSSL_FAILURE;
        }
        XMEMSET(cert, 0, sizeof(Cert));

    #ifdef WOLFSSL_CERT_REQ
        if (req) {
            if (ReqCertFromX509(cert, x509) != WOLFSSL_SUCCESS) {
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return WOLFSSL_FAILURE;
            }
        }
        else
    #endif
        {
            /* Create a Cert that has the certificate fields. */
            if (CertFromX509(cert, x509) != WOLFSSL_SUCCESS) {
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return WOLFSSL_FAILURE;
            }
        }

        /* Create a public key object from requests public key. */
    #ifndef NO_RSA
        if (x509->pubKeyOID == RSAk) {

            rsa = (RsaKey*)XMALLOC(sizeof(RsaKey), NULL, DYNAMIC_TYPE_RSA);
            if (rsa == NULL) {
                WOLFSSL_MSG("Failed to allocate memory for RsaKey");
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return WOLFSSL_FAILURE;
            }

            type = RSA_TYPE;
            ret = wc_InitRsaKey(rsa, x509->heap);
            if (ret != 0) {
                XFREE(rsa, NULL, DYNAMIC_TYPE_RSA);
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return ret;
            }
            ret = wc_RsaPublicKeyDecode(x509->pubKey.buffer, &idx, rsa,
                                                           x509->pubKey.length);
            if (ret != 0) {
                WOLFSSL_ERROR_VERBOSE(ret);
                wc_FreeRsaKey(rsa);
                XFREE(rsa, NULL, DYNAMIC_TYPE_RSA);
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return ret;
            }
            key = (void*)rsa;
        }
    #endif
    #ifdef HAVE_ECC
        if (x509->pubKeyOID == ECDSAk) {

            ecc = (ecc_key*)XMALLOC(sizeof(ecc_key), NULL, DYNAMIC_TYPE_ECC);
            if (ecc == NULL) {
                WOLFSSL_MSG("Failed to allocate memory for ecc_key");
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return WOLFSSL_FAILURE;
            }

            type = ECC_TYPE;
            ret = wc_ecc_init(ecc);
            if (ret != 0) {
                XFREE(ecc, NULL, DYNAMIC_TYPE_ECC);
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return ret;
            }
            ret = wc_EccPublicKeyDecode(x509->pubKey.buffer, &idx, ecc,
                                                           x509->pubKey.length);
            if (ret != 0) {
                WOLFSSL_ERROR_VERBOSE(ret);
                wc_ecc_free(ecc);
                XFREE(ecc, NULL, DYNAMIC_TYPE_ECC);
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return ret;
            }
            key = (void*)ecc;
        }
    #endif
    #ifndef NO_DSA
        if (x509->pubKeyOID == DSAk) {

            dsa = (DsaKey*)XMALLOC(sizeof(DsaKey), NULL, DYNAMIC_TYPE_DSA);
            if (dsa == NULL) {
                WOLFSSL_MSG("Failed to allocate memory for DsaKey");
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return WOLFSSL_FAILURE;
            }

            type = DSA_TYPE;
            ret = wc_InitDsaKey(dsa);
            if (ret != 0) {
                XFREE(dsa, NULL, DYNAMIC_TYPE_DSA);
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return ret;
            }
            ret = wc_DsaPublicKeyDecode(x509->pubKey.buffer, &idx, dsa,
                                                           x509->pubKey.length);
            if (ret != 0) {
                WOLFSSL_ERROR_VERBOSE(ret);
                wc_FreeDsaKey(dsa);
                XFREE(dsa, NULL, DYNAMIC_TYPE_DSA);
                XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
                return ret;
            }
            key = (void*)dsa;
        }
    #endif
        if (key == NULL) {
            WOLFSSL_MSG("No public key found for certificate");
            XFREE(cert, NULL, DYNAMIC_TYPE_CERT);
            return WOLFSSL_FAILURE;
        }

        /* Make the body of the certificate request. */
    #ifdef WOLFSSL_CERT_REQ
        if (req) {
            ret = wc_MakeCertReq_ex(cert, der, *derSz, type, key);
        }
        else
    #endif
        {
            ret = wc_InitRng(&rng);
            if (ret != 0) {
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }

            ret = wc_MakeCert_ex(cert, der, *derSz, type, key, &rng);
            wc_FreeRng(&rng);
        }
        if (ret <= 0) {
            WOLFSSL_ERROR_VERBOSE(ret);
            ret = WOLFSSL_FAILURE;
            goto cleanup;
        }

        if ((x509->serialSz == 0) &&
                (cert->serialSz <= EXTERNAL_SERIAL_SIZE) &&
                (cert->serialSz > 0)) {
        #if defined(OPENSSL_EXTRA)
            WOLFSSL_ASN1_INTEGER *i = wolfSSL_ASN1_INTEGER_new();

            if (i == NULL) {
                WOLFSSL_MSG("wolfSSL_ASN1_INTEGER_new error");
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }
            else {
                i->length = cert->serialSz + 2;
                i->data[0] = ASN_INTEGER;
                i->data[1] = (unsigned char)cert->serialSz;
                XMEMCPY(i->data + 2, cert->serial, cert->serialSz);
                if (wolfSSL_X509_set_serialNumber(x509, i) != WOLFSSL_SUCCESS) {
                    WOLFSSL_MSG("Issue setting generated serial number");
                    wolfSSL_ASN1_INTEGER_free(i);
                    ret = WOLFSSL_FAILURE;
                    goto cleanup;
                }
                wolfSSL_ASN1_INTEGER_free(i);
            }
        #else
            WOLFSSL_MSG("ASN1_INTEGER API not in build");

            ret = WOLFSSL_FAILURE;
            goto cleanup;
        #endif /* OPENSSL_EXTRA */
        }

        if (includeSig) {
            if (!x509->sig.buffer) {
                WOLFSSL_MSG("No signature buffer");
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }
            totalLen = AddSignature(NULL, ret, NULL, x509->sig.length,
                                  x509->sigOID);
            if (totalLen > *derSz) {
                WOLFSSL_MSG("Output der buffer too short");
                ret = WOLFSSL_FAILURE;
                goto cleanup;
            }
            ret = AddSignature(der, ret, x509->sig.buffer,
                               x509->sig.length, x509->sigOID);
        }

        *derSz = ret;
        ret = WOLFSSL_SUCCESS;
cleanup:
        /* Dispose of the public key object. */
    #ifndef NO_RSA
        if (x509->pubKeyOID == RSAk) {
            wc_FreeRsaKey(rsa);
            XFREE(rsa, NULL, DYNAMIC_TYPE_RSA);
        }
    #endif
    #ifdef HAVE_ECC
        if (x509->pubKeyOID == ECDSAk) {
            wc_ecc_free(ecc);
            XFREE(ecc, NULL, DYNAMIC_TYPE_ECC);
        }
    #endif
    #ifndef NO_DSA
        if (x509->pubKeyOID == DSAk) {
            wc_FreeDsaKey(dsa);
            XFREE(dsa, NULL, DYNAMIC_TYPE_DSA);
        }
    #endif
        XFREE(cert, NULL, DYNAMIC_TYPE_CERT);

        return ret;
    }


    /* signs a der buffer for the WOLFSSL_X509 structure using the PKEY and MD
     * hash passed in
     *
     * WARNING: this free's and replaces the existing DER buffer in the
     *          WOLFSSL_X509 with the newly signed buffer.
     * returns size of signed buffer on success and negative values on fail
     */
    static int wolfSSL_X509_resign_cert(WOLFSSL_X509* x509, int req,
            unsigned char* der, int derSz, int certBodySz, WOLFSSL_EVP_MD* md,
            WOLFSSL_EVP_PKEY* pkey)
    {
        int ret;
        void* key = NULL;
        int type = -1;
        int sigType;
        WC_RNG rng;

        (void)req;
        WOLFSSL_ENTER("wolfSSL_X509_resign_cert");

        sigType = wolfSSL_sigTypeFromPKEY(md, pkey);
        if (sigType == WOLFSSL_FAILURE) {
            WOLFSSL_MSG("Error getting signature type from pkey");
            return WOLFSSL_FATAL_ERROR;
        }


        /* Get the private key object and type from pkey. */
    #ifndef NO_RSA
        if (pkey->type == EVP_PKEY_RSA) {
            type = RSA_TYPE;
            key = pkey->rsa->internal;
        }
    #endif
    #ifdef HAVE_ECC
        if (pkey->type == EVP_PKEY_EC) {
            type = ECC_TYPE;
            key = pkey->ecc->internal;
        }
    #endif

        /* Sign the certificate (request) body. */
        ret = wc_InitRng(&rng);
        if (ret != 0)
            return ret;
        ret = wc_SignCert_ex(certBodySz, sigType, der, derSz, type, key, &rng);
        wc_FreeRng(&rng);
        if (ret < 0) {
            WOLFSSL_LEAVE("wolfSSL_X509_resign_cert", ret);
            return ret;
        }
        derSz = ret;

        /* Extract signature from buffer */
        {
            word32 idx = 0;
            int    len = 0;

            /* Read top level sequence */
            if (GetSequence(der, &idx, &len, derSz) < 0) {
                WOLFSSL_MSG("GetSequence error");
                return WOLFSSL_FATAL_ERROR;
            }
            /* Move idx to signature */
            idx += certBodySz;
            /* Read signature algo sequence */
            if (GetSequence(der, &idx, &len, derSz) < 0) {
                WOLFSSL_MSG("GetSequence error");
                return WOLFSSL_FATAL_ERROR;
            }
            idx += len;
            /* Read signature bit string */
            if (CheckBitString(der, &idx, &len, derSz, 0, NULL) != 0) {
                WOLFSSL_MSG("CheckBitString error");
                return WOLFSSL_FATAL_ERROR;
            }
            /* Sanity check */
            if (idx + len != (word32)derSz) {
                WOLFSSL_MSG("unexpected asn1 structure");
                return WOLFSSL_FATAL_ERROR;
            }
            x509->sig.length = 0;
            if (x509->sig.buffer)
                XFREE(x509->sig.buffer, x509->heap, DYNAMIC_TYPE_SIGNATURE);
            x509->sig.buffer = (byte*)XMALLOC(len, x509->heap,
                                              DYNAMIC_TYPE_SIGNATURE);
            if (!x509->sig.buffer) {
                WOLFSSL_MSG("malloc error");
                return WOLFSSL_FATAL_ERROR;
            }
            XMEMCPY(x509->sig.buffer, der + idx, len);
            x509->sig.length = len;
        }

        /* Put in the new certificate encoding into the x509 object. */
        FreeDer(&x509->derCert);
        type = CERT_TYPE;
    #ifdef WOLFSSL_CERT_REQ
        if (req) {
            type = CERTREQ_TYPE;
        }
    #endif
        if (AllocDer(&x509->derCert, derSz, type, NULL) != 0)
            return WOLFSSL_FATAL_ERROR;
        XMEMCPY(x509->derCert->buffer, der, derSz);
        x509->derCert->length = derSz;

        return ret;
    }


    #ifndef WC_MAX_X509_GEN
        /* able to override max size until dynamic buffer created */
        #define WC_MAX_X509_GEN 4096
    #endif

    /* returns the size of signature on success */
    int wolfSSL_X509_sign(WOLFSSL_X509* x509, WOLFSSL_EVP_PKEY* pkey,
            const WOLFSSL_EVP_MD* md)
    {
        int  ret;
        /* @TODO dynamic set based on expected cert size */
        byte *der = (byte *)XMALLOC(WC_MAX_X509_GEN, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        int  derSz = WC_MAX_X509_GEN;

        WOLFSSL_ENTER("wolfSSL_X509_sign");

        if (x509 == NULL || pkey == NULL || md == NULL) {
            ret = WOLFSSL_FAILURE;
            goto out;
        }

        x509->sigOID = wolfSSL_sigTypeFromPKEY((WOLFSSL_EVP_MD*)md, pkey);
        if ((ret = wolfssl_x509_make_der(x509, 0, der, &derSz, 0)) !=
                WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Unable to make DER for X509");
            WOLFSSL_LEAVE("wolfSSL_X509_sign", ret);
            (void)ret;
            ret = WOLFSSL_FAILURE;
            goto out;
        }

        ret = wolfSSL_X509_resign_cert(x509, 0, der, WC_MAX_X509_GEN, derSz,
                (WOLFSSL_EVP_MD*)md, pkey);
        if (ret <= 0) {
            WOLFSSL_LEAVE("wolfSSL_X509_sign", ret);
            ret = WOLFSSL_FAILURE;
            goto out;
        }

    out:
        if (der)
            XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);

        return ret;
    }

#if defined(OPENSSL_EXTRA)
    int wolfSSL_X509_sign_ctx(WOLFSSL_X509 *x509, WOLFSSL_EVP_MD_CTX *ctx)
    {
        WOLFSSL_ENTER("wolfSSL_X509_sign_ctx");

        if (!x509 || !ctx || !ctx->pctx || !ctx->pctx->pkey) {
            WOLFSSL_MSG("Bad parameter");
            return WOLFSSL_FAILURE;
        }

        return wolfSSL_X509_sign(x509, ctx->pctx->pkey, wolfSSL_EVP_MD_CTX_md(ctx));
    }
#endif /* OPENSSL_EXTRA */
#endif /* WOLFSSL_CERT_GEN */


#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
    defined(OPENSSL_EXTRA_X509_SMALL) || defined(WOLFSSL_WPAS_SMALL)
/* Converts from NID_* value to wolfSSL value if needed.
 *
 * @param [in] nid  Numeric Id of a domain name component.
 * @return  Domain name tag values - wolfSSL internal values.
 * @return  -1 when nid isn't known.
 */
static int ConvertNIDToWolfSSL(int nid)
{
    switch (nid) {
        case NID_commonName : return ASN_COMMON_NAME;
    #ifdef WOLFSSL_CERT_NAME_ALL
        case NID_name :       return ASN_NAME;
        case NID_givenName:   return ASN_GIVEN_NAME;
        case NID_dnQualifier :   return ASN_DNQUALIFIER;
        case NID_initials:   return ASN_INITIALS;
    #endif /* WOLFSSL_CERT_NAME_ALL */
        case NID_surname :    return ASN_SUR_NAME;
        case NID_countryName: return ASN_COUNTRY_NAME;
        case NID_localityName: return ASN_LOCALITY_NAME;
        case NID_stateOrProvinceName: return ASN_STATE_NAME;
        case NID_streetAddress: return ASN_STREET_ADDR;
        case NID_organizationName: return ASN_ORG_NAME;
        case NID_organizationalUnitName: return ASN_ORGUNIT_NAME;
        case NID_emailAddress: return ASN_EMAIL_NAME;
        case NID_pkcs9_contentType: return ASN_CONTENT_TYPE;
        case NID_serialNumber: return ASN_SERIAL_NUMBER;
        case NID_userId: return ASN_USER_ID;
        case NID_businessCategory: return ASN_BUS_CAT;
        case NID_domainComponent: return ASN_DOMAIN_COMPONENT;
        case NID_postalCode: return ASN_POSTAL_CODE;
        case NID_favouriteDrink: return ASN_FAVOURITE_DRINK;
        default:
            WOLFSSL_MSG("Attribute NID not found");
            return -1;
    }
}
#endif /* OPENSSL_ALL || OPENSSL_EXTRA ||
          OPENSSL_EXTRA_X509_SMALL || WOLFSSL_WPAS_SMALL*/

#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* This is to convert the x509 name structure into canonical DER format     */
/*  , which has the following rules:                                        */
/*   convert to UTF8                                                        */
/*   convert to lower case                                                  */
/*   multi-spaces collapsed                                                 */
/*   leading SEQUENCE header is skipped                                     */
/* @param  name a pointer to X509_NAME that is to be converted              */
/* @param  out  a pointer to converted data                                 */
/* @return a number of converted bytes, otherwise <=0 error code            */
int wolfSSL_i2d_X509_NAME_canon(WOLFSSL_X509_NAME* name, unsigned char** out)
{
    int  totalBytes = 0, i, idx;
    byte *output, *local = NULL;
#ifdef WOLFSSL_SMALL_STACK
    EncodedName* names = NULL;
#else
    EncodedName  names[MAX_NAME_ENTRIES];
#endif

    if (name == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    names = (EncodedName*)XMALLOC(sizeof(EncodedName) * MAX_NAME_ENTRIES, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (names == NULL)
        return MEMORY_E;
#endif

    XMEMSET(names, 0, sizeof(EncodedName) * MAX_NAME_ENTRIES);

    for (i = 0; i < MAX_NAME_ENTRIES; i++) {
        WOLFSSL_X509_NAME_ENTRY* entry;
        int ret;

        entry = wolfSSL_X509_NAME_get_entry(name, i);
        if (entry != NULL && entry->set >= 1) {
            const char* nameStr;
            WOLFSSL_ASN1_STRING* data;
            WOLFSSL_ASN1_STRING* cano_data;

            cano_data = wolfSSL_ASN1_STRING_new();
            if (cano_data == NULL) {
                #ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
                #endif
                return MEMORY_E;
            }

            data = wolfSSL_X509_NAME_ENTRY_get_data(entry);
            if (data == NULL) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif
                wolfSSL_ASN1_STRING_free(cano_data);
                WOLFSSL_MSG("Error getting entry data");
                return WOLFSSL_FATAL_ERROR;
            }
            if (wolfSSL_ASN1_STRING_canon(cano_data, data) != WOLFSSL_SUCCESS) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif
                wolfSSL_ASN1_STRING_free(cano_data);
                return WOLFSSL_FAILURE;
            }
            nameStr = (const char*)wolfSSL_ASN1_STRING_data(cano_data);

            ret = wc_EncodeNameCanonical(&names[i], nameStr, CTC_UTF8,
                (byte)ConvertNIDToWolfSSL(entry->nid));
            if (ret < 0) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif
                wolfSSL_ASN1_STRING_free(cano_data);
                WOLFSSL_MSG("EncodeName failed");
                return WOLFSSL_FATAL_ERROR;
            }
            totalBytes += ret;
            wolfSSL_ASN1_STRING_free(cano_data);
        }
    }

    if (out == NULL) {
        /* If out is NULL, caller just wants length. */
#ifdef WOLFSSL_SMALL_STACK
        XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return totalBytes;
    }

    /* skip header */
    /* check if using buffer passed in */
    if (*out == NULL) {
        *out = local = (unsigned char*)XMALLOC(totalBytes, NULL,
                DYNAMIC_TYPE_OPENSSL);
        if (*out == NULL) {
            return MEMORY_E;
        }
    }
    output = *out;
    idx = 0;

    for (i = 0; i < MAX_NAME_ENTRIES; i++) {
        if (names[i].used) {
            XMEMCPY(output + idx, names[i].encoded, names[i].totalLen);
            idx += names[i].totalLen;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    /* used existing buffer passed in, so increment pointer */
    if (local == NULL) {
        *out += totalBytes;
    }
    return totalBytes;
}
#endif /* OPENSSL_ALL || OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#ifdef WOLFSSL_CERT_GEN
/* Guarded by either
 * A) WOLFSSL_WPAS_SMALL is on or
 * B) (OPENSSL_EXTRA or OPENSSL_EXTRA_X509_SMALL) + WOLFSSL_CERT_GEN +
 *    (WOLFSSL_CERT_REQ or WOLFSSL_CERT_EXT or OPENSSL_EXTRA) has been
 *    defined
 */
#if defined(WOLFSSL_WPAS_SMALL) || \
    (defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)) && \
    defined(WOLFSSL_CERT_GEN) && \
    (defined(WOLFSSL_CERT_REQ) || defined(WOLFSSL_CERT_EXT) || \
     defined(OPENSSL_EXTRA))

/* Converts the x509 name structure into DER format.
 *
 * out  pointer to either a pre setup buffer or a pointer to null for
 *      creating a dynamic buffer. In the case that a pre-existing buffer is
 *      used out will be incremented the size of the DER buffer on success. If
 *      out is NULL, the function returns the necessary output buffer length.
 *
 * returns the size of the buffer on success, or negative value with failure
 */
int wolfSSL_i2d_X509_NAME(WOLFSSL_X509_NAME* name, unsigned char** out)
{
    int  totalBytes = 0, i, idx;
    byte temp[MAX_SEQ_SZ];
    byte *output, *local = NULL;
#ifdef WOLFSSL_SMALL_STACK
    EncodedName* names = NULL;
#else
    EncodedName  names[MAX_NAME_ENTRIES];
#endif

    if (name == NULL)
        return BAD_FUNC_ARG;

#ifdef WOLFSSL_SMALL_STACK
    names = (EncodedName*)XMALLOC(sizeof(EncodedName) * MAX_NAME_ENTRIES, NULL,
                                                       DYNAMIC_TYPE_TMP_BUFFER);
    if (names == NULL)
        return MEMORY_E;
#endif

    XMEMSET(names, 0, sizeof(EncodedName) * MAX_NAME_ENTRIES);

    for (i = 0; i < MAX_NAME_ENTRIES; i++) {
        WOLFSSL_X509_NAME_ENTRY* entry;
        int ret;

        entry = wolfSSL_X509_NAME_get_entry(name, i);
        if (entry != NULL && entry->set >= 1) {
            const char* nameStr;
            int type;
            WOLFSSL_ASN1_STRING* data;

            data = wolfSSL_X509_NAME_ENTRY_get_data(entry);
            if (data == NULL) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif
                WOLFSSL_MSG("Error getting entry data");
                return WOLFSSL_FATAL_ERROR;
            }

            nameStr = (const char*)wolfSSL_ASN1_STRING_data(data);
            type    = wolfSSL_ASN1_STRING_type(data);

            switch (type) {
                case MBSTRING_UTF8:
                    type = CTC_UTF8;
                    break;
                case V_ASN1_PRINTABLESTRING:
                    type = CTC_PRINTABLE;
                    break;
                default:
                    WOLFSSL_MSG("Unknown encoding type conversion UTF8 by default");
                    type = CTC_UTF8;
            }
            ret = wc_EncodeName(&names[i], nameStr, (char)type,
                (byte)ConvertNIDToWolfSSL(entry->nid));
            if (ret < 0) {
            #ifdef WOLFSSL_SMALL_STACK
                XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            #endif
                WOLFSSL_MSG("EncodeName failed");
                return WOLFSSL_FATAL_ERROR;
            }
            totalBytes += ret;
        }
    }

    /* header */
    idx = SetSequence(totalBytes, temp);
    if (totalBytes + idx > ASN_NAME_MAX) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        WOLFSSL_MSG("Total Bytes is greater than ASN_NAME_MAX");
        return BUFFER_E;
    }

    if (out == NULL) {
        /* If out is NULL, caller just wants length. */
        totalBytes += idx;
#ifdef WOLFSSL_SMALL_STACK
        XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return totalBytes;
    }

    /* check if using buffer passed in */
    if (*out == NULL) {
        *out = local = (unsigned char*)XMALLOC(totalBytes + idx, name->heap,
                DYNAMIC_TYPE_OPENSSL);
        if (*out == NULL) {
            return MEMORY_E;
        }
    }
    output = *out;

    idx = SetSequence(totalBytes, output);
    totalBytes += idx;
    for (i = 0; i < MAX_NAME_ENTRIES; i++) {
        if (names[i].used) {
            XMEMCPY(output + idx, names[i].encoded, names[i].totalLen);
            idx += names[i].totalLen;
        }
    }

#ifdef WOLFSSL_SMALL_STACK
    XFREE(names, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif

    /* used existing buffer passed in, so increment pointer */
    if (local == NULL) {
        *out += totalBytes;
    }
    return totalBytes;
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */
#endif /* WOLFSSL_CERT_GEN */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL) || \
    defined (WOLFSSL_WPAS_SMALL)

    WOLFSSL_X509_NAME *wolfSSL_d2i_X509_NAME(WOLFSSL_X509_NAME **name,
                                             unsigned char **in, long length)
    {
        WOLFSSL_X509_NAME* tmp = NULL;
    #ifdef WOLFSSL_SMALL_STACK
        DecodedCert* cert = NULL;
    #else
        DecodedCert cert[1];
    #endif

        WOLFSSL_ENTER("wolfSSL_d2i_X509_NAME");

        if (!in || !*in || length <= 0) {
            WOLFSSL_MSG("Bad argument");
            return NULL;
        }

    #ifdef WOLFSSL_SMALL_STACK
        cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL,
                                     DYNAMIC_TYPE_DCERT);
        if (cert == NULL) {
            return NULL;
        }
    #endif

        /* Set the X509_NAME buffer as the input data for cert.
         * in is NOT a full certificate. Just the name. */
        InitDecodedCert(cert, *in, (word32)length, NULL);

        /* Parse the X509 subject name */
        if (GetName(cert, SUBJECT, (int)length) != 0) {
            WOLFSSL_MSG("WOLFSSL_X509_NAME parse error");
            goto cleanup;
        }

        if (!(tmp = wolfSSL_X509_NAME_new_ex(cert->heap))) {
            WOLFSSL_MSG("wolfSSL_X509_NAME_new_ex error");
            goto cleanup;
        }

        if (wolfSSL_X509_NAME_copy((WOLFSSL_X509_NAME*)cert->subjectName,
                    tmp) != WOLFSSL_SUCCESS) {
            wolfSSL_X509_NAME_free(tmp);
            tmp = NULL;
            goto cleanup;
        }

        if (name)
            *name = tmp;
cleanup:
        FreeDecodedCert(cert);
    #ifdef WOLFSSL_SMALL_STACK
        XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
    #endif
        return tmp;
    }
#endif /* OPENSSL_EXTRA || OPENSSL_ALL || WOLFSSL_WPAS_SMALL */


#if defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)

    /* Compares the two X509 names. If the size of x is larger then y then a
     * positive value is returned if x is smaller a negative value is returned.
     * In the case that the sizes are equal a the value of strcmp between the
     * two names is returned.
     *
     * x First name for comparison
     * y Second name to compare with x
     */
    int wolfSSL_X509_NAME_cmp(const WOLFSSL_X509_NAME* x,
            const WOLFSSL_X509_NAME* y)
    {
        const char* _x;
        const char* _y;
        WOLFSSL_ENTER("wolfSSL_X509_NAME_cmp");

        if (x == NULL || y == NULL) {
            WOLFSSL_MSG("Bad argument passed in");
            return -2;
        }

        if (x == y) {
            return 0; /* match */
        }

        if (x->sz != y->sz) {
            return x->sz - y->sz;
        }

        /*
         * If the name member is not set or is immediately null terminated then
         * compare the staticName member
         */
        _x = (x->name && *x->name) ? x->name : x->staticName;
        _y = (y->name && *y->name) ? y->name : y->staticName;

        return XSTRNCMP(_x, _y, x->sz); /* y sz is the same */
    }

#ifndef NO_BIO

    static WOLFSSL_X509 *loadX509orX509REQFromPemBio(WOLFSSL_BIO *bp,
            WOLFSSL_X509 **x, wc_pem_password_cb *cb, void *u, int type)
    {
        WOLFSSL_X509* x509 = NULL;
#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
        unsigned char* pem = NULL;
        int pemSz;
        long  i = 0, l, footerSz;
        const char* footer = NULL;

        WOLFSSL_ENTER("loadX509orX509REQFromPemBio");

        if (bp == NULL || (type != CERT_TYPE && type != CERTREQ_TYPE)) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_bio_X509", BAD_FUNC_ARG);
            return NULL;
        }

        if ((l = wolfSSL_BIO_get_len(bp)) <= 0) {
            /* No certificate in buffer */
#if defined (WOLFSSL_HAPROXY)
            WOLFSSL_ERROR(PEM_R_NO_START_LINE);
#else
            WOLFSSL_ERROR(ASN_NO_PEM_HEADER);
#endif
            return NULL;
        }

        pemSz = (int)l;
        pem   = (unsigned char*)XMALLOC(pemSz, 0, DYNAMIC_TYPE_PEM);
        if (pem == NULL)
            return NULL;
        XMEMSET(pem, 0, pemSz);

        i = 0;
        if (wc_PemGetHeaderFooter(type, NULL, &footer) != 0) {
            XFREE(pem, 0, DYNAMIC_TYPE_PEM);
            return NULL;
        }
        footerSz = (long)XSTRLEN(footer);

        /* TODO: Inefficient
         * reading in one byte at a time until see the footer
         */
        while ((l = wolfSSL_BIO_read(bp, (char *)&pem[i], 1)) == 1) {
            i++;
            if (i > footerSz && XMEMCMP((char *)&pem[i-footerSz], footer,
                    footerSz) == 0) {
                if (wolfSSL_BIO_read(bp, (char *)&pem[i], 1) == 1) {
                    /* attempt to read newline following footer */
                    i++;
                    if (pem[i-1] == '\r') {
                        /* found \r , Windows line ending is \r\n so try to read one
                         * more byte for \n, ignoring return value */
                        (void)wolfSSL_BIO_read(bp, (char *)&pem[i++], 1);
                    }
                }
                break;
            }
        }
        if (l == 0)
            WOLFSSL_ERROR(ASN_NO_PEM_HEADER);
        if (i > pemSz) {
            WOLFSSL_MSG("Error parsing PEM");
        }
        else {
            pemSz = (int)i;
        #ifdef WOLFSSL_CERT_REQ
            if (type == CERTREQ_TYPE)
                x509 = wolfSSL_X509_REQ_load_certificate_buffer(pem, pemSz,
                                                          WOLFSSL_FILETYPE_PEM);
            else
        #endif
                x509 = wolfSSL_X509_load_certificate_buffer(pem, pemSz,
                                                          WOLFSSL_FILETYPE_PEM);
        }

        if (x != NULL) {
            *x = x509;
        }

        XFREE(pem, NULL, DYNAMIC_TYPE_PEM);

#endif /* WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM */
        (void)bp;
        (void)x;
        (void)cb;
        (void)u;

        return x509;
    }


    WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509(WOLFSSL_BIO *bp, WOLFSSL_X509 **x,
                                            wc_pem_password_cb *cb, void *u)
    {
        return loadX509orX509REQFromPemBio(bp, x, cb, u, CERT_TYPE);
    }

#ifdef WOLFSSL_CERT_REQ
    WOLFSSL_X509 *wolfSSL_PEM_read_bio_X509_REQ(WOLFSSL_BIO *bp, WOLFSSL_X509 **x,
                                                wc_pem_password_cb *cb, void *u)
    {
        return loadX509orX509REQFromPemBio(bp, x, cb, u, CERTREQ_TYPE);
    }

#ifndef NO_FILESYSTEM
    WOLFSSL_X509* wolfSSL_PEM_read_X509_REQ(XFILE fp, WOLFSSL_X509** x,
                                            wc_pem_password_cb* cb, void* u)
    {
        int err = 0;
        WOLFSSL_X509* ret = NULL;
        WOLFSSL_BIO* bio = NULL;

        WOLFSSL_ENTER("wolfSSL_PEM_read_X509_REQ");

        if (fp == XBADFILE) {
            WOLFSSL_MSG("Invalid file.");
            err = 1;
        }

        if (err == 0) {
            bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
            if (bio == NULL) {
                WOLFSSL_MSG("Failed to create new BIO with input file.");
                err = 1;
            }
        }
        if (err == 0 && wolfSSL_BIO_set_fp(bio, fp, BIO_CLOSE)
                != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Failed to set BIO file pointer.");
            err = 1;
        }
        if (err == 0) {
            ret = wolfSSL_PEM_read_bio_X509_REQ(bio, x, cb, u);
        }

        if (bio != NULL) {
            wolfSSL_BIO_free(bio);
        }

        return ret;
    }
#endif /* !NO_FILESYSTEM */
#endif /* WOLFSSL_CERT_REQ */

    WOLFSSL_X509_CRL *wolfSSL_PEM_read_bio_X509_CRL(WOLFSSL_BIO *bp,
            WOLFSSL_X509_CRL **x, wc_pem_password_cb *cb, void *u)
    {
#if defined(WOLFSSL_PEM_TO_DER) && defined(HAVE_CRL)
        unsigned char* pem = NULL;
        int pemSz;
        int derSz;
        DerBuffer* der = NULL;
        WOLFSSL_X509_CRL* crl = NULL;

        if ((pemSz = wolfSSL_BIO_get_len(bp)) <= 0) {
            goto err;
        }

        pem = (unsigned char*)XMALLOC(pemSz, 0, DYNAMIC_TYPE_PEM);
        if (pem == NULL) {
            goto err;
        }

        if (wolfSSL_BIO_read(bp, pem, pemSz) != pemSz) {
            goto err;
        }

        if((PemToDer(pem, pemSz, CRL_TYPE, &der, NULL, NULL, NULL)) < 0) {
            goto err;
        }
        derSz = der->length;
        if((crl = wolfSSL_d2i_X509_CRL(x, der->buffer, derSz)) == NULL) {
            goto err;
        }

err:
        if(pem != NULL) {
            XFREE(pem, 0, DYNAMIC_TYPE_PEM);
        }
        if(der != NULL) {
            FreeDer(&der);
        }

        (void)cb;
        (void)u;

        return crl;
#else
        (void)bp;
        (void)x;
        (void)cb;
        (void)u;

        return NULL;
#endif
    }

#endif /* !NO_BIO */

#if !defined(NO_FILESYSTEM)
    static void* wolfSSL_PEM_read_X509_ex(XFILE fp, void **x,
        wc_pem_password_cb *cb, void *u, int type)
    {
        unsigned char* pem = NULL;
        int pemSz;
        long i = 0, l;
        void *newx509;
        int derSz;
        DerBuffer* der = NULL;

        WOLFSSL_ENTER("wolfSSL_PEM_read_X509");

        if (fp == XBADFILE) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_X509", BAD_FUNC_ARG);
            return NULL;
        }
        /* Read cert from file */
        i = XFTELL(fp);
        if (i < 0) {
            WOLFSSL_LEAVE("wolfSSL_PEM_read_X509", BAD_FUNC_ARG);
            return NULL;
        }

        if (XFSEEK(fp, 0, XSEEK_END) != 0)
            return NULL;
        l = XFTELL(fp);
        if (l < 0)
            return NULL;
        if (XFSEEK(fp, i, SEEK_SET) != 0)
            return NULL;
        pemSz = (int)(l - i);

        /* check calculated length */
        if (pemSz > MAX_WOLFSSL_FILE_SIZE || pemSz < 0) {
            WOLFSSL_MSG("PEM_read_X509_ex file size error");
            return NULL;
        }

        /* allocate pem buffer */
        pem = (unsigned char*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_PEM);
        if (pem == NULL)
            return NULL;

        if ((int)XFREAD((char *)pem, 1, pemSz, fp) != pemSz)
            goto err_exit;

        switch (type) {
            case CERT_TYPE:
                newx509 = (void *)wolfSSL_X509_load_certificate_buffer(pem,
                    pemSz, WOLFSSL_FILETYPE_PEM);
                break;

        #ifdef HAVE_CRL
            case CRL_TYPE:
                if ((PemToDer(pem, pemSz, CRL_TYPE, &der, NULL, NULL, NULL)) < 0)
                    goto err_exit;
                derSz = der->length;
                newx509 = (void*)wolfSSL_d2i_X509_CRL((WOLFSSL_X509_CRL **)x,
                    (const unsigned char *)der->buffer, derSz);
                if (newx509 == NULL)
                    goto err_exit;
                FreeDer(&der);
                break;
        #endif

            default:
                goto err_exit;
        }
        if (x != NULL) {
            *x = newx509;
        }
        XFREE(pem, NULL, DYNAMIC_TYPE_PEM);
        return newx509;

    err_exit:
        if (pem != NULL)
            XFREE(pem, NULL, DYNAMIC_TYPE_PEM);
        if (der != NULL)
            FreeDer(&der);

        /* unused */
        (void)cb;
        (void)u;
        (void)derSz;

        return NULL;
    }

    WOLFSSL_API WOLFSSL_X509* wolfSSL_PEM_read_X509(XFILE fp, WOLFSSL_X509 **x,
                                                    wc_pem_password_cb *cb,
                                                    void *u)
    {
        return (WOLFSSL_X509* )wolfSSL_PEM_read_X509_ex(fp, (void **)x, cb, u, CERT_TYPE);
    }

#if defined(HAVE_CRL)
    WOLFSSL_API WOLFSSL_X509_CRL* wolfSSL_PEM_read_X509_CRL(XFILE fp, WOLFSSL_X509_CRL **crl,
                                                    wc_pem_password_cb *cb, void *u)
    {
        return (WOLFSSL_X509_CRL* )wolfSSL_PEM_read_X509_ex(fp, (void **)crl, cb, u, CRL_TYPE);
    }
#endif

#ifdef WOLFSSL_CERT_GEN
#ifndef NO_BIO
    int wolfSSL_PEM_write_X509(XFILE fp, WOLFSSL_X509* x)
    {
        int ret;
        WOLFSSL_BIO* bio;

        if (x == NULL)
            return 0;

        bio = wolfSSL_BIO_new(wolfSSL_BIO_s_file());
        if (bio == NULL)
            return 0;

        if (wolfSSL_BIO_set_fp(bio, fp, BIO_NOCLOSE) != WOLFSSL_SUCCESS) {
            wolfSSL_BIO_free(bio);
            bio = NULL;
        }

        ret = wolfSSL_PEM_write_bio_X509(bio, x);

        if (bio != NULL)
            wolfSSL_BIO_free(bio);

        return ret;
    }
#endif /* !NO_BIO */
#endif /* WOLFSSL_CERT_GEN */
#endif /* !NO_FILESYSTEM */

#endif /* OPENSSL_EXTRA || OPENSSL_ALL */
#ifdef OPENSSL_ALL

#ifndef NO_BIO
    /* create and return a new WOLFSSL_X509_PKEY structure or NULL on failure */
    static WOLFSSL_X509_PKEY* wolfSSL_X509_PKEY_new(void* heap)
    {
        WOLFSSL_X509_PKEY* ret;

        ret = (WOLFSSL_X509_PKEY*)XMALLOC(sizeof(WOLFSSL_X509_PKEY), heap,
            DYNAMIC_TYPE_KEY);
        if (ret != NULL) {
            XMEMSET(ret, 0, sizeof(WOLFSSL_X509_PKEY));
            ret->heap = heap;
        }
        return ret;
    }
#endif /* !NO_BIO */


    /* free up all memory used by "xPkey" passed in */
    static void wolfSSL_X509_PKEY_free(WOLFSSL_X509_PKEY* xPkey)
    {
        if (xPkey != NULL) {
            wolfSSL_EVP_PKEY_free(xPkey->dec_pkey);
            XFREE(xPkey, xPkey->heap, DYNAMIC_TYPE_KEY);
        }
    }


#ifndef NO_BIO

#define PEM_COMPARE_HEADER(start, end, header) \
        ((end) - (start) == XSTR_SIZEOF(header) && XMEMCMP(start, header, \
                XSTR_SIZEOF(header)) == 0)

    /**
     * This read one structure from bio and returns the read structure
     * in the appropriate output parameter (x509, crl, x_pkey). The
     * output parameters must be set to NULL.
     * @param bio    Input for reading structures
     * @param cb     Password callback
     * @param x509   Output
     * @param crl    Output
     * @param x_pkey Output
     * @return WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE otherwise
     */
    static int wolfSSL_PEM_X509_X509_CRL_X509_PKEY_read_bio(
            WOLFSSL_BIO* bio, wc_pem_password_cb* cb, WOLFSSL_X509** x509,
            WOLFSSL_X509_CRL** crl, WOLFSSL_X509_PKEY** x_pkey)
    {

#if defined(WOLFSSL_PEM_TO_DER) || defined(WOLFSSL_DER_TO_PEM)
        char* pem = NULL;
        long  i = pem_struct_min_sz, l;
        const char* header = NULL;
        const char* headerEnd = NULL;
        const char* footer = NULL;
        const char* footerEnd = NULL;
    #ifdef HAVE_CRL
        DerBuffer* der = NULL;
    #endif
        WOLFSSL_BIO* pemBio = NULL;

        if (!bio || !x509 || *x509 || !crl || *crl || !x_pkey || *x_pkey) {
            WOLFSSL_MSG("Bad input parameter or output parameters "
                        "not set to a NULL value.");
            return WOLFSSL_FAILURE;
        }

        if ((l = wolfSSL_BIO_get_len(bio)) <= 0) {
            /* No certificate in buffer */
            WOLFSSL_ERROR(ASN_NO_PEM_HEADER);
            return WOLFSSL_FAILURE;
        }

        pem = (char*)XMALLOC(l, 0, DYNAMIC_TYPE_PEM);
        if (pem == NULL)
            return WOLFSSL_FAILURE;

        if (wolfSSL_BIO_read(bio, &pem[0], pem_struct_min_sz) !=
                pem_struct_min_sz) {
            WOLFSSL_ERROR(ASN_NO_PEM_HEADER);
            goto err;
        }

        /* Read the header and footer */
        while (i < l && wolfSSL_BIO_read(bio, &pem[i], 1) == 1) {
            i++;
            if (!header) {
                header = XSTRNSTR(pem, "-----BEGIN ", (unsigned int)i);
            }
            else if (!headerEnd) {
                headerEnd = XSTRNSTR(header + XSTR_SIZEOF("-----BEGIN "),
                        "-----",
                        (unsigned int)
                        (i - (header + XSTR_SIZEOF("-----BEGIN ") - pem)));
                if (headerEnd) {
                    headerEnd += XSTR_SIZEOF("-----");
                    /* Read in the newline */
                    if (wolfSSL_BIO_read(bio, &pem[i], 1) != 1) {
                        WOLFSSL_MSG("wolfSSL_BIO_read error");
                        goto err;
                    }
                    i++;
                    if (*headerEnd != '\n' && *headerEnd != '\r') {
                        WOLFSSL_MSG("Missing newline after header");
                        goto err;
                    }
                }
            }
            else if (!footer) {
                footer = XSTRNSTR(headerEnd, "-----END ",
                        (unsigned int)(i - (headerEnd - pem)));
            }
            else if (!footerEnd) {
                footerEnd = XSTRNSTR(footer + XSTR_SIZEOF("-----"),
                        "-----", (unsigned int)(i -
                            (footer + XSTR_SIZEOF("-----") - pem)));
                if (footerEnd) {
                    footerEnd += XSTR_SIZEOF("-----");
                    /* Now check that footer matches header */
                    if ((headerEnd - (header + XSTR_SIZEOF("-----BEGIN "))) ==
                        (footerEnd - (footer + XSTR_SIZEOF("-----END "))) &&
                        XMEMCMP(header + XSTR_SIZEOF("-----BEGIN "),
                                footer + XSTR_SIZEOF("-----END "),
                        headerEnd - (header + XSTR_SIZEOF("-----BEGIN ")))
                            != 0) {
                        WOLFSSL_MSG("Header and footer don't match");
                        goto err;
                    }
                    /* header and footer match */
                    break;
                }
            }
        }
        if (!footerEnd) {
            /* Only check footerEnd since it is set last */
            WOLFSSL_ERROR(ASN_NO_PEM_HEADER);
            goto err;
        }
        else {
            if (PEM_COMPARE_HEADER(header, headerEnd,
                    "-----BEGIN CERTIFICATE-----")) {
                /* We have a certificate */
                WOLFSSL_MSG("Parsing x509 cert");
                *x509 = wolfSSL_X509_load_certificate_buffer(
                        (const unsigned char*) header,
                        (int)(footerEnd - header), WOLFSSL_FILETYPE_PEM);
                if (!*x509) {
                    WOLFSSL_MSG("wolfSSL_X509_load_certificate_buffer error");
                    goto err;
                }
            }
    #ifdef HAVE_CRL
            else if (PEM_COMPARE_HEADER(header, headerEnd,
                        "-----BEGIN X509 CRL-----")) {
                /* We have a crl */
                WOLFSSL_MSG("Parsing crl");
                if((PemToDer((const unsigned char*) header, footerEnd - header,
                        CRL_TYPE, &der, NULL, NULL, NULL)) < 0) {
                    WOLFSSL_MSG("PemToDer error");
                    goto err;
                }
                *crl = wolfSSL_d2i_X509_CRL(NULL, der->buffer, der->length);
                if (!*crl) {
                    WOLFSSL_MSG("wolfSSL_d2i_X509_CRL error");
                    goto err;
                }
            }
    #endif
            else {
                WOLFSSL_MSG("Parsing x509 key");

                if (!(*x_pkey = wolfSSL_X509_PKEY_new(NULL))) {
                    WOLFSSL_MSG("wolfSSL_X509_PKEY_new error");
                    goto err;
                }

                if (!(pemBio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()))) {
                    WOLFSSL_MSG("wolfSSL_BIO_new error");
                    goto err;
                }

                if (wolfSSL_BIO_write(pemBio, header,
                        (int)(footerEnd - header)) != footerEnd - header) {
                    WOLFSSL_MSG("wolfSSL_BIO_new error");
                    goto err;
                }

                if (wolfSSL_PEM_read_bio_PrivateKey(pemBio,
                        &(*x_pkey)->dec_pkey, cb, NULL) == NULL) {
                    WOLFSSL_MSG("wolfSSL_PEM_read_bio_PrivateKey error");
                    goto err;
                }

                wolfSSL_BIO_free(pemBio);
            }
        }

        XFREE(pem, 0, DYNAMIC_TYPE_PEM);
    #ifdef HAVE_CRL
        if (der)
            FreeDer(&der);
    #endif
        return WOLFSSL_SUCCESS;
err:
        if (pem)
            XFREE(pem, 0, DYNAMIC_TYPE_PEM);
    #ifdef HAVE_CRL
        if (der)
            FreeDer(&der);
    #endif
        if (*x_pkey) {
            wolfSSL_X509_PKEY_free(*x_pkey);
            *x_pkey = NULL;
        }
        if (pemBio)
            wolfSSL_BIO_free(pemBio);
        return WOLFSSL_FAILURE;
#else /* ! (WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM) */
        return WOLFSSL_FAILURE;
#endif /* WOLFSSL_PEM_TO_DER || WOLFSSL_DER_TO_PEM */
    }

#ifndef NO_FILESYSTEM
    WOLF_STACK_OF(WOLFSSL_X509_INFO)* wolfSSL_PEM_X509_INFO_read(
            XFILE fp, WOLF_STACK_OF(WOLFSSL_X509_INFO)* sk,
            pem_password_cb* cb, void* u)
    {
        WOLFSSL_BIO* fileBio = wolfSSL_BIO_new_fp(fp, BIO_NOCLOSE);
        WOLF_STACK_OF(WOLFSSL_X509_INFO)* ret = NULL;

        WOLFSSL_ENTER("wolfSSL_PEM_X509_INFO_read");
        if (fileBio != NULL) {
            ret = wolfSSL_PEM_X509_INFO_read_bio(fileBio, sk, cb, u);
            wolfSSL_BIO_free(fileBio);
        }
        return ret;
    }
#endif /* !NO_FILESYSTEM */

    /*
     * bio WOLFSSL_BIO to read certificates from
     * sk  possible stack to push more X509_INFO structs to. Can be NULL
     * cb  callback password for encrypted PEM certificates
     * u   user input such as password
     *
     * returns stack on success and NULL or default stack passed in on fail
     */
    WOLF_STACK_OF(WOLFSSL_X509_INFO)* wolfSSL_PEM_X509_INFO_read_bio(
        WOLFSSL_BIO* bio, WOLF_STACK_OF(WOLFSSL_X509_INFO)* sk,
        wc_pem_password_cb* cb, void* u)
    {
        WOLF_STACK_OF(WOLFSSL_X509_INFO)* localSk = NULL;
        int ret = WOLFSSL_SUCCESS;
        WOLFSSL_X509_INFO* current = NULL;
        WOLFSSL_X509*      x509 = NULL;
        WOLFSSL_X509_CRL*  crl  = NULL;
        WOLFSSL_X509_PKEY* x_pkey = NULL;

        (void)u;

        WOLFSSL_ENTER("wolfSSL_PEM_X509_INFO_read_bio");

        /* attempt to use passed in stack or create a new one */
        if (sk != NULL) {
            localSk = sk;
        }
        else {
            localSk = wolfSSL_sk_X509_INFO_new_null();
        }
        if (localSk == NULL) {
            WOLFSSL_LEAVE("wolfSSL_PEM_X509_INFO_read_bio",
                    MEMORY_E);
            return NULL;
        }

        /* parse through BIO and push new info's found onto stack */
        while (1) {
            x509 = NULL;
            crl  = NULL;
            x_pkey = NULL;

            if (wolfSSL_PEM_X509_X509_CRL_X509_PKEY_read_bio(bio, cb,
                    &x509, &crl, &x_pkey) == WOLFSSL_SUCCESS) {
                if (current == NULL ||
                        (x509 && current->x509) ||
                        (crl && current->crl) ||
                        (x_pkey && current->x_pkey)) {
                    /* Need to create new current since existing one already
                     * has the member filled or this is the first successful
                     * read. */
                    current = wolfSSL_X509_INFO_new();
                    if (current == NULL) {
                        ret = MEMORY_E;
                        break;
                    }
                    if (wolfSSL_sk_X509_INFO_push(localSk, current) !=
                            WOLFSSL_SUCCESS) {
                        wolfSSL_X509_INFO_free(current);
                        current = NULL;
                        ret = WOLFSSL_FAILURE;
                        break;
                    }
                }

                if (x509) {
                    current->x509 = x509;
                }
                else if (crl) {
                    current->crl = crl;
                }
                else if (x_pkey) {
                    current->x_pkey = x_pkey;
                }
                else {
                    WOLFSSL_MSG("No output parameters set");
                    ret = WOLFSSL_FAILURE;
                    break;
                }
            }
            else {
#ifdef WOLFSSL_HAVE_ERROR_QUEUE
                unsigned long err;
                CLEAR_ASN_NO_PEM_HEADER_ERROR(err);
                if (ERR_GET_LIB(err) != ERR_LIB_PEM ||
                    ERR_GET_REASON(err) != PEM_R_NO_START_LINE) {
                    ret = WOLFSSL_FAILURE;
                }
#else
                if (wolfSSL_sk_X509_INFO_num(localSk) > 0) {
                    WOLFSSL_MSG("At least one X509_INFO object on stack."
                                "Assuming error means EOF or no more PEM"
                                "headers found.");
                }
                else {
                    ret = WOLFSSL_FAILURE;
                }
#endif
                break;
            }
        }
        if (ret != WOLFSSL_SUCCESS ||
                wolfSSL_sk_X509_INFO_num(localSk) == 0) {
            /* current should always be pushed onto the localsk stack at this
             * point. The only case when it isn't is when
             * wolfSSL_sk_X509_INFO_push fails but in that case the current
             * free is handled inside the loop. */
            if (localSk != sk) {
                wolfSSL_sk_pop_free(localSk, NULL);
            }
            wolfSSL_X509_free(x509);
#ifdef HAVE_CRL
            wolfSSL_X509_CRL_free(crl);
#endif
            wolfSSL_X509_PKEY_free(x_pkey);
            localSk = NULL;
        }
        WOLFSSL_LEAVE("wolfSSL_PEM_X509_INFO_read_bio", ret);
        return localSk;
    }
#endif /* !NO_BIO */
#endif /* OPENSSL_ALL */

    void wolfSSL_X509_NAME_ENTRY_free(WOLFSSL_X509_NAME_ENTRY* ne)
    {
        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_free");
        if (ne != NULL) {
            wolfSSL_ASN1_OBJECT_free(ne->object);
            if (ne->value != NULL) {
                wolfSSL_ASN1_STRING_free(ne->value);
            }
            XFREE(ne, NULL, DYNAMIC_TYPE_NAME_ENTRY);
        }
    }


    WOLFSSL_X509_NAME_ENTRY* wolfSSL_X509_NAME_ENTRY_new(void)
    {
        WOLFSSL_X509_NAME_ENTRY* ne;

        ne = (WOLFSSL_X509_NAME_ENTRY*)XMALLOC(sizeof(WOLFSSL_X509_NAME_ENTRY),
                NULL, DYNAMIC_TYPE_NAME_ENTRY);
        if (ne != NULL) {
            XMEMSET(ne, 0, sizeof(WOLFSSL_X509_NAME_ENTRY));
        }

        return ne;
    }

    static void wolfssl_x509_name_entry_set(WOLFSSL_X509_NAME_ENTRY* ne,
        int nid, int type, const unsigned char *data, int dataSz)
    {
        WOLFSSL_ASN1_OBJECT* object;

        ne->nid = nid;
        /* Reuse the object if already available. */
        object = wolfSSL_OBJ_nid2obj_ex(nid, ne->object);
        if (object != NULL) {
            /* Set the object when no error. */
            ne->object = object;
        }
        ne->value = wolfSSL_ASN1_STRING_type_new(type);
        if (ne->value != NULL) {
            if (wolfSSL_ASN1_STRING_set(ne->value, (const void*)data,
                                            dataSz) == WOLFSSL_SUCCESS) {
                ne->set = 1;
            }
            else {
                /* Free the ASN1_STRING if it is not set. */
                wolfSSL_ASN1_STRING_free(ne->value);
                ne->value = NULL;
            }
        }
    }

    /* Create a new WOLFSSL_X509_NAME_ENTRY structure based on the text passed
     * in. Returns NULL on failure */
    WOLFSSL_X509_NAME_ENTRY* wolfSSL_X509_NAME_ENTRY_create_by_txt(
            WOLFSSL_X509_NAME_ENTRY **neIn, const char *txt, int type,
            const unsigned char *data, int dataSz)
    {
        int nid = -1;
        WOLFSSL_X509_NAME_ENTRY* ne = NULL;

        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_create_by_txt");

        if (txt == NULL) {
            return NULL;
        }

        if (neIn != NULL) {
            ne = *neIn;
        }

        nid = wolfSSL_OBJ_txt2nid(txt);
        if (nid == NID_undef) {
            WOLFSSL_MSG("Unable to find text");
            ne = NULL;
        }
        else {
            if (ne == NULL) {
                ne = wolfSSL_X509_NAME_ENTRY_new();
                if (ne == NULL) {
                    return NULL;
                }
            }

            wolfssl_x509_name_entry_set(ne, nid, type, data, dataSz);
        }

        return ne;
    }


    /* Creates a new entry given the NID, type, and data
     * "dataSz" is number of bytes in data, if set to -1 then XSTRLEN is used
     * "out" can be used to store the new entry data in an existing structure
     *       if NULL then a new WOLFSSL_X509_NAME_ENTRY structure is created
     * returns a pointer to WOLFSSL_X509_NAME_ENTRY on success and NULL on fail
     */
    WOLFSSL_X509_NAME_ENTRY* wolfSSL_X509_NAME_ENTRY_create_by_NID(
            WOLFSSL_X509_NAME_ENTRY** out, int nid, int type,
            const unsigned char* data, int dataSz)
    {
        WOLFSSL_X509_NAME_ENTRY* ne;

#ifdef WOLFSSL_DEBUG_OPENSSL
        WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_create_by_NID");
#endif

        if (!data) {
            WOLFSSL_MSG("Bad parameter");
            return NULL;
        }

        if (out == NULL || *out == NULL) {
            ne = wolfSSL_X509_NAME_ENTRY_new();
            if (ne == NULL) {
                return NULL;
            }
            if (out != NULL) {
                *out = ne;
            }
        }
        else {
            ne = *out;
        }

        wolfssl_x509_name_entry_set(ne, nid, type, data, dataSz);

        return ne;
    }
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
    defined(HAVE_LIGHTY) || defined(WOLFSSL_MYSQL_COMPATIBLE) || \
    defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
    defined(HAVE_POCO_LIB) || defined(WOLFSSL_HAPROXY)
WOLFSSL_ASN1_OBJECT* wolfSSL_X509_NAME_ENTRY_get_object(
    WOLFSSL_X509_NAME_ENTRY *ne)
{
    WOLFSSL_ASN1_OBJECT* object = NULL;

#ifdef WOLFSSL_DEBUG_OPENSSL
    WOLFSSL_ENTER("wolfSSL_X509_NAME_ENTRY_get_object");
#endif

    if (ne != NULL) {
        /* Create object from nid - reuse existing object if possible. */
        object = wolfSSL_OBJ_nid2obj_ex(ne->nid, ne->object);
        if (object != NULL) {
            /* Set the object when no error. */
            ne->object = object;
        }
    }

    return object;
}
#endif /* OPENSSL_ALL || HAVE_LIGHTY || WOLFSSL_MYSQL_COMPATIBLE ||
        * HAVE_STUNNEL || WOLFSSL_NGINX || HAVE_POCO_LIB || WOLFSSL_HAPROXY */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
    /* add all entry of type "nid" to the buffer "fullName" and advance "idx"
     * since number of entries is small, a brute force search is used here
     * returns the number of entries added
     */
    static int AddAllEntry(WOLFSSL_X509_NAME* name, char* fullName,
            int fullNameSz, int* idx)
    {
        int i;
        int ret = 0;

        for (i = 0; i < MAX_NAME_ENTRIES; i++) {
            if (name->entry[i].set) {
                WOLFSSL_X509_NAME_ENTRY* e;
                WOLFSSL_ASN1_OBJECT* obj;

                int sz;
                unsigned char* data;

                e = &name->entry[i];
                obj = wolfSSL_X509_NAME_ENTRY_get_object(e);
                if (obj == NULL) {
                    return BAD_FUNC_ARG;
                }

                XMEMCPY(fullName + *idx, "/", 1); *idx = *idx + 1;
                sz = (int)XSTRLEN(obj->sName);
                XMEMCPY(fullName + *idx, obj->sName, sz);
                *idx += sz;
                XMEMCPY(fullName + *idx, "=", 1); *idx = *idx + 1;

                data = wolfSSL_ASN1_STRING_data(e->value);
                if (data != NULL) {
                    sz = (int)XSTRLEN((const char*)data);
                    XMEMCPY(fullName + *idx, data, sz);
                    *idx += sz;
                }

                ret++;
            }
        }
        (void)fullNameSz;
        return ret;
    }


    /* Converts a list of entries in WOLFSSL_X509_NAME struct into a string
     * returns 0 on success */
    static int RebuildFullName(WOLFSSL_X509_NAME* name)
    {
        int totalLen = 0, i, idx, entryCount = 0;
        char* fullName;

        if (name == NULL)
            return BAD_FUNC_ARG;

        for (i = 0; i < MAX_NAME_ENTRIES; i++) {
            if (name->entry[i].set) {
                WOLFSSL_X509_NAME_ENTRY* e;
                WOLFSSL_ASN1_OBJECT* obj;

                e = &name->entry[i];
                obj = wolfSSL_X509_NAME_ENTRY_get_object(e);
                if (obj == NULL)
                    return BAD_FUNC_ARG;

                totalLen += (int)XSTRLEN(obj->sName) + 2;/*+2 for '/' and '=' */
                totalLen += wolfSSL_ASN1_STRING_length(e->value);
            }
        }

        fullName = (char*)XMALLOC(totalLen + 1, name->heap, DYNAMIC_TYPE_X509);
        if (fullName == NULL)
            return MEMORY_E;

        idx = 0;
        entryCount = AddAllEntry(name, fullName, totalLen, &idx);
        if (entryCount < 0) {
            XFREE(fullName, name->heap, DYNAMIC_TYPE_X509);
            return entryCount;
        }

        if (name->dynamicName) {
            XFREE(name->name, name->heap, DYNAMIC_TYPE_X509);
        }
        fullName[idx] = '\0';
        name->name = fullName;
        name->dynamicName = 1;
        name->sz = idx + 1; /* size includes null terminator */
        name->entrySz = entryCount;

        return 0;
    }

    /* Copies entry into name. With it being copied freeing entry becomes the
     * callers responsibility.
     * returns 1 for success and 0 for error */
    int wolfSSL_X509_NAME_add_entry(WOLFSSL_X509_NAME* name,
            WOLFSSL_X509_NAME_ENTRY* entry, int idx, int set)
    {
        WOLFSSL_X509_NAME_ENTRY* current = NULL;
        int ret, i;

#ifdef WOLFSSL_DEBUG_OPENSSL
        WOLFSSL_ENTER("wolfSSL_X509_NAME_add_entry");
#endif

        if (name == NULL || entry == NULL || entry->value == NULL) {
            WOLFSSL_MSG("NULL argument passed in");
            return WOLFSSL_FAILURE;
        }

        if (idx >= 0) {
            /* place in specific index */

            if (idx >= MAX_NAME_ENTRIES) {
                WOLFSSL_MSG("Error index to insert entry is larger than array");
                return WOLFSSL_FAILURE;
            }
            i = idx;
        }
        else {
            /* iterate through and find first open spot */
            for (i = 0; i < MAX_NAME_ENTRIES; i++) {
                if (name->entry[i].set != 1) { /* not set so overwritten */
                    WOLFSSL_MSG("Found place for name entry");
                    break;
                }
            }

            if (i == MAX_NAME_ENTRIES) {
                WOLFSSL_MSG("No spot found for name entry");
                return WOLFSSL_FAILURE;
            }
        }

        current = &name->entry[i];
        if (current->set == 0)
            name->entrySz++;

        if (wolfSSL_X509_NAME_ENTRY_create_by_NID(&current,
                            entry->nid,
                            wolfSSL_ASN1_STRING_type(entry->value),
                            wolfSSL_ASN1_STRING_data(entry->value),
                            wolfSSL_ASN1_STRING_length(entry->value)) != NULL)
        {
            ret = WOLFSSL_SUCCESS;
        #ifdef OPENSSL_ALL
            if (name->entries == NULL) {
                name->entries = wolfSSL_sk_X509_NAME_new(NULL);
            }
            if (wolfSSL_sk_X509_NAME_ENTRY_push(name->entries, current
                                                         ) != WOLFSSL_SUCCESS) {
                ret = WOLFSSL_FAILURE;
            }
        #endif
        }
        else {
            ret = WOLFSSL_FAILURE;
        }

        if (ret != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("Error adding the name entry");
            if (current->set == 0)
                name->entrySz--;
            return WOLFSSL_FAILURE;
        }

        if (RebuildFullName(name) != 0)
            return WOLFSSL_FAILURE;

        (void)set;
        return WOLFSSL_SUCCESS;
    }

    int wolfSSL_X509_NAME_add_entry_by_txt(WOLFSSL_X509_NAME *name,
                                           const char *field, int type,
                                           const unsigned char *bytes, int len,
                                           int loc, int set)
    {
        int ret = WOLFSSL_FAILURE;
        int nid;
        WOLFSSL_X509_NAME_ENTRY* entry;

        (void)type;
        WOLFSSL_ENTER("wolfSSL_X509_NAME_add_entry_by_txt");

        if (name == NULL || field == NULL)
            return WOLFSSL_FAILURE;

        if ((nid = wolfSSL_OBJ_txt2nid(field)) == NID_undef) {
            WOLFSSL_MSG("Unable convert text to NID");
            return WOLFSSL_FAILURE;
        }

        entry = wolfSSL_X509_NAME_ENTRY_create_by_NID(NULL,
                  nid, type, (unsigned char*)bytes, len);
        if (entry == NULL)
            return WOLFSSL_FAILURE;

        ret = wolfSSL_X509_NAME_add_entry(name, entry, loc, set);
        wolfSSL_X509_NAME_ENTRY_free(entry);

        return ret;
    }

    int wolfSSL_X509_NAME_add_entry_by_NID(WOLFSSL_X509_NAME *name, int nid,
                                           int type, const unsigned char *bytes,
                                           int len, int loc, int set)
    {
        int ret;
        WOLFSSL_X509_NAME_ENTRY* entry;
        WOLFSSL_ENTER("wolfSSL_X509_NAME_add_entry_by_NID");
        entry = wolfSSL_X509_NAME_ENTRY_create_by_NID(NULL, nid, type, bytes,
                len);
        if (entry == NULL)
            return WOLFSSL_FAILURE;
        ret = wolfSSL_X509_NAME_add_entry(name, entry, loc, set);
        wolfSSL_X509_NAME_ENTRY_free(entry);
        return ret;
    }

    WOLFSSL_X509_NAME_ENTRY *wolfSSL_X509_NAME_delete_entry(
            WOLFSSL_X509_NAME *name, int loc)
    {
        WOLFSSL_X509_NAME_ENTRY* ret;
        WOLFSSL_ENTER("wolfSSL_X509_NAME_delete_entry");

        if (!name) {
            WOLFSSL_MSG("Bad parameter");
            return NULL;
        }

        ret = wolfSSL_X509_NAME_get_entry(name, loc);
        if (!ret) {
            WOLFSSL_MSG("loc entry not found");
            return NULL;
        }
        name->entry[loc].set = 0;
        return ret;
    }

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA) && !defined(NO_ASN)
    int wolfSSL_X509_NAME_get_index_by_OBJ(WOLFSSL_X509_NAME *name,
                                           const WOLFSSL_ASN1_OBJECT *obj,
                                           int idx) {
        if (!name || idx >= MAX_NAME_ENTRIES ||
                !obj || !obj->obj) {
            return -1;
        }

        if (idx < 0) {
            idx = -1;
        }

        for (idx++; idx < MAX_NAME_ENTRIES; idx++) {
            /* Find index of desired name */
            if (name->entry[idx].set) {
                if (XSTRLEN(obj->sName) == XSTRLEN(name->entry[idx].object->sName) &&
                    XSTRNCMP((const char*) obj->sName,
                        name->entry[idx].object->sName, obj->objSz - 1) == 0) {
                    return idx;
                }
            }
        }
        return -1;
    }
#endif

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL) || \
    defined(OPENSSL_EXTRA_X509_SMALL)

    /* returns a pointer to the internal entry at location 'loc' on success,
     * a null pointer is returned in fail cases */
    WOLFSSL_X509_NAME_ENTRY *wolfSSL_X509_NAME_get_entry(
                                             WOLFSSL_X509_NAME *name, int loc)
    {
#ifdef WOLFSSL_DEBUG_OPENSSL
        WOLFSSL_ENTER("wolfSSL_X509_NAME_get_entry");
#endif

        if (name == NULL) {
            return NULL;
        }

        if (loc < 0 || loc >= MAX_NAME_ENTRIES) {
            WOLFSSL_MSG("Bad argument");
            return NULL;
        }

        if (name->entry[loc].set) {
#ifdef WOLFSSL_PYTHON
            /* "set" is not only flag use, but also stack index position use in
            *  OpenSSL. Python makes tuple based on this number. Therefore,
            *  updating "set" by position + 1. "plus 1" means to avoid "not set"
            *  zero.
            */
            name->entry[loc].set = loc + 1;
#endif
            return &name->entry[loc];
        }
        else {
            return NULL;
        }
    }
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#ifdef OPENSSL_EXTRA

    int wolfSSL_X509_check_private_key(WOLFSSL_X509 *x509, WOLFSSL_EVP_PKEY *key)
    {
        WOLFSSL_ENTER("wolfSSL_X509_check_private_key");

        if (!x509 || !key) {
            WOLFSSL_MSG("Bad parameter");
            return WOLFSSL_FAILURE;
        }

    #ifndef NO_CHECK_PRIVATE_KEY
        return wc_CheckPrivateKey((byte*)key->pkey.ptr, key->pkey_sz,
                x509->pubKey.buffer, x509->pubKey.length,
                (enum Key_Sum)x509->pubKeyOID) == 1 ?
                        WOLFSSL_SUCCESS : WOLFSSL_FAILURE;
    #else
        /* not compiled in */
        return WOLFSSL_SUCCESS;
    #endif
    }

#endif /* OPENSSL_EXTRA */

#if defined(HAVE_LIGHTY) || defined(HAVE_STUNNEL) \
    || defined(WOLFSSL_MYSQL_COMPATIBLE) || defined(OPENSSL_EXTRA)
#ifndef NO_BIO

#ifdef WOLFSSL_CERT_GEN

#ifdef WOLFSSL_CERT_REQ
/* writes the x509 from x to the WOLFSSL_BIO bp
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on fail
 */
int wolfSSL_PEM_write_bio_X509_REQ(WOLFSSL_BIO *bp, WOLFSSL_X509 *x)
{
    byte* pem;
    int   pemSz = 0;
    const unsigned char* der;
    int derSz;
    int ret;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_X509_REQ");

    if (x == NULL || bp == NULL) {
        return WOLFSSL_FAILURE;
    }

    der = wolfSSL_X509_get_der(x, &derSz);
    if (der == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* get PEM size */
    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, CERTREQ_TYPE);
    if (pemSz < 0) {
        return WOLFSSL_FAILURE;
    }

    /* create PEM buffer and convert from DER */
    pem = (byte*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pem == NULL) {
        return WOLFSSL_FAILURE;
    }
    if (wc_DerToPemEx(der, derSz, pem, pemSz, NULL, CERTREQ_TYPE) < 0) {
        XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    /* write the PEM to BIO */
    ret = wolfSSL_BIO_write(bp, pem, pemSz);
    XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (ret <= 0) return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}
#endif /* WOLFSSL_CERT_REQ */


/* writes the x509 from x to the WOLFSSL_BIO bp
 *
 * returns WOLFSSL_SUCCESS on success and WOLFSSL_FAILURE on fail
 */
int wolfSSL_PEM_write_bio_X509_AUX(WOLFSSL_BIO *bp, WOLFSSL_X509 *x)
{
    byte* pem;
    int   pemSz = 0;
    const unsigned char* der;
    int derSz;
    int ret;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_X509_AUX");

    if (bp == NULL || x == NULL) {
        WOLFSSL_MSG("NULL argument passed in");
        return WOLFSSL_FAILURE;
    }

    der = wolfSSL_X509_get_der(x, &derSz);
    if (der == NULL) {
        return WOLFSSL_FAILURE;
    }

    /* get PEM size */
    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, CERT_TYPE);
    if (pemSz < 0) {
        return WOLFSSL_FAILURE;
    }

    /* create PEM buffer and convert from DER */
    pem = (byte*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pem == NULL) {
        return WOLFSSL_FAILURE;
    }
    if (wc_DerToPemEx(der, derSz, pem, pemSz, NULL, CERT_TYPE) < 0) {
        XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        return WOLFSSL_FAILURE;
    }

    /* write the PEM to BIO */
    ret = wolfSSL_BIO_write(bp, pem, pemSz);
    XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (ret <= 0) return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}

int wolfSSL_PEM_write_bio_X509(WOLFSSL_BIO *bio, WOLFSSL_X509 *cert)
{
    byte* pem = NULL;
    int   pemSz = 0;
    /* Get large buffer to hold cert der */
    const byte* der = NULL;
    int derSz = X509_BUFFER_SZ;
    int ret;

    WOLFSSL_ENTER("wolfSSL_PEM_write_bio_X509");

    if (bio == NULL || cert == NULL) {
        WOLFSSL_MSG("NULL argument passed in");
        return WOLFSSL_FAILURE;
    }

    /* Do not call wolfssl_x509_make_der() here. If we did, then need to re-sign
     * because we don't know the original order of the extensions and so we must
     * assume our extensions are in a different order, thus need to re-sign. */
    der = wolfSSL_X509_get_der(cert, &derSz);
    if (der == NULL) {
        goto error;
    }

    /* get PEM size */
    pemSz = wc_DerToPemEx(der, derSz, NULL, 0, NULL, CERT_TYPE);
    if (pemSz < 0) {
        goto error;
    }

    /* create PEM buffer and convert from DER */
    pem = (byte*)XMALLOC(pemSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (pem == NULL) {
        goto error;
    }
    if (wc_DerToPemEx(der, derSz, pem, pemSz, NULL, CERT_TYPE) < 0) {
        goto error;
    }

    /* write the PEM to BIO */
    ret = wolfSSL_BIO_write(bio, pem, pemSz);
    XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);

    if (ret <= 0) return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;

error:
    if (pem)
        XFREE(pem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    return WOLFSSL_FAILURE;
}
#endif /* WOLFSSL_CERT_GEN */

#endif /* !NO_BIO */
#endif /* HAVE_LIGHTY || HAVE_STUNNEL || WOLFSSL_MYSQL_COMPATIBLE */

#if defined(OPENSSL_EXTRA) || defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
        defined(HAVE_LIGHTY) || defined(WOLFSSL_HAPROXY) || \
        defined(WOLFSSL_OPENSSH) || defined(HAVE_SBLIM_SFCB)

WOLF_STACK_OF(WOLFSSL_X509_NAME)* wolfSSL_sk_X509_NAME_new(
        WOLF_SK_COMPARE_CB(WOLFSSL_X509_NAME, cb))
{
    WOLFSSL_STACK* sk;
    (void)cb;

    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_new");

    sk = wolfSSL_sk_new_node(NULL);
    if (sk != NULL) {
        sk->type = STACK_TYPE_X509_NAME;
    }

    return sk;
}

int wolfSSL_sk_X509_NAME_num(const WOLF_STACK_OF(WOLFSSL_X509_NAME) *sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_num");

    if (sk == NULL)
        return BAD_FUNC_ARG;

    return (int)sk->num;
}

/* Getter function for WOLFSSL_X509_NAME pointer
 *
 * sk is the stack to retrieve pointer from
 * i  is the index value in stack
 *
 * returns a pointer to a WOLFSSL_X509_NAME structure on success and NULL on
 *         fail
 */
WOLFSSL_X509_NAME* wolfSSL_sk_X509_NAME_value(const STACK_OF(WOLFSSL_X509_NAME)* sk,
    int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_value");
    return (WOLFSSL_X509_NAME*)wolfSSL_sk_value(sk, i);
}

WOLFSSL_X509_NAME* wolfSSL_sk_X509_NAME_pop(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk)
{
    WOLFSSL_STACK* node;
    WOLFSSL_X509_NAME* name;

    if (sk == NULL) {
        return NULL;
    }

    node = sk->next;
    name = sk->data.name;

    if (node != NULL) { /* update sk and remove node from stack */
        sk->data.name = node->data.name;
        sk->next = node->next;
        XFREE(node, NULL, DYNAMIC_TYPE_OPENSSL);
    }
    else { /* last x509 in stack */
        sk->data.name = NULL;
    }

    if (sk->num > 0) {
        sk->num -= 1;
    }

    return name;
}

void wolfSSL_sk_X509_NAME_pop_free(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk,
    void (*f) (WOLFSSL_X509_NAME*))
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

/* Free only the sk structure, NOT X509_NAME members */
void wolfSSL_sk_X509_NAME_free(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_free");
    wolfSSL_sk_free(sk);
}

int wolfSSL_sk_X509_NAME_push(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk,
    WOLFSSL_X509_NAME* name)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_push");

    return wolfSSL_sk_push(sk, name);
}

/* return index of found, or negative to indicate not found */
int wolfSSL_sk_X509_NAME_find(const WOLF_STACK_OF(WOLFSSL_X509_NAME) *sk,
    WOLFSSL_X509_NAME *name)
{
    int i;

    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_find");

    if (sk == NULL)
        return BAD_FUNC_ARG;

    for (i = 0; sk; i++, sk = sk->next) {
        if (wolfSSL_X509_NAME_cmp(sk->data.name, name) == 0) {
            return i;
        }
    }
    return -1;
}

/* Name Entry */
WOLF_STACK_OF(WOLFSSL_X509_NAME_ENTRY)* wolfSSL_sk_X509_NAME_ENTRY_new(
    WOLF_SK_COMPARE_CB(WOLFSSL_X509_NAME_ENTRY, cb))
{
    WOLFSSL_STACK* sk = wolfSSL_sk_new_node(NULL);
    if (sk != NULL) {
        sk->type = STACK_TYPE_X509_NAME_ENTRY;
        (void)cb;
    }
    return sk;
}

int wolfSSL_sk_X509_NAME_ENTRY_push(WOLF_STACK_OF(WOLFSSL_X509_NAME_ENTRY)* sk,
    WOLFSSL_X509_NAME_ENTRY* name_entry)
{
    return wolfSSL_sk_push(sk, name_entry);
}

WOLFSSL_X509_NAME_ENTRY* wolfSSL_sk_X509_NAME_ENTRY_value(
    const WOLF_STACK_OF(WOLFSSL_X509_NAME_ENTRY)* sk, int i)
{
    return (WOLFSSL_X509_NAME_ENTRY*)wolfSSL_sk_value(sk, i);
}

int wolfSSL_sk_X509_NAME_ENTRY_num(const WOLF_STACK_OF(WOLFSSL_X509_NAME_ENTRY)* sk)
{
    if (sk == NULL)
        return BAD_FUNC_ARG;
    return (int)sk->num;
}

void wolfSSL_sk_X509_NAME_ENTRY_free(WOLF_STACK_OF(WOLFSSL_X509_NAME_ENTRY)* sk)
{
    wolfSSL_sk_free(sk);
}

#endif /* OPENSSL_EXTRA || HAVE_STUNNEL || WOLFSSL_NGINX ||
            HAVE_LIGHTY || WOLFSSL_HAPROXY ||
            WOLFSSL_OPENSSH || HAVE_SBLIM_SFCB */

#if defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA) || \
    (defined(HAVE_STUNNEL) || defined(WOLFSSL_NGINX) || \
    defined(HAVE_LIGHTY) || defined(WOLFSSL_HAPROXY) || \
    defined(WOLFSSL_OPENSSH) || defined(HAVE_SBLIM_SFCB))

#if defined(OPENSSL_ALL)
WOLFSSL_X509_INFO* wolfSSL_X509_INFO_new(void)
{
    WOLFSSL_X509_INFO* info;
    info = (WOLFSSL_X509_INFO*)XMALLOC(sizeof(WOLFSSL_X509_INFO), NULL,
        DYNAMIC_TYPE_X509);
    if (info) {
        XMEMSET(info, 0, sizeof(*info));
    }
    return info;
}

void wolfSSL_X509_INFO_free(WOLFSSL_X509_INFO* info)
{
    if (info == NULL)
        return;

    if (info->x509) {
        wolfSSL_X509_free(info->x509);
        info->x509 = NULL;
    }
#ifdef HAVE_CRL
    if (info->crl) {
        wolfSSL_X509_CRL_free(info->crl);
        info->crl = NULL;
    }
#endif
    wolfSSL_X509_PKEY_free(info->x_pkey);
    info->x_pkey = NULL;

    XFREE(info, NULL, DYNAMIC_TYPE_X509);
}
#endif

WOLFSSL_STACK* wolfSSL_sk_X509_INFO_new_null(void)
{
    WOLFSSL_STACK* sk = wolfSSL_sk_new_node(NULL);
    if (sk) {
        sk->type = STACK_TYPE_X509_INFO;
    }
    return sk;
}

int wolfSSL_sk_X509_INFO_num(const WOLF_STACK_OF(WOLFSSL_X509_INFO) *sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_INFO_num");

    return wolfSSL_sk_num(sk);
}

WOLFSSL_X509_INFO* wolfSSL_sk_X509_INFO_value(
        const WOLF_STACK_OF(WOLFSSL_X509_INFO) *sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_INFO_value");

    return (WOLFSSL_X509_INFO *)wolfSSL_sk_value(sk, i);
}

WOLFSSL_X509_INFO* wolfSSL_sk_X509_INFO_pop(
        WOLF_STACK_OF(WOLFSSL_X509_INFO)* sk)
{
    WOLFSSL_STACK* node;
    WOLFSSL_X509_INFO* info;

    if (sk == NULL) {
        return NULL;
    }

    node = sk->next;
    info = sk->data.info;

    if (node != NULL) { /* update sk and remove node from stack */
        sk->data.info = node->data.info;
        sk->next = node->next;
        wolfSSL_sk_free_node(node);
    }
    else { /* last x509 in stack */
        sk->data.info = NULL;
    }

    if (sk->num > 0) {
        sk->num -= 1;
    }

    return info;
}

#if defined(OPENSSL_ALL)
void wolfSSL_sk_X509_INFO_pop_free(WOLF_STACK_OF(WOLFSSL_X509_INFO)* sk,
    void (*f) (WOLFSSL_X509_INFO*))
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_INFO_pop_free");
    wolfSSL_sk_pop_free(sk, (wolfSSL_sk_freefunc)f);
}

void wolfSSL_sk_X509_INFO_free(WOLF_STACK_OF(WOLFSSL_X509_INFO) *sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_INFO_free");
    wolfSSL_sk_free(sk);
}

/* Adds the WOLFSSL_X509_INFO to the stack "sk". "sk" takes control of "in" and
 * tries to free it when the stack is free'd.
 *
 * return 1 on success 0 on fail
 */
int wolfSSL_sk_X509_INFO_push(WOLF_STACK_OF(WOLFSSL_X509_INFO)* sk,
                                                      WOLFSSL_X509_INFO* in)
{
    return wolfSSL_sk_push(sk, in);
}

/* Creates a duplicate of WOLF_STACK_OF(WOLFSSL_X509_NAME).
 * Returns a new WOLF_STACK_OF(WOLFSSL_X509_NAME) or NULL on failure */
WOLF_STACK_OF(WOLFSSL_X509_NAME) *wolfSSL_dup_CA_list(
    WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk)
{
    int i;
    const int num = wolfSSL_sk_X509_NAME_num(sk);
    WOLF_STACK_OF(WOLFSSL_X509_NAME) *copy;
    WOLFSSL_X509_NAME *name;

    WOLFSSL_ENTER("wolfSSL_dup_CA_list");

    copy = wolfSSL_sk_X509_NAME_new(NULL);
    if (copy == NULL) {
        WOLFSSL_MSG("Memory error");
        return NULL;
    }

    for (i = 0; i < num; i++) {
        name = wolfSSL_X509_NAME_dup(wolfSSL_sk_X509_NAME_value(sk, i));
        if (name == NULL || WOLFSSL_SUCCESS != wolfSSL_sk_X509_NAME_push(copy, name)) {
            WOLFSSL_MSG("Memory error");
            wolfSSL_sk_X509_NAME_pop_free(copy, wolfSSL_X509_NAME_free);
            return NULL;
        }
    }

    return copy;
}

void* wolfSSL_sk_X509_OBJECT_value(WOLF_STACK_OF(WOLFSSL_X509_OBJECT)* sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_value");
    for (; sk != NULL && i > 0; i--)
        sk = sk->next;

    if (i != 0 || sk == NULL)
        return NULL;
    return sk->data.x509_obj;
}

int wolfSSL_sk_X509_OBJECT_num(const WOLF_STACK_OF(WOLFSSL_X509_OBJECT) *s)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_num");
    if (s) {
        return (int)s->num;
    } else {
        return 0;
    }
}

int wolfSSL_sk_X509_NAME_set_cmp_func(WOLF_STACK_OF(WOLFSSL_X509_NAME)* sk,
    WOLF_SK_COMPARE_CB(WOLFSSL_X509_NAME, cb))
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_NAME_set_cmp_func");

    if (sk == NULL)
        return BAD_FUNC_ARG;

    WOLFSSL_MSG("Stack comparison not used in wolfSSL");
    (void)cb;
    return 0;
}
#endif /* OPENSSL_ALL */

#ifndef NO_BIO

/* Helper function for X509_NAME_print_ex. Sets *buf to string for domain
   name attribute based on NID. Returns size of buf */
static int get_dn_attr_by_nid(int n, const char** buf)
{
    int len = 0;
    const char *str;

    switch(n)
    {
        case NID_commonName :
            str = "CN";
            len = 2;
            break;
        case NID_countryName:
            str = "C";
            len = 1;
            break;
        case NID_localityName:
            str = "L";
            len = 1;
            break;
        case NID_stateOrProvinceName:
            str = "ST";
            len = 2;
            break;
        case NID_organizationName:
            str = "O";
            len = 1;
            break;
        case NID_organizationalUnitName:
            str = "OU";
            len = 2;
            break;
        case NID_emailAddress:
            str = "emailAddress";
            len = 12;
            break;
        case NID_surname:
            str = "SN";
            len = 2;
            break;
        case NID_givenName:
            str = "GN";
            len = 2;
            break;
        case NID_dnQualifier:
            str = "dnQualifier";
            len = 11;
            break;
        case NID_name:
            str = "name";
            len = 4;
            break;
        case NID_initials:
            str = "initials";
            len = 8;
            break;
        case NID_domainComponent:
            str = "DC";
            len = 2;
            break;
        case NID_pkcs9_contentType:
            str = "contentType";
            len = 11;
            break;
        default:
            WOLFSSL_MSG("Attribute type not found");
            str = NULL;

    }
    if (buf != NULL)
        *buf = str;
    return len;
}

/**
 * Escape input string for RFC2253 requirements. The following characters
 * are escaped with a backslash (\):
 *
 *     1. A space or '#' at the beginning of the string
 *     2. A space at the end of the string
 *     3. One of: ",", "+", """, "\", "<", ">", ";"
 *
 * in    - input string to escape
 * inSz  - length of in, not including the null terminator
 * out   - buffer for output string to be written, will be null terminated
 * outSz - size of out
 *
 * Returns size of output string (not counting NULL terminator) on success,
 * negative on error.
 */
static int wolfSSL_EscapeString_RFC2253(char* in, word32 inSz,
                                        char* out, word32 outSz)
{
    word32 inIdx = 0;
    word32 outIdx = 0;

    if (in == NULL || out == NULL || inSz == 0 || outSz == 0) {
        return BAD_FUNC_ARG;
    }

    for (inIdx = 0; inIdx < inSz; inIdx++) {

        char c = in[inIdx];

        if (((inIdx == 0) && (c == ' ' || c == '#')) ||
            ((inIdx == (inSz-1)) && (c == ' ')) ||
            c == ',' || c == '+' || c == '"' || c == '\\' ||
            c == '<' || c == '>' || c == ';') {

            if (outIdx > (outSz - 1)) {
                return BUFFER_E;
            }
            out[outIdx] = '\\';
            outIdx++;
        }
        if (outIdx > (outSz - 1)) {
            return BUFFER_E;
        }
        out[outIdx] = c;
        outIdx++;
    }

    /* null terminate out */
    if (outIdx > (outSz -1)) {
        return BUFFER_E;
    }
    out[outIdx] = '\0';

    return outIdx;
}

/*
 * Print human readable version of X509_NAME to provided BIO.
 *
 * bio    - output BIO to place name string. Does not include null terminator.
 * name   - input name to convert to string
 * indent - number of indent spaces to prepend to name string
 * flags  - flags to control function behavior. Not all flags are currently
 *          supported/implemented. Currently supported are:
 *              XN_FLAG_RFC2253 - only the backslash escape requirements from
 *                                RFC22523 currently implemented.
 *              XN_FLAG_DN_REV  - print name reversed. Automatically done by
 *                                XN_FLAG_RFC2253.
 *
 * Returns WOLFSSL_SUCCESS (1) on success, WOLFSSL_FAILURE (0) on failure.
 */
int wolfSSL_X509_NAME_print_ex(WOLFSSL_BIO* bio, WOLFSSL_X509_NAME* name,
                int indent, unsigned long flags)
{
    int i, count = 0, nameStrSz = 0, escapeSz = 0;
    char* tmp = NULL;
    char* nameStr = NULL;
    const char *buf = NULL;
    WOLFSSL_X509_NAME_ENTRY* ne;
    WOLFSSL_ASN1_STRING* str;
    char escaped[ASN_NAME_MAX];

    WOLFSSL_ENTER("wolfSSL_X509_NAME_print_ex");

    if ((name == NULL) || (name->sz == 0) || (bio == NULL))
        return WOLFSSL_FAILURE;

    for (i = 0; i < indent; i++) {
        if (wolfSSL_BIO_write(bio, " ", 1) != 1)
            return WOLFSSL_FAILURE;
    }

    count = wolfSSL_X509_NAME_entry_count(name);

    for (i = 0; i < count; i++) {
        int len;
        int tmpSz;

        /* reverse name order for RFC2253 and DN_REV */
        if ((flags & XN_FLAG_RFC2253) || (flags & XN_FLAG_DN_REV)) {
            ne = wolfSSL_X509_NAME_get_entry(name, count - i - 1);
        } else {
            ne = wolfSSL_X509_NAME_get_entry(name, i);
        }
        if (ne == NULL)
            return WOLFSSL_FAILURE;

        str = wolfSSL_X509_NAME_ENTRY_get_data(ne);
        if (str == NULL)
            return WOLFSSL_FAILURE;

        if (flags & XN_FLAG_RFC2253) {
            /* escape string for RFC 2253, ret sz not counting null term */
            escapeSz = wolfSSL_EscapeString_RFC2253(str->data,
                            str->length, escaped, sizeof(escaped));
            if (escapeSz < 0)
                return WOLFSSL_FAILURE;

            nameStr = escaped;
            nameStrSz = escapeSz;
        }
        else {
            nameStr = str->data;
            nameStrSz = str->length;
        }

        /* len is without null terminator */
        len = get_dn_attr_by_nid(ne->nid, &buf);
        if (len == 0 || buf == NULL)
            return WOLFSSL_FAILURE;

        tmpSz = nameStrSz + len + 4; /* + 4 for '=', comma space and '\0'*/
        tmp = (char*)XMALLOC(tmpSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (tmp == NULL) {
            return WOLFSSL_FAILURE;
        }

        if (i < count - 1) {
            if (XSNPRINTF(tmp, tmpSz, "%s=%s, ", buf, nameStr)
                >= tmpSz)
            {
                WOLFSSL_MSG("buffer overrun");
                return WOLFSSL_FAILURE;
            }

            tmpSz = len + nameStrSz + 3; /* 3 for '=', comma space */
        }
        else {
            if (XSNPRINTF(tmp, tmpSz, "%s=%s", buf, nameStr)
                >= tmpSz)
            {
                WOLFSSL_MSG("buffer overrun");
                return WOLFSSL_FAILURE;
            }
            tmpSz = len + nameStrSz + 1; /* 1 for '=' */
            if (bio->type != WOLFSSL_BIO_FILE && bio->type != WOLFSSL_BIO_MEMORY)
                ++tmpSz; /* include the terminating null when not writing to a
                          * file.
                          */
        }

        if (wolfSSL_BIO_write(bio, tmp, tmpSz) != tmpSz) {
            XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
            return WOLFSSL_FAILURE;
        }

        XFREE(tmp, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }

    return WOLFSSL_SUCCESS;
}

#ifndef NO_FILESYSTEM
int wolfSSL_X509_NAME_print_ex_fp(XFILE file, WOLFSSL_X509_NAME* name,
        int indent, unsigned long flags)
{
    WOLFSSL_BIO* bio;
    int ret;

    WOLFSSL_ENTER("wolfSSL_X509_NAME_print_ex_fp");

    if (!(bio = wolfSSL_BIO_new_fp(file, BIO_NOCLOSE))) {
        WOLFSSL_MSG("wolfSSL_BIO_new_fp error");
        return WOLFSSL_FAILURE;
    }

    ret = wolfSSL_X509_NAME_print_ex(bio, name, indent, flags);

    wolfSSL_BIO_free(bio);

    return ret;
}
#endif /* NO_FILESYSTEM */
#endif /* !NO_BIO */

#ifndef NO_WOLFSSL_STUB
WOLFSSL_ASN1_BIT_STRING* wolfSSL_X509_get0_pubkey_bitstr(const WOLFSSL_X509* x)
{
    (void)x;
    WOLFSSL_ENTER("wolfSSL_X509_get0_pubkey_bitstr");
    WOLFSSL_STUB("X509_get0_pubkey_bitstr");

    return NULL;
}
#endif

#ifdef OPENSSL_ALL
WOLFSSL_X509_LOOKUP_TYPE wolfSSL_X509_OBJECT_get_type(
        const WOLFSSL_X509_OBJECT* obj)
{
    if (obj == NULL)
        return WOLFSSL_X509_LU_NONE;
    return obj->type;
}

WOLFSSL_X509_OBJECT* wolfSSL_X509_OBJECT_new(void)
{
    WOLFSSL_X509_OBJECT* ret = (WOLFSSL_X509_OBJECT*)
            XMALLOC(sizeof(WOLFSSL_X509_OBJECT), NULL, DYNAMIC_TYPE_OPENSSL);
    if (ret != NULL)
        XMEMSET(ret, 0, sizeof(WOLFSSL_X509_OBJECT));
    return ret;
}

void wolfSSL_X509_OBJECT_free(WOLFSSL_X509_OBJECT *obj)
{
    WOLFSSL_ENTER("wolfSSL_X509_OBJECT_free");
    if (obj != NULL) {
        if (obj->type == WOLFSSL_X509_LU_X509) {
            wolfSSL_X509_free(obj->data.x509);
        }
        else {
            /* We don't free as this will point to
             * store->cm->crl which we don't own */
            WOLFSSL_MSG("Not free'ing CRL in WOLFSSL_X509_OBJECT");
        }
        XFREE(obj, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}
#endif /* OPENSSL_ALL */

#ifndef NO_WOLFSSL_STUB
WOLFSSL_X509_OBJECT* wolfSSL_sk_X509_OBJECT_delete(
    WOLF_STACK_OF(WOLFSSL_X509_OBJECT)* sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_delete");
    WOLFSSL_STUB("wolfSSL_sk_X509_OBJECT_delete");
    (void)sk;
    (void)i;
    return NULL;
}
#endif

WOLFSSL_X509 *wolfSSL_X509_OBJECT_get0_X509(const WOLFSSL_X509_OBJECT *obj)
{
    if (obj != NULL && obj->type == WOLFSSL_X509_LU_X509)
        return obj->data.x509;
    return NULL;
}

WOLFSSL_X509_CRL *wolfSSL_X509_OBJECT_get0_X509_CRL(WOLFSSL_X509_OBJECT *obj)
{
    if (obj != NULL && obj->type == WOLFSSL_X509_LU_CRL)
        return obj->data.crl;
    return NULL;
}

#endif /* OPENSSL_ALL || (OPENSSL_EXTRA && (HAVE_STUNNEL || WOLFSSL_NGINX ||
        * HAVE_LIGHTY || WOLFSSL_HAPROXY || WOLFSSL_OPENSSH ||
        * HAVE_SBLIM_SFCB)) */


#if defined(OPENSSL_EXTRA)

int wolfSSL_sk_X509_num(const WOLF_STACK_OF(WOLFSSL_X509) *s)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_num");

    if (s == NULL)
        return -1;
    return (int)s->num;
}

#endif /* OPENSSL_EXTRA */

#if defined(HAVE_EX_DATA) && (defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) \
     || defined(WOLFSSL_HAPROXY) || defined(OPENSSL_EXTRA)                   \
     || defined(HAVE_LIGHTY))

int wolfSSL_X509_get_ex_new_index(int idx, void *arg,
                                  WOLFSSL_CRYPTO_EX_new* new_func,
                                  WOLFSSL_CRYPTO_EX_dup* dup_func,
                                  WOLFSSL_CRYPTO_EX_free* free_func)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_ex_new_index");

    return wolfssl_get_ex_new_index(CRYPTO_EX_INDEX_X509, idx, arg,
                                    new_func, dup_func, free_func);
}
#endif

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL) || \
    defined(WOLFSSL_WPAS_SMALL)
void *wolfSSL_X509_get_ex_data(X509 *x509, int idx)
{
    WOLFSSL_ENTER("wolfSSL_X509_get_ex_data");
#ifdef HAVE_EX_DATA
    if (x509 != NULL) {
        return wolfSSL_CRYPTO_get_ex_data(&x509->ex_data, idx);
    }
#else
    (void)x509;
    (void)idx;
#endif
    return NULL;
}

int wolfSSL_X509_set_ex_data(X509 *x509, int idx, void *data)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_ex_data");
#ifdef HAVE_EX_DATA
    if (x509 != NULL)
    {
        return wolfSSL_CRYPTO_set_ex_data(&x509->ex_data, idx, data);
    }
#else
    (void)x509;
    (void)idx;
    (void)data;
#endif
    return WOLFSSL_FAILURE;
}

#ifdef HAVE_EX_DATA_CLEANUP_HOOKS
int wolfSSL_X509_set_ex_data_with_cleanup(
    X509 *x509,
    int idx,
    void *data,
    wolfSSL_ex_data_cleanup_routine_t cleanup_routine)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_ex_data_with_cleanup");
    if (x509 != NULL)
    {
        return wolfSSL_CRYPTO_set_ex_data_with_cleanup(&x509->ex_data, idx,
                                                       data, cleanup_routine);
    }
    return WOLFSSL_FAILURE;
}
#endif /* HAVE_EX_DATA_CLEANUP_HOOKS */

#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL || WOLFSSL_WPAS_SMALL */


#ifndef NO_ASN
int wolfSSL_X509_check_host(WOLFSSL_X509 *x, const char *chk, size_t chklen,
                    unsigned int flags, char **peername)
{
    int         ret;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert *dCert;
#else
    DecodedCert dCert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_X509_check_host");

    /* flags and peername not needed for Nginx. */
    (void)flags;
    (void)peername;

    if ((x == NULL) || (chk == NULL)) {
        WOLFSSL_MSG("Invalid parameter");
        return WOLFSSL_FAILURE;
    }

    if (flags == WOLFSSL_NO_WILDCARDS) {
        WOLFSSL_MSG("X509_CHECK_FLAG_NO_WILDCARDS not yet implemented");
        return WOLFSSL_FAILURE;
    }
    if (flags == WOLFSSL_NO_PARTIAL_WILDCARDS) {
        WOLFSSL_MSG("X509_CHECK_FLAG_NO_PARTIAL_WILDCARDS not yet implemented");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    dCert = (DecodedCert *)XMALLOC(sizeof(*dCert), x->heap,
                                   DYNAMIC_TYPE_DCERT);
    if (dCert == NULL) {
        WOLFSSL_MSG("\tout of memory");
        return WOLFSSL_FATAL_ERROR;
    }
#endif

    InitDecodedCert(dCert, x->derCert->buffer, x->derCert->length, NULL);
    ret = ParseCertRelative(dCert, CERT_TYPE, 0, NULL);
    if (ret != 0) {
        goto out;
    }

    ret = CheckHostName(dCert, (char *)chk, chklen);

out:

    FreeDecodedCert(dCert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(dCert, x->heap, DYNAMIC_TYPE_DCERT);
#endif

    if (ret != 0)
        return WOLFSSL_FAILURE;
    return WOLFSSL_SUCCESS;
}


int wolfSSL_X509_check_ip_asc(WOLFSSL_X509 *x, const char *ipasc,
        unsigned int flags)
{
    int ret = WOLFSSL_FAILURE;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert *dCert = NULL;
#else
    DecodedCert dCert[1];
#endif

    WOLFSSL_ENTER("wolfSSL_X509_check_ip_asc");

    /* flags not yet implemented */
    (void)flags;

    if ((x == NULL) || (x->derCert == NULL) || (ipasc == NULL)) {
        WOLFSSL_MSG("Invalid parameter");
    }
    else {
        ret = WOLFSSL_SUCCESS;
    }

#ifdef WOLFSSL_SMALL_STACK
    if (ret == WOLFSSL_SUCCESS) {
        dCert = (DecodedCert *)XMALLOC(sizeof(*dCert), x->heap,
                                       DYNAMIC_TYPE_DCERT);
        if (dCert == NULL) {
            WOLFSSL_MSG("\tout of memory");
            ret = WOLFSSL_FAILURE;
        }
    }
#endif

    if (ret == WOLFSSL_SUCCESS) {
        InitDecodedCert(dCert, x->derCert->buffer, x->derCert->length, NULL);
        ret = ParseCertRelative(dCert, CERT_TYPE, 0, NULL);
        if (ret != 0) {
            ret = WOLFSSL_FAILURE;
        }
        else {
            ret = CheckIPAddr(dCert, ipasc);
            if (ret != 0) {
                ret = WOLFSSL_FAILURE;
            }
            else {
                ret = WOLFSSL_SUCCESS;
            }
        }
        FreeDecodedCert(dCert);
    }

#ifdef WOLFSSL_SMALL_STACK
    if (dCert != NULL)
        XFREE(dCert, x->heap, DYNAMIC_TYPE_DCERT);
#endif

    return ret;
}
#endif

#if defined(OPENSSL_EXTRA) && defined(WOLFSSL_CERT_GEN)
int wolfSSL_X509_check_email(WOLFSSL_X509 *x, const char *chk, size_t chkLen,
                             unsigned int flags)
{
    WOLFSSL_X509_NAME *subjName;
    int emailLen;
    char *emailBuf;

    (void)flags;

    WOLFSSL_ENTER("wolfSSL_X509_check_email");

    if ((x == NULL) || (chk == NULL)) {
        WOLFSSL_MSG("Invalid parameter");
        return WOLFSSL_FAILURE;
    }

    subjName = wolfSSL_X509_get_subject_name(x);
    if (subjName == NULL)
        return WOLFSSL_FAILURE;

    /* Call with NULL buffer to get required length. */
    emailLen = wolfSSL_X509_NAME_get_text_by_NID(subjName, NID_emailAddress,
                                                 NULL, 0);
    if (emailLen < 0)
        return WOLFSSL_FAILURE;

    ++emailLen; /* Add 1 for the NUL. */

    emailBuf = (char*)XMALLOC(emailLen, x->heap, DYNAMIC_TYPE_OPENSSL);
    if (emailBuf == NULL)
        return WOLFSSL_FAILURE;

    emailLen = wolfSSL_X509_NAME_get_text_by_NID(subjName, NID_emailAddress,
                                                 emailBuf, emailLen);
    if (emailLen < 0) {
        XFREE(emailBuf, x->heap, DYNAMIC_TYPE_OPENSSL);
        return WOLFSSL_FAILURE;
    }

    if (chkLen == 0)
        chkLen = XSTRLEN(chk);

    if (chkLen != (size_t)emailLen
     || XSTRNCMP(chk, emailBuf, chkLen)) {
        XFREE(emailBuf, x->heap, DYNAMIC_TYPE_OPENSSL);
        return WOLFSSL_FAILURE;
    }

    XFREE(emailBuf, x->heap, DYNAMIC_TYPE_OPENSSL);
    return WOLFSSL_SUCCESS;
}
#endif /* OPENSSL_EXTRA && WOLFSSL_CERT_GEN */

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) \
    || defined(OPENSSL_EXTRA) || defined(HAVE_LIGHTY)

int wolfSSL_X509_NAME_digest(const WOLFSSL_X509_NAME *name,
        const WOLFSSL_EVP_MD *type, unsigned char *md, unsigned int *len)
{
    WOLFSSL_ENTER("wolfSSL_X509_NAME_digest");

    if (name == NULL || type == NULL)
        return WOLFSSL_FAILURE;

#if !defined(NO_FILESYSTEM) && !defined(NO_PWDBASED)
    return wolfSSL_EVP_Digest((unsigned char*)name->name,
                              name->sz, md, len, type, NULL);
#else
    (void)md;
    (void)len;
    return NOT_COMPILED_IN;
#endif
}

#endif /* OPENSSL_ALL || WOLFSSL_NGINX || WOLFSSL_HAPROXY ||
    OPENSSL_EXTRA || HAVE_LIGHTY */

#if defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) || \
    defined(OPENSSL_EXTRA) || defined(OPENSSL_ALL)

/**
 * Find the issuing cert of the input cert. On a self-signed cert this
 * function will return an error.
 * @param issuer The issuer x509 struct is returned here
 * @param cm     The cert manager that is queried for the issuer
 * @param x      This cert's issuer will be queried in cm
 * @return       WOLFSSL_SUCCESS on success
 *               WOLFSSL_FAILURE on error
 */
static int x509GetIssuerFromCM(WOLFSSL_X509 **issuer, WOLFSSL_CERT_MANAGER* cm,
        WOLFSSL_X509 *x)
{
    Signer* ca = NULL;
#ifdef WOLFSSL_SMALL_STACK
    DecodedCert* cert = NULL;
#else
    DecodedCert  cert[1];
#endif

    if (cm == NULL || x == NULL || x->derCert == NULL) {
        WOLFSSL_MSG("No cert DER buffer or NULL cm. Defining "
                    "WOLFSSL_SIGNER_DER_CERT could solve the issue");
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    cert = (DecodedCert*)XMALLOC(sizeof(DecodedCert), NULL, DYNAMIC_TYPE_DCERT);
    if (cert == NULL)
        return WOLFSSL_FAILURE;
#endif

    /* Use existing CA retrieval APIs that use DecodedCert. */
    InitDecodedCert(cert, x->derCert->buffer, x->derCert->length, NULL);
    if (ParseCertRelative(cert, CERT_TYPE, 0, NULL) == 0
            && !cert->selfSigned) {
    #ifndef NO_SKID
        if (cert->extAuthKeyIdSet)
            ca = GetCA(cm, cert->extAuthKeyId);
        if (ca == NULL)
            ca = GetCAByName(cm, cert->issuerHash);
    #else /* NO_SKID */
        ca = GetCA(cm, cert->issuerHash);
    #endif /* NO SKID */
    }
    FreeDecodedCert(cert);
#ifdef WOLFSSL_SMALL_STACK
    XFREE(cert, NULL, DYNAMIC_TYPE_DCERT);
#endif

    if (ca == NULL)
        return WOLFSSL_FAILURE;

#ifdef WOLFSSL_SIGNER_DER_CERT
    /* populate issuer with Signer DER */
    if (wolfSSL_X509_d2i(issuer, ca->derCert->buffer,
            ca->derCert->length) == NULL)
        return WOLFSSL_FAILURE;
#else
    /* Create an empty certificate as CA doesn't have a certificate. */
    *issuer = (WOLFSSL_X509 *)XMALLOC(sizeof(WOLFSSL_X509), 0,
        DYNAMIC_TYPE_OPENSSL);
    if (*issuer == NULL)
        return WOLFSSL_FAILURE;

    InitX509((*issuer), 1, NULL);
#endif

    return WOLFSSL_SUCCESS;
}

void wolfSSL_X509_email_free(WOLF_STACK_OF(WOLFSSL_STRING) *sk)
{
    WOLFSSL_STACK *curr;

    while (sk != NULL) {
        curr = sk;
        sk = sk->next;

        XFREE(curr, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

WOLF_STACK_OF(WOLFSSL_STRING) *wolfSSL_X509_get1_ocsp(WOLFSSL_X509 *x)
{
    WOLFSSL_STACK* list = NULL;
    char*          url;

    if (x == NULL || x->authInfoSz == 0)
        return NULL;

    list = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK) + x->authInfoSz + 1,
                                   NULL, DYNAMIC_TYPE_OPENSSL);
    if (list == NULL)
        return NULL;

    url = (char*)list;
    url += sizeof(WOLFSSL_STACK);
    XMEMCPY(url, x->authInfo, x->authInfoSz);
    url[x->authInfoSz] = '\0';

    list->data.string = url;
    list->next = NULL;
    list->num = 1;

    return list;
}

int wolfSSL_X509_check_issued(WOLFSSL_X509 *issuer, WOLFSSL_X509 *subject)
{
    WOLFSSL_X509_NAME *issuerName = wolfSSL_X509_get_issuer_name(subject);
    WOLFSSL_X509_NAME *subjectName = wolfSSL_X509_get_subject_name(issuer);

    if (issuerName == NULL || subjectName == NULL)
        return WOLFSSL_X509_V_ERR_SUBJECT_ISSUER_MISMATCH;

    /* Literal matching of encoded names and key ids. */
    if (issuerName->sz != subjectName->sz ||
           XMEMCMP(issuerName->name, subjectName->name, subjectName->sz) != 0) {
        return WOLFSSL_X509_V_ERR_SUBJECT_ISSUER_MISMATCH;
    }

    if (subject->authKeyId != NULL && issuer->subjKeyId != NULL) {
        if (subject->authKeyIdSz != issuer->subjKeyIdSz ||
                XMEMCMP(subject->authKeyId, issuer->subjKeyId,
                        issuer->subjKeyIdSz) != 0) {
            return WOLFSSL_X509_V_ERR_SUBJECT_ISSUER_MISMATCH;
        }
    }

    return WOLFSSL_X509_V_OK;
}

#endif /* WOLFSSL_NGINX || WOLFSSL_HAPROXY || OPENSSL_EXTRA || OPENSSL_ALL */

#if defined(OPENSSL_EXTRA) || defined(WOLFSSL_WPAS_SMALL) || \
    defined(KEEP_PEER_CERT)
WOLFSSL_X509* wolfSSL_X509_dup(WOLFSSL_X509 *x)
{
    WOLFSSL_ENTER("wolfSSL_X509_dup");

    if (x == NULL) {
        WOLFSSL_MSG("Error: NULL input");
        return NULL;
    }

    if (x->derCert == NULL) {
        WOLFSSL_MSG("Error: NULL derCert parameter");
        return NULL;
    }

    return wolfSSL_X509_d2i(NULL, x->derCert->buffer, x->derCert->length);
}
#endif /* OPENSSL_EXTRA || WOLFSSL_WPAS_SMALL */

#if defined(OPENSSL_EXTRA)
int wolfSSL_X509_check_ca(WOLFSSL_X509 *x509)
{
    WOLFSSL_ENTER("wolfSSL_X509_check_ca");

    if (x509 == NULL)
        return WOLFSSL_FAILURE;
    if (x509->isCa)
        return 1;
    if (x509->extKeyUsageCrit)
        return 4;

    return 0;
}
#endif /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
long wolfSSL_X509_get_version(const WOLFSSL_X509 *x509)
{
    int version = 0;

    WOLFSSL_ENTER("wolfSSL_X509_get_version");

    if (x509 == NULL){
        WOLFSSL_MSG("invalid parameter");
        return 0L;
    }
    version = x509->version;
    if (version != 0)
        return (long)version - 1L;

    return 0L;
}
#endif /* OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL */

#if defined(OPENSSL_EXTRA)
int wolfSSL_X509_get_signature_nid(const WOLFSSL_X509 *x)
{
    if (x == NULL)
        return 0;

    return oid2nid(x->sigOID, oidSigType);
}
#endif  /* OPENSSL_EXTRA */

#if defined(OPENSSL_EXTRA)
WOLFSSL_STACK* wolfSSL_sk_X509_new(WOLF_SK_COMPARE_CB(WOLFSSL_X509, cb))
{
    (void)cb;
    return wolfSSL_sk_X509_new_null();
}

WOLFSSL_STACK* wolfSSL_sk_X509_new_null(void)
{
    WOLFSSL_STACK* s = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
            DYNAMIC_TYPE_OPENSSL);
    if (s != NULL) {
        XMEMSET(s, 0, sizeof(*s));
        s->type = STACK_TYPE_X509;
    }

    return s;
}
#endif  /* OPENSSL_EXTRA */

#ifdef OPENSSL_ALL

WOLFSSL_STACK* wolfSSL_sk_X509_OBJECT_new(void)
{
    WOLFSSL_STACK* s = (WOLFSSL_STACK*)XMALLOC(sizeof(WOLFSSL_STACK), NULL,
            DYNAMIC_TYPE_OPENSSL);
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_new");
    if (s != NULL) {
        XMEMSET(s, 0, sizeof(*s));
        s->type = STACK_TYPE_X509_OBJ;
    }
    return s;
}

void wolfSSL_sk_X509_OBJECT_free(WOLFSSL_STACK* s)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_free");
    wolfSSL_sk_free(s);
}

void wolfSSL_sk_X509_OBJECT_pop_free(WOLFSSL_STACK* s,
        void (*f) (WOLFSSL_X509_OBJECT*))
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_pop_free");
    wolfSSL_sk_pop_free(s, (wolfSSL_sk_freefunc)f);
}

int wolfSSL_sk_X509_OBJECT_push(WOLFSSL_STACK* sk, WOLFSSL_X509_OBJECT* obj)
{
    WOLFSSL_ENTER("wolfSSL_sk_X509_OBJECT_push");

    if (sk == NULL || obj == NULL) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_sk_push(sk, obj);
}

#endif /* OPENSSL_ALL */

#if defined(OPENSSL_EXTRA) || defined(OPENSSL_EXTRA_X509_SMALL)
/* unlike wolfSSL_X509_NAME_dup this does not malloc a duplicate, only deep
 * copy. "to" is expected to be a fresh blank name, if not pointers could be
 * lost */
int wolfSSL_X509_NAME_copy(WOLFSSL_X509_NAME* from, WOLFSSL_X509_NAME* to)
{
    int i;

    WOLFSSL_ENTER("wolfSSL_X509_NAME_copy");

    if (from == NULL || to == NULL) {
        WOLFSSL_MSG("NULL parameter");
        return BAD_FUNC_ARG;
    }

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(HAVE_LIGHTY)
    if (from->rawLen > 0) {
        if (from->rawLen > ASN_NAME_MAX) {
            WOLFSSL_MSG("Bad raw size");
            return BAD_FUNC_ARG;
        }
        XMEMCPY(to->raw, from->raw, from->rawLen);
        to->rawLen = from->rawLen;
    }
#endif

    if (from->dynamicName) {
        to->name = (char*)XMALLOC(from->sz, to->heap, DYNAMIC_TYPE_SUBJECT_CN);
        if (to->name == NULL)
            return WOLFSSL_FAILURE;
        to->dynamicName = 1;
    }
    XMEMCPY(to->name, from->name, from->sz);
    to->sz = from->sz;

    for (i = 0; i < MAX_NAME_ENTRIES; i++) {
        WOLFSSL_X509_NAME_ENTRY* ne = wolfSSL_X509_NAME_get_entry(from, i);
        if (ne != NULL) {
            if (wolfSSL_X509_NAME_add_entry(to, ne, i, 1) != WOLFSSL_SUCCESS) {
                return WOLFSSL_FAILURE;
            }
        }
    }
    to->entrySz = from->entrySz;
    return WOLFSSL_SUCCESS;
}


/* copies over information from "name" to the "cert" subject name
 * returns WOLFSSL_SUCCESS on success */
int wolfSSL_X509_set_subject_name(WOLFSSL_X509 *cert, WOLFSSL_X509_NAME *name)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_subject_name");
    if (cert == NULL || name == NULL)
        return WOLFSSL_FAILURE;

    FreeX509Name(&cert->subject);
    InitX509Name(&cert->subject, 0, cert->heap);

    if (wolfSSL_X509_NAME_copy(name, &cert->subject) != WOLFSSL_SUCCESS) {
        FreeX509Name(&cert->subject);
        return WOLFSSL_FAILURE;
    }

    cert->subject.x509 = cert;
    return WOLFSSL_SUCCESS;
}


/* copies over information from "name" to the "cert" issuer name
 * returns WOLFSSL_SUCCESS on success */
int wolfSSL_X509_set_issuer_name(WOLFSSL_X509 *cert, WOLFSSL_X509_NAME *name)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_issuer_name");
    if (cert == NULL || name == NULL)
        return WOLFSSL_FAILURE;

    FreeX509Name(&cert->issuer);
    InitX509Name(&cert->issuer, 0, cert->heap);

    if (wolfSSL_X509_NAME_copy(name, &cert->issuer) != WOLFSSL_SUCCESS) {
        FreeX509Name(&cert->issuer);
        return WOLFSSL_FAILURE;
    }

    cert->issuer.x509 = cert;
    cert->issuerSet = 1;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_X509_set_notAfter(WOLFSSL_X509* x509, const WOLFSSL_ASN1_TIME* t)
{
    if (x509 == NULL || t == NULL) {
        return WOLFSSL_FAILURE;
    }

    x509->notAfter.type = t->type;
    x509->notAfter.length = t->length;

    XMEMCPY(x509->notAfter.data, t->data, CTC_DATE_SIZE);

    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_set_notBefore(WOLFSSL_X509* x509, const WOLFSSL_ASN1_TIME* t)
{
    if (x509 == NULL || t == NULL) {
        return WOLFSSL_FAILURE;
    }

    x509->notBefore.type = t->type;
    x509->notBefore.length = t->length;

    XMEMCPY(x509->notBefore.data, t->data, CTC_DATE_SIZE);

    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_set_serialNumber(WOLFSSL_X509* x509, WOLFSSL_ASN1_INTEGER* s)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_serialNumber");
    if (x509 == NULL || s == NULL || s->length >= EXTERNAL_SERIAL_SIZE)
        return WOLFSSL_FAILURE;

    /* WOLFSSL_ASN1_INTEGER has type | size | data
     * Sanity check that the data is actually in ASN format */
    if (s->length < 3 && s->data[0] != ASN_INTEGER &&
            s->data[1] != s->length - 2) {
        return WOLFSSL_FAILURE;
    }
    XMEMCPY(x509->serial, s->data + 2, s->length - 2);
    x509->serialSz = s->length - 2;
    x509->serial[s->length] = 0;

    return WOLFSSL_SUCCESS;
}


int wolfSSL_X509_set_pubkey(WOLFSSL_X509 *cert, WOLFSSL_EVP_PKEY *pkey)
{
    byte* p = NULL;
    int derSz = 0;
    WOLFSSL_ENTER("wolfSSL_X509_set_pubkey");

    if (cert == NULL || pkey == NULL)
        return WOLFSSL_FAILURE;

    /* Regenerate since pkey->pkey.ptr may contain private key */
    switch (pkey->type) {
#if (defined(WOLFSSL_KEY_GEN) || defined(OPENSSL_EXTRA)) && !defined(NO_RSA)
    case EVP_PKEY_RSA:
        {
            RsaKey* rsa;

            if (pkey->rsa == NULL || pkey->rsa->internal == NULL)
                return WOLFSSL_FAILURE;

            rsa = (RsaKey*)pkey->rsa->internal;
            derSz = wc_RsaPublicKeyDerSize(rsa, 1);
            if (derSz <= 0)
                return WOLFSSL_FAILURE;

            p = (byte*)XMALLOC(derSz, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            if (p == NULL)
                return WOLFSSL_FAILURE;

            if ((derSz = wc_RsaKeyToPublicDer(rsa, p, derSz)) <= 0) {
                XFREE(p, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                return WOLFSSL_FAILURE;
            }
            cert->pubKeyOID = RSAk;
        }
        break;
#endif /* (WOLFSSL_KEY_GEN || OPENSSL_EXTRA) && !NO_RSA */
#if !defined(HAVE_SELFTEST) && (defined(WOLFSSL_KEY_GEN) || \
        defined(WOLFSSL_CERT_GEN)) && !defined(NO_DSA)
    case EVP_PKEY_DSA:
        {
            DsaKey* dsa;

            if (pkey->dsa == NULL || pkey->dsa->internal == NULL)
                return WOLFSSL_FAILURE;

            dsa = (DsaKey*)pkey->dsa->internal;
            /* size of pub, priv, p, q, g + ASN.1 additional information */
            derSz = 5 * mp_unsigned_bin_size(&dsa->g) + MAX_ALGO_SZ;
            p = (byte*)XMALLOC(derSz, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            if (p == NULL)
                return WOLFSSL_FAILURE;

            if ((derSz = wc_DsaKeyToPublicDer(dsa, p, derSz)) <= 0) {
                XFREE(p, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                return WOLFSSL_FAILURE;
            }
            cert->pubKeyOID = RSAk;
        }
        break;
#endif /* !HAVE_SELFTEST && (WOLFSSL_KEY_GEN || WOLFSSL_CERT_GEN) && !NO_DSA */
#ifdef HAVE_ECC
    case EVP_PKEY_EC:
        {
            ecc_key* ecc;

            if (pkey->ecc == NULL || pkey->ecc->internal == NULL)
                return WOLFSSL_FAILURE;

            ecc = (ecc_key*)pkey->ecc->internal;
            derSz = wc_EccPublicKeyDerSize(ecc, 1);
            if (derSz <= 0)
                return WOLFSSL_FAILURE;

            p = (byte*)XMALLOC(derSz, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
            if (p == NULL)
                return WOLFSSL_FAILURE;

            if ((derSz = wc_EccPublicKeyToDer(ecc, p, derSz, 1)) <= 0) {
                XFREE(p, cert->heap, DYNAMIC_TYPE_PUBLIC_KEY);
                return WOLFSSL_FAILURE;
            }
            cert->pubKeyOID = ECDSAk;
        }
        break;
#endif
    default:
        return WOLFSSL_FAILURE;
    }
    cert->pubKey.buffer = p;
    cert->pubKey.length = derSz;

    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_set_version(WOLFSSL_X509* x509, long v)
{
    WOLFSSL_ENTER("wolfSSL_X509_set_version");
    if ((x509 == NULL) || (v < 0) || (v >= INT_MAX)) {
        return WOLFSSL_FAILURE;
    }
    x509->version = (int) v + 1;

    return WOLFSSL_SUCCESS;
}

#endif /* (OPENSSL_EXTRA || OPENSSL_EXTRA_X509_SMALL) && WOLFSSL_CERT_GEN */

#if defined(OPENSSL_ALL) && \
    defined(WOLFSSL_CERT_GEN) && defined(WOLFSSL_CERT_REQ)

void wolfSSL_X509V3_set_ctx(WOLFSSL_X509V3_CTX* ctx, WOLFSSL_X509* issuer,
        WOLFSSL_X509* subject, WOLFSSL_X509* req, WOLFSSL_X509_CRL* crl,
        int flag)
{
    int ret = WOLFSSL_SUCCESS;
    WOLFSSL_ENTER("wolfSSL_X509V3_set_ctx");
    if (!ctx)
        return;

    /* not checking ctx->x509 for null first since app won't have initialized
     * this X509V3_CTX before this function call */
    ctx->x509 = wolfSSL_X509_new();
    if (!ctx->x509)
        return;

    /* Set parameters in ctx as long as ret == WOLFSSL_SUCCESS */
    if (issuer)
        ret = wolfSSL_X509_set_issuer_name(ctx->x509,&issuer->issuer);

    if (subject && ret == WOLFSSL_SUCCESS)
        ret = wolfSSL_X509_set_subject_name(ctx->x509,&subject->subject);

    if (req && ret == WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("req not implemented.");
    }

    if (crl && ret == WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("crl not implemented.");
    }

    if (flag && ret == WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("flag not implemented.");
    }

    if (!ret) {
        WOLFSSL_MSG("Error setting WOLFSSL_X509V3_CTX parameters.");
    }
}

#ifndef NO_BIO
int wolfSSL_i2d_X509_REQ(WOLFSSL_X509* req, unsigned char** out)
{
    int derSz = 0;
    int ret = WOLFSSL_FAILURE;
    WOLFSSL_BIO* bio = NULL;
    WOLFSSL_ENTER("wolfSSL_i2d_X509_REQ");

    if (req == NULL || out == NULL) {
        return BAD_FUNC_ARG;
    }

    if (!(bio = wolfSSL_BIO_new(wolfSSL_BIO_s_mem()))) {
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_i2d_X509_REQ_bio(bio, req) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_i2d_X509_REQ_bio error");
        goto cleanup;
    }

    derSz = wolfSSL_BIO_get_len(bio);

    if (*out == NULL) {
        *out = (unsigned char*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!*out) {
            WOLFSSL_MSG("malloc error");
            ret = MEMORY_E;
            goto cleanup;
        }
    }

    if (wolfSSL_BIO_read(bio, *out, derSz) != derSz) {
        WOLFSSL_MSG("wolfSSL_BIO_read error");
        goto cleanup;
    }

    ret = derSz;
cleanup:
    wolfSSL_BIO_free(bio);

    return ret;
}
#endif /* !NO_BIO */

WOLFSSL_X509* wolfSSL_X509_REQ_new(void)
{
    return wolfSSL_X509_new();
}

void wolfSSL_X509_REQ_free(WOLFSSL_X509* req)
{
    wolfSSL_X509_free(req);
}

int wolfSSL_X509_REQ_sign(WOLFSSL_X509 *req, WOLFSSL_EVP_PKEY *pkey,
                          const WOLFSSL_EVP_MD *md)
{
    int ret;
#ifdef WOLFSSL_SMALL_STACK
    byte* der = NULL;
#else
    byte der[2048];
#endif
    int derSz = 2048;

    if (req == NULL || pkey == NULL || md == NULL) {
        WOLFSSL_LEAVE("wolfSSL_X509_REQ_sign", BAD_FUNC_ARG);
        return WOLFSSL_FAILURE;
    }

#ifdef WOLFSSL_SMALL_STACK
    der = (byte*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (der == NULL) {
        return WOLFSSL_FAILURE;
    }
#endif

    /* Create a Cert that has the certificate request fields. */
    req->sigOID = wolfSSL_sigTypeFromPKEY((WOLFSSL_EVP_MD*)md, pkey);
    ret = wolfssl_x509_make_der(req, 1, der, &derSz, 0);
    if (ret != WOLFSSL_SUCCESS) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        WOLFSSL_MSG("Unable to make DER for X509");
        WOLFSSL_LEAVE("wolfSSL_X509_REQ_sign", ret);
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_X509_resign_cert(req, 1, der, 2048, derSz,
            (WOLFSSL_EVP_MD*)md, pkey) <= 0) {
#ifdef WOLFSSL_SMALL_STACK
        XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
        return WOLFSSL_FAILURE;
    }
#ifdef WOLFSSL_SMALL_STACK
    XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return WOLFSSL_SUCCESS;
}

int wolfSSL_X509_REQ_sign_ctx(WOLFSSL_X509 *req,
                              WOLFSSL_EVP_MD_CTX* md_ctx)
{
    if (md_ctx && md_ctx->pctx)
        return wolfSSL_X509_REQ_sign(req, md_ctx->pctx->pkey,
                wolfSSL_EVP_MD_CTX_md(md_ctx));
    else
        return WOLFSSL_FAILURE;
}

static int regenX509REQDerBuffer(WOLFSSL_X509* x509)
{
    int derSz = X509_BUFFER_SZ;
    int ret = WOLFSSL_FAILURE;
#ifdef WOLFSSL_SMALL_STACK
    byte* der;
    der = (byte*)XMALLOC(derSz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (!der) {
        WOLFSSL_MSG("malloc failed");
        return WOLFSSL_FAILURE;
    }
#else
    byte der[X509_BUFFER_SZ];
#endif

    if (wolfssl_x509_make_der(x509, 1, der, &derSz, 0) == WOLFSSL_SUCCESS) {
        FreeDer(&x509->derCert);
        if (AllocDer(&x509->derCert, derSz, CERT_TYPE, x509->heap) == 0) {
            XMEMCPY(x509->derCert->buffer, der, derSz);
            ret = WOLFSSL_SUCCESS;
        }
        else {
            WOLFSSL_MSG("Failed to allocate DER buffer for X509");
        }
    }
    else {
        WOLFSSL_MSG("Unable to make DER for X509 REQ");
    }
#ifdef WOLFSSL_SMALL_STACK
    XFREE(der, NULL, DYNAMIC_TYPE_TMP_BUFFER);
#endif
    return ret;
}

int wolfSSL_X509_REQ_add_extensions(WOLFSSL_X509* req,
        WOLF_STACK_OF(WOLFSSL_X509_EXTENSION)* ext_sk)
{
    WOLFSSL_X509_EXTENSION* ext = NULL;

    if (!req || !ext_sk) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    /* It is not an error if the stack is empty. */
    ext = ext_sk->data.ext;
    if (ext == NULL) {
        return WOLFSSL_SUCCESS;
    }

    while (ext_sk) {
        ext = ext_sk->data.ext;

        if (wolfSSL_X509_add_ext(req, ext, -1) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("wolfSSL_X509_add_ext error");
            return WOLFSSL_FAILURE;
        }

        ext_sk = ext_sk->next;
    }

    return regenX509REQDerBuffer(req);
}

int wolfSSL_X509_REQ_add1_attr_by_txt(WOLFSSL_X509 *req,
                              const char *attrname, int type,
                              const unsigned char *bytes, int len)
{
    WOLFSSL_ENTER("wolfSSL_X509_REQ_add1_attr_by_txt");

#ifdef HAVE_LIBEST
    if (!req || !attrname || !bytes || type != MBSTRING_ASC) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if (len < 0) {
        len = (int)XSTRLEN((char*)bytes);
    }

    /* For now just pretend that we support this for libest testing */
    if (len == XSTR_SIZEOF("1.3.6.1.1.1.1.22") &&
            XMEMCMP("1.3.6.1.1.1.1.22", bytes, len) == 0) {
        /* MAC Address */
    }
    else if (len == XSTR_SIZEOF("1.2.840.10045.2.1") &&
            XMEMCMP("1.2.840.10045.2.1", bytes, len) == 0) {
        /* ecPublicKey */
    }
    else if (len == XSTR_SIZEOF("1.2.840.10045.4.3.3") &&
            XMEMCMP("1.2.840.10045.4.3.3", bytes, len) == 0) {
        /* ecdsa-with-SHA384 */
    }
    else {
        return WOLFSSL_FAILURE;
    }

    /* return error if not built for libest */
    return WOLFSSL_SUCCESS;
#else
    (void)req;
    (void)attrname;
    (void)type;
    (void)bytes;
    (void)len;
    return WOLFSSL_FAILURE;
#endif
}


static int wolfSSL_X509_ATTRIBUTE_set(WOLFSSL_X509_ATTRIBUTE* attr,
        const char* data, int dataSz, int type, int nid)
{
    if (attr) {
        attr->value->value.asn1_string = wolfSSL_ASN1_STRING_new();
        if (wolfSSL_ASN1_STRING_set(attr->value->value.asn1_string,
                data, dataSz) != WOLFSSL_SUCCESS) {
            wolfSSL_ASN1_STRING_free(attr->value->value.asn1_string);
            WOLFSSL_MSG("wolfSSL_ASN1_STRING_set error");
            return WOLFSSL_FAILURE;
        }
        attr->value->type = type;
        attr->object->nid = nid;
    }
    else {
        WOLFSSL_MSG("wolfSSL_X509_ATTRIBUTE_new error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}


int wolfSSL_X509_REQ_add1_attr_by_NID(WOLFSSL_X509 *req,
                                      int nid, int type,
                                      const unsigned char *bytes,
                                      int len)
{
    int ret;
    WOLFSSL_X509_ATTRIBUTE* attr;

    WOLFSSL_ENTER("wolfSSL_X509_REQ_add1_attr_by_NID");

    if (!req || !bytes || type != MBSTRING_ASC) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    switch (nid) {
    case NID_pkcs9_challengePassword:
        if (len < 0)
            len = (int)XSTRLEN((char*)bytes);
        if (len < CTC_NAME_SIZE) {
            XMEMCPY(req->challengePw, bytes, len);
            req->challengePw[len] = '\0';
        }
        else {
            WOLFSSL_MSG("Challenge password too long");
            WOLFSSL_ERROR_VERBOSE(BUFFER_E);
            return WOLFSSL_FAILURE;
        }
        break;
    case NID_serialNumber:
        if (len < 0)
            len = (int)XSTRLEN((char*)bytes);
        if (len + 1 > EXTERNAL_SERIAL_SIZE) {
            WOLFSSL_MSG("SerialNumber too long");
            WOLFSSL_ERROR_VERBOSE(BUFFER_E);
            return WOLFSSL_FAILURE;
        }
        XMEMCPY(req->serial, bytes, len);
        req->serialSz = len;
        break;

    case NID_pkcs9_unstructuredName:
    case NID_pkcs9_contentType:
    case NID_surname:
    case NID_initials:
    case NID_givenName:
    case NID_dnQualifier:
        break;

    default:
        WOLFSSL_MSG("Unsupported attribute");
        return WOLFSSL_FAILURE;
    }

    attr = wolfSSL_X509_ATTRIBUTE_new();
    ret = wolfSSL_X509_ATTRIBUTE_set(attr, (const char*)bytes, len,
            V_ASN1_PRINTABLESTRING, nid);
    if (ret != WOLFSSL_SUCCESS) {
        wolfSSL_X509_ATTRIBUTE_free(attr);
    }
    else {
        if (req->reqAttributes == NULL) {
            req->reqAttributes = wolfSSL_sk_new_node(req->heap);
            if (req->reqAttributes != NULL) {
                req->reqAttributes->type = STACK_TYPE_X509_REQ_ATTR;
            }
        }
        ret = wolfSSL_sk_push(req->reqAttributes, attr);
        if (ret != WOLFSSL_SUCCESS) {
            wolfSSL_X509_ATTRIBUTE_free(attr);
        }
    }

    return ret;
}

WOLFSSL_X509 *wolfSSL_X509_to_X509_REQ(WOLFSSL_X509 *x,
        WOLFSSL_EVP_PKEY *pkey, const WOLFSSL_EVP_MD *md)
{
    WOLFSSL_ENTER("wolfSSL_X509_to_X509_REQ");
    (void)pkey;
    (void)md;
    return wolfSSL_X509_dup(x);
}

int wolfSSL_X509_REQ_set_subject_name(WOLFSSL_X509 *req,
                                      WOLFSSL_X509_NAME *name)
{
    return wolfSSL_X509_set_subject_name(req, name);
}

int wolfSSL_X509_REQ_set_pubkey(WOLFSSL_X509 *req, WOLFSSL_EVP_PKEY *pkey)
{
    return wolfSSL_X509_set_pubkey(req, pkey);
}
#endif /* OPENSSL_ALL && WOLFSSL_CERT_GEN && WOLFSSL_CERT_REQ */

#if (defined(OPENSSL_ALL) || defined(OPENSSL_EXTRA)) && \
    (defined(WOLFSSL_CERT_GEN) || defined(WOLFSSL_CERT_REQ))

WOLFSSL_ASN1_TYPE *wolfSSL_X509_ATTRIBUTE_get0_type(
        WOLFSSL_X509_ATTRIBUTE *attr, int idx)
{
    WOLFSSL_ENTER("wolfSSL_X509_ATTRIBUTE_get0_type");

    if (!attr || idx != 0) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    return attr->value;
}


/**
 * @param req X509_REQ containing attribute
 * @return the number of attributes
 */
int wolfSSL_X509_REQ_get_attr_count(const WOLFSSL_X509 *req)
{
    if (req == NULL || req->reqAttributes == NULL)
        return 0;

    return wolfSSL_sk_num(req->reqAttributes);
}


/**
 * @param req X509_REQ containing attribute
 * @param loc NID of the attribute to return
 */
WOLFSSL_X509_ATTRIBUTE *wolfSSL_X509_REQ_get_attr(
        const WOLFSSL_X509 *req, int loc)
{
    WOLFSSL_ENTER("wolfSSL_X509_REQ_get_attr");

    if (!req || req->reqAttributes == NULL) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    return (WOLFSSL_X509_ATTRIBUTE*)wolfSSL_sk_value(req->reqAttributes, loc);
}

/* Return NID as the attr index */
int wolfSSL_X509_REQ_get_attr_by_NID(const WOLFSSL_X509 *req,
        int nid, int lastpos)
{
    WOLFSSL_STACK* sk;
    int idx;

    WOLFSSL_ENTER("wolfSSL_X509_REQ_get_attr_by_NID");

    if (!req) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FATAL_ERROR;
    }

    /* search through stack for first matching nid */
    idx = lastpos + 1;
    do {
        sk = wolfSSL_sk_get_node(req->reqAttributes, idx);
        if (sk != NULL) {
            WOLFSSL_X509_ATTRIBUTE* attr;
            attr = (WOLFSSL_X509_ATTRIBUTE*)sk->data.generic;
            if (nid == attr->object->nid) {
                /* found a match */
                break;
            }
        }
        idx++;
    } while (sk != NULL);

    /* no matches found */
    if (sk == NULL) {
        idx = WOLFSSL_FATAL_ERROR;
    }

    return idx;
}

WOLFSSL_X509_ATTRIBUTE* wolfSSL_X509_ATTRIBUTE_new(void)
{
    WOLFSSL_X509_ATTRIBUTE* ret;
    WOLFSSL_ENTER("wolfSSL_X509_ATTRIBUTE_new");
    ret = (WOLFSSL_X509_ATTRIBUTE*)XMALLOC(sizeof(WOLFSSL_X509_ATTRIBUTE),
            NULL, DYNAMIC_TYPE_OPENSSL);
    if (!ret) {
        WOLFSSL_MSG("malloc error");
        return NULL;
    }
    XMEMSET(ret, 0, sizeof(WOLFSSL_X509_ATTRIBUTE));
    ret->object = wolfSSL_ASN1_OBJECT_new();
    ret->value = wolfSSL_ASN1_TYPE_new();
    /* Don't allocate ret->set since WOLFSSL_ASN1_TYPE
     * is not supported as a stack type */
    if (!ret->object || !ret->value) {
        WOLFSSL_MSG("wolfSSL_ASN1_OBJECT_new or wolfSSL_ASN1_TYPE_new error");
        wolfSSL_X509_ATTRIBUTE_free(ret);
        return NULL;
    }
    return ret;
}

void wolfSSL_X509_ATTRIBUTE_free(WOLFSSL_X509_ATTRIBUTE* attr)
{
    WOLFSSL_ENTER("wolfSSL_X509_ATTRIBUTE_free");
    if (attr) {
        if (attr->object) {
            wolfSSL_ASN1_OBJECT_free(attr->object);
        }
        if (attr->value) {
            wolfSSL_ASN1_TYPE_free(attr->value);
        }
        if (attr->set) {
            wolfSSL_sk_pop_free(attr->set, NULL);
        }
        XFREE(attr, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}
#endif

#endif /* !NO_CERT */

#endif /* !WOLFCRYPT_ONLY */

#endif /* WOLFSSL_X509_INCLUDED */
