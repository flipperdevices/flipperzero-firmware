#pragma once

#include <notification/notification_messages.h>
#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <cli/cli.h>
#include <input/input.h>
#include <u8g2_glue.h>

#include <gui/view_dispatcher.h>

#include "flizzer_tracker_hal.h"
#include "sound_engine/freqs.h"
#include "sound_engine/sound_engine_filter.h"
#include "sound_engine/sound_engine_defs.h"
#include "tracker_engine/tracker_engine_defs.h"

#define MIDDLE_C (12 * 4)
#define MAX_NOTE (12 * 7 + 11)

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
	SI_INSTRUMENTNAME,
	/* ========  */
	SI_PARAMS,
} SongInfoParam;

typedef struct
{
    View* view;
    void* context;
} TrackerView;

typedef struct 
{
	NotificationApp* notification;
	FuriMessageQueue* event_queue;
	Gui* gui;
	TrackerView* tracker_view;
	ViewDispatcher* view_dispatcher;
	bool was_it_back_keypress;
	uint32_t current_time;
	uint32_t period;

	SoundEngine sound_engine;
	TrackerEngine tracker_engine;

	TrackerSong song;

	uint8_t selected_param;

	uint8_t mode, focus;
	uint8_t patternx, current_channel, current_digit, program_step, current_instrument, current_note;
	bool editing;
	bool was_editing;

	bool quit;
} FlizzerTrackerApp;

typedef struct
{
	FlizzerTrackerApp* tracker;
} TrackerViewModel;