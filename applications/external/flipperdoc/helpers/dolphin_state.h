#pragma once

#include <stdint.h>
#include <stdbool.h>

#define MAX_LEVEL 30
#define MAX_BUTTHURT 14

#define DOLPHIN_STATE_PATH CFG_PATH("dolphin.state")
#define DOLPHIN_STATE_HEADER_MAGIC 0xD0
#define DOLPHIN_STATE_HEADER_VERSION 0x01

#define BUTTHURT_MIN 0
#define BUTTHURT_MAX 14

enum {
    XpCategorySubGhz,
    XpCategoryRFID,
    XpCategoryNFC,
    XpCategoryIR,
    XpCategoryIButton,
    XpCategoryBadUSB,
    XpCategoryPlugin,
    XpCategoryMax,
};

enum {
    ButthurtCategoryDeeds,
    ButthurtCategoryNext,
    ButthurtCategoryMax,
};

#define DOLPHIN_LEVEL_COUNT 29

typedef struct DolphinState DolphinState;
typedef struct {
    uint8_t icounter_daily_limit[XpCategoryMax];
    uint8_t butthurt_daily_limit;

    uint32_t flags;
    uint32_t icounter;
    int32_t butthurt;
    uint64_t timestamp;
} DolphinStoreData;

struct DolphinState {
    DolphinStoreData data;
    bool dirty;
};

uint8_t doc_dolphin_get_level(uint32_t icounter);
uint32_t doc_dolphin_state_xp_to_levelup(uint32_t icounter);