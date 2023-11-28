#pragma once

#include <furi.h>

typedef struct SubGhzSignal SubGhzSignal;

SubGhzSignal* subghz_signal_load_file(char* path);
void subghz_signal_free(SubGhzSignal* signal);
void subghz_signal_send(SubGhzSignal* signal, bool use_external_radio);
