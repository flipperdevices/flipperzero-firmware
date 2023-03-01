#ifndef __ZKP_CONTEXT_H__
#define __ZKP_CONTEXT_H__

#include <stdint.h>

#include "vendor/secp256k1-zkp/include/secp256k1_preallocated.h"

int secp256k1_context_writable_randomize(secp256k1_context *context);
bool zkp_context_is_initialized(void);
int zkp_context_init(void);
void zkp_context_destroy(void);
const secp256k1_context *zkp_context_get_read_only(void);
secp256k1_context *zkp_context_acquire_writable(void);
void zkp_context_release_writable(void);

#endif
