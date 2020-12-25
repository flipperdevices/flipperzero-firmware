#include "dolphin_state.h"
#include <api-hal-flash.h>
#include <flipper_v2.h>

typedef struct {
    uint8_t magic;
    uint8_t version;
    uint8_t checksum;
    uint8_t flags;
    uint32_t timestamp;
} DolphinStateHeader;

#define DOLPHIN_STATE_PAGE 0xC0
#define DOLPHIN_STATE_HEADER_ADDRESS 0x080C0000U
#define DOLPHIN_STATE_DATA_ADDRESS (DOLPHIN_STATE_HEADER_ADDRESS + sizeof(DolphinStateHeader))

#define DOLPHIN_STATE_HEADER_MAGIC 0xD0
#define DOLPHIN_STATE_HEADER_VERSION 0x00

typedef struct {
    uint32_t ibutton;
    uint32_t nfc;
    uint32_t ir;
    uint32_t rfid;
} DolphinLimit;

struct DolphinState {
    uint32_t icounter;
    uint32_t butthurt;

    DolphinLimit limit;
};

DolphinState* dolphin_state_alloc() {
    DolphinState* dolphin_state = furi_alloc(sizeof(DolphinState));
    return dolphin_state;
}

void dolphin_state_release(DolphinState* dolphin_state) {
    free(dolphin_state);
}

void dolphin_state_save(DolphinState* dolphin_state) {
    api_hal_flash_erase(DOLPHIN_STATE_PAGE, 1);
    uint8_t* source = (uint8_t*)dolphin_state;
    uint8_t checksum = 0;
    for(size_t i = 0; i < sizeof(DolphinState); i++) {
        checksum += source[i];
    }
    DolphinStateHeader header;
    header.magic = DOLPHIN_STATE_HEADER_MAGIC;
    header.version = DOLPHIN_STATE_HEADER_VERSION;
    header.checksum = checksum;
    header.flags = 0;
    header.timestamp = 0;
    api_hal_flash_write_dword(DOLPHIN_STATE_HEADER_ADDRESS, *(uint64_t*)&header);

    uint8_t destination[sizeof(uint64_t)];
    size_t block_count = sizeof(DolphinState) / sizeof(uint64_t) + 1;
    size_t offset = 0;
    for(size_t i = 0; i < block_count; i++) {
        for(size_t n = 0; n < sizeof(uint64_t); n++) {
            if(offset < sizeof(DolphinState)) {
                destination[n] = source[offset];
            } else {
                destination[n] = 0;
            }
            offset++;
        }
        api_hal_flash_write_dword(
            DOLPHIN_STATE_DATA_ADDRESS + i * sizeof(uint64_t), *(uint64_t*)destination);
    }
}

void dolphin_state_load(DolphinState* dolphin_state) {
    const DolphinStateHeader* header = (const DolphinStateHeader*)DOLPHIN_STATE_HEADER_ADDRESS;
    if(header->magic == DOLPHIN_STATE_HEADER_MAGIC &&
       header->version == DOLPHIN_STATE_HEADER_VERSION) {
        uint8_t checksum = 0;
        const uint8_t* source = (const uint8_t*)DOLPHIN_STATE_DATA_ADDRESS;
        for(size_t i = 0; i < sizeof(DolphinState); i++) {
            checksum += source[i];
        }
        if(header->checksum == checksum) {
            memcpy(dolphin_state, (const void*)DOLPHIN_STATE_DATA_ADDRESS, sizeof(DolphinState));
        }
    }
}

void dolphin_state_clear(DolphinState* dolphin_state) {
    memset(dolphin_state, 0, sizeof(DolphinState));
}

void dolphin_state_on_deed(DolphinState* dolphin_state, DolphinDeed deed) {
    const DolphinDeedWeight* deed_weight = dolphin_deed_weight(deed);
    int32_t icounter = dolphin_state->icounter + deed_weight->icounter;

    if(icounter >= 0) {
        dolphin_state->icounter = icounter;
    }
}

uint32_t dolphin_state_get_icounter(DolphinState* dolphin_state) {
    return dolphin_state->icounter;
}

uint32_t dolphin_state_get_butthurt(DolphinState* dolphin_state) {
    return dolphin_state->butthurt;
}
