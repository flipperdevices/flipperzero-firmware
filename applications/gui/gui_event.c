#include "gui_event.h"

#include <furi.h>

#define GUI_EVENT_MQUEUE_SIZE 16

struct gui_event_t {
    FuriRecordSubscriber    *input_event_record;
    osMessageQueueId_t      mqueue;
};

void gui_event_input_events_callback(const void* value, size_t size, void* ctx)
{
    assert(ctx);
    gui_event_t * gui_event = ctx;

    gui_message_t message;
    message.type = GUI_MESSAGE_TYPE_INPUT;
    message.input = *(InputEvent *)value;

    osMessageQueuePut(gui_event->mqueue, &message, 0, 0);
}


gui_event_t * gui_event_alloc()
{
    gui_event_t * gui_event = furi_alloc(sizeof(gui_event_t));
    // Allocate message que
    gui_event->mqueue = osMessageQueueNew(GUI_EVENT_MQUEUE_SIZE, sizeof(gui_event_t), NULL);
    assert(gui_event->mqueue);

    // Input
    gui_event->input_event_record = furi_open("input_events", false, false, gui_event_input_events_callback, NULL, gui_event);
    assert(gui_event->input_event_record != NULL);

    return gui_event;
}

void gui_event_free(gui_event_t *gui_event)
{
    assert(gui_event);
    assert(osMessageQueueDelete(gui_event->mqueue) == osOK);
    free(gui_event);
}

void gui_event_messsage_send(gui_event_t *gui_event, gui_message_t *message)
{
    osMessageQueuePut(gui_event->mqueue, message, 0, 0);
}

gui_message_t gui_event_message_next(gui_event_t *gui_event)
{
    assert(gui_event);
    gui_message_t message;
    while (osMessageQueueGet(gui_event->mqueue, &message, NULL, osWaitForever) != osOK) {};
    return message;
}
