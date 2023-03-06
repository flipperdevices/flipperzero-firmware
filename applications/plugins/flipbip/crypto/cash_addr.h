/* Copyright (c) 2017 Jochen Hoenicke, Pieter Wuille
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#ifndef _CASH_ADDR_H_
#define _CASH_ADDR_H_ 1

#include <stdint.h>

/** Encode a Cashaddr address
 *
 *  Out: output:   Pointer to a buffer of size 105 + strlen(hrp) that will be
 *                 updated to contain the null-terminated address.
 *  In:  hrp:      Pointer to the null-terminated human readable part to use
 *                 (chain/network specific).
 *       prog:     Data bytes for the hash (between 21 and 65 bytes).
 *       prog_len: Number of data bytes in prog.
 *  Returns 1 if successful.
 */
int cash_addr_encode(char *output, const char *hrp, const uint8_t *prog,
                     size_t prog_len);

/** Decode a CashAddr address
 *
 *  Out: prog:     Pointer to a buffer of size 65 that will be updated to
 *                 contain the witness program bytes.
 *       prog_len: Pointer to a size_t that will be updated to contain the
 * length of bytes in prog. hrp:      Pointer to the null-terminated human
 * readable part that is expected (chain/network specific). addr:     Pointer to
 * the null-terminated address. Returns 1 if successful.
 */
int cash_addr_decode(uint8_t *prog, size_t *prog_len, const char *hrp,
                     const char *addr);

/** Encode a Cash string
 *
 *  Out: output:  Pointer to a buffer of size strlen(hrp) + data_len + 8 that
 *                will be updated to contain the null-terminated Cash string.
 *  In: hrp :     Pointer to the null-terminated human readable part.
 *      data :    Pointer to an array of 5-bit values.
 *      data_len: Length of the data array.
 *  Returns 1 if successful.
 */
int cash_encode(char *output, const char *hrp, const uint8_t *data,
                size_t data_len);

/** Decode a Cash string
 *
 *  Out: hrp:      Pointer to a buffer of size strlen(input) - 6. Will be
 *                 updated to contain the null-terminated human readable part.
 *       data:     Pointer to a buffer of size strlen(input) - 8 that will
 *                 hold the encoded 5-bit data values.
 *       data_len: Pointer to a size_t that will be updated to be the number
 *                 of entries in data.
 *  In: input:     Pointer to a null-terminated Cash string.
 *  Returns 1 if succesful.
 */
int cash_decode(char *hrp, uint8_t *data, size_t *data_len, const char *input);

#endif
