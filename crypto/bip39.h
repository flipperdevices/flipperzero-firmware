/**
 * Copyright (c) 2013-2014 Tomas Dzetkulic
 * Copyright (c) 2013-2014 Pavol Rusnak
 *
 * Permission is hereby granted, free of charge, to any person obtaining
 * a copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, including without limitation
 * the rights to use, copy, modify, merge, publish, distribute, sublicense,
 * and/or sell copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
 * OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
 * THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES
 * OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
 * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 */

#ifndef __BIP39_H__
#define __BIP39_H__

#include <stdbool.h>
#include <stdint.h>

#include "options.h"

#define BIP39_WORD_COUNT 2048
#define BIP39_PBKDF2_ROUNDS 2048

#if USE_BIP39_CACHE
void bip39_cache_clear(void);
#endif

extern const char *const BIP39_WORDLIST_ENGLISH[BIP39_WORD_COUNT];

const char *mnemonic_generate(int strength);  // strength in bits
const char *mnemonic_from_data(const uint8_t *data, int len);
void mnemonic_clear(void);

int mnemonic_check(const char *mnemonic);

int mnemonic_to_bits(const char *mnemonic, uint8_t *bits);

// passphrase must be at most 256 characters otherwise it would be truncated
void mnemonic_to_seed(const char *mnemonic, const char *passphrase,
                      uint8_t seed[512 / 8],
                      void (*progress_callback)(uint32_t current,
                                                uint32_t total));

int mnemonic_find_word(const char *word);
const char *mnemonic_complete_word(const char *prefix, int len);
const char *mnemonic_get_word(int index);
uint32_t mnemonic_word_completion_mask(const char *prefix, int len);

#endif
