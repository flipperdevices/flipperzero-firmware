// /*
//  * This file is part of the Trezor project, https://trezor.io/
//  *
//  * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
//  *
//  * This library is free software: you can redistribute it and/or modify
//  * it under the terms of the GNU Lesser General Public License as published by
//  * the Free Software Foundation, either version 3 of the License, or
//  * (at your option) any later version.
//  *
//  * This library is distributed in the hope that it will be useful,
//  * but WITHOUT ANY WARRANTY; without even the implied warranty of
//  * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//  * GNU Lesser General Public License for more details.
//  *
//  * You should have received a copy of the GNU Lesser General Public License
//  * along with this library.  If not, see <http://www.gnu.org/licenses/>.
//  */

// #ifndef __CRYPTO_H__
// #define __CRYPTO_H__

// #include "../crypto/bip32.h"
// #include "../crypto/ecdsa.h"
// #include <pb.h>
// #include "../crypto/sha2.h"
// #include <stdbool.h>
// #include <stdint.h>
// #include <stdlib.h>
// #include "coins.h"
// #include "../crypto/hasher.h"
// // #include "messages-bitcoin.pb.h"
// // #include "messages-crypto.pb.h"

// #define BITCOIN_ONLY 1

// #define PATH_HARDENED 0x80000000
// #define PATH_UNHARDEN_MASK 0x7fffffff
// #define PATH_MAX_ACCOUNT 100
// #define PATH_MAX_CHANGE 1
// #define PATH_MAX_ADDRESS_INDEX 1000000
// #define PATH_SLIP25_PURPOSE (PATH_HARDENED | 10025)

// #define ser_length_size(len) ((len) < 253 ? 1 : (len) < 0x10000 ? 3 : 5)

// uint32_t ser_length(uint32_t len, uint8_t *out);

// uint32_t ser_length_hash(Hasher *hasher, uint32_t len);

// int sshMessageSign(HDNode *node, const uint8_t *message, size_t message_len,
//                    uint8_t *signature);

// int gpgMessageSign(HDNode *node, const uint8_t *message, size_t message_len,
//                    uint8_t *signature);

// int signifyMessageSign(HDNode *node, const uint8_t *message, size_t message_len,
//                        uint8_t *signature);

// int cryptoMessageSign(const CoinInfo *coin, HDNode *node,
//                       InputScriptType script_type, bool no_script_type,
//                       const uint8_t *message, size_t message_len,
//                       uint8_t *signature);

// int cryptoMessageVerify(const CoinInfo *coin, const uint8_t *message,
//                         size_t message_len, const char *address,
//                         const uint8_t *signature);

// const HDNode *cryptoMultisigPubkey(const CoinInfo *coin,
//                                    const MultisigRedeemScriptType *multisig,
//                                    uint32_t index);

// uint32_t cryptoMultisigPubkeyCount(const MultisigRedeemScriptType *multisig);

// int cryptoMultisigPubkeyIndex(const CoinInfo *coin,
//                               const MultisigRedeemScriptType *multisig,
//                               const uint8_t *pubkey);

// int cryptoMultisigFingerprint(const MultisigRedeemScriptType *multisig,
//                               uint8_t *hash);

// int cryptoIdentityFingerprint(const IdentityType *identity, uint8_t *hash);

// bool coin_path_check(const CoinInfo *coin, InputScriptType script_type,
//                      uint32_t address_n_count, const uint32_t *address_n,
//                      bool has_multisig, bool full_check);

// #endif
