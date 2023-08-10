/* conf.c
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

#if !defined(WOLFSSL_CONF_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning conf.c does not need to be compiled separately from ssl.c
    #endif
#else

/*******************************************************************************
 * START OF TXT_DB API
 ******************************************************************************/

#if defined(OPENSSL_ALL) && !defined(NO_BIO)
/**
 * This function reads a tab delimetered CSV input and returns
 * a populated WOLFSSL_TXT_DB structure.
 * @param in Tab delimetered CSV input
 * @param num Number of fields in each row.
 * @return
 */
WOLFSSL_TXT_DB *wolfSSL_TXT_DB_read(WOLFSSL_BIO *in, int num)
{
    WOLFSSL_TXT_DB *ret = NULL;
    char *buf = NULL;
    char *bufEnd = NULL;
    char *idx = NULL;
    char* lineEnd = NULL;
    int bufSz;
    int failed = 1;
    /* Space in front of str reserved for field pointers + \0 */
    int fieldsSz = (num + 1) * sizeof(char *);
    WOLFSSL_ENTER("wolfSSL_TXT_DB_read");

    if (!in || num <= 0 || num > WOLFSSL_TXT_DB_MAX_FIELDS) {
        WOLFSSL_MSG("Bad parameter or too many fields");
        return NULL;
    }

    if (!(ret = (WOLFSSL_TXT_DB*)XMALLOC(sizeof(WOLFSSL_TXT_DB), NULL,
            DYNAMIC_TYPE_OPENSSL))) {
        WOLFSSL_MSG("malloc error");
        goto error;
    }
    XMEMSET (ret, 0, sizeof(WOLFSSL_TXT_DB));
    ret->num_fields = num;

    if (!(ret->data = wolfSSL_sk_WOLFSSL_STRING_new())) {
        WOLFSSL_MSG("wolfSSL_sk_WOLFSSL_STRING_new error");
        goto error;
    }

    bufSz = wolfSSL_BIO_get_len(in);
    if (bufSz <= 0 ||
            !(buf = (char*)XMALLOC(bufSz+1, NULL,
                    DYNAMIC_TYPE_TMP_BUFFER))) {
        WOLFSSL_MSG("malloc error or no data in BIO");
        goto error;
    }

    if (wolfSSL_BIO_read(in, buf, bufSz) != bufSz) {
        WOLFSSL_MSG("malloc error or no data in BIO");
        goto error;
    }

    buf[bufSz] = '\0';
    idx = buf;
    for (bufEnd = buf + bufSz; idx < bufEnd; idx = lineEnd + 1) {
        char* strBuf = NULL;
        char** fieldPtr = NULL;
        int fieldPtrIdx = 0;
        char* fieldCheckIdx = NULL;
        lineEnd = XSTRNSTR(idx, "\n", (unsigned int)(bufEnd - idx));
        if (!lineEnd)
            lineEnd = bufEnd;
        if (idx == lineEnd) /* empty line */
            continue;
        if (*idx == '#')
            continue;
        *lineEnd = '\0';
        strBuf = (char*)XMALLOC(fieldsSz + lineEnd - idx + 1, NULL,
                                DYNAMIC_TYPE_OPENSSL);
        if (!strBuf) {
            WOLFSSL_MSG("malloc error");
            goto error;
        }
        XMEMCPY(strBuf + fieldsSz, idx, lineEnd - idx + 1); /* + 1 for NULL */
        XMEMSET(strBuf, 0, fieldsSz);
        /* Check for appropriate number of fields */
        fieldPtr = (char**)strBuf;
        fieldCheckIdx = strBuf + fieldsSz;
        fieldPtr[fieldPtrIdx++] = fieldCheckIdx;
        while (*fieldCheckIdx != '\0') {
            /* Handle escaped tabs */
            if (*fieldCheckIdx == '\t' && fieldCheckIdx[-1] != '\\') {
                fieldPtr[fieldPtrIdx++] = fieldCheckIdx + 1;
                *fieldCheckIdx = '\0';
                if (fieldPtrIdx > num) {
                    WOLFSSL_MSG("too many fields");
                    XFREE(strBuf, NULL, DYNAMIC_TYPE_OPENSSL);
                    goto error;
                }
            }
            fieldCheckIdx++;
        }
        if (fieldPtrIdx != num) {
            WOLFSSL_MSG("wrong number of fields");
            XFREE(strBuf, NULL, DYNAMIC_TYPE_OPENSSL);
            goto error;
        }
        if (wolfSSL_sk_push(ret->data, strBuf) != WOLFSSL_SUCCESS) {
            WOLFSSL_MSG("wolfSSL_sk_push error");
            XFREE(strBuf, NULL, DYNAMIC_TYPE_OPENSSL);
            goto error;
        }
    }

    failed = 0;
error:
    if (failed && ret) {
        wolfSSL_TXT_DB_free(ret);
        ret = NULL;
    }
    if (buf) {
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    }
    return ret;
}

long wolfSSL_TXT_DB_write(WOLFSSL_BIO *out, WOLFSSL_TXT_DB *db)
{
    const WOLF_STACK_OF(WOLFSSL_STRING)* data;
    long totalLen = 0;
    char buf[512]; /* Should be more than enough for a single row */
    char* bufEnd = buf + sizeof(buf);
    int i;

    WOLFSSL_ENTER("wolfSSL_TXT_DB_write");

    if (!out || !db || !db->num_fields) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    data = db->data;
    while (data) {
        char** fields = (char**)data->data.string;
        char* idx = buf;
        int sz;

        if (!fields) {
            WOLFSSL_MSG("Missing row");
            return WOLFSSL_FAILURE;
        }

        for (i = 0; i < db->num_fields; i++) {
            const char* fieldValue = fields[i];
            if (!fieldValue) {
                fieldValue = "";
            }

            /* Copy over field escaping tabs */
            while (*fieldValue != '\0') {
                if (idx+1 < bufEnd) {
                    if (*fieldValue == '\t')
                        *idx++ = '\\';
                    *idx++ = *fieldValue++;
                }
                else {
                    WOLFSSL_MSG("Data row is too big");
                    return WOLFSSL_FAILURE;
                }
            }
            if (idx < bufEnd) {
                *idx++ = '\t';
            }
            else {
                WOLFSSL_MSG("Data row is too big");
                return WOLFSSL_FAILURE;
            }
        }
        idx[-1] = '\n';
        sz = (int)(idx - buf);

        if (wolfSSL_BIO_write(out, buf, sz) != sz) {
            WOLFSSL_MSG("wolfSSL_BIO_write error");
            return WOLFSSL_FAILURE;
        }
        totalLen += sz;

        data = data->next;
    }

    return totalLen;
}

int wolfSSL_TXT_DB_insert(WOLFSSL_TXT_DB *db, WOLFSSL_STRING *row)
{
    WOLFSSL_ENTER("wolfSSL_TXT_DB_insert");

    if (!db || !row || !db->data) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if (wolfSSL_sk_push(db->data, row) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_sk_push error");
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

void wolfSSL_TXT_DB_free(WOLFSSL_TXT_DB *db)
{
    WOLFSSL_ENTER("wolfSSL_TXT_DB_free");
    if (db) {
        if (db->data) {
            wolfSSL_sk_pop_free(db->data, NULL);
        }
        XFREE(db, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

int wolfSSL_TXT_DB_create_index(WOLFSSL_TXT_DB *db, int field,
        void* qual, wolf_sk_hash_cb hash, wolf_lh_compare_cb cmp)
{
    WOLFSSL_ENTER("wolfSSL_TXT_DB_create_index");
    (void)qual;
    (void)cmp;

    if (!db || !hash || !cmp || field >= db->num_fields || field < 0) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    db->hash_fn[field] = hash;

    return WOLFSSL_SUCCESS;
}

WOLFSSL_STRING *wolfSSL_TXT_DB_get_by_index(WOLFSSL_TXT_DB *db, int idx,
        WOLFSSL_STRING *value)
{
    WOLFSSL_ENTER("wolfSSL_TXT_DB_get_by_index");

    if (!db || !db->data || idx < 0 || idx >= db->num_fields) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    if (!db->hash_fn[idx]) {
        WOLFSSL_MSG("Missing hash functions");
        return NULL;
    }

    /* If first data struct has correct hash function
     * then assume others do too */
    if (db->data->hash_fn != db->hash_fn[idx]) {
        /* Set the hash and comp functions */
        WOLF_STACK_OF(WOLFSSL_STRING)* data = db->data;
        while (data) {
            if (data->hash_fn != db->hash_fn[idx]) {
                data->hash_fn = db->hash_fn[idx];
                data->hash = 0;
            }
            data= data->next;
        }
    }
    return (WOLFSSL_STRING*) wolfSSL_lh_retrieve(db->data, value);
}

#endif /* OPENSSL_ALL && !NO_BIO */

/*******************************************************************************
 * END OF TXT_DB API
 ******************************************************************************/

/*******************************************************************************
 * START OF CONF API
 ******************************************************************************/

#if defined(OPENSSL_ALL) || defined(WOLFSSL_NGINX) || defined(WOLFSSL_HAPROXY) \
 || defined(HAVE_STUNNEL)
#ifndef NO_WOLFSSL_STUB
void wolfSSL_OPENSSL_config(char *config_name)
{
    (void)config_name;
    WOLFSSL_STUB("OPENSSL_config");
}
#endif /* !NO_WOLFSSL_STUB */
#endif /* OPENSSL_ALL || WOLFSSL_NGINX || WOLFSSL_HAPROXY || HAVE_STUNNEL*/

#if !defined(NO_CERTS) && defined(OPENSSL_EXTRA) && defined(OPENSSL_ALL)

/**
 * This is the same hashing algo for WOLFSSL_CONF_VALUE as OpenSSL
 */
static unsigned long wolfSSL_CONF_VALUE_hash(const WOLFSSL_CONF_VALUE *val)
{
    if (val)
        return (wolfSSL_LH_strhash(val->section) << 2) ^
                wolfSSL_LH_strhash(val->name);
    else
        return 0;
}

/* Use SHA[256] for hashing as OpenSSL uses a hash algorithm that is
 * "not as good as MD5, but still good" so using SHA should be more
 * than good enough for this application. The produced hashes don't
 * need to line up between OpenSSL and wolfSSL. The hashes are for
 * internal indexing only */
unsigned long wolfSSL_LH_strhash(const char *str)
{
    unsigned long ret = 0;
    int strLen;
#if !defined(NO_SHA)
    wc_Sha sha;
    byte digest[WC_SHA_DIGEST_SIZE];
#elif !defined(NO_SHA256)
    wc_Sha256 sha;
    byte digest[WC_SHA256_DIGEST_SIZE];
#endif
    WOLFSSL_ENTER("wolfSSL_LH_strhash");

    if (!str)
        return 0;
    strLen = (int)XSTRLEN(str);

#if !defined(NO_SHA)
    if (wc_InitSha_ex(&sha, NULL, 0) != 0) {
        WOLFSSL_MSG("SHA1 Init failed");
        return 0;
    }

    ret = wc_ShaUpdate(&sha, (const byte *)str, (word32)strLen);
    if (ret != 0) {
        WOLFSSL_MSG("SHA1 Update failed");
    } else {
        ret = wc_ShaFinal(&sha, digest);
        if (ret != 0) {
            WOLFSSL_MSG("SHA1 Final failed");
        }
    }
    wc_ShaFree(&sha);
#elif !defined(NO_SHA256)
    if (wc_InitSha256_ex(&sha, NULL, 0) != 0) {
        WOLFSSL_MSG("SHA256 Init failed");
        return 0;
    }

    ret = wc_Sha256Update(&sha, (const byte *)str, (word32)strLen);
    if (ret != 0) {
        WOLFSSL_MSG("SHA256 Update failed");
    } else {
        ret = wc_Sha256Final(&sha, digest);
        if (ret != 0) {
            WOLFSSL_MSG("SHA256 Final failed");
        }
    }
    wc_Sha256Free(&sha);
#endif

#if !defined(NO_SHA) || !defined(NO_SHA256)
    if (ret != 0)
        return 0;

    /* Take first 4 bytes in small endian as unsigned long */
    ret  =  (unsigned int)digest[0];
    ret |= ((unsigned int)digest[1] << 8 );
    ret |= ((unsigned int)digest[2] << 16);
    ret |= ((unsigned int)digest[3] << 24);
#else
    WOLFSSL_MSG("No SHA available for wolfSSL_LH_strhash");
#endif
    return ret;
}

WOLFSSL_CONF_VALUE *wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve(
        WOLF_LHASH_OF(WOLFSSL_CONF_VALUE) *sk, WOLFSSL_CONF_VALUE *data)
{
    WOLFSSL_ENTER("wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve");

    if (!sk || !data) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    return (WOLFSSL_CONF_VALUE*)wolfSSL_lh_retrieve(sk, data);
}

int wolfSSL_CONF_modules_load(const WOLFSSL_CONF *cnf, const char *appname,
                      unsigned long flags)
{
    WOLFSSL_ENTER("wolfSSL_CONF_modules_load");
    WOLFSSL_MSG("All wolfSSL modules are already compiled in. "
                "wolfSSL_CONF_modules_load doesn't load anything new.");
    (void)cnf;
    (void)appname;
    (void)flags;
    return WOLFSSL_SUCCESS;
}

WOLFSSL_CONF_VALUE *wolfSSL_CONF_VALUE_new(void)
{
    WOLFSSL_CONF_VALUE* ret;

    WOLFSSL_ENTER("wolfSSL_CONF_new");

    ret = (WOLFSSL_CONF_VALUE*)XMALLOC(sizeof(WOLFSSL_CONF_VALUE),
            NULL, DYNAMIC_TYPE_OPENSSL);
    if (ret)
        XMEMSET(ret, 0, sizeof(WOLFSSL_CONF_VALUE));
    return ret;
}

int wolfSSL_CONF_add_string(WOLFSSL_CONF *conf,
        WOLFSSL_CONF_VALUE *section, WOLFSSL_CONF_VALUE *value)
{
    WOLF_STACK_OF(WOLFSSL_CONF_VALUE) *sk = NULL;

    if (!conf || !section || !value) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    sk = (WOLF_STACK_OF(WOLFSSL_CONF_VALUE) *)section->value;
    value->section = section->section;

    if (wolfSSL_sk_CONF_VALUE_push(sk, value) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_sk_CONF_VALUE_push error");
        return WOLFSSL_FAILURE;
    }
    if (wolfSSL_sk_CONF_VALUE_push(conf->data, value) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_sk_CONF_VALUE_push error");
        wolfssl_sk_pop_type(sk, STACK_TYPE_CONF_VALUE);
        return WOLFSSL_FAILURE;
    }

    return WOLFSSL_SUCCESS;
}

WOLFSSL_CONF_VALUE *wolfSSL_CONF_new_section(WOLFSSL_CONF *conf,
        const char *section)
{
    WOLFSSL_CONF_VALUE* ret = NULL;
    WOLF_STACK_OF(WOLFSSL_CONF_VALUE) *sk = NULL;
    int slen;

    WOLFSSL_ENTER("wolfSSL_CONF_new_section");

    if (!conf || !section) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    slen = (int)XSTRLEN(section);

    if (!(ret = wolfSSL_CONF_VALUE_new())) {
        WOLFSSL_MSG("wolfSSL_CONF_new error");
        goto error;
    }

    if (!(ret->section = (char*)XMALLOC(slen+1, NULL, DYNAMIC_TYPE_OPENSSL))) {
        WOLFSSL_MSG("section malloc error");
        goto error;
    }
    XMEMCPY(ret->section, section, slen+1);

    if (!(sk = wolfSSL_sk_CONF_VALUE_new(NULL))) {
        WOLFSSL_MSG("wolfSSL_sk_CONF_VALUE_new error");
        goto error;
    }

    ret->value = (char*)sk;

    if (wolfSSL_sk_CONF_VALUE_push(conf->data, ret) != WOLFSSL_SUCCESS) {
        WOLFSSL_MSG("wolfSSL_sk_CONF_VALUE_push error");
        goto error;
    }

    return ret;
error:
    if (ret) {
        /* NULL so that wolfSSL_X509V3_conf_free doesn't attempt to free it */
        ret->value = NULL;
        wolfSSL_X509V3_conf_free(ret);
    }
    if (sk) {
        wolfSSL_sk_CONF_VALUE_free(sk);
    }
    return NULL;
}

WOLFSSL_CONF_VALUE *wolfSSL_CONF_get_section(WOLFSSL_CONF *conf,
        const char *section)
{
    WOLF_STACK_OF(WOLFSSL_CONF_VALUE) *sk = NULL;

    WOLFSSL_ENTER("wolfSSL_CONF_get_section");

    if (!conf || !section) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    sk = conf->data;

    while (sk) {
        WOLFSSL_CONF_VALUE* val = sk->data.conf;
        if (val) {
            if (!val->name && XSTRCMP(section, val->section) == 0) {
                return val;
            }
        }
        sk = sk->next;
    }

    return NULL;
}

WOLFSSL_CONF *wolfSSL_NCONF_new(void *meth)
{
    WOLFSSL_CONF* ret;
    WOLFSSL_ENTER("wolfSSL_NCONF_new");

    if (meth) {
        WOLFSSL_MSG("wolfSSL does not support CONF_METHOD");
    }

    ret = (WOLFSSL_CONF*)XMALLOC(sizeof(WOLFSSL_CONF), NULL, DYNAMIC_TYPE_OPENSSL);
    if (ret) {
        XMEMSET(ret, 0, sizeof(WOLFSSL_CONF));
        ret->data = wolfSSL_sk_CONF_VALUE_new(NULL);
        if (!ret->data) {
            wolfSSL_NCONF_free(ret);
            return NULL;
        }
    }
    return ret;
}

char *wolfSSL_NCONF_get_string(const WOLFSSL_CONF *conf,
        const char *group, const char *name)
{
    WOLFSSL_CONF_VALUE find_val;
    WOLFSSL_CONF_VALUE *val;
    WOLFSSL_ENTER("wolfSSL_NCONF_get_string");

    if (!conf) {
#ifdef HAVE_SECURE_GETENV
        return secure_getenv(name);
#else
        WOLFSSL_MSG("Missing secure_getenv");
        return NULL;
#endif
    }

    find_val.name = (char *)name;
    if (group) {
        find_val.section = (char *)group;
        val = wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve(conf->data, &find_val);
        if (val)
            return val->value;
        if (XSTRCMP(group, "ENV") == 0) {
#ifdef HAVE_SECURE_GETENV
            return secure_getenv(name);
#else
        WOLFSSL_MSG("Missing secure_getenv");
            return NULL;
#endif
        }
    }

    find_val.section = (char *)"default";
    val = wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve(conf->data, &find_val);
    if (val)
        return val->value;
    else
        return NULL;
}

int wolfSSL_NCONF_get_number(const CONF *conf, const char *group,
        const char *name, long *result)
{
    char *str;
    WOLFSSL_ENTER("wolfSSL_NCONF_get_number");

    if (!conf || !name || !result) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    if (!(str = wolfSSL_NCONF_get_string(conf, group, name))) {
        WOLFSSL_MSG("wolfSSL_NCONF_get_string error");
        return WOLFSSL_FAILURE;
    }

    *result = atol(str);
    return WOLFSSL_SUCCESS;
}

/**
 * The WOLFSSL_CONF->value member is treated as a
 * WOLFSSL_STACK_OF(WOLFSSL_CONF_VALUE) which becomes
 * the return value.
 * @param conf
 * @param section
 * @return WOLFSSL_STACK_OF(WOLFSSL_CONF_VALUE)
 */
WOLFSSL_STACK *wolfSSL_NCONF_get_section(
        const WOLFSSL_CONF *conf, const char *section)
{
    WOLFSSL_CONF_VALUE *val;
    WOLFSSL_CONF_VALUE find_val;

    WOLFSSL_ENTER("wolfSSL_NCONF_get_section");

    if (!conf || !section) {
        WOLFSSL_MSG("Bad parameter");
        return NULL;
    }

    find_val.name = NULL;
    find_val.section = (char*)section;
    val = wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve(conf->data, &find_val);
    if (val)
        return (WOLFSSL_STACK*)val->value;
    else
        return NULL;
}

#if  !defined(NO_BIO)
static WOLFSSL_CONF_VALUE *wolfSSL_CONF_VALUE_new_values(char* section,
        char* name, char* value)
{
    WOLFSSL_CONF_VALUE* ret;
    int len;

    WOLFSSL_ENTER("wolfSSL_CONF_VALUE_new_values");

    if (!(ret = wolfSSL_CONF_VALUE_new())) {
        WOLFSSL_MSG("wolfSSL_CONF_VALUE_new error");
        return NULL;
    }

    if (section) {
        len = (int)XSTRLEN(section);
        ret->section = (char*)XMALLOC(len+1, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!ret->section) {
            WOLFSSL_MSG("malloc error");
            wolfSSL_X509V3_conf_free(ret);
            return NULL;
        }
        XMEMCPY(ret->section, section, len+1);
    }

    if (name) {
        len = (int)XSTRLEN(name);
        ret->name = (char*)XMALLOC(len+1, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!ret->name) {
            WOLFSSL_MSG("malloc error");
            wolfSSL_X509V3_conf_free(ret);
            return NULL;
        }
        XMEMCPY(ret->name, name, len+1);
    }

    if (value) {
        len = (int)XSTRLEN(value);
        ret->value = (char*)XMALLOC(len+1, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!ret->value) {
            WOLFSSL_MSG("malloc error");
            wolfSSL_X509V3_conf_free(ret);
            return NULL;
        }
        XMEMCPY(ret->value, value, len+1);
    }

    return ret;
}

static char* expandValue(WOLFSSL_CONF *conf, const char* section,
        char *str)
{
    int strLen = (int)XSTRLEN(str);
    char* ret = NULL;

    /* Check to see if there is anything to expand */
    if (XSTRNSTR(str, "$", strLen)) {
        int idx = 0;
        char* strIdx = str;
        ret = (char*)XMALLOC(strLen + 1, NULL, DYNAMIC_TYPE_OPENSSL);
        if (!ret) {
            WOLFSSL_MSG("malloc error");
            return str;
        }

        while (*strIdx) {
            if (*strIdx == '$') {
                /* Expand variable */
                char* startIdx = ++strIdx;
                char* endIdx;
                const char* s = section;
                const char* value;
                char prevValue;

                if (*startIdx == '{') {
                    /* First read the section.
                     * format: ${section_name::var_name} */
                    s = ++startIdx;
                    while (*strIdx && *strIdx != ':') strIdx++;
                    if (!*strIdx || s == strIdx || strIdx[1] != ':') {
                        WOLFSSL_MSG("invalid section name in "
                                    "variable expansion");
                        goto expand_cleanup;
                    }
                    *strIdx = '\0';
                    strIdx += 2;
                    startIdx = strIdx;
                }
                while (*strIdx && (XISALNUM((int)(*strIdx)) || *strIdx == '_'))
                    strIdx++;
                endIdx = strIdx;
                if (startIdx == endIdx) {
                    WOLFSSL_MSG("invalid variable name in config");
                    goto expand_cleanup;
                }
                if (s != section) {
                    /* We are expecting a trailing '}' */
                    if (*strIdx != '}') {
                        WOLFSSL_MSG("Missing '}' in variable");
                        goto expand_cleanup;
                    }
                    strIdx++;
                }
                /* Save char value at the end of the name so that we can place
                 * a null char there. */
                prevValue = *endIdx;
                *endIdx = '\0';
                value = wolfSSL_NCONF_get_string(conf, s, startIdx);
                *endIdx = prevValue;
                /* Skip copy if no value or zero-length value */
                if (value && *value) {
                    int valueLen = (int)XSTRLEN(value);
                    char* newRet;
                    /* This will allocate slightly more memory than necessary
                     * but better be safe */
                    strLen += valueLen;
                    newRet = (char*)XREALLOC(ret, strLen + 1, NULL,
                            DYNAMIC_TYPE_OPENSSL);
                    if (!newRet) {
                        WOLFSSL_MSG("realloc error");
                        goto expand_cleanup;
                    }
                    ret = newRet;
                    XMEMCPY(ret + idx, value, valueLen);
                    idx += valueLen;
                }
            }
            else {
                ret[idx++] = *strIdx++;
            }
        }
        ret[idx] = '\0';
    }

    return ret ? ret : str;

expand_cleanup:
    if (ret)
        XFREE(ret, NULL, DYNAMIC_TYPE_OPENSSL);
    return NULL;
}

#define SKIP_WHITESPACE(idx, max_idx) \
    while ((idx) < (max_idx) && (*(idx) == ' ' || *(idx) == '\t'))      \
        {(idx)++;}
int wolfSSL_NCONF_load(WOLFSSL_CONF *conf, const char *file, long *eline)
{
    int ret = WOLFSSL_FAILURE;
    WOLFSSL_BIO *in = NULL;
    char* buf = NULL;
    char* idx = NULL;
    char* bufEnd = NULL;
    CONF_VALUE* section = NULL;
    long line = 0;
    int bufLen = 0;

    if (!conf || !file) {
        WOLFSSL_MSG("Bad parameter");
        return WOLFSSL_FAILURE;
    }

    /* Open file */
    if (!(in = wolfSSL_BIO_new_file(file, "rb"))) {
        WOLFSSL_MSG("wolfSSL_BIO_new_file error");
        return WOLFSSL_FAILURE;
    }

    /* Read file */
    bufLen = wolfSSL_BIO_get_len(in);
    if (bufLen <= 0) {
        WOLFSSL_MSG("wolfSSL_BIO_get_len error");
        goto cleanup;
    }
    if (!(buf = (char*)XMALLOC(bufLen + 1, NULL, DYNAMIC_TYPE_TMP_BUFFER))) {
        WOLFSSL_MSG("malloc error");
        goto cleanup;
    }
    if (wolfSSL_BIO_read(in, buf, bufLen) != bufLen) {
        WOLFSSL_MSG("wolfSSL_BIO_read error");
        goto cleanup;
    }

    if (!(section = wolfSSL_CONF_new_section(conf, "default"))) {
        WOLFSSL_MSG("wolfSSL_CONF_new_section error");
        goto cleanup;
    }

    /* LETS START READING SOME CONFIGS */
    idx = buf;
    bufEnd = buf + bufLen;
    while (idx < bufEnd) {
        char* lineEnd = XSTRNSTR(idx, "\n", (unsigned int)(bufEnd - idx));
        char* maxIdx;
        if (!lineEnd)
            lineEnd = bufEnd; /* Last line in file */
        maxIdx = XSTRNSTR(idx, "#", (unsigned int)(lineEnd - idx));
        if (!maxIdx)
            maxIdx = lineEnd;
        line++;
        SKIP_WHITESPACE(idx, maxIdx);
        if (idx == maxIdx) {
            /* Empty line */
            idx = lineEnd + 1;
            continue;
        }

        if (*idx == '[') {
            /* New section. Spaces not allowed in section name. */
            char* sectionName;
            int sectionNameLen;

            if (idx < maxIdx)
                idx++;
            else {
                WOLFSSL_MSG("Invalid section definition.");
                goto cleanup;
            }

            SKIP_WHITESPACE(idx, maxIdx);
            sectionName = idx;
            /* Find end of section name */
            while (idx < maxIdx && *idx != ' ' && *idx != ']')
                idx++;
            sectionNameLen = (int)(idx - sectionName);
            SKIP_WHITESPACE(idx, maxIdx);

            if (*idx != ']') {
                WOLFSSL_MSG("Section definition error. "
                            "Closing brace not found.");
                goto cleanup;
            }

            sectionName[sectionNameLen] = '\0';
            if (!(section = wolfSSL_CONF_get_section(conf, sectionName))) {
                section = wolfSSL_CONF_new_section(conf, sectionName);
                if (!section)
                    goto cleanup;
            }
        }
        else {
            char* name;
            int   nameLen;
            char* value;
            char* exValue; /* expanded value */
            int   valueLen;
            WOLFSSL_CONF_VALUE* newVal = NULL;

            SKIP_WHITESPACE(idx, maxIdx);
            name = idx;
            /* Find end of name */
            while (idx < maxIdx && *idx != ' ' && *idx != '=')
                idx++;
            nameLen = (int)(idx - name);
            SKIP_WHITESPACE(idx, maxIdx);
            if (*idx != '=') {
                WOLFSSL_MSG("Missing equals sign");
                goto cleanup;
            }
            idx++;
            SKIP_WHITESPACE(idx, maxIdx);
            value = idx;
            /* Find end of value */
            idx = maxIdx-1;
            while (idx >= value && (*idx == ' ' || *idx == '\t' || *idx == '\r'))
                idx--;
            valueLen = (int)(idx - value + 1);

            /* Sanity checks */
            if (nameLen <= 0 || valueLen <= 0) {
                WOLFSSL_MSG("Sanity checks failed");
                goto cleanup;
            }
            name[nameLen] = '\0';
            value[valueLen] = '\0';

            if (!(exValue = expandValue(conf, section->section, value))) {
                WOLFSSL_MSG("Variable expansion failed");
                goto cleanup;
            }

            if (!(newVal = wolfSSL_CONF_VALUE_new_values(NULL,
                    name, exValue))) {
                WOLFSSL_MSG("wolfSSL_CONF_VALUE_new_values error");
                if (exValue != value)
                    XFREE(exValue, NULL, DYNAMIC_TYPE_OPENSSL);
                goto cleanup;
            }

            if (exValue != value)
                XFREE(exValue, NULL, DYNAMIC_TYPE_OPENSSL);

            if (wolfSSL_CONF_add_string(conf, section, newVal) !=
                    WOLFSSL_SUCCESS) {
                wolfSSL_X509V3_conf_free(newVal);
                WOLFSSL_MSG("wolfSSL_CONF_add_string error");
                goto cleanup;
            }
        }
        idx = lineEnd + 1;
    }

    ret = WOLFSSL_SUCCESS;
cleanup:
    if (in)
        wolfSSL_BIO_free(in);
    if (buf)
        XFREE(buf, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    if (eline)
        *eline = line;
    return ret;
}
#endif /* !NO_BIO */

void wolfSSL_NCONF_free(WOLFSSL_CONF *conf)
{
    WOLFSSL_ENTER("wolfSSL_NCONF_free");
    if (conf) {
        wolfSSL_sk_CONF_VALUE_free(conf->data);
        XFREE(conf, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

void wolfSSL_X509V3_conf_free(WOLFSSL_CONF_VALUE *val)
{
    WOLF_STACK_OF(WOLFSSL_CONF_VALUE) *sk = NULL;

    if (val) {
        if (val->name) {
            /* Not a section. Don't free section as it is a shared pointer. */
            XFREE(val->name, NULL, DYNAMIC_TYPE_OPENSSL);
            if (val->value)
                XFREE(val->value, NULL, DYNAMIC_TYPE_OPENSSL);
        }
        else {
            /* Section so val->value is a stack */
            if (val->section)
                XFREE(val->section, NULL, DYNAMIC_TYPE_OPENSSL);
            /* Only free the stack structures. The contained conf values
             * will be freed in wolfSSL_NCONF_free */
            sk = (WOLF_STACK_OF(WOLFSSL_CONF_VALUE)*)val->value;
            while (sk) {
                WOLF_STACK_OF(WOLFSSL_CONF_VALUE) *tmp = sk->next;
                XFREE(sk, NULL, DYNAMIC_TYPE_OPENSSL);
                sk = tmp;
            }
        }
        XFREE(val, NULL, DYNAMIC_TYPE_OPENSSL);
    }
}

WOLFSSL_STACK *wolfSSL_sk_CONF_VALUE_new(
    WOLF_SK_COMPARE_CB(WOLFSSL_CONF_VALUE, compFunc))
{
    WOLFSSL_STACK* ret;
    WOLFSSL_ENTER("wolfSSL_sk_CONF_VALUE_new");
    ret = wolfSSL_sk_new_node(NULL);
    if (!ret)
        return NULL;
    ret->hash_fn = (wolf_sk_hash_cb)wolfSSL_CONF_VALUE_hash;
    ret->type = STACK_TYPE_CONF_VALUE;
    (void)compFunc;
    return ret;
}

/* Free the structure for WOLFSSL_CONF_VALUE stack
 *
 * sk  stack to free nodes in
 */
void wolfSSL_sk_CONF_VALUE_free(WOLF_STACK_OF(WOLFSSL_CONF_VALUE)* sk)
{
    WOLFSSL_STACK* tmp;
    WOLFSSL_ENTER("wolfSSL_sk_CONF_VALUE_free");

    if (sk == NULL)
        return;

    /* parse through stack freeing each node */
    while (sk) {
        tmp = sk->next;
        wolfSSL_X509V3_conf_free(sk->data.conf);
        XFREE(sk, NULL, DYNAMIC_TYPE_OPENSSL);
        sk = tmp;
    }
}

int wolfSSL_sk_CONF_VALUE_num(const WOLFSSL_STACK *sk)
{
    WOLFSSL_ENTER("wolfSSL_sk_CONF_VALUE_num");
    if (sk)
        return wolfSSL_sk_num(sk);
    return 0;
}

WOLFSSL_CONF_VALUE *wolfSSL_sk_CONF_VALUE_value(const WOLFSSL_STACK *sk, int i)
{
    WOLFSSL_ENTER("wolfSSL_sk_CONF_VALUE_value");
    if (sk)
        return (WOLFSSL_CONF_VALUE*)wolfSSL_sk_value(sk, i);
    return NULL;
}

/* return 1 on success 0 on fail */
int wolfSSL_sk_CONF_VALUE_push(WOLF_STACK_OF(WOLFSSL_CONF_VALUE)* sk,
        WOLFSSL_CONF_VALUE* val)
{
    WOLFSSL_ENTER("wolfSSL_sk_CONF_VALUE_push");

    if (sk == NULL || val == NULL) {
        return WOLFSSL_FAILURE;
    }

    return wolfSSL_sk_push(sk, val);
}

#endif /* !NO_CERTS && OPENSSL_EXTRA && OPENSSL_ALL */

#ifdef OPENSSL_EXTRA
#ifndef NO_WOLFSSL_STUB
/* Returns default file name and path of config file. However
   a wolfssl.cnf file is not currently supported */
char* wolfSSL_CONF_get1_default_config_file(void)
{
    WOLFSSL_ENTER("wolfSSL_CONF_get1_default_config_file");
    WOLFSSL_STUB("CONF_get1_default_config_file");
    return NULL;
}
#endif

/**
 * Allocate WOLFSSL_CONF_CTX instance
 * @return pointer to WOLFSSL_CONF_CTX structure on success and NULL on fail
 */
WOLFSSL_CONF_CTX* wolfSSL_CONF_CTX_new(void)
{
    WOLFSSL_CONF_CTX* cctx;

    WOLFSSL_ENTER("wolfSSL_CONF_CTX_new");

    cctx = (WOLFSSL_CONF_CTX*)XMALLOC(sizeof(WOLFSSL_CONF_CTX), NULL,
                                                    DYNAMIC_TYPE_OPENSSL);
    if (!cctx) {
        WOLFSSL_MSG("malloc error");
        return NULL;
    }
    XMEMSET(cctx, 0, sizeof(WOLFSSL_CONF_CTX));

    return cctx;
}
/**
 * Release WOLFSSL_CONF_CTX instance
 * @param cctx a pointer to WOLFSSL_CONF_CTX structure to be freed
 */
void wolfSSL_CONF_CTX_free(WOLFSSL_CONF_CTX* cctx)
{
    WOLFSSL_ENTER("wolfSSL_CONF_CTX_free");

    XFREE(cctx, NULL, DYNAMIC_TYPE_OPENSSL);

    WOLFSSL_LEAVE("wolfSSL_CONF_CTX_free", 1);
}
/**
 * Set WOLFSSL_CTX instance to WOLFSSL_CONF_CTX
 * @param cctx a pointer to WOLFSSL_CONF_CTX structure to set a WOLFSSL_CTX
 *             pointer to its ctx
 * @param ctx  a pointer to WOLFSSL_CTX structure to be set
 */
void wolfSSL_CONF_CTX_set_ssl_ctx(WOLFSSL_CONF_CTX* cctx, WOLFSSL_CTX *ctx)
{
    WOLFSSL_ENTER("wolfSSL_CONF_CTX_set_ssl_ctx");

    /* sanity check */
    if (cctx == NULL) {
        WOLFSSL_MSG("cctx is null");
        return;
    }

    cctx->ctx = ctx;
    WOLFSSL_LEAVE("wolfSSL_CONF_CTX_set_ssl_ctx", 1);
}
/**
 * set flag value into WOLFSSL_CONF_CTX
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure to be set
 * @param flags flag value to be OR'd
 * @return OR'd flag value, otherwise 0
 */
unsigned int wolfSSL_CONF_CTX_set_flags(WOLFSSL_CONF_CTX* cctx,
                                                            unsigned int flags)
{
    /* sanity check */
    if (cctx == NULL)
        return 0;

    cctx->flags |= flags;
    return cctx->flags;
}

/**
 * finish configuration command operation
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure to be set
 * @return WOLFSSL_SUCCESS on success
 */
int wolfSSL_CONF_CTX_finish(WOLFSSL_CONF_CTX* cctx)
{
    (void)cctx;
    return WOLFSSL_SUCCESS;
}
/*
 * The following definitions and static functions are used for
 * wolfSSL_CONF_cmd() to handle command.
 *
 * Definitions below are a part of conf_cmds_tbl[] contents.
 *  WOLFSSL_CONF_FILE_CMDx  represents command name in configuration file
 *  WOLFSSL_CONF_CMDL_CMDx  represents command name on command line
 *
 * The static functions after the definition section process
 *                 those FILE or CMDL which are defined in the conf_cmds_tbl.
 *
 * To add a new command handling:
 *  1. Add new #define to a section of WOLFSSL_CONF_FILE_CMD* and
 *                                      WOLFSSL_CONF_CMDL_CMD*
 *  2. Add new static function after #define section, before
 *                      "typedef struct conf_cmd_tbl {" line
 *  3. Add new entry to conf_cmds_tbl[] by following other command entries
 */
#define WOLFSSL_CONF_FILE_CMD1  "Curves"
#define WOLFSSL_CONF_FILE_CMD2  "Certificate"
#define WOLFSSL_CONF_FILE_CMD3  "PrivateKey"
#define WOLFSSL_CONF_FILE_CMD4  "Protocol"
#define WOLFSSL_CONF_FILE_CMD5  "Options"
#define WOLFSSL_CONF_FILE_CMD6  "ServerInfoFile"
#define WOLFSSL_CONF_FILE_CMD7  "SignatureAlgorithms"
#define WOLFSSL_CONF_FILE_CMD8  "ClientSignatureAlgorithms"
#define WOLFSSL_CONF_FILE_CMD9  "CipherString"

#define WOLFSSL_CONF_CMDL_CMD1  "curves"
#define WOLFSSL_CONF_CMDL_CMD2  "cert"
#define WOLFSSL_CONF_CMDL_CMD3  "key"
#define WOLFSSL_CONF_CMDL_CMD4  NULL
#define WOLFSSL_CONF_CMDL_CMD5  NULL
#define WOLFSSL_CONF_CMDL_CMD6  NULL
#define WOLFSSL_CONF_CMDL_CMD7  "sigalgs"
#define WOLFSSL_CONF_CMDL_CMD8  "client_sigalgs"
#define WOLFSSL_CONF_CMDL_CMD9  "cipher"

#if !defined(NO_DH) && !defined(NO_BIO)
#define WOLFSSL_CONF_FILE_CMD10 "DHParameters"
#define WOLFSSL_CONF_CMDL_CMD10 "dhparam"
#endif

#ifdef HAVE_ECC
#define WOLFSSL_CONF_FILE_CMD11 "ECDHParameters"
#define WOLFSSL_CONF_CMDL_CMD11 "named_curves"
#endif

/**
 * process Cipher String command
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure
 * @param value arguments for cmd
 * @return WOLFSSL_SUCCESS on success,
 *     otherwise WOLFSSL_FAILURE or
 *               -3 if value is null or
 *               negative value on other failure
 */
static int cmdfunc_cipherstring(WOLFSSL_CONF_CTX* cctx, const char* value)
{
    int ret = -3;

    WOLFSSL_ENTER("cmdfunc_cipherstring");

     /* sanity check */
    if (cctx == NULL)
        return WOLFSSL_FAILURE;

    if (value == NULL) {
        WOLFSSL_MSG("bad arguments");
        return ret;
    }

    if (cctx->ctx) {
        ret = wolfSSL_CTX_set_cipher_list(cctx->ctx, value);
    }

    if (((cctx->ctx  && ret == WOLFSSL_SUCCESS) ||
         (!cctx->ctx && ret == -3)) &&
        cctx->ssl) {
        ret = wolfSSL_set_cipher_list(cctx->ssl, value);
    }

    WOLFSSL_LEAVE("cmdfunc_cipherstring", ret);
    return ret;
}

/**
 * process curves command
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure
 * @param value arguments for cmd
 * @return WOLFSSL_SUCCESS on success,
 *     otherwise WOLFSSL_FAILURE or
 *               -3 if value is null or
 *               negative value on other failure
 */
#if defined(HAVE_ECC)
static int cmdfunc_curves(WOLFSSL_CONF_CTX* cctx, const char* value)
{
    int ret = -3;

    WOLFSSL_ENTER("cmdfunc_curves");

     /* sanity check */
    if (cctx == NULL)
        return WOLFSSL_FAILURE;

    if (value == NULL) {
        WOLFSSL_MSG("bad arguments");
        return ret;
    }

    if (cctx->ctx) {
        ret = wolfSSL_CTX_set1_curves_list(cctx->ctx, value);
    }

    if (((cctx->ctx  && ret == WOLFSSL_SUCCESS) ||
         (!cctx->ctx && ret == -3)) &&
        cctx->ssl) {
        ret = wolfSSL_set1_curves_list(cctx->ssl, value);
    }

    WOLFSSL_LEAVE("cmdfunc_curves", ret);
    return ret;
}
#endif

#ifndef NO_FILESYSTEM
/**
 * process cert command
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure
 * @param value arguments for cmd
 * @return WOLFSSL_SUCCESS on success,
 *     otherwise WOLFSSL_FAILURE or
 *               -3 if value is null or
 *               negative value on other failure
 */
static int cmdfunc_cert(WOLFSSL_CONF_CTX* cctx, const char* value)
{
    int ret = -3;

    WOLFSSL_ENTER("cmdfunc_cert");

    /* sanity check */
    if (cctx == NULL)
        return WOLFSSL_FAILURE;

    if (value == NULL) {
        WOLFSSL_MSG("bad arguments");
        return ret;
    }

    if (!(cctx->flags & WOLFSSL_CONF_FLAG_CERTIFICATE)) {
        WOLFSSL_MSG("certificate flag is not set");
        return -2;
    }

    if (cctx->ctx) {
        ret = wolfSSL_CTX_use_certificate_chain_file(cctx->ctx, value);
    }

    if (((cctx->ctx  && ret == WOLFSSL_SUCCESS) ||
         (!cctx->ctx && ret == -3)) &&
        cctx->ssl) {
        ret = wolfSSL_use_certificate_file(cctx->ssl, value,
                                                    WOLFSSL_FILETYPE_PEM);
    }

    WOLFSSL_LEAVE("cmdfunc_cert", ret);
    return ret;
}
/**
 * process key command
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure
 * @param value arguments for cmd
 * @return WOLFSSL_SUCCESS on success,
 *     otherwise WOLFSSL_FAILURE or
 *               -3 if value is null or
 *               negative value on other failure
 */
static int cmdfunc_key(WOLFSSL_CONF_CTX* cctx, const char* value)
{
    int ret = -3;

    WOLFSSL_ENTER("cmdfunc_key");

     /* sanity check */
    if (cctx == NULL)
        return WOLFSSL_FAILURE;

    if (value == NULL) {
        WOLFSSL_MSG("bad arguments");
        return ret;
    }

    if (!(cctx->flags & WOLFSSL_CONF_FLAG_CERTIFICATE)) {
        WOLFSSL_MSG("certificate flag is not set");
        return -2;
    }

    if (cctx->ctx) {
        ret = wolfSSL_CTX_use_PrivateKey_file(cctx->ctx, value,
                                                    WOLFSSL_FILETYPE_PEM);
    }

    if (((cctx->ctx  && ret == WOLFSSL_SUCCESS) ||
         (!cctx->ctx && ret == -3)) &&
        cctx->ssl) {
        ret = wolfSSL_use_PrivateKey_file(cctx->ssl, value,
                                                    WOLFSSL_FILETYPE_PEM);
    }

    WOLFSSL_LEAVE("cmdfunc_key", ret);
    return ret;
}
#endif /* NO_FILESYSTEM */
/**
 * process DH parameter command
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure
 * @param value arguments for cmd
 * @return WOLFSSL_SUCCESS on success,
 *     otherwise WOLFSSL_FAILURE or
 *               -3 if value is null or
 *               negative value on other failure
 */
#if !defined(NO_DH) && !defined(NO_BIO)
static int cmdfunc_dhparam(WOLFSSL_CONF_CTX* cctx, const char* value)
{
    int ret = -3;
    WOLFSSL_DH* dh = NULL;
    WOLFSSL_BIO* bio = NULL;

    WOLFSSL_MSG("cmdfunc_dhparam");

     /* sanity check */
    if (cctx == NULL)
        return WOLFSSL_FAILURE;

    if (value == NULL) {
        WOLFSSL_MSG("bad arguments");
        return ret;
    }

    if (cctx->ctx || cctx->ssl) {
        bio = wolfSSL_BIO_new_file(value, "rb");
        if (!bio) {
            WOLFSSL_MSG("bio new file failed");
            return WOLFSSL_FAILURE;
        }
        dh = wolfSSL_PEM_read_bio_DHparams(bio, NULL, NULL, NULL);
        if (!dh) {
            wolfSSL_BIO_free(bio);
            WOLFSSL_MSG("PEM read bio failed");
            return WOLFSSL_FAILURE;
        }
    } else {
        return 1;
    }

    if (cctx->ctx) {
        ret = (int)wolfSSL_CTX_set_tmp_dh(cctx->ctx, dh);
    }

    if (((cctx->ctx  && ret == WOLFSSL_SUCCESS) ||
         (!cctx->ctx && ret == -3)) &&
        cctx->ssl) {
        ret = (int)wolfSSL_CTX_set_tmp_dh(cctx->ssl->ctx, dh);
    }

    if (dh)
        wolfSSL_DH_free(dh);
    if (bio)
        wolfSSL_BIO_free(bio);

    WOLFSSL_LEAVE("cmdfunc_dhparam", ret);
    return ret;
}
#endif /* !NO_DH && !NO_BIO */
/**
 * command table
 */
typedef struct conf_cmd_tbl {
    const char* file_cmd;
    const char* cmdline_cmd;
    word32 data_type;
    int (*cmdfunc)(WOLFSSL_CONF_CTX* cctx, const char* value);
}conf_cmd_tbl;

static const conf_cmd_tbl conf_cmds_tbl[] = {
#if defined(HAVE_ECC)
    /* cmd Curves */
    {WOLFSSL_CONF_FILE_CMD1, WOLFSSL_CONF_CMDL_CMD1,
                                    WOLFSSL_CONF_TYPE_STRING, cmdfunc_curves},
#endif
#if !defined(NO_FILESYSTEM)
    /* cmd Certificate */
    {WOLFSSL_CONF_FILE_CMD2, WOLFSSL_CONF_CMDL_CMD2,
                                    WOLFSSL_CONF_TYPE_FILE, cmdfunc_cert},
    /* cmd PrivateKey */
    {WOLFSSL_CONF_FILE_CMD3, WOLFSSL_CONF_CMDL_CMD3,
                                    WOLFSSL_CONF_TYPE_FILE, cmdfunc_key},
#endif
    /* cmd Protocol */
    {WOLFSSL_CONF_FILE_CMD4, WOLFSSL_CONF_CMDL_CMD4,
                                    WOLFSSL_CONF_TYPE_STRING, NULL},
    /* cmd Options */
    {WOLFSSL_CONF_FILE_CMD5, WOLFSSL_CONF_CMDL_CMD5,
                                    WOLFSSL_CONF_TYPE_STRING, NULL},
    /* cmd ServerInfoFile */
    {WOLFSSL_CONF_FILE_CMD6, WOLFSSL_CONF_CMDL_CMD6,
                                    WOLFSSL_CONF_TYPE_FILE, NULL},
    /* cmd SignatureAlgorithms */
    {WOLFSSL_CONF_FILE_CMD7, WOLFSSL_CONF_CMDL_CMD7,
                                    WOLFSSL_CONF_TYPE_STRING, NULL},
    /* cmd ClientSignatureAlgorithms */
    {WOLFSSL_CONF_FILE_CMD8, WOLFSSL_CONF_CMDL_CMD8,
                                    WOLFSSL_CONF_TYPE_STRING, NULL},
    /* cmd CipherString */
    {WOLFSSL_CONF_FILE_CMD9, WOLFSSL_CONF_CMDL_CMD9,
                              WOLFSSL_CONF_TYPE_STRING, cmdfunc_cipherstring},
#if !defined(NO_DH) && !defined(NO_BIO)
    /* cmd DHParameters */
    {WOLFSSL_CONF_FILE_CMD10, WOLFSSL_CONF_CMDL_CMD10,
                                    WOLFSSL_CONF_TYPE_FILE, cmdfunc_dhparam},
#endif
#ifdef HAVE_ECC
    /* cmd ECHDParameters */
    {WOLFSSL_CONF_FILE_CMD11, WOLFSSL_CONF_CMDL_CMD11,
                                    WOLFSSL_CONF_TYPE_STRING, NULL},
#endif
};
/* size of command table */
static const size_t size_of_cmd_tbls = sizeof(conf_cmds_tbl)
                                                    / sizeof(conf_cmd_tbl);

static const conf_cmd_tbl* wolfssl_conf_find_cmd(WOLFSSL_CONF_CTX* cctx,
                                         const char* cmd)
{
    size_t i = 0;

    if (cctx->flags & WOLFSSL_CONF_FLAG_CMDLINE) {
        size_t cmdlen = XSTRLEN(cmd);

        if (cmdlen < 2) {
            WOLFSSL_MSG("bad cmdline command");
            return NULL;
        }
        /* skip "-" prefix */
        ++cmd;
    }

    for (i = 0; i < size_of_cmd_tbls; i++) {
        /* check if the cmd is valid */
        if (cctx->flags & WOLFSSL_CONF_FLAG_CMDLINE) {
            if (conf_cmds_tbl[i].cmdline_cmd != NULL &&
                XSTRCMP(cmd, conf_cmds_tbl[i].cmdline_cmd) == 0) {
                return &conf_cmds_tbl[i];
            }
        }

        if (cctx->flags & WOLFSSL_CONF_FLAG_FILE) {
            if (conf_cmds_tbl[i].file_cmd != NULL &&
                XSTRCMP(cmd, conf_cmds_tbl[i].file_cmd) == 0) {
                return &conf_cmds_tbl[i];
            }
        }
    }

    return NULL;
}

/**
 * send configuration command
 * @param cctx  a pointer to WOLFSSL_CONF_CTX structure
 * @param cmd   configuration command
 * @param value arguments for cmd
 * @return 1 when cmd is recognised, but value is not used
 *         2 both cmd and value are used
 *  otherwise WOLFSSL_FAILURE
 *         -2 if cmd is not recognised
 *         -3 if value is NULL, but cmd is recognized
 */
int wolfSSL_CONF_cmd(WOLFSSL_CONF_CTX* cctx, const char* cmd, const char* value)
{
    int ret = WOLFSSL_FAILURE;
    const conf_cmd_tbl* confcmd = NULL;
    WOLFSSL_ENTER("wolfSSL_CONF_cmd");

    /* sanity check */
    if (cctx == NULL || cmd == NULL) {
        WOLFSSL_MSG("bad arguments");
        return ret;
    }

    confcmd = wolfssl_conf_find_cmd(cctx, cmd);
    if (confcmd == NULL)
        return -2;

    if (confcmd->cmdfunc == NULL) {
        WOLFSSL_MSG("cmd not yet implemented");
        return -2;
    }

    ret = confcmd->cmdfunc(cctx, value);

    /* return code compliant with OpenSSL */
    if (ret < -3)
        ret = 0;

    WOLFSSL_LEAVE("wolfSSL_CONF_cmd", ret);
    return ret;
}

/**
 *
 * @param cctx a pointer to WOLFSSL_CONF_CTX structure
 * @param cmd  configuration command
 * @return The SSL_CONF_TYPE_* type or SSL_CONF_TYPE_UNKNOWN if an
 *         invalid command
 */
int wolfSSL_CONF_cmd_value_type(WOLFSSL_CONF_CTX *cctx, const char *cmd)
{
    const conf_cmd_tbl* confcmd = NULL;
    WOLFSSL_ENTER("wolfSSL_CONF_cmd_value_type");

    confcmd = wolfssl_conf_find_cmd(cctx, cmd);
    if (confcmd == NULL)
        return SSL_CONF_TYPE_UNKNOWN;
    return (int)confcmd->data_type;
}

#endif /* OPENSSL_EXTRA */


/*******************************************************************************
 * END OF CONF API
 ******************************************************************************/

#endif /* WOLFSSL_CONF_INCLUDED */
