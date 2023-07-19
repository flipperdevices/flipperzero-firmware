/*
 * Copyright (c) 1988 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 * 3. Neither the name of the University nor the names of its contributors
 *    may be used to endorse or promote products derived from this software
 *    without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE REGENTS AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */
#include "flipbip_string.h"
#include <ctype.h>
#include <stdint.h>
#include <string.h>
// From: lib/crypto
#include <memzero.h>
#include <rc4.h>

char* flipbip_strtok(char* s, const char* delim) {
    static char* last;
    return flipbip_strtok_r(s, delim, &last);
}
char* flipbip_strtok_r(char* s, const char* delim, char** last) {
    char* spanp;
    int c, sc;
    char* tok;
    if(s == NULL && (s = *last) == NULL) return (NULL);
    /*
	 * Skip (span) leading delimiters (s += strspn(s, delim), sort of).
	 */
cont:
    c = *s++;
    for(spanp = (char*)delim; (sc = *spanp++) != 0;) {
        if(c == sc) goto cont;
    }
    if(c == 0) { /* no non-delimiter characters */
        *last = NULL;
        return (NULL);
    }
    tok = s - 1;
    /*
	 * Scan token (scan for delimiters: s += strcspn(s, delim), sort of).
	 * Note that delim must have one NUL; we stop if we see that, too.
	 */
    for(;;) {
        c = *s++;
        spanp = (char*)delim;
        do {
            if((sc = *spanp++) == c) {
                if(c == 0)
                    s = NULL;
                else
                    s[-1] = 0;
                *last = s;
                return (tok);
            }
        } while(sc != 0);
    }
    /* NOTREACHED */
}

void flipbip_btox(const unsigned char* in, int in_len, char* str) {
    for(int i = 0; i < in_len; i++) {
        unsigned char n;
        unsigned char x = in[i];

        str += 2;
        *(str + (i * 2)) = '\0';

        for(n = 2; n != 0; --n) {
            *(--str + (i * 2)) = "0123456789abcdef"[x & 0x0F];
            x >>= 4;
        }
    }
}
void flipbip_xtob(const char* str, unsigned char* out, int out_len) {
    int len = strlen(str) / 2;
    if(len > out_len) len = out_len;
    for(int i = 0; i < len; i++) {
        char c = 0;
        if(str[i * 2] >= '0' && str[i * 2] <= '9') c += (str[i * 2] - '0') << 4;
        if((str[i * 2] & ~0x20) >= 'A' && (str[i * 2] & ~0x20) <= 'F')
            c += (10 + (str[i * 2] & ~0x20) - 'A') << 4;
        if(str[i * 2 + 1] >= '0' && str[i * 2 + 1] <= '9') c += (str[i * 2 + 1] - '0');
        if((str[i * 2 + 1] & ~0x20) >= 'A' && (str[i * 2 + 1] & ~0x20) <= 'F')
            c += (10 + (str[i * 2 + 1] & ~0x20) - 'A');
        out[i] = c;
    }
}

void flipbip_cipher(
    const unsigned char* key_in,
    const unsigned int key_len,
    const char* in,
    char* out,
    const unsigned int io_len) {
    if(io_len > 512) return;

    RC4_CTX ctx;
    uint8_t buf[256];
    memzero(buf, 256);

    flipbip_xtob(in, buf, io_len / 2);

    rc4_init(&ctx, key_in, key_len);
    rc4_encrypt(&ctx, buf, 256);

    flipbip_btox(buf, io_len / 2, out);

    memzero(buf, 256);
}