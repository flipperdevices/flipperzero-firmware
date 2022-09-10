#pragma once

#include "clock_settings_filename.h"

#include <furi_hal.h>
#include <stdint.h>
#include <stdbool.h>
#include <toolbox/saved_struct.h>
#include <storage/storage.h>

#define CLOCK_SETTINGS_VER (1)
#define CLOCK_SETTINGS_PATH INT_PATH(CLOCK_SETTINGS_FILE_NAME)
#define CLOCK_SETTINGS_MAGIC (0xC1)

#define SAVE_CLOCK_SETTINGS(x) \
    saved_struct_save(           \
        CLOCK_SETTINGS_PATH,   \
        (x),                     \
        sizeof(ClockSettings), \
        CLOCK_SETTINGS_MAGIC,  \
        CLOCK_SETTINGS_VER)

#define LOAD_CLOCK_SETTINGS(x) \
    saved_struct_load(           \
        CLOCK_SETTINGS_PATH,   \
        (x),                     \
        sizeof(ClockSettings), \
        CLOCK_SETTINGS_MAGIC,  \
        CLOCK_SETTINGS_VER)

typedef enum {
    SoundVibro,
    VibroOnly,
    Silent,
} AlarmMode;

typedef enum {
    H12,
    H24,
} TimeFormat;

typedef enum {
    Iso,
    Rfc,
} DateFormat;

typedef struct {
    uint8_t TimeFormat;
    uint8_t DateFormat;
    uint8_t increment_precision;
    AlarmMode alarm_mode;
    uint8_t alarm_sound;
} ClockSettings;