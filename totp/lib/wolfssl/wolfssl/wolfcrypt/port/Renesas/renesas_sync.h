/* renesas_sync.h
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

#ifndef _RENESAS_SYNC_H_
#define _RENESAS_SYNC_H_

#ifdef HAVE_RENESAS_SYNC

struct WOLFSSL;
struct User_SCEPKCbInfo;
extern User_SCEPKCbInfo guser_PKCbInfo;

WOLFSSL_API int wc_CryptoCb_CryptInitRenesasCmn(struct WOLFSSL* ssl, void* ctx);
WOLFSSL_API void wc_CryptoCb_CleanupRenesasCmn(int* id);

#endif /* HAVE_RENESAS_SYNC */
#endif /* _RENESAS_SYNC_H_ */
