/* Copyright (C) 2022-2023 Salvatore Sanfilippo -- All Rights Reserved
 * See the LICENSE file for information about the license. */

#pragma once

#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/gui.h>
#include <stdlib.h>
#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/submenu.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/widget.h>
#include <notification/notification_messages.h>
#include <lib/subghz/subghz_setting.h>
#include <lib/subghz/subghz_worker.h>
#include <lib/subghz/receiver.h>
#include <lib/subghz/transmitter.h>
#include <lib/subghz/registry.h>
#include "app_buffer.h"

#define TAG "ProtoView"

typedef struct ProtoViewApp ProtoViewApp;

/* Subghz system state */
typedef enum {
    TxRxStateIDLE,
    TxRxStateRx,
    TxRxStateSleep,
} TxRxState;

/* Currently active view. */
typedef enum {
    ViewRawPulses,
    ViewFrequencySettings,
    ViewModulationSettings,
    ViewLast, /* Just a sentinel to wrap around. */
} ProtoViewCurrentView;

typedef struct {
    const char* name;
    FuriHalSubGhzPreset preset;
} ProtoViewModulation;

extern ProtoViewModulation ProtoViewModulations[]; /* In app_subghz.c */

/* This is the context of our subghz worker and associated thread.
 * It receives data and we get our protocol "feed" callback called
 * with the level (1 or 0) and duration. */
struct ProtoViewTxRx {
    SubGhzWorker* worker; /* Our background worker. */
    SubGhzEnvironment* environment;
    SubGhzReceiver* receiver;
    TxRxState txrx_state; /* Receiving, idle or sleeping? */
};

typedef struct ProtoViewTxRx ProtoViewTxRx;

struct ProtoViewApp {
    /* GUI */
    Gui* gui;
    ViewPort* view_port; /* We just use a raw viewport and we render
                                everything into the low level canvas. */
    ProtoViewCurrentView current_view; /* Active view ID. */
    FuriMessageQueue* event_queue; /* Keypress events go here. */

    /* Radio related. */
    ProtoViewTxRx* txrx; /* Radio state. */
    SubGhzSetting* setting; /* A list of valid frequencies. */

    /* Generic app state. */
    int running; /* Once false exists the app. */
    uint32_t signal_bestlen; /* Longest coherent signal observed so far. */

    /* Raw view apps state. */
    uint32_t us_scale; /* microseconds per pixel. */
    uint32_t signal_offset; /* Long press left/right panning in raw view. */

    /* Configuration view app state. */
    uint32_t frequency; /* Current frequency. */
    uint8_t modulation; /* Current modulation ID, array index in the
                                ProtoViewModulations table. */
};

extern RawSamplesBuffer *RawSamples, *DetectedSamples;

/* app_radio.c */
void radio_begin(ProtoViewApp* app);
uint32_t radio_rx(ProtoViewApp* app);
void radio_idle(ProtoViewApp* app);
void radio_rx_end(ProtoViewApp* app);
void radio_sleep(ProtoViewApp* app);

/* signal.c */
uint32_t duration_delta(uint32_t a, uint32_t b);
void scan_for_signal(ProtoViewApp* app);

/* view_*.c */
void render_view_raw_pulses(Canvas* const canvas, ProtoViewApp* app);
void process_input_raw_pulses(ProtoViewApp* app, InputEvent input);
void render_view_settings(Canvas* const canvas, ProtoViewApp* app);
void process_input_settings(ProtoViewApp* app, InputEvent input);

/* ui.c */
void canvas_draw_str_with_border(
    Canvas* canvas,
    uint8_t x,
    uint8_t y,
    const char* str,
    Color text_color,
    Color border_color);
