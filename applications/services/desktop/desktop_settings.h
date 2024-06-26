#pragma once

#include "desktop_settings_filename.h"

#include <storage/storage.h>

#define DESKTOP_SETTINGS_VER_10 (10)
#define DESKTOP_SETTINGS_VER (11)

#define DESKTOP_SETTINGS_PATH INT_PATH(DESKTOP_SETTINGS_FILE_NAME)
#define DESKTOP_SETTINGS_MAGIC (0x17)

#define DESKTOP_SETTINGS_RUN_PIN_SETUP_ARG "run_pin_setup"

#define MAX_APP_LENGTH 128

typedef enum {
    FavoriteAppLeftShort = 0,
    FavoriteAppLeftLong,
    FavoriteAppRightShort,
    FavoriteAppRightLong,
    FavoriteAppNumber,
} FavoriteAppShortcut;

typedef enum {
    DummyAppLeft = 0,
    DummyAppRight,
    DummyAppDown,
    DummyAppOk,
    DummyAppNumber,
} DummyAppShortcut;

typedef struct {
    char name_or_path[MAX_APP_LENGTH];
} FavoriteApp;

typedef struct {
    uint32_t auto_lock_delay_ms;
    uint8_t dummy_mode;
    uint8_t display_clock;
    FavoriteApp favorite_apps[FavoriteAppNumber];
    FavoriteApp dummy_apps[DummyAppNumber];
} DesktopSettings;

typedef struct {
    uint8_t reserved[11];
    DesktopSettings settings;
} DesktopSettingsV10;

// Actual size of DesktopSettings v10
static_assert(sizeof(DesktopSettingsV10) == 1044);

void desktop_settings_load(DesktopSettings* settings);
void desktop_settings_save(const DesktopSettings* settings);
