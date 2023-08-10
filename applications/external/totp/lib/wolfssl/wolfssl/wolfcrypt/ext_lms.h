/* ext_lms.h
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

#ifndef EXT_LMS_H
#define EXT_LMS_H

#ifdef WOLFSSL_HAVE_LMS
#include <wolfssl/wolfcrypt/lms.h>

#if !defined(HAVE_LIBLMS)
#error "This code requires liblms"
#endif

/* hash-sigs LMS HSS includes */
#include <hss.h>

#if defined(WOLFSSL_WC_LMS)
#error "This code is incompatible with wolfCrypt's implementation of LMS."
#endif

/*
 * The hash-sigs LMS lib supports from MIN_HSS_LEVELS to MAX_HSS_LEVELS
 * number of levels of Merkle trees. It allows for the tree height and
 * winternitz parameter to be unique per level.
 */

/* hss structs */
typedef struct hss_working_key hss_working_key;
typedef struct hss_extra_info  hss_extra_info;

struct LmsKey {
    unsigned             levels;                      /* Number of tree levels. */
    param_set_t          lm_type[MAX_HSS_LEVELS];     /* Height parm per level. */
    param_set_t          lm_ots_type[MAX_HSS_LEVELS]; /* Winternitz parm per level. */
    unsigned char        pub[HSS_MAX_PUBLIC_KEY_LEN];
    hss_working_key *    working_key;
    write_private_key_cb write_private_key; /* Callback to write/update key. */
    read_private_key_cb  read_private_key;  /* Callback to read key. */
    void *               context;           /* Context arg passed to callbacks. */
    hss_extra_info       info;
    enum wc_LmsState     state;
};

#endif /* WOLFSSL_HAVE_LMS */
#endif /* EXT_LMS_H */
