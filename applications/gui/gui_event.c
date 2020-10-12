#include "gui_event.h"

#include <furi.h>

#define GUI_EVENT_MQUEUE_SIZE 16

struct GUIEvent {
    FuriRecordSubscriber* input_event_record;
    osMessageQueueId_t mqueue;
};

void gui_event_input_events_callback(const void* value, size_t size, void* ctx) {
    assert(ctx);
    GUIEvent* gui_event = ctx;

    GUIMessage message;
    message.type = GUIMessageTypeInput;
    message.input = *(InputEvent*)value;

    osMessageQueuePut(gui_event->mqueue, &message, 0, 0);
}

GUIEvent* gui_event_alloc() {
    GUIEvent* gui_event = furi_alloc(sizeof(GUIEvent));
    // Allocate message que
    gui_event->mqueue = osMessageQueueNew(GUI_EVENT_MQUEUE_SIZE, sizeof(GUIEvent), NULL);
    assert(gui_event->mqueue);

    // Input
    gui_event->input_event_record =
        furi_open("input_events", false, false, gui_event_input_events_callback, NULL, gui_event);
    assert(gui_event->input_event_record != NULL);

    return gui_event;
}

void gui_event_free(GUIEvent* gui_event) {
    assert(gui_event);
    assert(osMessageQueueDelete(gui_event->mqueue) == osOK);
    free(gui_event);
}

void gui_event_messsage_send(GUIEvent* gui_event, GUIMessage* message) {
    osMessageQueuePut(gui_event->mqueue, message, 0, 0);
}

GUIMessage gui_event_message_next(GUIEvent* gui_event) {
    assert(gui_event);
    GUIMessage message;
    while(osMessageQueueGet(gui_event->mqueue, &message, NULL, osWaitForever) != osOK) {
    };
    return message;
}
