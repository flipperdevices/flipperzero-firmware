#include "ibutton-app.h"

void iButtonApp::run(void) {
    iButtonEvent event;
    bool consumed;
    bool exit = false;

    scenes[current_scene]->on_enter(this);

    while(!exit) {
        view.receive_event(&event);

        consumed = scenes[current_scene]->on_event(this, &event);

        if(!consumed) {
            if(event.type == iButtonEvent::Type::EventTypeBack) {
                exit = switch_to_prevous_scene();
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

iButtonAppViewManager* iButtonApp::get_view_manager() {
    return &view;
}

void iButtonApp::switch_to_next_scene(Scene mode) {
    prevous_scene.push_front(current_scene);

    if(mode != Scene::iButtonAppSceneExit) {
        scenes[current_scene]->on_exit(this);
        current_scene = mode;
        scenes[current_scene]->on_enter(this);
    }
}

bool iButtonApp::switch_to_prevous_scene() {
    Scene mode = prevous_scene.front();
    prevous_scene.pop_front();

    if(mode == Scene::iButtonAppSceneExit) {
        return true;
    } else {
        scenes[current_scene]->on_exit(this);
        current_scene = mode;
        scenes[current_scene]->on_enter(this);
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