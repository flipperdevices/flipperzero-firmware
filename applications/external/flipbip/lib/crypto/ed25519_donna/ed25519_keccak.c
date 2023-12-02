#include "../options.h"

#if USE_KECCAK

#include <stddef.h>

#include "ed25519_keccak.h"
#include "ed25519_hash_custom_keccak.h"

#define ED25519_SUFFIX _keccak

#include "ed25519.c"

#endif // USE_KECCAK