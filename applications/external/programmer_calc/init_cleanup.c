#include "init_cleanup.h"
#include "ui_drawing.h"
#include "input_handling.h"
#include <notification/notification.h>
#include <notification/notification_messages.h>
#include "calculator.h"

FuriMessageQueue* initialize_event_queue() {
    return furi_message_queue_alloc(8, sizeof(InputEvent));
}

Calculator* initialize_calculator_state() {
    Calculator* state = malloc(sizeof(Calculator));
    state->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    if(!state->mutex) {
        free(state);
        return NULL;
    }
    return state;
}


ViewPort* initialize_view_port(Calculator* calculator_state, FuriMessageQueue* event_queue) {
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, calculator_draw_callback, calculator_state);
    view_port_input_callback_set(view_port, calculator_input_callback, event_queue);
    view_port_set_orientation(view_port, ViewPortOrientationVertical);
    return view_port;
}


Gui* initialize_gui(ViewPort* view_port) {
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);
    return gui;
}


void cleanup_resources(Calculator* calculator_state, ViewPort* view_port, Gui* gui, FuriMessageQueue* event_queue) {
    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_mutex_free(calculator_state->mutex);
    furi_message_queue_free(event_queue);
    furi_record_close(RECORD_NOTIFICATION);
    furi_record_close(RECORD_GUI);
    free(calculator_state);
}