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
    notify_init();
    api_hal_timebase_insomnia_enter();

    key_worker = new KeyWorker(&ibutton_gpio);

    // we need random
    srand(DWT->CYCCNT);
}

iButtonApp::~iButtonApp() {
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

KeyWorker* iButtonApp::get_key_worker() {
    return key_worker;
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
    gpio_write(&led_gpio[1], false);
}

void iButtonApp::notify_green_off() {
    gpio_write(&led_gpio[1], true);
}

void iButtonApp::notify_red_on() {
    gpio_write(&led_gpio[0], false);
}

void iButtonApp::notify_red_off() {
    gpio_write(&led_gpio[0], true);
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
    //gpio_write(&vibro_gpio, true);
}

void iButtonApp::notify_vibro_off() {
    gpio_write(&vibro_gpio, false);
}

void iButtonApp::set_text_store(const char* text...) {
    va_list args;
    va_start(args, text);

    vsnprintf(text_store, text_store_size, text, args);

    va_end(args);
}

char* iButtonApp::get_text_store() {
    return text_store;
}

uint8_t iButtonApp::get_text_store_size() {
    return text_store_size;
}

KeyStore* iButtonApp::get_key_store() {
    return &store;
}

uint8_t iButtonApp::get_stored_key_index() {
    return key_index;
}

void iButtonApp::set_stored_key_index(uint8_t _index) {
    key_index = _index;
}

void iButtonApp::generate_random_name(char* name, uint8_t max_name_size) {
    const uint8_t prefix_size = 9;
    const char* prefix[prefix_size] = {
        "ancient",
        "hollow",
        "strange",
        "disappeared",
        "unknown",
        "unthinkable",
        "unnamable",
        "nameless",
        "my",
    };

    const uint8_t suffix_size = 8;
    const char* suffix[suffix_size] = {
        "door",
        "entrance",
        "doorway",
        "entry",
        "portal",
        "entree",
        "opening",
        "crack",
    };

    sniprintf(
        name, max_name_size, "%s_%s", prefix[rand() % prefix_size], suffix[rand() % suffix_size]);
    // to upper
    name[0] = name[0] - 0x20;
}