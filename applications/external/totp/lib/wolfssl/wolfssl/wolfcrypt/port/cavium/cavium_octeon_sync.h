/* cavium_octeon_sync.h
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

#ifndef _CAVIUM_OCTEON_SYNC_H_
#define _CAVIUM_OCTEON_SYNC_H_

#ifdef HAVE_CAVIUM_OCTEON_SYNC

WOLFSSL_API int wc_CryptoCb_InitOcteon(void);
WOLFSSL_API void wc_CryptoCb_CleanupOcteon(int* id);

#endif /* HAVE_CAVIUM_OCTEON_SYNC */
#endif /* _CAVIUM_OCTEON_SYNC_H_ */
