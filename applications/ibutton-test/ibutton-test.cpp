#include "ibutton-test.h"
#include "ibutton-test-mode-dallas-read.h"
#include "ibutton-test-mode-dallas-emulate.h"
#include "ibutton-test-mode-dallas-write.h"
#include "ibutton-test-mode-cyfral-read.h"
#include "ibutton-test-mode-cyfral-emulate.h"

// start app
void AppiButtonTest::run() {
    mode[0] = new AppiButtonTestModeDallasRead(this);
    mode[1] = new AppiButtonTestModeDallasEmulate(this);
    mode[2] = new AppiButtonTestModeDallasWrite(this);
    mode[3] = new AppiButtonTestModeCyfralRead(this);
    mode[4] = new AppiButtonTestModeCyfralEmulate(this);

    switch_to_mode(0);

    // TODO open record
    red_led_record = &led_gpio[0];
    green_led_record = &led_gpio[1];

    // configure pin
    gpio_init(red_led_record, GpioModeOutputOpenDrain);
    gpio_init(green_led_record, GpioModeOutputOpenDrain);

    api_hal_power_insomnia_enter();
    app_ready();

    AppiButtonTestEvent event;
    while(1) {
        if(get_event(&event, 20)) {
            if(event.type == AppiButtonTestEvent::EventTypeKey) {
                // press events
                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyBack) {
                    view_port_enabled_set(view_port, false);
                    gui_remove_view_port(gui, view_port);
                    api_hal_power_insomnia_exit();

                    osThreadExit();
                }

                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyLeft) {
                    decrease_mode();
                }

                if(event.value.input.type == InputTypeShort &&
                   event.value.input.key == InputKeyRight) {
                    increase_mode();
                }
            }
        } else {
            event.type = AppiButtonTestEvent::EventTypeTick;
        }

        acquire_state();
        mode[state.mode_index]->event(&event, &state);
        release_state();

        view_port_update(view_port);
    };
}

// render app
void AppiButtonTest::render(Canvas* canvas) {
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 12, "iButton");

    mode[state.mode_index]->render(canvas, &state);
}

void AppiButtonTest::render_dallas_list(Canvas* canvas, AppiButtonTestState* state) {
    const uint8_t buffer_size = 50;
    char buf[buffer_size];
    for(uint8_t i = 0; i < state->dallas_address_count; i++) {
        snprintf(
            buf,
            buffer_size,
            "%s[%u] %x:%x:%x:%x:%x:%x:%x:%x",
            (i == state->dallas_address_index) ? "> " : "",
            i + 1,
            state->dallas_address[i][0],
            state->dallas_address[i][1],
            state->dallas_address[i][2],
            state->dallas_address[i][3],
            state->dallas_address[i][4],
            state->dallas_address[i][5],
            state->dallas_address[i][6],
            state->dallas_address[i][7]);
        canvas_draw_str(canvas, 2, 37 + i * 12, buf);
    }
}

void AppiButtonTest::render_cyfral_list(Canvas* canvas, AppiButtonTestState* state) {
    const uint8_t buffer_size = 50;
    char buf[buffer_size];
    for(uint8_t i = 0; i < state->cyfral_address_count; i++) {
        snprintf(
            buf,
            buffer_size,
            "%s[%u] %x:%x:%x:%x",
            (i == state->cyfral_address_index) ? "> " : "",
            i + 1,
            state->cyfral_address[i][0],
            state->cyfral_address[i][1],
            state->cyfral_address[i][2],
            state->cyfral_address[i][3]);
        canvas_draw_str(canvas, 2, 37 + i * 12, buf);
    }
}

void AppiButtonTest::blink_red() {
    gpio_write(red_led_record, 0);
    delay(10);
    gpio_write(red_led_record, 1);
}

void AppiButtonTest::blink_green() {
    gpio_write(green_led_record, 0);
    delay(10);
    gpio_write(green_led_record, 1);
}

void AppiButtonTest::increase_mode() {
    acquire_state();
    if(state.mode_index < (modes_count - 1)) {
        mode[state.mode_index]->release();
        state.mode_index++;
        mode[state.mode_index]->acquire();
    }
    release_state();
}

void AppiButtonTest::decrease_mode() {
    acquire_state();
    if(state.mode_index > 0) {
        mode[state.mode_index]->release();
        state.mode_index--;
        mode[state.mode_index]->acquire();
    }
    release_state();
}

void AppiButtonTest::increase_dallas_address() {
    if(state.dallas_address_index < (state.dallas_address_count - 1)) {
        state.dallas_address_index++;
    }
}

void AppiButtonTest::decrease_dallas_address() {
    if(state.dallas_address_index > 0) {
        state.dallas_address_index--;
    }
}

void AppiButtonTest::increase_cyfral_address() {
    if(state.cyfral_address_index < (state.cyfral_address_count - 1)) {
        state.cyfral_address_index++;
    }
}

void AppiButtonTest::decrease_cyfral_address() {
    if(state.cyfral_address_index > 0) {
        state.cyfral_address_index--;
    }
}

void AppiButtonTest::switch_to_mode(uint8_t mode_index) {
    mode[state.mode_index]->release();
    state.mode_index = mode_index;
    mode[state.mode_index]->acquire();
}

// app enter function
extern "C" int32_t app_ibutton_test(void* p) {
    AppiButtonTest* app = new AppiButtonTest();
    app->run();
    return 0;
}