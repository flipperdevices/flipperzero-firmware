#include "menu_event.h"

#include <cmsis_os2.h>
#include <assert.h>
#include <string.h>
#include <stdlib.h>

#include <furi.h>

#define MENU_MESSAGE_MQUEUE_SIZE  16

struct menu_event_t {
    osMessageQueueId_t      mqueue;
    osTimerId_t             timeout_timer;
};

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


    return menu_event;
}

void menu_event_free(menu_event_t *menu_event)
{
    assert(menu_event);
    assert(osMessageQueueDelete(menu_event->mqueue) == osOK);
    free(menu_event);
}

void menu_event_activity_notify(menu_event_t *menu_event)
{
    assert(menu_event);
    osTimerStart(menu_event->timeout_timer, 60000U); // 1m timeout, return to main
}

menu_message_t menu_event_next(menu_event_t *menu_event) {
    assert(menu_event);
    menu_message_t message;
    while (osMessageQueueGet(menu_event->mqueue, &message, NULL, osWaitForever) != osOK) {};
    return message;
}

void menu_event_input_callback(InputEvent *input_event, menu_event_t *menu_event)
{
    menu_message_t message;

    if (!input_event->state) return;

    if (input_event->input == InputUp) {
        message.type = MENU_MESSAGE_TYPE_UP;
    } else if (input_event->input == InputDown) {
        message.type = MENU_MESSAGE_TYPE_DOWN;
    } else if (input_event->input == InputRight) {
        message.type = MENU_MESSAGE_TYPE_RIGHT;
    } else if (input_event->input == InputLeft) {
        message.type = MENU_MESSAGE_TYPE_LEFT;
    } else if (input_event->input == InputOk) {
        message.type = MENU_MESSAGE_TYPE_OK;
    } else if (input_event->input == InputBack) {
        message.type = MENU_MESSAGE_TYPE_BACK;
    } else {
        message.type = MENU_MESSAGE_TYPE_WTF;
    }

    osMessageQueuePut(menu_event->mqueue, &message, 0, 0);
}
