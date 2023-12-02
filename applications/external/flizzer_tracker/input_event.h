#pragma once

#include <furi.h>
#include <input/input.h>
#include <stdio.h>

#include "flizzer_tracker.h"
#include "sound_engine/sound_engine_defs.h"
#include "tracker_engine/tracker_engine_defs.h"
#include "util.h"

#include "input/instrument.h"
#include "input/instrument_program.h"
#include "input/pattern.h"
#include "input/sequence.h"
#include "input/songinfo.h"

void return_from_keyboard_callback(void* ctx);

void overwrite_file_widget_yes_input_callback(GuiButtonType result, InputType type, void* ctx);
void overwrite_file_widget_no_input_callback(GuiButtonType result, InputType type, void* ctx);

void overwrite_instrument_file_widget_yes_input_callback(
    GuiButtonType result,
    InputType type,
    void* ctx);
void overwrite_instrument_file_widget_no_input_callback(
    GuiButtonType result,
    InputType type,
    void* ctx);

uint32_t submenu_exit_callback(void* context);
uint32_t submenu_settings_exit_callback(void* context);
void submenu_callback(void* context, uint32_t index);
void submenu_copypaste_callback(void* context, uint32_t index);
void audio_output_changed_callback(VariableItem* item);
void process_input_event(FlizzerTrackerApp* tracker, FlizzerTrackerEvent* event);