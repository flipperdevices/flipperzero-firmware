/* txt_db.h
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

#ifndef WOLFSSL_TXT_DB_H_
#define WOLFSSL_TXT_DB_H_

#include <wolfssl/openssl/ssl.h>

#define WOLFSSL_TXT_DB_MAX_FIELDS 10

struct WOLFSSL_TXT_DB {
    int num_fields;
    WOLF_STACK_OF(WOLFSSL_STRING) *data;
    long error;
    long arg1;
    long arg2;
    wolf_sk_hash_cb hash_fn[WOLFSSL_TXT_DB_MAX_FIELDS];
};

typedef struct WOLFSSL_TXT_DB WOLFSSL_TXT_DB;
typedef int (*wolf_lh_compare_cb)(const void* a,
                                  const void* b);

WOLFSSL_API WOLFSSL_TXT_DB *wolfSSL_TXT_DB_read(WOLFSSL_BIO *in, int num);
WOLFSSL_API long wolfSSL_TXT_DB_write(WOLFSSL_BIO  *out, WOLFSSL_TXT_DB *db);
WOLFSSL_API int wolfSSL_TXT_DB_insert(WOLFSSL_TXT_DB *db, WOLFSSL_STRING *row);
WOLFSSL_API void wolfSSL_TXT_DB_free(WOLFSSL_TXT_DB *db);
WOLFSSL_API int wolfSSL_TXT_DB_create_index(WOLFSSL_TXT_DB *db, int field,
        void* qual, wolf_sk_hash_cb hash, wolf_lh_compare_cb cmp);
WOLFSSL_API WOLFSSL_STRING *wolfSSL_TXT_DB_get_by_index(WOLFSSL_TXT_DB *db,
        int idx, WOLFSSL_STRING *value);

#define TXT_DB                  WOLFSSL_TXT_DB

#define TXT_DB_read             wolfSSL_TXT_DB_read
#define TXT_DB_write            wolfSSL_TXT_DB_write
#define TXT_DB_insert           wolfSSL_TXT_DB_insert
#define TXT_DB_free             wolfSSL_TXT_DB_free
#define TXT_DB_create_index     wolfSSL_TXT_DB_create_index
#define TXT_DB_get_by_index     wolfSSL_TXT_DB_get_by_index

#endif /* WOLFSSL_TXT_DB_H_ */
