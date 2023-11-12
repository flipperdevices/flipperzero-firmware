/* conf.h
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

/* conf.h for openSSL */

#ifndef WOLFSSL_conf_H_
#define WOLFSSL_conf_H_

#ifdef __cplusplus
    extern "C" {
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/version.h>

typedef struct WOLFSSL_CONF_VALUE {
    char *section;
    char *name;
    char *value;
} WOLFSSL_CONF_VALUE;

/* ssl.h requires WOLFSSL_CONF_VALUE */
#include <wolfssl/ssl.h>

typedef struct WOLFSSL_CONF {
    void *meth_data;
    WOLF_LHASH_OF(WOLFSSL_CONF_VALUE) *data;
} WOLFSSL_CONF;

typedef WOLFSSL_CONF CONF;
typedef WOLFSSL_CONF_VALUE CONF_VALUE;

#ifdef OPENSSL_EXTRA

WOLFSSL_API WOLFSSL_CONF_VALUE *wolfSSL_CONF_VALUE_new(void);
WOLFSSL_API int wolfSSL_CONF_add_string(WOLFSSL_CONF *conf,
        WOLFSSL_CONF_VALUE *section, WOLFSSL_CONF_VALUE *value);
WOLFSSL_API void wolfSSL_X509V3_conf_free(WOLFSSL_CONF_VALUE *val);

WOLFSSL_API WOLFSSL_CONF *wolfSSL_NCONF_new(void *meth);
WOLFSSL_API char *wolfSSL_NCONF_get_string(const WOLFSSL_CONF *conf,
        const char *group, const char *name);
WOLFSSL_API int wolfSSL_NCONF_get_number(const CONF *conf, const char *group,
        const char *name, long *result);
WOLFSSL_API WOLFSSL_STACK *wolfSSL_NCONF_get_section(
        const WOLFSSL_CONF *conf, const char *section);
WOLFSSL_API int wolfSSL_NCONF_load(WOLFSSL_CONF *conf, const char *file, long *eline);
WOLFSSL_API void wolfSSL_NCONF_free(WOLFSSL_CONF *conf);

WOLFSSL_API WOLFSSL_CONF_VALUE *wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve(
        WOLF_LHASH_OF(WOLFSSL_CONF_VALUE) *sk, WOLFSSL_CONF_VALUE *data);

WOLFSSL_API int wolfSSL_CONF_modules_load(const WOLFSSL_CONF *cnf, const char *appname,
                      unsigned long flags);
WOLFSSL_API WOLFSSL_CONF_VALUE *wolfSSL_CONF_new_section(WOLFSSL_CONF *conf,
        const char *section);
WOLFSSL_API WOLFSSL_CONF_VALUE *wolfSSL_CONF_get_section(WOLFSSL_CONF *conf,
        const char *section);

WOLFSSL_API WOLFSSL_X509_EXTENSION* wolfSSL_X509V3_EXT_nconf_nid(WOLFSSL_CONF* conf,
        WOLFSSL_X509V3_CTX *ctx, int nid, const char *value);
WOLFSSL_API WOLFSSL_X509_EXTENSION* wolfSSL_X509V3_EXT_nconf(WOLFSSL_CONF *conf,
        WOLFSSL_X509V3_CTX *ctx, const char *sName, const char *value);

#define sk_CONF_VALUE_new               wolfSSL_sk_CONF_VALUE_new
#define sk_CONF_VALUE_free              wolfSSL_sk_CONF_VALUE_free
#define sk_CONF_VALUE_pop_free(a,b)     wolfSSL_sk_CONF_VALUE_free(a)
#define sk_CONF_VALUE_num               wolfSSL_sk_CONF_VALUE_num
#define sk_CONF_VALUE_value             wolfSSL_sk_CONF_VALUE_value

#define lh_CONF_VALUE_retrieve          wolfSSL_lh_WOLFSSL_CONF_VALUE_retrieve
#define lh_CONF_VALUE_insert            wolfSSL_sk_CONF_VALUE_push

#define NCONF_new                       wolfSSL_NCONF_new
#define NCONF_free                      wolfSSL_NCONF_free
#define NCONF_get_string                wolfSSL_NCONF_get_string
#define NCONF_get_section               wolfSSL_NCONF_get_section
#define NCONF_get_number                wolfSSL_NCONF_get_number
#define NCONF_load                      wolfSSL_NCONF_load

#define CONF_modules_load               wolfSSL_CONF_modules_load
#define _CONF_new_section               wolfSSL_CONF_new_section
#define _CONF_get_section               wolfSSL_CONF_get_section

#define X509V3_EXT_nconf_nid            wolfSSL_X509V3_EXT_nconf_nid
#define X509V3_EXT_nconf                wolfSSL_X509V3_EXT_nconf
#define X509V3_conf_free                wolfSSL_X509V3_conf_free

#endif /* OPENSSL_EXTRA */

#ifdef  __cplusplus
} /* extern "C" */
#endif

#endif /* WOLFSSL_conf_H_ */
