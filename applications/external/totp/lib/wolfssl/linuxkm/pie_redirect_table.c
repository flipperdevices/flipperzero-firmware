/* pie_redirect_table.c -- module load/unload hooks for libwolfssl.ko
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
    #error pie_redirect_table.c must be compiled -fPIE.
#endif

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/ssl.h>

/* compiling -fPIE results in references to the GOT or equivalent thereof, which remain after linking
 * even if all other symbols are resolved by the link.  naturally there is no
 * GOT in the kernel, and the wolfssl Kbuild script explicitly checks that no
 * GOT relocations occur in the PIE objects, but we still need to include a
 * dummy value here, scoped to the module, to eliminate the otherwise unresolved
 * symbol.
 */
#if defined(CONFIG_X86)
    extern void * const _GLOBAL_OFFSET_TABLE_;
    void * const _GLOBAL_OFFSET_TABLE_ = 0;
#elif defined(CONFIG_MIPS)
  extern void * const _gp_disp;
  void * const _gp_disp = 0;
#endif

struct wolfssl_linuxkm_pie_redirect_table wolfssl_linuxkm_pie_redirect_table;

const struct wolfssl_linuxkm_pie_redirect_table
*wolfssl_linuxkm_get_pie_redirect_table(void) {
    return &wolfssl_linuxkm_pie_redirect_table;
}

/* placeholder implementations for missing functions. */
#if defined(CONFIG_MIPS)
    #undef memcpy
    void *memcpy(void *dest, const void *src, size_t n) {
        char *dest_i = (char *)dest;
        char *dest_end = dest_i + n;
        char *src_i = (char *)src;
        while (dest_i < dest_end)
            *dest_i++ = *src_i++;
        return dest;
    }

    #undef memset
    void *memset(void *dest, int c, size_t n) {
        char *dest_i = (char *)dest;
        char *dest_end = dest_i + n;
        while (dest_i < dest_end)
            *dest_i++ = c;
        return dest;
    }
#endif
