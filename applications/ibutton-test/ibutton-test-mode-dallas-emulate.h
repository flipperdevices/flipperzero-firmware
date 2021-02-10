#pragma once
#include "ibutton-test.h"
#include "one_wire_slave.h"
#include "one_wire_device_ds_1990.h"
#include "callback-connector.h"
#include <atomic>

class AppiButtonTestModeDallasEmulate
    : public AppTemplateMode<AppiButtonTestState, AppiButtonTestEvent> {
private:
    void result_callback(bool success, void* ctx);

public:
    const char* name = "dallas emulate";
    AppiButtonTest* app;
    DS1990 key;
    OneWireSlave* onewire_slave;

    void event(AppiButtonTestEvent* event, AppiButtonTestState* state);
    void render(Canvas* canvas, AppiButtonTestState* state);
    void acquire();
    void release();

    std::atomic<bool> emulated_result{false};

    AppiButtonTestModeDallasEmulate(AppiButtonTest* parent_app)
        : key(1, 2, 3, 4, 5, 6, 7) {
        app = parent_app;

        // TODO open record
        const GpioPin* one_wire_pin_record = &ibutton_gpio;
        onewire_slave = new OneWireSlave(one_wire_pin_record);
        onewire_slave->attach(&key);

        auto cb = cbc::obtain_connector(this, &AppiButtonTestModeDallasEmulate::result_callback);
        onewire_slave->set_result_callback(cb, this);
    };
};

void AppiButtonTestModeDallasEmulate::result_callback(bool success, void* ctx) {
    AppiButtonTestModeDallasEmulate* _this = static_cast<AppiButtonTestModeDallasEmulate*>(ctx);
    _this->emulated_result = success;
}

void AppiButtonTestModeDallasEmulate::event(AppiButtonTestEvent* event, AppiButtonTestState* state) {
    if(event->type == AppiButtonTestEvent::EventTypeTick) {
        if(emulated_result) {
            emulated_result = false;
            app->blink_green();
        }
    } else if(event->type == AppiButtonTestEvent::EventTypeKey) {
        if(event->value.input.type == InputTypeShort && event->value.input.key == InputKeyUp) {
            app->decrease_dallas_address();
        }

        if(event->value.input.type == InputTypeShort && event->value.input.key == InputKeyDown) {
            app->increase_dallas_address();
        }
    }

    onewire_slave->deattach();
    memcpy(key.id_storage, state->dallas_address[state->dallas_address_index], 8);
    onewire_slave->attach(&key);
}

void AppiButtonTestModeDallasEmulate::render(Canvas* canvas, AppiButtonTestState* state) {
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 2, 25, "< Dallas emulate >");

    app->render_dallas_list(canvas, state);
}

void AppiButtonTestModeDallasEmulate::acquire() {
    onewire_slave->start();
}

void AppiButtonTestModeDallasEmulate::release() {
    onewire_slave->stop();
}