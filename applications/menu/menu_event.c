#include "menu_event.h"

#include <cmsis_os2.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <furi.h>
#include <input/input.h>

#define MENU_MESSAGE_MQUEUE_SIZE  32

struct menu_event_t {
    FuriRecordSubscriber    * event_record;
    osMessageQueueId_t      mqueue;
    osTimerId_t             timeout_timer;
    menu_message_t          current_message;
};

static void menu_event_input_callback(const void* value, size_t size, void* ctx) {
    menu_event_t * menu_event = ctx;
    const InputEvent* event = value;
    menu_message_t message;

    if (!event->state) return;

    if (event->input == InputUp) {
        message.type = MENU_MESSAGE_TYPE_UP;
    } else if (event->input == InputDown) {
        message.type = MENU_MESSAGE_TYPE_DOWN;
    } else if (event->input == InputRight) {
        message.type = MENU_MESSAGE_TYPE_RIGHT;
    } else if (event->input == InputLeft) {
        message.type = MENU_MESSAGE_TYPE_LEFT;
    } else if (event->input == InputOk) {
        message.type = MENU_MESSAGE_TYPE_OK;
    } else if (event->input == InputBack) {
        message.type = MENU_MESSAGE_TYPE_BACK;
    } else {
        message.type = MENU_MESSAGE_TYPE_WTF;
    }

    osMessageQueuePut(menu_event->mqueue, &message, 0, 0);
}

void menu_event_timeout_callback(void *arg)
{
    menu_event_t * menu_event = arg;
    menu_message_t message;
    message.type = MENU_MESSAGE_TYPE_IDLE;
    osMessageQueuePut(menu_event->mqueue, &message, 0, 0);
}

menu_event_t * menu_event_alloc()
{
    menu_event_t * menu_event = furi_alloc(sizeof(menu_event_t));

    menu_event->mqueue = osMessageQueueNew(MENU_MESSAGE_MQUEUE_SIZE, sizeof(menu_event_t), NULL);
    assert(menu_event->mqueue != NULL);

    menu_event->timeout_timer = osTimerNew(menu_event_timeout_callback, osTimerOnce, menu_event, NULL);
    assert(menu_event->timeout_timer != NULL);

    menu_event->event_record = furi_open("input_events", false, false, menu_event_input_callback, NULL, menu_event);
    assert(menu_event->event_record != NULL);

    return menu_event;
}

void menu_event_activity_notify(menu_event_t * menu_event)
{
    osTimerStart(menu_event->timeout_timer, 60000U); // 1m timeout, return to main
}

menu_message_t * menu_event_next(menu_event_t * menu_event) {
    if (osMessageQueueGet(menu_event->mqueue, &menu_event->current_message, NULL, osWaitForever) == osOK) {
        return &menu_event->current_message;
    } else {
        return NULL;
    }
}

