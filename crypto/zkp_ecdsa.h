#ifndef __ZKP_ECDSA_H__
#define __ZKP_ECDSA_H__

#include <stdint.h>

#include "hasher.h"

int zkp_ecdsa_get_public_key33(const ecdsa_curve *curve,
                               const uint8_t *private_key_bytes,
                               uint8_t *public_key_bytes);
int zkp_ecdsa_get_public_key65(const ecdsa_curve *curve,
                               const uint8_t *private_key_bytes,
                               uint8_t *public_key_bytes);
int zkp_ecdsa_sign_digest(const ecdsa_curve *curve,
                          const uint8_t *private_key_bytes,
                          const uint8_t *digest, uint8_t *signature_bytes,
                          uint8_t *pby,
                          int (*is_canonical)(uint8_t by, uint8_t sig[64]));
int zkp_ecdsa_recover_pub_from_sig(const ecdsa_curve *curve,
                                   uint8_t *public_key_bytes,
                                   const uint8_t *signature_bytes,
                                   const uint8_t *digest, int recid);
int zkp_ecdsa_verify_digest(const ecdsa_curve *curve,
                            const uint8_t *public_key_bytes,
                            const uint8_t *signature_bytes,
                            const uint8_t *digest);
int zkp_ecdsa_verify(const ecdsa_curve *curve, HasherType hasher_sign,
                     const uint8_t *pub_key, const uint8_t *sig,
                     const uint8_t *msg, uint32_t msg_len);
#endif
