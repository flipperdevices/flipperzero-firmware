#include "ibutton-app.h"

void iButtonApp::run(void) {
    iButtonEvent event;
    bool consumed;
    bool exit = false;

    mode_data[current_mode]->on_enter(this);

    while(!exit) {
        view.receive_event(&event);

        consumed = mode_data[current_mode]->on_event(this, &event);

        if(!consumed) {
            if(event.type == iButtonEvent::Type::EventTypeBack) {
                exit = switch_to_prevous();
            }
        }
    };
}

iButtonApp::iButtonApp() {
    onewire_master = new OneWireMaster(get_ibutton_pin());
    notify_init();
}

iButtonApp::~iButtonApp() {
    delete onewire_master;
}

iButtonAppView* iButtonApp::get_view() {
    return &view;
}

void iButtonApp::switch_to_next(Mode mode) {
    prevous_mode.push_front(current_mode);

    if(mode != Mode::iButtonAppModeExit) {
        mode_data[current_mode]->on_exit(this);
        current_mode = mode;
        mode_data[current_mode]->on_enter(this);
    }
}

bool iButtonApp::switch_to_prevous() {
    Mode mode = prevous_mode.front();
    prevous_mode.pop_front();

    if(mode == Mode::iButtonAppModeExit) {
        return true;
    } else {
        mode_data[current_mode]->on_exit(this);
        current_mode = mode;
        mode_data[current_mode]->on_enter(this);
        return false;
    }
}

const GpioPin* iButtonApp::get_ibutton_pin() {
    // TODO open record
    return &ibutton_gpio;
}

OneWireMaster* iButtonApp::get_onewire_master() {
    return onewire_master;
}

void iButtonApp::notify_init() {
    // TODO open record
    const GpioPin* led_r_record = &led_gpio[0];
    const GpioPin* led_g_record = &led_gpio[1];

    gpio_init(led_r_record, GpioModeOutputOpenDrain);
    gpio_init(led_g_record, GpioModeOutputOpenDrain);
    gpio_write(led_r_record, true);
    gpio_write(led_g_record, true);
}

void iButtonApp::notify_green_blink() {
    notify_green_on();
    delay(10);
    notify_green_off();
}

void iButtonApp::notify_red_blink() {
    notify_red_on();
    delay(10);
    notify_red_off();
}

void iButtonApp::notify_green_on() {
    const GpioPin* led_g_record = &led_gpio[1];
    gpio_write(led_g_record, false);
}

void iButtonApp::notify_green_off() {
    const GpioPin* led_g_record = &led_gpio[1];
    gpio_write(led_g_record, true);
}

void iButtonApp::notify_red_on() {
    const GpioPin* led_r_record = &led_gpio[0];
    gpio_write(led_r_record, false);
}

void iButtonApp::notify_red_off() {
    const GpioPin* led_r_record = &led_gpio[0];
    gpio_write(led_r_record, true);
}