/**
 * This file is part of the TREZOR project, https://trezor.io/
 *
 * Copyright (c) SatoshiLabs
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

#include "slip39.h"
#include <stdio.h>
#include <string.h>
#include "slip39_wordlist.h"

/**
 * Returns word at position `index`.
 */
const char* get_word(uint16_t index) {
  if (index >= WORDS_COUNT) {
    return NULL;
  }

  return slip39_wordlist[index];
}

/**
 * Finds the index of a given word.
 * Returns true on success and stores result in `index`.
 */
bool word_index(uint16_t* index, const char* word, uint8_t word_length) {
  uint16_t lo = 0;
  uint16_t hi = WORDS_COUNT;
  uint16_t mid = 0;

  while ((hi - lo) > 1) {
    mid = (hi + lo) / 2;
    if (strncmp(slip39_wordlist[mid], word, word_length) > 0) {
      hi = mid;
    } else {
      lo = mid;
    }
  }
  if (strncmp(slip39_wordlist[lo], word, word_length) != 0) {
    return false;
  }
  *index = lo;
  return true;
}

/**
 * Returns the index of the first sequence in words_button_seq[] which is not
 * less than the given sequence. Returns WORDS_COUNT if there is no such
 * sequence.
 */
static uint16_t find_sequence(uint16_t sequence) {
  if (sequence <= words_button_seq[0].sequence) {
    return 0;
  }

  uint16_t lo = 0;
  uint16_t hi = WORDS_COUNT;

  while (hi - lo > 1) {
    uint16_t mid = (hi + lo) / 2;
    if (words_button_seq[mid].sequence >= sequence) {
      hi = mid;
    } else {
      lo = mid;
    }
  }

  return hi;
}

/**
 * Returns a word matching the button sequence prefix or NULL if no match is
 * found.
 */
const char* button_sequence_to_word(uint16_t sequence) {
  if (sequence == 0) {
    return slip39_wordlist[words_button_seq[0].index];
  }

  uint16_t multiplier = 1;
  while (sequence < 1000) {
    sequence *= 10;
    multiplier *= 10;
  }

  uint16_t i = find_sequence(sequence);
  if (i >= WORDS_COUNT ||
      words_button_seq[i].sequence - sequence >= multiplier) {
    return NULL;
  }

  return slip39_wordlist[words_button_seq[i].index];
}

/**
 * Calculates which buttons on the T9 keyboard can still be pressed after the
 * prefix was entered. Returns a 9-bit bitmask, where each bit specifies which
 * buttons can be pressed (there are still words in this combination). The least
 * significant bit corresponds to the first button.
 *
 * Example: 110000110 - second, third, eighth and ninth button still can be
 * pressed.
 */
uint16_t slip39_word_completion_mask(uint16_t prefix) {
  if (prefix >= 1000) {
    // Four char prefix -> the mask is zero.
    return 0;
  }

  // Determine the range of sequences [min, max), which have the given prefix.
  uint16_t min = prefix;
  uint16_t max = prefix + 1;
  uint16_t divider = 1;
  while (max <= 1000) {
    min *= 10;
    max *= 10;
    divider *= 10;
  }
  divider /= 10;

  // Determine the range we will be searching in words_button_seq[].
  min = find_sequence(min);
  max = find_sequence(max);

  uint16_t bitmap = 0;
  for (uint16_t i = min; i < max; ++i) {
    uint8_t digit = (words_button_seq[i].sequence / divider) % 10;
    bitmap |= 1 << (digit - 1);
  }

  return bitmap;
}
