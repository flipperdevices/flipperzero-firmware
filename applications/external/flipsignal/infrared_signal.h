#pragma once

#include <furi.h>

typedef struct InfraredSignal InfraredSignal;

InfraredSignal* infrared_signal_load_file(char* path, char* action);
void infrared_signal_free(InfraredSignal* signal);
bool infrared_signal_load_next(InfraredSignal* signal);
bool infrared_signal_send(InfraredSignal* signal);
