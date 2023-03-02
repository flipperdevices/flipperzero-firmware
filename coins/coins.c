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

#include "coins.h"
#include <string.h>
#include "../crypto/address.h"
#include "../crypto/base58.h"
#include "../crypto/ecdsa.h"

const CoinInfo *coinByName(const char *name) {
  if (!name) return 0;
  for (int i = 0; i < COINS_COUNT; i++) {
    if (strcmp(name, coins[i].coin_name) == 0) {
      return &(coins[i]);
    }
  }
  return 0;
}

const CoinInfo *coinByAddressType(uint32_t address_type) {
  for (int i = 0; i < COINS_COUNT; i++) {
    if (address_type == coins[i].address_type) {
      return &(coins[i]);
    }
  }
  return 0;
}

const CoinInfo *coinBySlip44(uint32_t coin_type) {
  if (coin_type == SLIP44_TESTNET) {
    // The slip44 coin type is the same for all testnets, so we return the
    // Bitcoin Testnet.
    return coinByName("Testnet");
  }

  for (int i = 0; i < COINS_COUNT; i++) {
    if (coin_type == coins[i].coin_type) {
      return &(coins[i]);
    }
  }
  return 0;
}

bool coinExtractAddressType(const CoinInfo *coin, const char *addr,
                            uint32_t *address_type) {
  if (!addr) return false;
  uint8_t addr_raw[MAX_ADDR_RAW_SIZE] = {0};
  int len = base58_decode_check(addr, coin->curve->hasher_base58, addr_raw,
                                MAX_ADDR_RAW_SIZE);
  if (len >= 21) {
    return coinExtractAddressTypeRaw(coin, addr_raw, address_type);
  }
  return false;
}

bool coinExtractAddressTypeRaw(const CoinInfo *coin, const uint8_t *addr_raw,
                               uint32_t *address_type) {
  if (address_check_prefix(addr_raw, coin->address_type)) {
    *address_type = coin->address_type;
    return true;
  }
  if (address_check_prefix(addr_raw, coin->address_type_p2sh)) {
    *address_type = coin->address_type_p2sh;
    return true;
  }
  *address_type = 0;
  return false;
}
