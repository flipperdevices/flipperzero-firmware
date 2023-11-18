#include <furi.h>

void send_signal(
    char* protocol,
    uint32_t frequency,
    FuriString* sub_file_contents,
    bool use_external_radio);