/**
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

#include <assert.h>
#include <stdatomic.h>
#include <stdbool.h>

#include "memzero.h"
#include "rand.h"
#include "zkp_context.h"

#include "vendor/secp256k1-zkp/include/secp256k1.h"

static uint8_t context_buffer[SECP256K1_CONTEXT_SIZE];
static secp256k1_context *context;
static volatile atomic_flag locked;

// returns 0 on success
int secp256k1_context_writable_randomize(secp256k1_context *context_writable) {
  uint8_t seed[32] = {0};
  random_buffer(seed, sizeof(seed));
  int returned = secp256k1_context_randomize(context_writable, seed);
  memzero(seed, sizeof(seed));

  if (returned != 1) {
    return 1;
  }

  return 0;
}

bool zkp_context_is_initialized(void) { return context != NULL; }

// returns 0 on success
int zkp_context_init(void) {
  assert(context == NULL);

  const unsigned int context_flags =
      SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY;

  const size_t context_size =
      secp256k1_context_preallocated_size(context_flags);
  // Assert the context is as small as possible
  assert(context_size == SECP256K1_CONTEXT_SIZE);
  if (context_size == 0 || context_size > SECP256K1_CONTEXT_SIZE) {
    return 1;
  }

  context =
      secp256k1_context_preallocated_create(context_buffer, context_flags);
  if (context == NULL) {
    return 1;
  }

  secp256k1_context_writable_randomize(context);

  atomic_flag_clear(&locked);

  return 0;
}

void zkp_context_destroy(void) {
  assert(context != NULL);

  secp256k1_context_preallocated_destroy(context);
  memzero(context_buffer, sizeof(context_buffer));
  atomic_flag_clear(&locked);
  context = NULL;
}

const secp256k1_context *zkp_context_get_read_only(void) {
  assert(context != NULL);

  return context;
}

// returns NULL if context cannot be acquired
secp256k1_context *zkp_context_acquire_writable(void) {
  assert(context != NULL);

  // We don't expect the context to be used by multiple threads
  if (atomic_flag_test_and_set(&locked)) {
    return NULL;
  }

  return context;
}

void zkp_context_release_writable(void) {
  assert(context != NULL);

  atomic_flag_clear(&locked);
}
