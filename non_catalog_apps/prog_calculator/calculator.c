#include "calculator.h"

int32_t calculator_app(void* p) {
    UNUSED(p);
    FuriMessageQueue* event_queue = initialize_event_queue();
    Calculator* calculator_state = initialize_calculator_state();
    if(!calculator_state) return -1;

    ViewPort* view_port = initialize_view_port(calculator_state, event_queue);
    Gui* gui = initialize_gui(view_port);

    InputEvent event;
    while(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk) {
        if(event.type == InputTypeShort && event.key == InputKeyBack) {
            break;
        }
        handle_event(calculator_state, view_port, &event);
    }

    cleanup_resources(calculator_state, view_port, gui, event_queue);
    return 0;
}

