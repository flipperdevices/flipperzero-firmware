#include "xremote_ir_remote_button.h"

#include <stdlib.h>

struct InfraredRemoteButton {
    FuriString* name;
    InfraredSignal* signal;
};

InfraredRemoteButton* xremote_ir_remote_button_alloc() {
    InfraredRemoteButton* button = malloc(sizeof(InfraredRemoteButton));
    button->name = furi_string_alloc();
    button->signal = xremote_ir_signal_alloc();
    return button;
}

void xremote_ir_remote_button_free(InfraredRemoteButton* button) {
    furi_string_free(button->name);
    xremote_ir_signal_free(button->signal);
    free(button);
}