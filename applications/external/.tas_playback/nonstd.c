/* Teensyduino Core Library
 * http://www.pjrc.com/teensy/
 * Copyright (c) 2017 PJRC.COM, LLC.
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the
 * "Software"), to deal in the Software without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to
 * the following conditions:
 *
 * 1. The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * 2. If the Software is incorporated into a build system that allows
 * selection among a list of target devices, then similar target
 * devices manufactured by PJRC.COM must be included in the list of
 * target devices and selectable in the same manner.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS
 * BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
 * ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
 * CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <furi.h>
#include "avr_functions.h"

char* ultoa(unsigned long val, char* buf, int radix) {
    unsigned digit;
    int i = 0, j;
    char t;

    while(1) {
        digit = val % radix;
        buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
        val /= radix;
        if(val == 0) break;
        i++;
    }
    buf[i + 1] = 0;
    for(j = 0; j < i; j++, i--) {
        t = buf[j];
        buf[j] = buf[i];
        buf[i] = t;
    }
    return buf;
}

char* ltoa(long val, char* buf, int radix) {
    if(val >= 0) {
        return ultoa(val, buf, radix);
    } else {
        buf[0] = '-';
        ultoa(-val, buf + 1, radix);
        return buf;
    }
}

char* ulltoa(unsigned long long val, char* buf, int radix) {
    unsigned digit;
    int i = 0, j;
    char t;

    while(1) {
        digit = val % radix;
        buf[i] = ((digit < 10) ? '0' + digit : 'A' + digit - 10);
        val /= radix;
        if(val == 0) break;
        i++;
    }
    buf[i + 1] = 0;
    for(j = 0; j < i; j++, i--) {
        t = buf[j];
        buf[j] = buf[i];
        buf[i] = t;
    }
    return buf;
}

char* lltoa(long long val, char* buf, int radix) {
    if(val >= 0) {
        return ulltoa(val, buf, radix);
    } else {
        buf[0] = '-';
        ulltoa(-val, buf + 1, radix);
        return buf;
    }
}

#define DTOA_UPPER 0x04

char* dtostrf(float val, int width, unsigned int precision, char* buf) {
    sprintf(buf, "%*.*f", width, (int)precision, (double)val);
    return buf;
}
