#include <stdio.h>

extern "C" {
    #include "main.h"
    #include "cmsis_os.h"
    #include "gpio.h"
}

#include "events.h"
#include "ui.h"
#include "vendor.h"

#include "modules/button_input.h"

extern QueueHandle_t event_queue;

#define DEBUG 0

TickType_t DEBOUNCE_TIME = 250;

// sequence of this buttons must correspond to Buttons enum
const GpioPin buttons_gpio[] = {
    {BUTTON_UP_GPIO_Port, BUTTON_UP_Pin},
    {BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin},
    {BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin},
    {BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin},
    {BUTTON_OK_GPIO_Port, BUTTON_OK_Pin},
    {BUTTON_BACK_GPIO_Port, BUTTON_BACK_Pin},
    {CHRG_GPIO_Port, CHRG_Pin}
};

const bool buttons_invert[] = {
    BUTON_INVERT, // {BUTTON_UP_GPIO_Port, BUTTON_UP_Pin},
    BUTON_INVERT, // {BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin},
    BUTON_INVERT, // {BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin},
    BUTON_INVERT, // {BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin},
    BUTON_INVERT, // {BUTTON_OK_GPIO_Port, BUTTON_OK_Pin},
    BUTON_INVERT, // {BUTTON_BACK_GPIO_Port, BUTTON_BACK_Pin},
    false // {CHRG_GPIO_Port, CHRG_Pin}
};

void debounce_callback(TimerHandle_t timer) {
    Buttons button = (Buttons)(size_t)pvTimerGetTimerID(timer);

    Event event;
    event.type = EventTypeDebounce;
    event.value.button.id = button;
    event.value.button.state = app_gpio_read(buttons_gpio[button]) ^ buttons_invert[button];

    xQueueSend(event_queue, (void*)&event, 0);
}


ButtonInput::ButtonInput() {
    for(size_t i = 0; i < ButtonsSize; i++) {
        this->debouncing_timers[i] = xTimerCreate("timer", DEBOUNCE_TIME, pdFALSE, (void*)i, debounce_callback);
        if(this->debouncing_timers[i] == NULL) {
            printf("failed to create timer %d\n", i);
        }

        this->button_state[i] = !(app_gpio_read(buttons_gpio[i]) ^ buttons_invert[i]);
    }
}

void ButtonInput::handle(Event* event, struct _Store* store, u8g2_t* u8g2, ScreenArea area) {
    if(event->type == EventTypeDebounce || event->type == EventTypeRawButton) {

        if(event->type == EventTypeRawButton) {
            #if DEBUG
                printf("get raw button event\n");
            #endif
            TimerHandle_t timer = this->debouncing_timers[(size_t)event->value.button.id];

            // ignore event if timer is still running
            if(timer != NULL && xTimerIsTimerActive(timer) == pdTRUE) {
                #if DEBUG
                printf("debouncing %d button\n", event->value.button.id);
                #endif
                return;
            } else {
                // start timer at first transition
                if(xTimerStart(timer, 0) != pdPASS) {
                    printf("fail to start debouncing timer %d\n", event->value.button.id);
                }
            }
        }

        if(event->type == EventTypeDebounce) {
            #if DEBUG
            printf("get debounce event\n");
            #endif
        }

        // prevent duplicated events
        if(this->button_state[event->value.button.id] == event->value.button.state) {
            #if DEBUG
            printf("ignore %d button\n", event->value.button.id);
            #endif
            return;
        } else {
            this->button_state[event->value.button.id] = event->value.button.state;
        }

        Event button_event;
        button_event.type = EventTypeButton;
        button_event.value = event->value;
        xQueueSend(event_queue, (void*)&button_event, 0);
    }

    if(event->type == EventTypeTick) {

        for(size_t i = 0; i < ButtonsSize; i++) {
            // printf("button %d = %d\n", i, app_gpio_read(buttons_gpio[i]) ^ buttons_invert[i]);
            bool button_state = app_gpio_read(buttons_gpio[i]);
            button_state ^= buttons_invert[i];

            if(this->button_state[i] != button_state) {
                printf("safety check at %d\n", i);
                Event event;
                event.type = EventTypeButton;
                event.value.button.id = (Buttons)i;
                event.value.button.state = app_gpio_read(buttons_gpio[i]) ^ buttons_invert[i];

                xQueueSend(event_queue, (void*)&event, 0);

                this->button_state[i] = button_state;
            }
        }
    }
}

extern "C" void HAL_GPIO_EXTI_Callback(uint16_t pin) {
    Event event;
    event.type = EventTypeRawButton;

    for(size_t i = 0; i < ButtonsSize; i++) {
        if(pin == buttons_gpio[i].pin) {
            event.value.button.id = (Buttons)i;
            break;
        }
    }

    event.value.button.state = 
        app_gpio_read(buttons_gpio[event.value.button.id]) ^
        buttons_invert[event.value.button.id];

    BaseType_t task_woken = pdFALSE;

    // TODO handle errors
    xQueueSendFromISR(event_queue, (void*)&event, &task_woken);

    if(task_woken) {
        portYIELD_FROM_ISR(task_woken);
    }
}
