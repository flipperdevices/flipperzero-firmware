#ifndef __ZKP_BIP340_H__
#define __ZKP_BIP340_H__

#include <stdint.h>

int zkp_bip340_get_public_key(const uint8_t *private_key_bytes,
                              uint8_t *public_key_bytes);
int zkp_bip340_sign_digest(const uint8_t *private_key_bytes,
                           const uint8_t *digest, uint8_t *signature_bytes,
                           uint8_t *auxiliary_data);
int zkp_bip340_verify_digest(const uint8_t *public_key_bytes,
                             const uint8_t *signature_bytes,
                             const uint8_t *digest);
int zkp_bip340_verify_publickey(const uint8_t *public_key_bytes);
int zkp_bip340_tweak_public_key(const uint8_t *internal_public_key,
                                const uint8_t *root_hash,
                                uint8_t *output_public_key);
int zkp_bip340_tweak_private_key(const uint8_t *internal_private_key,
                                 const uint8_t *root_hash,
                                 uint8_t *output_private_key);

#endif
