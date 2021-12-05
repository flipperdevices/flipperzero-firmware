#pragma once

#include <gui/view.h>
#include "../spectrum_analyzer.h"

void view_spectrum_analyzer_timer_callback(void* context);

void view_spectrum_analyzer_chart_draw_callback(Canvas* canvas, void* context);

void view_spectrum_analyzer_chart_clear(SpectrumAnalyzer* instance);

void view_spectrum_analyzer_chart_zoom_out(SpectrumAnalyzer* instance);

void view_spectrum_analyzer_chart_zoom_in(SpectrumAnalyzer* instance);

bool view_spectrum_analyzer_chart_input_callback(InputEvent* event, void* context);

void view_spectrum_analyzer_chart_exit(void* context);

void view_spectrum_analyzer_chart_enter(void* context);