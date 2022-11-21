#pragma once

#include <gui/view.h>
#include "../helpers/subghz_custom_event.h"

typedef struct SubGhzFrequencyAnalyzer SubGhzFrequencyAnalyzer;

typedef void (*SubGhzFrequencyAnalyzerCallback)(SubGhzCustomEvent event, void* context);

void subghz_frequency_analyzer_set_callback(
    SubGhzFrequencyAnalyzer* subghz_frequency_analyzer,
    SubGhzFrequencyAnalyzerCallback callback,
    void* context);

SubGhzFrequencyAnalyzer* subghz_frequency_analyzer_alloc();

void subghz_frequency_analyzer_free(SubGhzFrequencyAnalyzer* subghz_static);

View* subghz_frequency_analyzer_get_view(SubGhzFrequencyAnalyzer* subghz_static);

bool subghz_frequency_analyzer_log_to_file();
void subghz_frequency_analyzer_log(float rssi, uint32_t frequency);
