/* silabs_random.c
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

/* Generic SILABS Entropy random */

#ifdef HAVE_CONFIG_H
    #include <config.h>
#endif

#include <wolfssl/wolfcrypt/settings.h>

#if defined(WOLFSSL_SILABS_SE_ACCEL)

#include <sl_se_manager.h>
#include <sl_se_manager_entropy.h>

#include <wolfssl/wolfcrypt/port/silabs/silabs_random.h>

int silabs_GenerateRand(byte* output, word32 sz)
{
    sl_se_command_context_t cmd_ctx = SL_SE_COMMAND_CONTEXT_INIT;
    sl_status_t status = sl_se_init();

    if (status == SL_STATUS_OK)
        status = sl_se_get_random(&cmd_ctx, output,  sz);

    return (status != SL_STATUS_OK);
}

#endif /* WOLFSSL_SILABS_SE_ACCEL */
