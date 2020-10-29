#include <stdio.h>

extern "C" {
    #include "main.h"
    #include "cmsis_os.h"
    #include "gpio.h"
}

#include "events.h"
#include "ui.h"
#include "vendor.h"

#include "onewire/onewire.h"

#include "modules/ibutton_backend.h"

extern QueueHandle_t event_queue;
QueueHandle_t ibutton_event_queue;

#define DEBUG 0

bool dallas_read(GpioPin pin, uint8_t* addr) {
    if(!onewire_reset(pin)) {
        return false;
    }

    osDelay(50);

    onewire_write(pin, 0x33);

    onewire_read_bytes(pin, addr, 8);

    return true;
}

// TODO maybe no need queue, only shared memory and mutexes
extern "C" void ibutton_th(void* ctx) {
    IButton* self = (IButton*)ctx;

    bool enabled = false;
    IButtonMode mode;

    uint8_t* addr_ptr;

    // printf("start ibutton thread\n");

    while(1) {
        Event event;

        if(xQueueReceive(ibutton_event_queue, (void*)&event, enabled ? 50 : 0)) {
            // handle event
            if(event.type == EventTypeIButtonControl) {
                mode = event.value.ibutton_control.mode;
                addr_ptr = event.value.ibutton_control.ptr;

                enabled = (mode == IButtonModeRead) && addr_ptr != NULL;

                if(enabled) {
                    printf("[ibutton] start reading\n");
                } else {
                    printf("[ibutton] end reading\n");
                }
            }
        } else {
            if(enabled) {
                if(dallas_read(self->pin, addr_ptr)) {
                    enabled = false;

                    // send ibutton presence event
                    Event event;
                    event.type = EventTypeIButton;
                    xQueueSend(event_queue, (void*)&event, 0);
                }
            }
        }
    }
}

uint8_t* dallas_key_id = NULL;

typedef enum {
    OneWireStateIdle,
    OneWireStateWriting,
    OneWireStateReadROM,
    OneWireStateSearchROM
} OneWireState;

GpioPin debug_pin = {GPIOB, GPIO_PIN_12};

void onewire_callback(uint16_t ccr, TimerEvent tim_event) {
    static int32_t prev_time = 0;
    static int32_t onewire_counter = 0;
    static uint8_t onewire_cmd = 0;
    static OneWireState state = OneWireStateWriting;
    static uint16_t pulse_counter = 0;

    if(tim_event == TimerEventInputCapture) {
        if(dallas_key_id == NULL) return;

        app_gpio_write(debug_pin, true);

        int32_t pulse_width = ((ccr - prev_time + 32768) % 32768);
        prev_time = ccr;

        if(pulse_width > 460 && pulse_width < 550) {
            // reset pulse
            onewire_counter = 0;
            onewire_cmd = 0;
            state = OneWireStateWriting;
            pulse_counter = 0;

            // app_gpio_write(debug_pin, false);

            delay_us(18);
            app_tim_pulse(150);
        }

        app_gpio_write(debug_pin, false);

        pulse_counter++;

        if((pulse_counter & 0x01) == 0) return;
        

        if(state == OneWireStateReadROM && onewire_counter < 64) {
            if(pulse_width > 1 && pulse_width < 15) {
                // get reading pulse
                if(((dallas_key_id[onewire_counter / 8] >> (onewire_counter % 8)) & 0x1) == 0) {
                    app_tim_pulse(30);
                }

                onewire_counter++;
            }
        }

        if(state == OneWireStateSearchROM && onewire_counter < 64 * 3) {
            uint8_t id_bit = ((dallas_key_id[(onewire_counter / 3) / 8] >> ((onewire_counter / 3) % 8)) & 0x1) != 0;

            if(onewire_counter % 3 == 2) {
                // app_gpio_write(debug_pin, true);

                if(pulse_width > 1 && pulse_width < 15) {
                    if(id_bit == 0) {
                        state = OneWireStateIdle;
                    }

                    onewire_counter++;
                } else if(pulse_width > 40 && pulse_width < 120) {
                    if(id_bit == 1) {
                        state = OneWireStateIdle;
                    }

                    onewire_counter++;
                }
            } else if(pulse_width > 1 && pulse_width < 15) {
                if(onewire_counter % 3 == 0) {
                    // send id
                    if(!id_bit) {
                        app_tim_pulse(30);
                    } else {
                        // app_gpio_write(debug_pin, false);
                    }

                    onewire_counter++;
                } else if(onewire_counter % 3 == 1) {
                    // app_gpio_write(debug_pin, false);

                    // send id compliment
                    if(id_bit) {
                        app_tim_pulse(30);
                    } else {
                        // app_gpio_write(debug_pin, false);
                    }

                    onewire_counter++;
                }
            }
        }

        if(state == OneWireStateWriting) {
            if(pulse_width > 1 && pulse_width < 30) {
                // write 1

                if(onewire_counter < 8) {
                    onewire_cmd |= 1 << (onewire_counter % 8);
                    onewire_counter++;
                }
            } else if(pulse_width > 40 && pulse_width < 120) {
                // write 0

                if(onewire_counter < 8) {
                    onewire_cmd |= 0 << (onewire_counter % 8);
                    onewire_counter++;
                }
            }

            if(onewire_counter == 8 && state == OneWireStateWriting) {
                if(onewire_cmd == 0x33 || onewire_cmd == 0x0F) {
                    state = OneWireStateReadROM;
                    onewire_counter = 0;
                }

                if(onewire_cmd == 0xF0) {
                    state = OneWireStateSearchROM;
                    onewire_counter = 0;
                }
                

                /*
                Event event;
                event.type = EventTypeDummy;
                event.value.dummy = onewire_cmd;

                BaseType_t xHigherPriorityTaskWoken = pdFALSE;
                xQueueSendFromISR(event_queue, (void*)&event, &xHigherPriorityTaskWoken);
                if( xHigherPriorityTaskWoken ) {
                    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
                }
                */
            }
        }
    }

    if(tim_event == TimerEventEndOfPulse) {
        app_tim_ic_init(true);
    }
}

IButton::IButton(GpioPin pin) {
    this->pin = pin;

    ibutton_event_queue = xQueueCreate(1, sizeof(Event));
    

    TaskHandle_t handle = NULL;
    xTaskCreate(
        ibutton_th, "ibutton", 256, (void*)this, (UBaseType_t)osPriorityNormal, &handle
    );

    app_gpio_init(debug_pin, GpioModeOutput);

    register_tim8_callback_ch2(onewire_callback);
}

void IButton::handle(Event* event, struct _Store* store, u8g2_t* u8g2, ScreenArea area) {
    if(event->type == EventTypeIButtonControl) {

        IButtonMode mode = event->value.ibutton_control.mode;
        uint8_t* addr_ptr = event->value.ibutton_control.ptr;

        switch(mode) {
            case IButtonModeRead:
                if(addr_ptr != NULL) {
                    app_gpio_init(this->pin, GpioModeOpenDrain);
                }
            break;

            case IButtonModeEmulateDallas:
                if(addr_ptr != NULL) {
                    printf("[ibutton] start emulating\n");

                    GPIO_InitTypeDef GPIO_InitStruct = {0};
                    GPIO_InitStruct.Pin = iButton_Pin;
                    GPIO_InitStruct.Mode = GPIO_MODE_AF_OD;
                    GPIO_InitStruct.Pull = GPIO_NOPULL;
                    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_MEDIUM;
                    GPIO_InitStruct.Alternate = GPIO_AF3_TIM8;
                    HAL_GPIO_Init(iButton_GPIO_Port, &GPIO_InitStruct);

                    dallas_key_id = addr_ptr;

                    register_tim8_callback_ch2(onewire_callback);
                    app_tim_ic_init(true);
                }
            break;

            case IButtonModeDisabled:
                printf("[ibutton] stop\n");
                register_tim8_callback_ch2(NULL);
                app_tim_stop();
            break;
        }

        xQueueSend(ibutton_event_queue, (void*)event, 0);
    }
}
