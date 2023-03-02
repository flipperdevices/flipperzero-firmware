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

// #include "crypto.h"
// #include <string.h>
// #include "../crypto/address.h"
// #include "../crypto/aes/aes.h"
// #include "../crypto/base58.h"
// #include "../crypto/bip32.h"
// #include "coins.h"
// #include "../crypto/curves.h"
// #include "../crypto/hmac.h"
// //#include "layout.h"
// #include "../crypto/pbkdf2.h"
// #include "../crypto/secp256k1.h"
// #include "../crypto/segwit_addr.h"
// #include "../crypto/sha2.h"

// #if !BITCOIN_ONLY
// #include "cash_addr.h"
// #endif

// uint32_t ser_length(uint32_t len, uint8_t *out) {
//   if (len < 253) {
//     out[0] = len & 0xFF;
//     return 1;
//   }
//   if (len < 0x10000) {
//     out[0] = 253;
//     out[1] = len & 0xFF;
//     out[2] = (len >> 8) & 0xFF;
//     return 3;
//   }
//   out[0] = 254;
//   out[1] = len & 0xFF;
//   out[2] = (len >> 8) & 0xFF;
//   out[3] = (len >> 16) & 0xFF;
//   out[4] = (len >> 24) & 0xFF;
//   return 5;
// }

// uint32_t ser_length_hash(Hasher *hasher, uint32_t len) {
//   if (len < 253) {
//     hasher_Update(hasher, (const uint8_t *)&len, 1);
//     return 1;
//   }
//   if (len < 0x10000) {
//     uint8_t d = 253;
//     hasher_Update(hasher, &d, 1);
//     hasher_Update(hasher, (const uint8_t *)&len, 2);
//     return 3;
//   }
//   uint8_t d = 254;
//   hasher_Update(hasher, &d, 1);
//   hasher_Update(hasher, (const uint8_t *)&len, 4);
//   return 5;
// }

// uint32_t deser_length(const uint8_t *in, uint32_t *out) {
//   if (in[0] < 253) {
//     *out = in[0];
//     return 1;
//   }
//   if (in[0] == 253) {
//     *out = in[1] + (in[2] << 8);
//     return 1 + 2;
//   }
//   if (in[0] == 254) {
//     *out = in[1] + (in[2] << 8) + (in[3] << 16) + ((uint32_t)in[4] << 24);
//     return 1 + 4;
//   }
//   *out = 0;  // ignore 64 bit
//   return 1 + 8;
// }

// int sshMessageSign(HDNode *node, const uint8_t *message, size_t message_len,
//                    uint8_t *signature) {
//   signature[0] = 0;  // prefix: pad with zero, so all signatures are 65 bytes
//   return hdnode_sign(node, message, message_len, HASHER_SHA2, signature + 1,
//                      NULL, NULL);
// }

// int gpgMessageSign(HDNode *node, const uint8_t *message, size_t message_len,
//                    uint8_t *signature) {
//   signature[0] = 0;  // prefix: pad with zero, so all signatures are 65 bytes
//   const curve_info *ed25519_curve_info = get_curve_by_name(ED25519_NAME);
//   if (ed25519_curve_info && node->curve == ed25519_curve_info) {
//     // GPG supports variable size digest for Ed25519 signatures
//     return hdnode_sign(node, message, message_len, 0, signature + 1, NULL,
//                        NULL);
//   } else {
//     // Ensure 256-bit digest before proceeding
//     if (message_len != 32) {
//       return 1;
//     }
//     return hdnode_sign_digest(node, message, signature + 1, NULL, NULL);
//   }
// }

// int signifyMessageSign(HDNode *node, const uint8_t *message, size_t message_len,
//                        uint8_t *signature) {
//   signature[0] = 0;  // prefix: pad with zero, so all signatures are 65 bytes
//   const curve_info *ed25519_curve_info = get_curve_by_name(ED25519_NAME);
//   // only ed25519 is supported
//   if (!ed25519_curve_info || node->curve != ed25519_curve_info) {
//     return 1;
//   }
//   return hdnode_sign(node, message, message_len, 0, signature + 1, NULL, NULL);
// }

// static void cryptoMessageHash(const CoinInfo *coin, const uint8_t *message,
//                               size_t message_len,
//                               uint8_t hash[HASHER_DIGEST_LENGTH]) {
//   Hasher hasher = {0};
//   hasher_Init(&hasher, coin->curve->hasher_sign);
//   hasher_Update(&hasher, (const uint8_t *)coin->signed_message_header,
//                 strlen(coin->signed_message_header));
//   uint8_t varint[5] = {0};
//   uint32_t l = ser_length(message_len, varint);
//   hasher_Update(&hasher, varint, l);
//   hasher_Update(&hasher, message, message_len);
//   hasher_Final(&hasher, hash);
// }

// int cryptoMessageSign(const CoinInfo *coin, HDNode *node,
//                       InputScriptType script_type, bool no_script_type,
//                       const uint8_t *message, size_t message_len,
//                       uint8_t *signature) {
//   uint8_t script_type_info = 0;
//   switch (script_type) {
//     case InputScriptType_SPENDADDRESS:
//       // p2pkh
//       script_type_info = 0;
//       break;
//     case InputScriptType_SPENDP2SHWITNESS:
//       // segwit-in-p2sh
//       script_type_info = 4;
//       break;
//     case InputScriptType_SPENDWITNESS:
//       // segwit
//       script_type_info = 8;
//       break;
//     default:
//       // unsupported script type
//       return 1;
//   }

//   if (no_script_type) {
//     script_type_info = 0;
//   }

//   uint8_t hash[HASHER_DIGEST_LENGTH] = {0};
//   cryptoMessageHash(coin, message, message_len, hash);

//   uint8_t pby = 0;
//   int result = hdnode_sign_digest(node, hash, signature + 1, &pby, NULL);
//   if (result == 0) {
//     signature[0] = 31 + pby + script_type_info;
//   }
//   return result;
// }

// // Determines the script type from a non-multisig address.
// static InputScriptType address_to_script_type(const CoinInfo *coin,
//                                               const char *address) {
//   uint8_t addr_raw[MAX_ADDR_RAW_SIZE] = {0};
//   size_t addr_raw_len = 0;

//   // Native SegWit
//   if (coin->bech32_prefix) {
//     int witver = 0;
//     if (segwit_addr_decode(&witver, addr_raw, &addr_raw_len,
//                            coin->bech32_prefix, address)) {
//       switch (witver) {
//         case 0:
//           return InputScriptType_SPENDWITNESS;
//         case 1:
//           return InputScriptType_SPENDTAPROOT;
//         default:
//           return InputScriptType_EXTERNAL;  // unknown script type
//       }
//     }
//   }

// #if !BITCOIN_ONLY
//   if (coin->cashaddr_prefix &&
//       cash_addr_decode(addr_raw, &addr_raw_len, coin->cashaddr_prefix,
//                        address)) {
//     return InputScriptType_SPENDADDRESS;
//   }
// #endif

//   addr_raw_len = base58_decode_check(address, coin->curve->hasher_base58,
//                                      addr_raw, sizeof(addr_raw));

//   // P2PKH
//   if (addr_raw_len > address_prefix_bytes_len(coin->address_type) &&
//       address_check_prefix(addr_raw, coin->address_type)) {
//     return InputScriptType_SPENDADDRESS;
//   }

//   // P2SH
//   if (addr_raw_len > address_prefix_bytes_len(coin->address_type_p2sh) &&
//       address_check_prefix(addr_raw, coin->address_type_p2sh)) {
//     return InputScriptType_SPENDP2SHWITNESS;
//   }

//   return InputScriptType_EXTERNAL;  // unknown script type
// }

// int cryptoMessageVerify(const CoinInfo *coin, const uint8_t *message,
//                         size_t message_len, const char *address,
//                         const uint8_t *signature) {
//   // check if the address is correct
//   InputScriptType script_type = address_to_script_type(coin, address);
//   if (script_type == InputScriptType_EXTERNAL) {
//     return 1;  // invalid address
//   }

//   if (signature[0] >= 27 && signature[0] <= 34) {
//     // p2pkh or no script type provided
//     // use the script type from the address
//   } else if (signature[0] >= 35 && signature[0] <= 38) {
//     // segwit-in-p2sh
//     if (script_type != InputScriptType_SPENDP2SHWITNESS) {
//       return 2;  // script type mismatch
//     }
//   } else if (signature[0] >= 39 && signature[0] <= 42) {
//     // segwit
//     if (script_type != InputScriptType_SPENDWITNESS) {
//       return 2;  // script type mismatch
//     }
//   } else {
//     return 3;  // invalid signature prefix
//   }

//   uint8_t hash[HASHER_DIGEST_LENGTH] = {0};
//   cryptoMessageHash(coin, message, message_len, hash);

//   uint8_t recid = (signature[0] - 27) % 4;
//   bool compressed = signature[0] >= 31;

//   // check if signature verifies the digest and recover the public key
//   uint8_t pubkey[65] = {0};
//   if (ecdsa_recover_pub_from_sig(coin->curve->params, pubkey, signature + 1,
//                                  hash, recid) != 0) {
//     return 4;  // invalid signature data
//   }

//   // convert public key to compressed pubkey if necessary
//   if (compressed) {
//     pubkey[0] = 0x02 | (pubkey[64] & 1);
//   }

//   uint8_t addr_raw[MAX_ADDR_RAW_SIZE] = {0};
//   uint8_t recovered_raw[MAX_ADDR_RAW_SIZE] = {0};

//   if (script_type == InputScriptType_SPENDADDRESS) {
//     // p2pkh
//     size_t len = 0;
// #if !BITCOIN_ONLY
//     if (coin->cashaddr_prefix) {
//       if (!cash_addr_decode(addr_raw, &len, coin->cashaddr_prefix, address)) {
//         return 1;  // invalid address
//       }
//     } else
// #endif
//     {
//       len = base58_decode_check(address, coin->curve->hasher_base58, addr_raw,
//                                 MAX_ADDR_RAW_SIZE);
//     }
//     ecdsa_get_address_raw(pubkey, coin->address_type,
//                           coin->curve->hasher_pubkey, recovered_raw);
//     if (memcmp(recovered_raw, addr_raw, len) != 0 ||
//         len != address_prefix_bytes_len(coin->address_type) + 20) {
//       return 5;  // signature does not match address and message
//     }
//   } else if (script_type == InputScriptType_SPENDP2SHWITNESS) {
//     // segwit-in-p2sh
//     size_t len = base58_decode_check(address, coin->curve->hasher_base58,
//                                      addr_raw, MAX_ADDR_RAW_SIZE);
//     ecdsa_get_address_segwit_p2sh_raw(pubkey, coin->address_type_p2sh,
//                                       coin->curve->hasher_pubkey,
//                                       recovered_raw);
//     if (memcmp(recovered_raw, addr_raw, len) != 0 ||
//         len != address_prefix_bytes_len(coin->address_type_p2sh) + 20) {
//       return 5;  // signature does not match address and message
//     }
//   } else if (script_type == InputScriptType_SPENDWITNESS) {
//     // segwit
//     int witver = 0;
//     size_t len = 0;
//     if (!coin->bech32_prefix ||
//         !segwit_addr_decode(&witver, recovered_raw, &len, coin->bech32_prefix,
//                             address)) {
//       return 1;  // invalid address
//     }
//     ecdsa_get_pubkeyhash(pubkey, coin->curve->hasher_pubkey, addr_raw);
//     if (memcmp(recovered_raw, addr_raw, len) != 0 || witver != 0 || len != 20) {
//       return 5;  // signature does not match address and message
//     }
//   } else {
//     return 1;  // invalid address
//   }

//   return 0;
// }

// const HDNode *cryptoMultisigPubkey(const CoinInfo *coin,
//                                    const MultisigRedeemScriptType *multisig,
//                                    uint32_t index) {
//   const HDNodeType *node_ptr = NULL;
//   const uint32_t *address_n = NULL;
//   uint32_t address_n_count = 0;
//   if (multisig->nodes_count) {  // use multisig->nodes
//     if (index >= multisig->nodes_count) {
//       return 0;
//     }
//     node_ptr = &(multisig->nodes[index]);
//     address_n = multisig->address_n;
//     address_n_count = multisig->address_n_count;
//   } else if (multisig->pubkeys_count) {  // use multisig->pubkeys
//     if (index >= multisig->pubkeys_count) {
//       return 0;
//     }
//     node_ptr = &(multisig->pubkeys[index].node);
//     address_n = multisig->pubkeys[index].address_n;
//     address_n_count = multisig->pubkeys[index].address_n_count;
//   } else {
//     return 0;
//   }
//   if (node_ptr->chain_code.size != 32) return 0;
//   if (node_ptr->public_key.size != 33) return 0;
//   static HDNode node;
//   if (!hdnode_from_xpub(node_ptr->depth, node_ptr->child_num,
//                         node_ptr->chain_code.bytes, node_ptr->public_key.bytes,
//                         coin->curve_name, &node)) {
//     return 0;
//   }
//   //layoutProgressUpdate(true);
//   for (uint32_t i = 0; i < address_n_count; i++) {
//     if (!hdnode_public_ckd(&node, address_n[i])) {
//       return 0;
//     }
//     //layoutProgressUpdate(true);
//   }
//   return &node;
// }

// uint32_t cryptoMultisigPubkeyCount(const MultisigRedeemScriptType *multisig) {
//   return multisig->nodes_count ? multisig->nodes_count
//                                : multisig->pubkeys_count;
// }

// int cryptoMultisigPubkeyIndex(const CoinInfo *coin,
//                               const MultisigRedeemScriptType *multisig,
//                               const uint8_t *pubkey) {
//   for (size_t i = 0; i < cryptoMultisigPubkeyCount(multisig); i++) {
//     const HDNode *pubnode = cryptoMultisigPubkey(coin, multisig, i);
//     if (pubnode && memcmp(pubnode->public_key, pubkey, 33) == 0) {
//       return i;
//     }
//   }
//   return -1;
// }

// int cryptoMultisigFingerprint(const MultisigRedeemScriptType *multisig,
//                               uint8_t *hash) {
//   static const HDNodeType *pubnodes[15], *swap;
//   const uint32_t n = cryptoMultisigPubkeyCount(multisig);
//   if (n < 1 || n > 15) {
//     return 0;
//   }
//   if (multisig->m < 1 || multisig->m > 15) {
//     return 0;
//   }
//   for (uint32_t i = 0; i < n; i++) {
//     if (multisig->nodes_count) {  // use multisig->nodes
//       pubnodes[i] = &(multisig->nodes[i]);
//     } else if (multisig->pubkeys_count) {  // use multisig->pubkeys
//       pubnodes[i] = &(multisig->pubkeys[i].node);
//     } else {
//       return 0;
//     }
//   }
//   for (uint32_t i = 0; i < n; i++) {
//     if (pubnodes[i]->public_key.size != 33) return 0;
//     if (pubnodes[i]->chain_code.size != 32) return 0;
//   }
//   // minsort according to pubkey
//   for (uint32_t i = 0; i < n - 1; i++) {
//     for (uint32_t j = n - 1; j > i; j--) {
//       if (memcmp(pubnodes[i]->public_key.bytes, pubnodes[j]->public_key.bytes,
//                  33) > 0) {
//         swap = pubnodes[i];
//         pubnodes[i] = pubnodes[j];
//         pubnodes[j] = swap;
//       }
//     }
//   }
//   // hash sorted nodes
//   SHA256_CTX ctx = {0};
//   sha256_Init(&ctx);
//   sha256_Update(&ctx, (const uint8_t *)&(multisig->m), sizeof(uint32_t));
//   for (uint32_t i = 0; i < n; i++) {
//     sha256_Update(&ctx, (const uint8_t *)&(pubnodes[i]->depth),
//                   sizeof(uint32_t));
//     sha256_Update(&ctx, (const uint8_t *)&(pubnodes[i]->fingerprint),
//                   sizeof(uint32_t));
//     sha256_Update(&ctx, (const uint8_t *)&(pubnodes[i]->child_num),
//                   sizeof(uint32_t));
//     sha256_Update(&ctx, pubnodes[i]->chain_code.bytes, 32);
//     sha256_Update(&ctx, pubnodes[i]->public_key.bytes, 33);
//   }
//   sha256_Update(&ctx, (const uint8_t *)&n, sizeof(uint32_t));
//   sha256_Final(&ctx, hash);
//   //layoutProgressUpdate(true);
//   return 1;
// }

// int cryptoIdentityFingerprint(const IdentityType *identity, uint8_t *hash) {
//   SHA256_CTX ctx = {0};
//   sha256_Init(&ctx);
//   sha256_Update(&ctx, (const uint8_t *)&(identity->index), sizeof(uint32_t));
//   if (identity->has_proto && identity->proto[0]) {
//     sha256_Update(&ctx, (const uint8_t *)(identity->proto),
//                   strlen(identity->proto));
//     sha256_Update(&ctx, (const uint8_t *)"://", 3);
//   }
//   if (identity->has_user && identity->user[0]) {
//     sha256_Update(&ctx, (const uint8_t *)(identity->user),
//                   strlen(identity->user));
//     sha256_Update(&ctx, (const uint8_t *)"@", 1);
//   }
//   if (identity->has_host && identity->host[0]) {
//     sha256_Update(&ctx, (const uint8_t *)(identity->host),
//                   strlen(identity->host));
//   }
//   if (identity->has_port && identity->port[0]) {
//     sha256_Update(&ctx, (const uint8_t *)":", 1);
//     sha256_Update(&ctx, (const uint8_t *)(identity->port),
//                   strlen(identity->port));
//   }
//   if (identity->has_path && identity->path[0]) {
//     sha256_Update(&ctx, (const uint8_t *)(identity->path),
//                   strlen(identity->path));
//   }
//   sha256_Final(&ctx, hash);
//   return 1;
// }

// static bool check_cointype(const CoinInfo *coin, uint32_t slip44, bool full) {
// #if BITCOIN_ONLY
//   (void)full;
// #else
//   if (!full) {
//     // Some wallets such as Electron-Cash (BCH) store coins on Bitcoin paths.
//     // We can allow spending these coins from Bitcoin paths if the coin has
//     // implemented strong replay protection via SIGHASH_FORKID. However, we
//     // cannot allow spending any testnet coins from Bitcoin paths, because
//     // otherwise an attacker could trick the user into spending BCH on a Bitcoin
//     // path by signing a seemingly harmless BCH Testnet transaction.
//     if (slip44 == SLIP44_BITCOIN && coin->has_fork_id &&
//         coin->coin_type != SLIP44_TESTNET) {
//       return true;
//     }
//   }
// #endif
//   return coin->coin_type == slip44;
// }

// bool coin_path_check(const CoinInfo *coin, InputScriptType script_type,
//                      uint32_t address_n_count, const uint32_t *address_n,
//                      bool has_multisig, bool full_check) {
//   // This function checks that the path is a recognized path for the given coin.
//   // Used by GetAddress to prevent ransom attacks where a user could be coerced
//   // to use an address with an unenumerable path and used by SignTx to ensure
//   // that a user cannot be coerced into signing a testnet transaction or a
//   // Litecoin transaction which in fact spends Bitcoin. If full_check is true,
//   // then this function also checks that the path fully matches the script type
//   // and coin type. This is used to determine whether a warning should be shown.

//   if (address_n_count == 0) {
//     return false;
//   }

//   bool valid = true;
//   // m/44' : BIP44 Legacy
//   // m / purpose' / coin_type' / account' / change / address_index
//   if (address_n[0] == PATH_HARDENED + 44) {
//     valid = valid && (address_n_count == 5);
//     valid = valid && check_cointype(coin, address_n[1], full_check);
//     valid = valid && (address_n[2] & PATH_HARDENED);
//     valid = valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//     valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
//     valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       valid = valid && (script_type == InputScriptType_SPENDADDRESS);
//       valid = valid && (!has_multisig);
//     }
//     return valid;
//   }

//   if (address_n[0] == PATH_HARDENED + 45) {
//     if (address_n_count == 4) {
//       // m/45' - BIP45 Copay Abandoned Multisig P2SH
//       // m / purpose' / cosigner_index / change / address_index
//       // Patterns without a coin_type field must be treated as Bitcoin paths.
//       valid = valid && check_cointype(coin, SLIP44_BITCOIN, false);
//       valid = valid && (address_n[1] <= 100);
//       valid = valid && (address_n[2] <= PATH_MAX_CHANGE);
//       valid = valid && (address_n[3] <= PATH_MAX_ADDRESS_INDEX);
//     } else if (address_n_count == 5) {
//       // Unchained Capital compatibility pattern. Will be removed in the
//       // future.
//       // m / 45' / coin_type' / account' / [0-1000000] / address_index
//       valid = valid && check_cointype(coin, address_n[1], full_check);
//       valid = valid && (address_n[2] & PATH_HARDENED);
//       valid =
//           valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//       valid = valid && (address_n[3] <= 1000000);
//       valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//     } else if (address_n_count == 6) {
//       // Unchained Capital compatibility pattern. Will be removed in the
//       // future.
//       // m/45'/coin_type'/account'/[0-1000000]/change/address_index
//       // m/45'/coin_type/account/[0-1000000]/change/address_index
//       valid = valid &&
//               check_cointype(coin, PATH_HARDENED | address_n[1], full_check);
//       valid = valid && ((address_n[1] & PATH_HARDENED) ==
//                         (address_n[2] & PATH_HARDENED));
//       valid =
//           valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//       valid = valid && (address_n[3] <= 1000000);
//       valid = valid && (address_n[4] <= PATH_MAX_CHANGE);
//       valid = valid && (address_n[5] <= PATH_MAX_ADDRESS_INDEX);
//     } else {
//       return false;
//     }

//     if (full_check) {
//       valid = valid && (script_type == InputScriptType_SPENDADDRESS ||
//                         script_type == InputScriptType_SPENDMULTISIG);
//       valid = valid && has_multisig;
//     }

//     return valid;
//   }

//   if (address_n[0] == PATH_HARDENED + 48) {
//     valid = valid && (address_n_count == 5 || address_n_count == 6);
//     valid = valid && check_cointype(coin, address_n[1], full_check);
//     valid = valid && (address_n[2] & PATH_HARDENED);
//     valid = valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//     if (address_n_count == 5) {
//       // [OBSOLETE] m/48' Copay Multisig P2SH
//       // m / purpose' / coin_type' / account' / change / address_index
//       // NOTE: this pattern is not recognized by trezor-core
//       valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
//       valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//       if (full_check) {
//         valid = valid && has_multisig;
//         valid = valid && (script_type == InputScriptType_SPENDMULTISIG);
//       }
//     } else if (address_n_count == 6) {
//       // BIP-48:
//       // m / purpose' / coin_type' / account' / type' / change / address_index
//       valid = valid && (address_n[3] & PATH_HARDENED);
//       uint32_t type = address_n[3] & PATH_UNHARDEN_MASK;
//       valid = valid && (type <= 2);
//       valid = valid && (type == 0 || coin->has_segwit);
//       valid = valid && (address_n[4] <= PATH_MAX_CHANGE);
//       valid = valid && (address_n[5] <= PATH_MAX_ADDRESS_INDEX);
//       if (full_check) {
//         valid = valid && has_multisig;
//         switch (type) {
//           case 0:
//             valid = valid && (script_type == InputScriptType_SPENDMULTISIG ||
//                               script_type == InputScriptType_SPENDADDRESS);
//             break;
//           case 1:
//             valid = valid && (script_type == InputScriptType_SPENDP2SHWITNESS);
//             break;
//           case 2:
//             valid = valid && (script_type == InputScriptType_SPENDWITNESS);
//             break;
//           default:
//             return false;
//         }
//       }
//     } else {
//       return false;
//     }
//     return valid;
//   }

//   // m/49' : BIP49 SegWit
//   // m / purpose' / coin_type' / account' / change / address_index
//   if (address_n[0] == PATH_HARDENED + 49) {
//     valid = valid && coin->has_segwit;
//     valid = valid && (address_n_count == 5);
//     valid = valid && check_cointype(coin, address_n[1], full_check);
//     valid = valid && (address_n[2] & PATH_HARDENED);
//     valid = valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//     valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
//     valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       valid = valid && (script_type == InputScriptType_SPENDP2SHWITNESS);
//     }
//     return valid;
//   }

//   // m/84' : BIP84 Native SegWit
//   // m / purpose' / coin_type' / account' / change / address_index
//   if (address_n[0] == PATH_HARDENED + 84) {
//     valid = valid && coin->has_segwit;
//     valid = valid && (coin->bech32_prefix != NULL);
//     valid = valid && (address_n_count == 5);
//     valid = valid && check_cointype(coin, address_n[1], full_check);
//     valid = valid && (address_n[2] & PATH_HARDENED);
//     valid = valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//     valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
//     valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       valid = valid && (script_type == InputScriptType_SPENDWITNESS);
//     }
//     return valid;
//   }

//   // m/86' : BIP86 Taproot
//   // m / purpose' / coin_type' / account' / change / address_index
//   if (address_n[0] == PATH_HARDENED + 86) {
//     valid = valid && coin->has_taproot;
//     valid = valid && (coin->bech32_prefix != NULL);
//     valid = valid && (address_n_count == 5);
//     valid = valid && check_cointype(coin, address_n[1], full_check);
//     valid = valid && (address_n[2] & PATH_HARDENED);
//     valid = valid && ((address_n[2] & PATH_UNHARDEN_MASK) <= PATH_MAX_ACCOUNT);
//     valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
//     valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       // we do not support Multisig with Taproot yet
//       valid = valid && !has_multisig;
//       valid = valid && (script_type == InputScriptType_SPENDTAPROOT);
//     }
//     return valid;
//   }

//   // Green Address compatibility pattern. Will be removed in the future.
//   // m / [1,4] / address_index
//   if (address_n[0] == 1 || address_n[0] == 4) {
//     valid = valid && (coin->coin_type == SLIP44_BITCOIN);
//     valid = valid && (address_n_count == 2);
//     valid = valid && (address_n[1] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       valid = valid && (script_type != InputScriptType_SPENDTAPROOT);
//     }
//     return valid;
//   }

//   // Green Address compatibility pattern. Will be removed in the future.
//   // m / 3' / [1-100]' / [1,4] / address_index
//   if (address_n[0] == PATH_HARDENED + 3) {
//     valid = valid && (coin->coin_type == SLIP44_BITCOIN);
//     valid = valid && (address_n_count == 4);
//     valid = valid && (address_n[1] & PATH_HARDENED);
//     valid = valid && ((address_n[1] & PATH_UNHARDEN_MASK) <= 100);
//     valid = valid && (address_n[2] == 1 || address_n[2] == 4);
//     valid = valid && (address_n[3] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       valid = valid && (script_type != InputScriptType_SPENDTAPROOT);
//     }
//     return valid;
//   }

//   // Green Address compatibility patterns. Will be removed in the future.
//   // m / 1195487518
//   // m / 1195487518 / 6 / address_index
//   if (address_n[0] == 1195487518) {
//     valid = valid && (coin->coin_type == SLIP44_BITCOIN);
//     if (address_n_count == 3) {
//       valid = valid && (address_n[1] == 6);
//       valid = valid && (address_n[2] <= PATH_MAX_ADDRESS_INDEX);
//     } else if (address_n_count != 1) {
//       return false;
//     }
//     if (full_check) {
//       return false;
//     }
//     return valid;
//   }

//   // Casa compatibility pattern. Will be removed in the future.
//   // m / 49 / coin_type / account / change / address_index
//   if (address_n[0] == 49) {
//     valid = valid && (address_n_count == 5);
//     valid =
//         valid && check_cointype(coin, PATH_HARDENED | address_n[1], full_check);
//     valid = valid && ((address_n[1] & PATH_HARDENED) == 0);
//     valid = valid && (address_n[2] <= PATH_MAX_ACCOUNT);
//     valid = valid && (address_n[3] <= PATH_MAX_CHANGE);
//     valid = valid && (address_n[4] <= PATH_MAX_ADDRESS_INDEX);
//     if (full_check) {
//       valid = valid && (script_type == InputScriptType_SPENDP2SHWITNESS);
//     }
//     return valid;
//   }

//   // unknown path
//   return false;
// }
