#pragma once

#include <core/check.h>
#include <core/record.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <gui/canvas_i.h>
#include <furi_hal_version.h>

#include "math.h"
#include <dolphin/helpers/dolphin_state.h>
#include <dolphin/helpers/dolphin_deed.h>
#include <dolphin/dolphin.h>
#include "applications/settings/desktop_settings/desktop_settings_app.h"
#include "passport_settings/passport_settings.h"
#include "passport_icons.h"

#define MOODS_TOTAL 1
#define BUTTHURT_MAX 14

typedef enum {
    AniRedVirus,
    AniYelVirus,
    AniBluVirus,
    AniRabbit,
    AniSlime,
    AniSonic,
    AniMaxNum
} Animations;

static IconAnimation* animations[AniMaxNum];

typedef enum { EventGameTick, EventKeyPress } EventType;

typedef struct {
    EventType type;
    InputEvent input;
} PassportEvent;

typedef enum {
    Main,
    DrMario,
} Pages;

static const Icon* const portrait[7] = {
    &I_passport_happy1_46x49,
    &I_passport_happy2_46x49,
    &I_passport_happy3_46x49,
    &I_G0ku,
    &I_g0ku_1,
    &I_g0ku_2,
    &I_g0ku_3};
static const Icon* const portrait_happy[3] = {
    &I_passport_happy1_46x49,
    &I_passport_happy2_46x49,
    &I_passport_happy3_46x49};
static const Icon* const portrait_ok[3] = {
    &I_passport_okay1_46x49,
    &I_passport_okay2_46x49,
    &I_passport_okay3_46x49};
static const Icon* const portrait_bad[3] = {
    &I_passport_bad1_46x49,
    &I_passport_bad2_46x49,
    &I_passport_bad3_46x49};

static const Icon* const* portraitsMoody[3] = {portrait_happy, portrait_ok, portrait_bad};

static const char* const moods[MOOD_SET_COUNT][16] = {
    {},
    {"Joyful",
     "Good",
     "Satisfied",
     "Happy",
     "Dreaming",
     "Relaxed",
     "Okay",
     "Tired",
     "Bored",
     "Sad",
     "Disappointed",
     "Annoyed",
     "Grumpy",
     "Upset",
     "Angry",
     "Furious"},
    {"Stoned",
     "Baked",
     "Ripped",
     "Joyful",
     "Happy",
     "Satisfied",
     "Relaxed",
     "Nostalgic",
     "Okay",
     "Tired",
     "Bored",
     "Sad",
     "Annoyed",
     "Upset",
     "Angry",
     "Furious"}};

typedef struct {
    FuriMutex* mutex;
    Dolphin* dolphin;
    DolphinStats stats;

    DesktopSettings desktop_settings;
    PassportSettings settings;

    Pages page;

    uint32_t xp_to_levelup;
    uint32_t xp_above_last_levelup;
    uint32_t xp_for_current_level;
    char xp_str[14];
    char xp_str1[7];
    char xp_str2[7];
    double xp_progress;
    int xp_fill;

    int max_level;
    uint16_t tmpLvl;
    uint16_t tmpMood;
    char level_str[12];
    char level_str1[12];

    char my_name[9];

    char mood_str[20];
    uint8_t mood;
    uint8_t moodStrIndex;
    char dumbmode_mood[20];
} Passport;