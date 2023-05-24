#pragma once

#include "dolphin_deed.h"
#include <stdbool.h>
#include <stdint.h>
#include <time.h>
#include <storage/storage.h>

#define DOLPHIN_STATE_OLD_PATH INT_PATH(".dolphin.state")
#define DOLPHIN_STATE_PATH CFG_PATH("dolphin.state")

#ifdef __cplusplus
extern "C" {
#endif

#define DOLPHIN_LEVEL_COUNT 29

typedef struct DolphinState DolphinState;
typedef struct {
    uint8_t icounter_daily_limit[DolphinAppMAX];
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

DolphinState* dolphin_state_alloc();

void dolphin_state_free(DolphinState* dolphin_state);

bool dolphin_state_save(DolphinState* dolphin_state);

bool dolphin_state_load(DolphinState* dolphin_state);

void dolphin_state_clear_limits(DolphinState* dolphin_state);

uint64_t dolphin_state_timestamp();

int dolphin_state_max_level();

void dolphin_state_on_deed(DolphinState* dolphin_state, DolphinDeed deed);

void dolphin_state_butthurted(DolphinState* dolphin_state);

uint32_t dolphin_state_xp_to_levelup(int icounter);

uint32_t dolphin_state_xp_above_last_levelup(int icounter);

const int* dolphin_get_levels();

bool dolphin_state_is_levelup(int icounter);

void dolphin_state_increase_level(DolphinState* dolphin_state);

uint8_t dolphin_get_level(int icounter);

#ifdef __cplusplus
}
#endif
