#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "nfc_util.h"
#include <furi_hal_nfc.h>

#define ISO15693_CMD_NXP_GET_RANDOM_NUMBER        0xB2
#define ISO15693_CMD_NXP_SET_PASSWORD             0xB3
#define ISO15693_MANUFACTURER_NXP                 0x04


bool slix_check_card_type(FuriHalNfcDevData* nfc_data);
bool slix2_check_card_type(FuriHalNfcDevData* nfc_data);
bool slix_s_check_card_type(FuriHalNfcDevData* nfc_data);
bool slix_l_check_card_type(FuriHalNfcDevData* nfc_data);
ReturnCode slix_l_get_random(uint8_t* rand);
ReturnCode slix_l_unlock(uint32_t id, uint8_t* rand, uint32_t password);

