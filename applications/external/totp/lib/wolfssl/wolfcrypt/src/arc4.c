/* arc4.c
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

#ifndef NO_RC4

#include <wolfssl/wolfcrypt/error-crypt.h>
#include <wolfssl/wolfcrypt/arc4.h>


int wc_Arc4SetKey(Arc4* arc4, const byte* key, word32 length)
{
    int ret = 0;
    word32 i;
    word32 keyIndex = 0, stateIndex = 0;

    if (arc4 == NULL || key == NULL || length == 0) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4) && \
        defined(HAVE_CAVIUM) && !defined(HAVE_CAVIUM_V)
    if (arc4->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ARC4) {
        return NitroxArc4SetKey(arc4, key, length);
    }
#endif

    arc4->x = 1;
    arc4->y = 0;

    for (i = 0; i < ARC4_STATE_SIZE; i++)
        arc4->state[i] = (byte)i;

    for (i = 0; i < ARC4_STATE_SIZE; i++) {
        word32 a = arc4->state[i];
        stateIndex += key[keyIndex] + a;
        stateIndex &= 0xFF;
        arc4->state[i] = arc4->state[stateIndex];
        arc4->state[stateIndex] = (byte)a;

        if (++keyIndex >= length)
            keyIndex = 0;
    }

    return ret;
}


static WC_INLINE byte MakeByte(word32* x, word32* y, byte* s)
{
    word32 a = s[*x], b;
    *y = (*y+a) & 0xff;

    b = s[*y];
    s[*x] = (byte)b;
    s[*y] = (byte)a;
    *x = (*x+1) & 0xff;

    return s[(a+b) & 0xff];
}


int wc_Arc4Process(Arc4* arc4, byte* out, const byte* in, word32 length)
{
    int ret = 0;
    word32 x;
    word32 y;

    if (arc4 == NULL || out == NULL || in == NULL) {
        return BAD_FUNC_ARG;
    }

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4) && \
        defined(HAVE_CAVIUM) && !defined(HAVE_CAVIUM_V)
    if (arc4->asyncDev.marker == WOLFSSL_ASYNC_MARKER_ARC4) {
        return NitroxArc4Process(arc4, out, in, length);
    }
#endif

    x = arc4->x;
    y = arc4->y;

    while(length--)
        *out++ = *in++ ^ MakeByte(&x, &y, arc4->state);

    arc4->x = (byte)x;
    arc4->y = (byte)y;

    return ret;
}

/* Initialize Arc4 for use with async device */
int wc_Arc4Init(Arc4* arc4, void* heap, int devId)
{
    int ret = 0;

    if (arc4 == NULL)
        return BAD_FUNC_ARG;

    arc4->heap = heap;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4)
    ret = wolfAsync_DevCtxInit(&arc4->asyncDev, WOLFSSL_ASYNC_MARKER_ARC4,
        arc4->heap, devId);
#else
    (void)devId;
#endif /* WOLFSSL_ASYNC_CRYPT */

    return ret;
}


/* Free Arc4 from use with async device */
void wc_Arc4Free(Arc4* arc4)
{
    if (arc4 == NULL)
        return;

#if defined(WOLFSSL_ASYNC_CRYPT) && defined(WC_ASYNC_ENABLE_ARC4)
    wolfAsync_DevCtxFree(&arc4->asyncDev, WOLFSSL_ASYNC_MARKER_ARC4);
#endif /* WOLFSSL_ASYNC_CRYPT */
}

#endif /* NO_RC4 */

