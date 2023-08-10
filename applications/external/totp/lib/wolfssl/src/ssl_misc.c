/* ssl_misc.c
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

#if !defined(WOLFSSL_SSL_MISC_INCLUDED)
    #ifndef WOLFSSL_IGNORE_FILE_WARN
        #warning ssl_misc.c does not need to be compiled separately from ssl.c
    #endif
#else

#if defined(OPENSSL_EXTRA) && !defined(WOLFCRYPT_ONLY)
#ifndef NO_BIO

#ifdef WOLFSSL_NO_FSEEK
/* Amount of memory to allocate/add. */
#define READ_BIO_FILE_CHUNK     128

/* Read a file in chunks.
 *
 * Allocates a chunk and reads into it until it is full.
 *
 * @param [in, out] bio   BIO object to read with.
 * @param [out]     data  Read data in a new buffer.
 * @return  Negative on error.
 * @return  Number of bytes read on success.
 */
static int wolfssl_read_bio_file(WOLFSSL_BIO* bio, char** data)
{
    int ret = 0;
    char* mem;
    char* p;

    /* Allocate buffer to hold a chunk of data. */
    mem = (char*)XMALLOC(READ_BIO_FILE_CHUNK, bio->heap, DYNAMIC_TYPE_OPENSSL);
    if (mem == NULL) {
        WOLFSSL_ERROR_MSG("Memory allocation error");
        ret = MEMORY_E;
    }

    if (ret == 0) {
        int sz;

        /* ret is the number of bytes read and is zero. */

        /* p is where to read in next chunk. */
        p = mem;
        /* Memory available to read into is one chunk. */
        sz = READ_BIO_FILE_CHUNK;
        /* Keep reading in chunks until no more or an error. */
        while ((sz = wolfSSL_BIO_read(bio, p, sz)) > 0) {
            int remaining;

            /* Update total read. */
            ret += sz;
            /* Calculate remaining unused memory. */
            remaining = READ_BIO_FILE_CHUNK - (ret % READ_BIO_FILE_CHUNK);
            /* Check for space remaining. */
            if (remaining != READ_BIO_FILE_CHUNK) {
                /* Update where data is read into. */
                p += sz;
                /* Maximum possible size is the remaining buffer size. */
                sz = remaining;
            }
            else {
                /* No space left for more data to be read - add a chunk. */
                p = (char*)XREALLOC(mem, ret + READ_BIO_FILE_CHUNK, bio->heap,
                    DYNAMIC_TYPE_OPENSSL);
                if (p == NULL) {
                    sz = MEMORY_E;
                    break;
                }

                /* Set mem to new pointer. */
                mem = p;
                /* Set p to where to read in next chunk. */
                p += ret;
                /* Read in a new chunk. */
                sz = READ_BIO_FILE_CHUNK;
            }
        }
        if ((sz < 0) || (ret == 0)) {
            /* Dispose of memory on error or no data read. */
            XFREE(mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
            mem = NULL;
            /* Return error. */
            ret = sz;
        }
    }

    *data = mem;
    return ret;
}
#endif

/* Read exactly the required amount into a newly allocated buffer.
 *
 * @param [in, out] bio   BIO object to read with.
 * @param [in       sz    Amount of data to read.
 * @param [out]     data  Read data in a new buffer.
 * @return  Negative on error.
 * @return  Number of bytes read on success.
 */
static int wolfssl_read_bio_len(WOLFSSL_BIO* bio, int sz, char** data)
{
    int ret = 0;
    char* mem;

    /* Allocate buffer to hold data. */
    mem = (char*)XMALLOC((size_t)sz, bio->heap, DYNAMIC_TYPE_OPENSSL);
    if (mem == NULL) {
        WOLFSSL_ERROR_MSG("Memory allocation error");
        ret = MEMORY_E;
    }
    else if ((ret = wolfSSL_BIO_read(bio, mem, sz)) != sz) {
        /* Pending data not read. */
        XFREE(mem, bio->heap, DYNAMIC_TYPE_OPENSSL);
        mem = NULL;
        ret = MEMORY_E;
    }

    *data = mem;
    return ret;
}

/* Read all the data from a BIO.
 *
 * @param [in, out] bio         BIO object to read with.
 * @param [out]     data        Read data in a buffer.
 * @param [out]     dataSz      Amount of data read in bytes.
 * @param [out]     memAlloced  Indicates whether return buffer was allocated.
 * @return  Negative on error.
 * @return  0 on success.
 */
static int wolfssl_read_bio(WOLFSSL_BIO* bio, char** data, int* dataSz,
    int* memAlloced)
{
    int ret;
    int sz;

    if (bio->type == WOLFSSL_BIO_MEMORY) {
        ret = wolfSSL_BIO_get_mem_data(bio, data);
        if (ret > 0) {
            bio->rdIdx += ret;
        }
        *memAlloced = 0;
    }
#ifndef WOLFSSL_NO_FSEEK
    /* Get pending or, when a file BIO, get length of file. */
    else if ((sz = wolfSSL_BIO_get_len(bio)) > 0) {
        ret = wolfssl_read_bio_len(bio, sz, data);
        if (ret > 0) {
            *memAlloced = 1;
        }
    }
#else
    else if ((sz = wolfSSL_BIO_pending(bio)) > 0) {
        ret = wolfssl_read_bio_len(bio, sz, data);
        if (ret > 0) {
            *memAlloced = 1;
        }
    }
    else if (bio->type == WOLFSSL_BIO_FILE) {
        ret = wolfssl_read_bio_file(bio, data);
        if (ret > 0) {
            *memAlloced = 1;
        }
    }
#endif
    else {
        WOLFSSL_ERROR_MSG("No data read from bio");
        *memAlloced = 0;
        ret = NOT_COMPILED_IN;
    }

    if (ret >= 0) {
        *dataSz = ret;
        ret = 0;
    }

    return ret;
}
#endif /* !NO_BIO */
#endif /* OPENSSL_EXTRA && !WOLFCRYPT_ONLY */

#if (defined(OPENSSL_EXTRA) || defined(PERSIST_CERT_CACHE) || \
     (!defined(NO_CERTS) && (!defined(NO_WOLFSSL_CLIENT) || \
      !defined(WOLFSSL_NO_CLIENT_AUTH)))) && !defined(WOLFCRYPT_ONLY) && \
    !defined(NO_FILESYSTEM)
/* Read all the data from a file.
 *
 * @param [in]  fp          File pointer to read with.
 * @param [out] fileSz      Amount of data remaining in file in bytes.
 * @return  WOLFSSL_BAD_FILE on error.
 * @return  0 on success.
 */
static int wolfssl_file_len(XFILE fp, long* fileSz)
{
    int ret = 0;
    long sz = 0;
    long curr = 0;

    if (fp == XBADFILE) {
        ret = WOLFSSL_BAD_FILE;
    }
    if (ret == 0) {
        /* Get file offset at end of file. */
        curr = (long)XFTELL(fp);
        if (curr < 0) {
            ret = WOLFSSL_BAD_FILE;
        }
    }
    /* Move to end of file. */
    if ((ret == 0) && (XFSEEK(fp, 0, SEEK_END) != 0)) {
        ret = WOLFSSL_BAD_FILE;
    }
    if (ret == 0) {
        /* Get file offset at end of file and subtract current offset. */
        sz = (long)XFTELL(fp) - curr;
        if (sz < 0) {
            ret = WOLFSSL_BAD_FILE;
        }
    }
    /* Go back to original offset in file. */
    if ((ret == 0) && (XFSEEK(fp, curr, SEEK_SET) != 0)) {
        ret = WOLFSSL_BAD_FILE;
    }
    /* Validate size. */
    if ((ret == 0) && ((sz > MAX_WOLFSSL_FILE_SIZE) || (sz <= 0L))) {
        ret = WOLFSSL_BAD_FILE;
    }
    if (ret == 0) {
        *fileSz = sz;
    }

    return ret;
}
#endif

#if (defined(OPENSSL_EXTRA) || defined(PERSIST_CERT_CACHE)) && \
    !defined(WOLFCRYPT_ONLY) && !defined(NO_FILESYSTEM)
/* Read all the data from a file.
 *
 * @param [in]  fp          File pointer to read with.
 * @param [out] data        Read data in an allocated buffer.
 * @param [out] dataSz      Amount of data read in bytes.
 * @return  WOLFSSL_BAD_FILE when reading fails.
 * @return  MEMORY_E when memory allocation fails.
 * @return  0 on success.
 */
static int wolfssl_read_file(XFILE fp, char** data, int* dataSz)
{
    int ret = 0;
    long sz = 0;
    char* mem = NULL;

    ret = wolfssl_file_len(fp, &sz);
    if (ret == 0) {
        /* Allocate memory big enough to hold whole file. */
        mem = (char*)XMALLOC((size_t)sz, NULL, DYNAMIC_TYPE_TMP_BUFFER);
        if (mem == NULL) {
            ret = MEMORY_E;
        }
    }
    /* Read whole file into new buffer. */
    if ((ret == 0) && ((int)XFREAD(mem, 1, (size_t)sz, fp) != sz)) {
        ret = WOLFSSL_BAD_FILE;
    }
    if (ret == 0) {
        *dataSz = (int)sz;
        *data = mem;
        mem = NULL;
    }

    XFREE(mem, NULL, DYNAMIC_TYPE_TMP_BUFFER);
    return ret;
}
#endif /* (OPENSSL_EXTRA || PERSIST_CERT_CACHE) && !WOLFCRYPT_ONLY &&
        * !NO_FILESYSTEM */
#endif /* !WOLFSSL_SSL_MISC_INCLUDED */

