/* Groestl hash from https://github.com/Groestlcoin/vanitygen
 * Trezor adaptation by Yura Pakhuchiy <pakhuchiy@gmail.com>. */
/**
 * Groestl interface. This code implements Groestl with the recommended
 * parameters for SHA-3, with outputs of 224, 256, 384 and 512 bits.
 *
 * ==========================(LICENSE BEGIN)============================
 *
 * Copyright (c) 2007-2010  Projet RNRT SAPHIR
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
 *
 * ===========================(LICENSE END)=============================
 *
 * @file     sph_groestl.h
 * @author   Thomas Pornin <thomas.pornin@cryptolog.com>
 */

#ifndef GROESTL_H__
#define GROESTL_H__

#include <stddef.h>

/**
 * This structure is a context for Groestl-384 and Groestl-512 computations:
 * it contains the intermediate values and some data from the last
 * entered block. Once a Groestl computation has been performed, the
 * context can be reused for another computation.
 *
 * The contents of this structure are private. A running Groestl
 * computation can be cloned by copying the context (e.g. with a simple
 * <code>memcpy()</code>).
 */
typedef struct {
	unsigned char buf[128];    /* first field, for alignment */
	size_t ptr;
	union {
		uint64_t wide[16];
		uint32_t narrow[32];
	} state;
	uint64_t count;
} sph_groestl_big_context;

typedef sph_groestl_big_context GROESTL512_CTX;

/**
 * Initialize a Groestl-512 context. This process performs no memory allocation.
 *
 * @param cc   the Groestl-512 context (pointer to a
 *             <code>GROESTL512_CTX</code>)
 */
void groestl512_Init(void *cc);

/**
 * Process some data bytes. It is acceptable that <code>len</code> is zero
 * (in which case this function does nothing).
 *
 * @param cc     the Groestl-512 context
 * @param data   the input data
 * @param len    the input data length (in bytes)
 */
void groestl512_Update(void *cc, const void *data, size_t len);

/**
 * Terminate the current Groestl-512 computation and output the result into
 * the provided buffer. The destination buffer must be wide enough to
 * accomodate the result (64 bytes). The context is automatically
 * reinitialized.
 *
 * @param cc    the Groestl-512 context
 * @param dst   the destination buffer
 */
void groestl512_Final(void *cc, void *dst);

/* Calculate double Groestl-512 hash and truncate it to 256-bits. */
void groestl512_DoubleTrunc(void *cc, void *dst);

#endif
