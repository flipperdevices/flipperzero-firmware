#include "ibutton-app.h"
#include <stdarg.h>

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
    api_hal_timebase_insomnia_enter();
}

iButtonApp::~iButtonApp() {
    delete onewire_master;
    api_hal_timebase_insomnia_exit();
}

iButtonAppViewManager* iButtonApp::get_view_manager() {
    return &view;
}

void iButtonApp::switch_to_next_scene(Scene mode) {
    prevous_scene.push_front(current_scene);

    if(mode != Scene::SceneExit) {
        scenes[current_scene]->on_exit(this);
        current_scene = mode;
        scenes[current_scene]->on_enter(this);
    }
}

bool iButtonApp::switch_to_prevous_scene() {
    return switch_to_prevous_scene(1);
}

bool iButtonApp::switch_to_prevous_scene(uint8_t count) {
    Scene mode;

    for(uint8_t i = 0; i < count; i++) {
        mode = get_prevous_scene();
        if(mode == Scene::SceneExit) break;
    }

    if(mode == Scene::SceneExit) {
        return true;
    } else {
        scenes[current_scene]->on_exit(this);
        current_scene = mode;
        scenes[current_scene]->on_enter(this);
        return false;
    }
}

iButtonApp::Scene iButtonApp::get_prevous_scene() {
    Scene scene = prevous_scene.front();
    prevous_scene.pop_front();
    return scene;
}

const GpioPin* iButtonApp::get_ibutton_pin() {
    // TODO open record
    return &ibutton_gpio;
}

OneWireMaster* iButtonApp::get_onewire_master() {
    return onewire_master;
}

iButtonKey* iButtonApp::get_key() {
    return &key;
}

void iButtonApp::notify_init() {
    // TODO open record
    const GpioPin* led_r_record = &led_gpio[0];
    const GpioPin* led_g_record = &led_gpio[1];
    const GpioPin* vibro_record = &vibro_gpio;

    gpio_init(vibro_record, GpioModeOutputPushPull);
    gpio_init(led_r_record, GpioModeOutputOpenDrain);
    gpio_init(led_g_record, GpioModeOutputOpenDrain);

    gpio_write(led_r_record, true);
    gpio_write(led_g_record, true);
    gpio_write(vibro_record, false);
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

void iButtonApp::notify_error() {
    notify_vibro_on();
    delay(50);
    notify_vibro_off();
    delay(100);
    notify_vibro_on();
    delay(50);
    notify_vibro_off();
    delay(100);
}

void iButtonApp::notify_success() {
    notify_vibro_on();
    hal_pwm_set(0.5, 1760, &SPEAKER_TIM, SPEAKER_CH);
    delay(50);
    hal_pwm_stop(&SPEAKER_TIM, SPEAKER_CH);
    notify_vibro_off();

    delay(100);
}

void iButtonApp::notify_vibro_on() {
    const GpioPin* vibro_record = &vibro_gpio;
    //gpio_write(vibro_record, true);
}

void iButtonApp::notify_vibro_off() {
    const GpioPin* vibro_record = &vibro_gpio;
    //gpio_write(vibro_record, false);
}

void iButtonApp::pause_os() {
    __disable_irq();
}

void iButtonApp::resume_os() {
    __enable_irq();
}

void iButtonApp::set_text_store(const char* text...) {
    va_list args;
    va_start(args, text);

    vsnprintf(text_store, text_store_size, text, args);

    va_end(args);
}

const char* iButtonApp::get_text_store() {
    return text_store;
}