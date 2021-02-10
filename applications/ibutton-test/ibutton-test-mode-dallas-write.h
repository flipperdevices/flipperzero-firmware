#pragma once
#include "ibutton-test.h"
#include "blanks_writer.h"
#include "maxim_crc.h"

class AppiButtonTestModeDallasWrite
    : public AppTemplateMode<AppiButtonTestState, AppiButtonTestEvent> {
public:
    const char* name = "dallas read";
    AppiButtonTest* app;
    BlanksWriter* writer;

    void event(AppiButtonTestEvent* event, AppiButtonTestState* state);
    void render(Canvas* canvas, AppiButtonTestState* state);
    void acquire();
    void release();

    const GpioPin* one_wire_pin_record;

    AppiButtonTestModeDallasWrite(AppiButtonTest* parent_app) {
        app = parent_app;

        // TODO open record
        one_wire_pin_record = &ibutton_gpio;
        writer = new BlanksWriter(one_wire_pin_record);
    };
};

void AppiButtonTestModeDallasWrite::event(AppiButtonTestEvent* event, AppiButtonTestState* state) {
    if(event->type == AppiButtonTestEvent::EventTypeTick) {
        WriterResult result =
            writer->write(KEY_DS1990, state->dallas_address[state->dallas_address_index], 8);

        if(result == WR_SAME_KEY) {
            app->blink_green();
        }

        if(result == WR_OK) {
            app->blink_red();
        }

    } else if(event->type == AppiButtonTestEvent::EventTypeKey) {
        if(event->value.input.type == InputTypeShort && event->value.input.key == InputKeyUp) {
            app->decrease_dallas_address();
        }

        if(event->value.input.type == InputTypeShort && event->value.input.key == InputKeyDown) {
            app->increase_dallas_address();
        }
    }
}

void AppiButtonTestModeDallasWrite::render(Canvas* canvas, AppiButtonTestState* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "< Dallas write >");
    app->render_dallas_list(canvas, state);
}

void AppiButtonTestModeDallasWrite::acquire() {
    writer->start();
}

void AppiButtonTestModeDallasWrite::release() {
    writer->stop();
}