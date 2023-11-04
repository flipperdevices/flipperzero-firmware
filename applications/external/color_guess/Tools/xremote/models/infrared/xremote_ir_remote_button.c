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

void xremote_ir_remote_button_set_name(InfraredRemoteButton* button, const char* name) {
    furi_string_set(button->name, name);
}

const char* xremote_ir_remote_button_get_name(InfraredRemoteButton* button) {
    return furi_string_get_cstr(button->name);
}

InfraredSignal* xremote_ir_remote_button_get_signal(InfraredRemoteButton* button) {
    return button->signal;
}