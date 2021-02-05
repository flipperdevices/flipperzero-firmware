#pragma once
#include "ibutton.h"
#include "cyfral_reader_comp.h"

class AppiButtonTestModeCyfralRead
    : public AppTemplateMode<AppiButtonTestState, AppiButtonTestEvent> {
public:
    const char* name = "cyfral read";
    AppiButtonTest* app;
    CyfralReaderComp* reader;

    void event(AppiButtonTestEvent* event, AppiButtonTestState* state);
    void render(Canvas* canvas, AppiButtonTestState* state);
    void acquire();
    void release();

    AppiButtonTestModeCyfralRead(AppiButtonTest* parent_app) {
        app = parent_app;

        // TODO open record
        const GpioPin* one_wire_pin_record = &ibutton_gpio;
        reader = new CyfralReaderComp(one_wire_pin_record);
    };

    static const uint8_t key_length = 4;
    static const uint8_t num_keys_to_check = 4;
    uint8_t key_index = 0;
    uint8_t keys[num_keys_to_check][4];
};

void AppiButtonTestModeCyfralRead::event(AppiButtonTestEvent* event, AppiButtonTestState* state) {
    if(event->type == AppiButtonTestEvent::EventTypeTick) {
        // if we read a key
        if(reader->read(keys[key_index], key_length)) {
            // read next key
            key_index++;

            // if we read sufficient amount of keys
            if(key_index >= num_keys_to_check) {
                bool result = true;
                key_index = 0;

                // compare all keys
                for(uint8_t i = 1; i < num_keys_to_check; i++) {
                    if(memcmp(keys[i], keys[i - 1], key_length) != 0) {
                        result = false;
                        break;
                    }
                }

                // if all keys is same
                if(result) {
                    // copy key to mem and blink
                    memcpy(
                        app->state.cyfral_address[app->state.cyfral_address_index],
                        keys[key_index],
                        key_length);
                    app->blink_green();
                }
            }
        }
    } else if(event->type == AppiButtonTestEvent::EventTypeKey) {
        if(event->value.input.state && event->value.input.input == InputUp) {
            app->decrease_cyfral_address();
        }

        if(event->value.input.state && event->value.input.input == InputDown) {
            app->increase_cyfral_address();
        }
    }
}

void AppiButtonTestModeCyfralRead::render(Canvas* canvas, AppiButtonTestState* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "< Cyfral read >");

    app->render_cyfral_list(canvas, state);
}

void AppiButtonTestModeCyfralRead::acquire() {
    reader->start();
}

void AppiButtonTestModeCyfralRead::release() {
    reader->stop();
}