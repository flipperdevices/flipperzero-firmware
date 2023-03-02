/*
 * This file is part of the Trezor project, https://trezor.io/
 *
 * Copyright (C) 2014 Pavol Rusnak <stick@satoshilabs.com>
 *
 * This library is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef __COINS_H__
#define __COINS_H__

#include <stdbool.h>
#include <stdint.h>

#include "../crypto/bip32.h"
#include "../crypto/hasher.h"

typedef struct _CoinInfo {
  const char *coin_name;
  const char *coin_shortcut;
  uint64_t maxfee_kb;
  const char *signed_message_header;
  uint32_t decimals;
  bool has_segwit;
  bool has_taproot;
  bool has_fork_id;
  bool force_bip143;
  bool decred;
  // address types > 0xFF represent a two-byte prefix in big-endian order
  uint32_t address_type;
  uint32_t address_type_p2sh;
  uint32_t xpub_magic;
  uint32_t xpub_magic_segwit_p2sh;
  uint32_t xpub_magic_segwit_native;
  uint32_t xpub_magic_multisig_segwit_p2sh;
  uint32_t xpub_magic_multisig_segwit_native;
  uint32_t fork_id;
  const char *bech32_prefix;
  const char *cashaddr_prefix;
  uint32_t coin_type;
  bool negative_fee;
  const char *curve_name;
  const curve_info *curve;
  bool extra_data;
  bool timestamp;
  bool overwintered;
} CoinInfo;

#include "coin_info.h"

// SLIP-44 hardened coin type for Bitcoin
#define SLIP44_BITCOIN 0x80000000

// SLIP-44 hardened coin type for all Testnet coins
#define SLIP44_TESTNET 0x80000001

const CoinInfo *coinByName(const char *name);
const CoinInfo *coinByAddressType(uint32_t address_type);
const CoinInfo *coinBySlip44(uint32_t coin_type);
bool coinExtractAddressType(const CoinInfo *coin, const char *addr,
                            uint32_t *address_type);
bool coinExtractAddressTypeRaw(const CoinInfo *coin, const uint8_t *addr_raw,
                               uint32_t *address_type);

#endif
