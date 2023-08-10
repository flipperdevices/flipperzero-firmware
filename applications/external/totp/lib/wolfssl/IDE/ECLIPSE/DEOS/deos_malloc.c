/* deos_malloc.c
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
#include <wolfssl/wolfcrypt/settings.h>
#include <wolfssl/wolfcrypt/types.h>
#include <deos.h>

void free_deos(void *ptr) {
    free(ptr);
    return;
}

void *realloc_deos(void *ptr, size_t size) {
    void *newptr;

    if (size == 0)
        return ptr;
    newptr = malloc_deos(size);

    if (ptr != NULL && newptr != NULL) {
        XMEMCPY((char *) newptr, (const char *) ptr, size);
        free_deos(ptr);
    }

    return newptr;
}

void *malloc_deos(size_t size) {
  return malloc(size);
}
