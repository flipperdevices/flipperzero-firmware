#pragma once
#include "ibutton.h"
#include "cyfral_reader_gpio.h"

class AppiButtonModeCyfralRead : public AppTemplateMode<AppiButtonState, AppiButtonEvent> {
public:
    const char* name = "cyfral read";
    AppiButton* app;
    CyfralReaderGpio* reader;

    void event(AppiButtonEvent* event, AppiButtonState* state);
    void render(CanvasApi* canvas, AppiButtonState* state);
    void acquire();
    void release();

    AppiButtonModeCyfralRead(AppiButton* parent_app) {
        app = parent_app;

        // TODO open record
        const GpioPin* one_wire_pin_record = &ibutton_gpio;
        reader = new CyfralReaderGpio(one_wire_pin_record);
    };
};

void AppiButtonModeCyfralRead::event(AppiButtonEvent* event, AppiButtonState* state) {
    if(event->type == AppiButtonEvent::EventTypeTick) {
        uint8_t data[8];
        if(reader->read(data, 4)) {
            memcpy(app->state.cyfral_address[app->state.cyfral_address_index], data, 4);
            app->blink_green();
        }
    } else if(event->type == AppiButtonEvent::EventTypeKey) {
        if(event->value.input.state && event->value.input.input == InputUp) {
            app->decrease_cyfral_address();
        }

        if(event->value.input.state && event->value.input.input == InputDown) {
            app->increase_cyfral_address();
        }
    }
}

void AppiButtonModeCyfralRead::render(CanvasApi* canvas, AppiButtonState* state) {
    canvas->set_font(canvas, FontSecondary);
    canvas->draw_str(canvas, 2, 25, "< Cyfral read >");

    app->render_cyfral_list(canvas, state);
}

void AppiButtonModeCyfralRead::acquire() {
    reader->start();
}

void AppiButtonModeCyfralRead::release() {
    reader->stop();
}