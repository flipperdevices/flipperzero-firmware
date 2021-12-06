#pragma once

#include <gui/view.h>
#include "../spectrum_analyzer_worker.h"

typedef struct {
    View* view;
    osTimerId_t timer;
} ViewSpectrumAnalyzerChart;

typedef struct {
    FrequencyRSSI* rssi_buf;
} ViewSpectrumAnalyzerModel;

ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart_alloc();///init

void view_spectrum_analyzer_chart_free(ViewSpectrumAnalyzerChart* instance);///exit

View* view_spectrum_analyzer_chart_get_view(ViewSpectrumAnalyzerChart* instance);///follow view-model priciples

void view_spectrum_analyzer_draw_callback(Canvas* canvas, void* context);

void view_spectrum_analyzer_enter_callback(void* context);

void view_spectrum_analyzer_exit_callback(void* context);

void view_spectrum_analyzer_timer_callback(void* context);