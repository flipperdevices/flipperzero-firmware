#pragma once
#include "ibutton-test.h"
#include "one_wire_master.h"
#include "maxim_crc.h"

class AppiButtonTestModeDallasRead
    : public AppTemplateMode<AppiButtonTestState, AppiButtonTestEvent> {
public:
    const char* name = "dallas read";
    AppiButtonTest* app;
    OneWireMaster* onewire;

    void event(AppiButtonTestEvent* event, AppiButtonTestState* state);
    void render(Canvas* canvas, AppiButtonTestState* state);
    void acquire();
    void release();

    AppiButtonTestModeDallasRead(AppiButtonTest* parent_app) {
        app = parent_app;

        // TODO open record
        const GpioPin* one_wire_pin_record = &ibutton_gpio;
        onewire = new OneWireMaster(one_wire_pin_record);
    };
};

void AppiButtonTestModeDallasRead::event(AppiButtonTestEvent* event, AppiButtonTestState* state) {
    if(event->type == AppiButtonTestEvent::EventTypeTick) {
        bool result = 0;
        uint8_t address[8];

        osKernelLock();
        result = onewire->reset();
        osKernelUnlock();

        if(result) {
            osKernelLock();
            __disable_irq();
            onewire->write(0x33);
            onewire->read_bytes(address, 8);
            __enable_irq();
            osKernelUnlock();

            if(maxim_crc8(address, 8) == 0) {
                memcpy(app->state.dallas_address[app->state.dallas_address_index], address, 8);
                app->blink_green();
            }
        }
    } else if(event->type == AppiButtonTestEvent::EventTypeKey) {
        if(event->value.input.state && event->value.input.input == InputUp) {
            app->decrease_dallas_address();
        }

        if(event->value.input.state && event->value.input.input == InputDown) {
            app->increase_dallas_address();
        }
    }
}

void AppiButtonTestModeDallasRead::render(Canvas* canvas, AppiButtonTestState* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "Dallas read >");
    app->render_dallas_list(canvas, state);
}

void AppiButtonTestModeDallasRead::acquire() {
    onewire->start();
}

void AppiButtonTestModeDallasRead::release() {
    onewire->stop();
}