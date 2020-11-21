#pragma once
#include "ibutton.h"
#include "one_wire_gpio.h"
#include "maxim_crc.h"

class AppiButtonModeDallasRead : public AppTemplateMode<AppiButtonState, AppiButtonEvent> {
public:
    const char* name = "dallas read";
    AppiButton* app;
    OneWireGpio* onewire;

    void event(AppiButtonEvent* event, AppiButtonState* state);
    void render(CanvasApi* canvas, AppiButtonState* state);
    void acquire();
    void release();

    AppiButtonModeDallasRead(AppiButton* parent_app) {
        app = parent_app;

        // TODO open record
        const GpioPin* one_wire_pin_record = &ibutton_gpio;
        onewire = new OneWireGpio(one_wire_pin_record);
    };
};

void AppiButtonModeDallasRead::event(AppiButtonEvent* event, AppiButtonState* state) {
    if(event->type == AppiButtonEvent::EventTypeTick) {
        bool result = 0;
        uint8_t address[8];

        osKernelLock();
        result = onewire->reset();
        osKernelUnlock();

        if(result) {
            printf("device on line\n");

            delay(50);
            osKernelLock();
            onewire->write(0x33);
            onewire->read_bytes(address, 8);
            osKernelUnlock();

            printf("address: %x", address[0]);
            for(uint8_t i = 1; i < 8; i++) {
                printf(":%x", address[i]);
            }
            printf("\n");

            printf("crc8: %x\n", maxim_crc8(address, 7));

            if(maxim_crc8(address, 8) == 0) {
                printf("CRC valid\n");
                memcpy(app->state.dallas_address[app->state.dallas_address_index], address, 8);
                app->blink_green();
            } else {
                printf("CRC invalid\n");
            }
        } else {
        }
    }
}

void AppiButtonModeDallasRead::render(CanvasApi* canvas, AppiButtonState* state) {
    canvas->set_font(canvas, FontSecondary);
    canvas->draw_str(canvas, 2, 25, "Dallas read >");

    const uint8_t buffer_size = 50;
    char buf[buffer_size];
    for(uint8_t i = 0; i < state->dallas_address_count; i++) {
        snprintf(
            buf,
            buffer_size,
            "%s[%u] %x:%x:%x:%x:%x:%x:%x:%x",
            (i == state->dallas_address_index) ? "> " : "",
            i+1,
            state->dallas_address[i][0],
            state->dallas_address[i][1],
            state->dallas_address[i][2],
            state->dallas_address[i][3],
            state->dallas_address[i][4],
            state->dallas_address[i][5],
            state->dallas_address[i][6],
            state->dallas_address[i][7]);
        canvas->draw_str(canvas, 2, 37 + i * 12, buf);
    }
}

void AppiButtonModeDallasRead::acquire() {
    onewire->start();
}

void AppiButtonModeDallasRead::release() {
    onewire->stop();
}