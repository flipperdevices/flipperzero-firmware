#pragma once

#include <cli/cli.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <notification/notification_messages.h>
#include <stdio.h>
#include <u8g2_glue.h>

#include <gui/modules/text_input.h>
#include <gui/view_dispatcher.h>

#include "flizzer_tracker_hal.h"
#include "sound_engine/freqs.h"
#include "sound_engine/sound_engine_defs.h"
#include "sound_engine/sound_engine_filter.h"
#include "tracker_engine/tracker_engine_defs.h"

typedef enum
{
    EventTypeInput,
} EventType;

typedef struct
{
    EventType type;
    InputEvent input;
    uint32_t period;
} FlizzerTrackerEvent;

typedef enum
{
    PATTERN_VIEW,
    INST_EDITOR_VIEW,
    EXPORT_WAV_VIEW,
} TrackerMode;

typedef enum
{
    EDIT_PATTERN,
    EDIT_SEQUENCE,
    EDIT_SONGINFO,
    EDIT_INSTRUMENT,
    EDIT_PROGRAM,
} TrackerFocus;

typedef enum
{
    SI_PATTERNPOS,
    SI_SEQUENCEPOS,
    SI_SONGSPEED,
    SI_SONGRATE,
    SI_MASTERVOL,

    SI_SONGNAME,
    SI_CURRENTINSTRUMENT,
    SI_INSTRUMENTNAME,
    /* ========  */
    SI_PARAMS,
} SongInfoParam;

typedef enum
{
    INST_CURRENTINSTRUMENT,
    INST_INSTRUMENTNAME,

    INST_CURRENT_NOTE,
    INST_FINETUNE,

    INST_SLIDESPEED,
    INST_SETPW,
    INST_PW,
    INST_SETCUTOFF,

    INST_WAVE_NOISE,
    INST_WAVE_PULSE,
    INST_WAVE_TRIANGLE,
    INST_WAVE_SAWTOOTH,
    INST_WAVE_NOISE_METAL,
    INST_WAVE_SINE,

    INST_ATTACK,
    INST_DECAY,
    INST_SUSTAIN,
    INST_RELEASE,
    INST_VOLUME,

    INST_ENABLEFILTER,
    INST_FILTERCUTOFF,
    INST_FILTERRESONANCE,

    INST_FILTERTYPE,
    INST_ENABLERINGMOD,
    INST_RINGMODSRC,
    INST_ENABLEHARDSYNC,
    INST_HARDSYNCSRC,

    INST_RETRIGGERONSLIDE,
    INST_ENABLEKEYSYNC,

    INST_ENABLEVIBRATO,
    INST_VIBRATOSPEED,
    INST_VIBRATODEPTH,
    INST_VIBRATODELAY,

    INST_ENABLEPWM,
    INST_PWMSPEED,
    INST_PWMDEPTH,
    INST_PWMDELAY,

    INST_PROGRAMEPERIOD,
    /* ========  */
    INST_PARAMS,
} InstrumentParam;

typedef struct
{
    View *view;
    void *context;
} TrackerView;

typedef enum
{
    VIEW_TRACKER,
    VIEW_KEYBOARD,
    VIEW_FILE_MANAGER,
    VIEW_SUBMENU,
} FlizzerTrackerViews;

typedef struct
{
    NotificationApp *notification;
    FuriMessageQueue *event_queue;
    Gui *gui;
    TrackerView *tracker_view;
    ViewDispatcher *view_dispatcher;
    TextInput *text_input;
    bool was_it_back_keypress;
    uint32_t current_time;
    uint32_t period;

    SoundEngine sound_engine;
    TrackerEngine tracker_engine;

    TrackerSong song;

    uint8_t selected_param;

    uint8_t mode, focus;
    uint8_t patternx, current_channel, current_digit, program_step, current_instrument, current_note;

    uint8_t inst_editor_shift;

    bool editing;
    bool was_editing;

    bool quit;
} FlizzerTrackerApp;

typedef struct
{
    FlizzerTrackerApp *tracker;
} TrackerViewModel;