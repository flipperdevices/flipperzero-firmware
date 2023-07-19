/* Copyright (c) 2017, 2021 Pieter Wuille
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

#ifndef _SEGWIT_ADDR_H_
#define _SEGWIT_ADDR_H_ 1

#include <stdint.h>

// The maximum length of the Bech32 human-readable part according to BIP-173.
#define BECH32_MAX_HRP_LEN 83

/** Encode a SegWit address
 *
 *  Out: output:   Pointer to a buffer of size 73 + strlen(hrp) that will be
 *                 updated to contain the null-terminated address.
 *  In:  hrp:      Pointer to the null-terminated human readable part to use
 *                 (chain/network specific).
 *       ver:      Version of the witness program (between 0 and 16 inclusive).
 *       prog:     Data bytes for the witness program (between 2 and 40 bytes).
 *       prog_len: Number of data bytes in prog.
 *  Returns 1 if successful.
 */
int segwit_addr_encode(char* output, const char* hrp, int ver, const uint8_t* prog, size_t prog_len);

/** Decode a SegWit address
 *
 *  Out: ver:      Pointer to an int that will be updated to contain the witness
 *                 program version (between 0 and 16 inclusive).
 *       prog:     Pointer to a buffer of size 40 that will be updated to
 *                 contain the witness program bytes.
 *       prog_len: Pointer to a size_t that will be updated to contain the length
 *                 of bytes in prog.
 *       hrp:      Pointer to the null-terminated human readable part that is
 *                 expected (chain/network specific).
 *       addr:     Pointer to the null-terminated address.
 *  Returns 1 if successful.
 */
int segwit_addr_decode(int* ver, uint8_t* prog, size_t* prog_len, const char* hrp, const char* addr);

/** Supported encodings. */
typedef enum {
    BECH32_ENCODING_NONE,
    BECH32_ENCODING_BECH32,
    BECH32_ENCODING_BECH32M
} bech32_encoding;

/** Encode a Bech32 or Bech32m string
 *
 *  Out: output:  Pointer to a buffer of size strlen(hrp) + data_len + 8 that
 *                will be updated to contain the null-terminated Bech32 string.
 *  In: hrp :     Pointer to the null-terminated human readable part.
 *      data :    Pointer to an array of 5-bit values.
 *      data_len: Length of the data array.
 *      enc:      Which encoding to use (BECH32_ENCODING_BECH32{,M}).
 *  Returns 1 if successful.
 */
int bech32_encode(
    char* output,
    const char* hrp,
    const uint8_t* data,
    size_t data_len,
    bech32_encoding enc);

/** Decode a Bech32 or Bech32m string
 *
 *  Out: hrp:      Pointer to a buffer of size BECH32_MAX_HRP_LEN + 1. Will be
 *                 updated to contain the null-terminated human readable part.
 *       data:     Pointer to a buffer of size strlen(input) - 8 that will
 *                 hold the encoded 5-bit data values.
 *       data_len: Pointer to a size_t that will be updated to be the number
 *                 of entries in data.
 *  In: input:     Pointer to a null-terminated Bech32 string.
 *  Returns BECH32_ENCODING_BECH32{,M} to indicate decoding was successful
 *  with the specified encoding standard. BECH32_ENCODING_NONE is returned if
 *  decoding failed.
 */
bech32_encoding bech32_decode(char* hrp, uint8_t* data, size_t* data_len, const char* input);

#endif
