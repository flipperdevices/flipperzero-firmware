#include <furi.h>

typedef struct Signal Signal;

void send_signal(
    FuriString* sub_file_contents,
    const char* protocol,
    uint32_t frequency,
    FuriHalSubGhzPreset preset,
    bool use_external_radio);

Signal* signal_load_file(char* path);
void signal_free(Signal* signal);
void signal_send(Signal* signal, bool use_external_radio);
