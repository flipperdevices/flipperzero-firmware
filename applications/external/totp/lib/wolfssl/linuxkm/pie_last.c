/* linuxkm/pie_last.c -- memory fenceposts for checking binary image stability
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

#ifndef __PIE__
    #error pie_last.c must be compiled -fPIE.
#endif

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/ssl.h>

int wolfCrypt_PIE_last_function(void);
int wolfCrypt_PIE_last_function(void) {
    return 1;
}

const unsigned int wolfCrypt_PIE_rodata_end[];
const unsigned int wolfCrypt_PIE_rodata_end[] =
/* random values, analogous to wolfCrypt_FIPS_ro_{start,end} */
{ 0xa4aaaf71, 0x55c4b7d0 };
