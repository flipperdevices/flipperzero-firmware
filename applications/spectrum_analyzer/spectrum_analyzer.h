#pragma once

#include "spectrum_analyzer_worker.h"

#include <furi-hal.h>
#include <furi.h>

#include <gui/gui_i.h>
#include <gui/canvas_i.h>

#include <stdint.h>
#include <m-string.h>

#define TAG "Spectrum Analyzer"

typedef enum {
    SpectrumAnalyzerEventTypeTick,
    SpectrumAnalyzerEventTypeKey,
    SpectrumAnalyzerEventTypeDbg
} SpectrumAnalyzerEventType;

typedef struct {
    SpectrumAnalyzerEventType type;
    InputEvent input;
} SpectrumAnalyzerEvent;

/* Application */
typedef struct {
    Gui* gui;///ptr to gui
    ViewPort* view_port;///view_port for drawing

    SpectrumAnalyzerWorker* worker;///worker updates RSSI

    uint8_t curr_freq_id;
    uint8_t curr_step_id;

    osMessageQueueId_t event_queue;///queue for selfdispatch
    osTimerId_t timer;///tick timer
} SpectrumAnalyzer;

SpectrumAnalyzer* spectrum_analyzer_alloc();//init

void spectrum_analyzer_free();//termination

void spectrum_analyzer_draw_callback(Canvas* canvas, void* context);

void spectrum_analyzer_timer_callback(void* context);

void spectrum_analyzer_input_callback(InputEvent* input_event, void* context);

void spectrum_analyzer_draw_bottom_menu(Canvas* canvas, void* context);