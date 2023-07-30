#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

#include <lib/flipper_format/flipper_format.h>
#include "furi.h"
#include <furi_hal.h>
#include <lib/subghz/types.h>
#include <locale/locale.h>

#ifdef __cplusplus
extern "C" {
#endif

#define TPMS_NO_ID 0xFFFFFFFF
#define TPMS_NO_BATT 0xFF
#define TPMS_NO_HUMIDITY 0xFF
#define TPMS_NO_CHANNEL 0xFF
#define TPMS_NO_BTN 0xFF
#define TPMS_NO_TEMPERATURE -273.0f

typedef struct TPMSBlockGeneric TPMSBlockGeneric;

struct TPMSBlockGeneric {
    const char* protocol_name;
    uint64_t data;
    uint32_t id;
    uint8_t data_count_bit;
    uint8_t battery_low;
    uint8_t humidity;
    uint32_t timestamp;
    uint8_t channel;
    uint8_t btn;
    float temp;
};

/**
 * Get name preset.
 * @param preset_name name preset
 * @param preset_str Output name preset
 */
void tpms_block_generic_get_preset_name(const char* preset_name, FuriString* preset_str);

/**
 * Serialize data TPMSBlockGeneric.
 * @param instance Pointer to a TPMSBlockGeneric instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param preset The modulation on which the signal was received, SubGhzRadioPreset
 * @return status
 */
SubGhzProtocolStatus tpms_block_generic_serialize(
    TPMSBlockGeneric* instance,
    FlipperFormat* flipper_format,
    SubGhzRadioPreset* preset);

/**
 * Deserialize data TPMSBlockGeneric.
 * @param instance Pointer to a TPMSBlockGeneric instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @return status
 */
SubGhzProtocolStatus
    tpms_block_generic_deserialize(TPMSBlockGeneric* instance, FlipperFormat* flipper_format);

/**
 * Deserialize data TPMSBlockGeneric.
 * @param instance Pointer to a TPMSBlockGeneric instance
 * @param flipper_format Pointer to a FlipperFormat instance
 * @param count_bit Count bit protocol
 * @return status
 */
SubGhzProtocolStatus tpms_block_generic_deserialize_check_count_bit(
    TPMSBlockGeneric* instance,
    FlipperFormat* flipper_format,
    uint16_t count_bit);

#ifdef __cplusplus
}
#endif