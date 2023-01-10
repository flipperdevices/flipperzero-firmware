#pragma once

#include <notification/notification_messages.h>
#include <stdio.h>
#include <furi.h>
#include <gui/gui.h>
#include <input/input.h>
#include <u8g2_glue.h>

#include "flizzer_tracker_hal.h"
#include "sound_engine/freqs.h"

typedef struct 
{
	bool stop;
	uint32_t counter;
	uint32_t counter_2;
	NotificationApp* notification;

	SoundEngine sound_engine;

	uint32_t frequency;
	uint8_t current_waveform_index;
	uint16_t pw;

	uint8_t selected_param;

	uint16_t flags;
	uint16_t cutoff;
	uint8_t resonance;
	uint8_t filter_type;
} FlizzerTrackerApp;