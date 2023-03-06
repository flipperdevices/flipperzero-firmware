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
// #include <string.h>
#include <ctype.h>
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
/* Copyright (c) 2012, The Linux Foundation. All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are
 * met:
 * * Redistributions of source code must retain the above copyright
 *  notice, this list of conditions and the following disclaimer.
 *  * Redistributions in binary form must reproduce the above
 * copyright notice, this list of conditions and the following
 * disclaimer in the documentation and/or other materials provided
 *  with the distribution.
 *   * Neither the name of The Linux Foundation nor the names of its
 * contributors may be used to endorse or promote products derived
 * from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED "AS IS" AND ANY EXPRESS OR IMPLIED
 * WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS
 * BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 * BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE
 * OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN
 * IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
// void
// flipbip_strrev(unsigned char *str)
// {
// 	int i;
// 	int j;
// 	unsigned char a;
// 	unsigned len = strlen((const char *)str);
// 	for (i = 0, j = len - 1; i < j; i++, j--)
// 	{
// 		a = str[i];
// 		str[i] = str[j];
// 		str[j] = a;
// 	}
// }
// int
// flipbip_itoa(int num, unsigned char* str, int len, int base)
// {
// 	int sum = num;
// 	int i = 0;
// 	int digit;
// 	if (len == 0)
// 		return -1;
// 	do
// 	{
// 		digit = sum % base;
// 		if (digit < 0xA)
// 			str[i++] = '0' + digit;
// 		else
// 			str[i++] = 'A' + digit - 0xA;
// 		sum /= base;
// 	}while (sum && (i < (len - 1)));
// 	if (i == (len - 1) && sum)
// 		return -1;
// 	str[i] = '\0';
// 	flipbip_strrev(str);
// 	return 0;
// }