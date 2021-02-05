#pragma once
#include "ibutton.h"
#include "cyfral_emulator.h"

class AppiButtonTestModeCyfralEmulate
    : public AppTemplateMode<AppiButtonTestState, AppiButtonTestEvent> {
public:
    const char* name = "cyfral emulate";
    AppiButtonTest* app;
    CyfralEmulator* cyfral_emulator;

    void event(AppiButtonTestEvent* event, AppiButtonTestState* state);
    void render(Canvas* canvas, AppiButtonTestState* state);
    void acquire();
    void release();

    AppiButtonTestModeCyfralEmulate(AppiButtonTest* parent_app) {
        app = parent_app;

        // TODO open record
        const GpioPin* one_wire_pin_record = &ibutton_gpio;
        cyfral_emulator = new CyfralEmulator(one_wire_pin_record);
    };
};

void AppiButtonTestModeCyfralEmulate::event(AppiButtonTestEvent* event, AppiButtonTestState* state) {
    if(event->type == AppiButtonTestEvent::EventTypeTick) {
        // repeat key sending 8 times
        cyfral_emulator->send(state->cyfral_address[state->cyfral_address_index], 4, 8);
        app->blink_green();

    } else if(event->type == AppiButtonTestEvent::EventTypeKey) {
        if(event->value.input.state && event->value.input.input == InputUp) {
            app->decrease_cyfral_address();
        }

        if(event->value.input.state && event->value.input.input == InputDown) {
            app->increase_cyfral_address();
        }
    }
}

void AppiButtonTestModeCyfralEmulate::render(Canvas* canvas, AppiButtonTestState* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "< Cyfral emulate");

    app->render_cyfral_list(canvas, state);
}

void AppiButtonTestModeCyfralEmulate::acquire() {
    cyfral_emulator->start();
}

void AppiButtonTestModeCyfralEmulate::release() {
    cyfral_emulator->stop();
}