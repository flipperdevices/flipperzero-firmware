#pragma once

#include <gui/view.h>

typedef struct ViewSpectrumAnalyzerChart ViewSpectrumAnalyzerChart;

ViewSpectrumAnalyzerChart* view_spectrum_analyzer_chart_alloc();

void view_spectrum_analyzer_chart_free(ViewSpectrumAnalyzerChart* instance);

View* view_spectrum_analyzer_chart_get_view(ViewSpectrumAnalyzerChart* instance);