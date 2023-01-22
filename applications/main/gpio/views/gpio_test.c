#include "gpio_test.h"
#include "digital_signal.h"
#include "furi_hal.h"
#include "advanced_pwm.h"

#include "../gpio_item.h"

#include <gui/elements.h>
#include <math.h>

#define MAX_MENU_OPTIONS 20
#define MAX_MENU_ENTRIES 20
#define MAX_WS2812 500
#define COUNT(x) (sizeof(x) / sizeof(x[0]))

struct GpioTest {
    View* view;
    bool button_held;
    int32_t button_ticks;
    InputKey button;
    GpioTestOkCallback callback;
    void* context;
};

typedef struct {
    const char* name;
    const char* options[MAX_MENU_OPTIONS];
    int32_t value_min;
    int32_t value_max;
    void (*update_cbr)(void* _model, uint32_t entry);
} MenuEntry;

typedef struct {
    MenuEntry* menu;
    bool menu_entries_changed;
    int32_t menu_entry;
    int32_t menu_entry_value[MAX_MENU_ENTRIES];
    bool menu_entry_enabled[MAX_MENU_ENTRIES];
    int32_t pin_mode[GPIO_ITEM_COUNT];
    int32_t pin_in_config[GPIO_ITEM_COUNT];
    int32_t pin_out_config[GPIO_ITEM_COUNT];
    int32_t pin_signal[GPIO_ITEM_COUNT];

    bool pwm_enabled[GPIO_ITEM_COUNT];
    int32_t pwm_freq;
    int32_t pwm_duty;
    bool pwm_updated;
    bool pwm_started;
    AdvancedPwm* pwm_ctx;

    float ws2812_time;
    uint32_t ws2812_led;
    uint32_t ws2812_mode;
    uint32_t ws2812_color;
    FuriMutex* mutex;

    DigitalSignal* signal_bit_0;
    DigitalSignal* signal_bit_1;
    DigitalSignal* signal_reset;
    DigitalSignal* signal_off;
    DigitalSignal* signal_color;
} GpioTestModel;

static bool gpio_test_process_down(GpioTest* gpio_test);
static bool gpio_test_process_up(GpioTest* gpio_test);
static bool gpio_test_process_left(GpioTest* gpio_test);
static bool gpio_test_process_right(GpioTest* gpio_test);
static bool gpio_test_process_ok(GpioTest* gpio_test, InputEvent* event);

static void gpio_menu_mode_cbr(void* _model, uint32_t entry);
static void gpio_menu_config_cbr(void* _model, uint32_t entry);
static void gpio_menu_signal_cbr(void* _model, uint32_t entry);
static void gpio_menu_gpio_cbr(void* _model, uint32_t entry);
static void gpio_menu_ws2812_cbr(void* _model, uint32_t entry);
static void gpio_menu_pwm_cbr(void* _model, uint32_t entry);
static void gpio_menu_servo_cbr(void* _model, uint32_t entry);

static void modulate_colors(GpioTestModel* model);
static void gpio_test_ws2812_update(GpioTestModel* model);
static void gpio_test_pwm_update(GpioTestModel* model);

typedef enum {
    MenuGpio,
    MenuMode,
    MenuInputConfig,
    MenuOutputConfig,
    MenuSignal,
    MenuWs2812,
    MenuWs2812Led,
    MenuPwmFreq,
    MenuPwmDuty,
    MenuServoAngle,
    MenuServoMinPulse,
    MenuServoMaxPulse,
    MenuServoFreq
} MenuIndex;

typedef enum { MenuModeInput, MenuModeOutput, MenuModeOutputToggle, MenuModeSignal } MenuModeIndex;
typedef enum { MenuSignalWs2812, MenuSignalPwm, MenuSignalServo } MenuSignalModeIndex;

static MenuEntry gpio_menu_entries[] = {
    [MenuGpio] =
        {.name = "GPIO",
         .options = {},
         .value_min = 0,
         .value_max = GPIO_ITEM_COUNT - 1,
         .update_cbr = &gpio_menu_gpio_cbr},
    [MenuMode] =
        {.name = "Mode",
         .options = {"Input", "Output", "Toggle", "Signal"},
         .update_cbr = &gpio_menu_mode_cbr},
    [MenuInputConfig] =
        {.name = "Config",
         .options = {"Hi-Z", "Pull Up", "Pull Down"},
         .update_cbr = &gpio_menu_config_cbr},
    [MenuOutputConfig] =
        {.name = "Config",
         .options = {"Push Pull", "Open Drain"},
         .update_cbr = &gpio_menu_config_cbr},
    [MenuSignal] =
        {.name = "Signal",
         .options = {"WS2812", "PWM", "Servo"},
         .update_cbr = &gpio_menu_signal_cbr},
    [MenuWs2812] =
        {.name = "Color", .options = {"RGB", "RGBW"}, .update_cbr = &gpio_menu_ws2812_cbr},
    [MenuWs2812Led] =
        {.name = "LED",
         .options = {},
         .value_min = 1,
         .value_max = MAX_WS2812,
         .update_cbr = &gpio_menu_ws2812_cbr},
    [MenuPwmFreq] =
        {.name = "Freq",
         .options = {},
         .value_min = 1000,
         .value_max = 1000000,
         .update_cbr = &gpio_menu_pwm_cbr},
    [MenuPwmDuty] =
        {.name = "Duty",
         .options = {},
         .value_min = 1,
         .value_max = 99,
         .update_cbr = &gpio_menu_pwm_cbr},
    [MenuServoAngle] =
        {.name = "Angle",
         .options =
             {"-90",
              "-80",
              "-70",
              "-60",
              "-50",
              "-40",
              "-30",
              "-20",
              "-10",
              "0",
              "10",
              "20",
              "30",
              "40",
              "50",
              "60",
              "70",
              "80",
              "90"},
         .update_cbr = &gpio_menu_servo_cbr},
    [MenuServoMinPulse] =
        {.name = "Min", .options = {"500 us", "1000 us"}, .update_cbr = &gpio_menu_servo_cbr},
    [MenuServoMaxPulse] =
        {.name = "Max", .options = {"2000 us", "2500 us"}, .update_cbr = &gpio_menu_servo_cbr},
    [MenuServoFreq] = {
        .name = "Freq",
        .options = {"50 Hz", "100 Hz", "200 Hz"},
        .update_cbr = &gpio_menu_servo_cbr}};

#define MENU_ENTRY_COUNT (int32_t) COUNT(gpio_menu_entries)

/* accessors for the menu */
static int32_t gpio_menu_value_get(GpioTestModel* model, int32_t entry) {
    return model->menu_entry_value[entry];
}

static void gpio_menu_value_set(GpioTestModel* model, int32_t entry, int32_t value) {
    if(model->menu_entry_value[entry] != value) {
        model->menu_entry_value[entry] = value;
        model->menu_entries_changed = true;
        if(model->menu[entry].update_cbr) {
            model->menu[entry].update_cbr(model, entry);
        }
    }
}

static void gpio_menu_enable(GpioTestModel* model, int32_t entry, bool state) {
    if(model->menu_entry_enabled[entry] != state) {
        model->menu_entry_enabled[entry] = state;
        model->menu_entries_changed = true;
    }
}

static bool gpio_menu_enabled(GpioTestModel* model, int32_t entry) {
    return model->menu_entry_enabled[entry];
}

static void gpio_test_draw_callback(Canvas* canvas, void* _model) {
    GpioTestModel* model = _model;
    int32_t pos_y = 2;
    int32_t pos_x = 0;

    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, pos_y, AlignCenter, AlignTop, "GPIO Menu");
    pos_y += 15;

    if(model->menu_entry < 0 || model->menu_entry >= MENU_ENTRY_COUNT) {
        model->menu_entry = 0;
    }

    int32_t max_entries = 3;

    switch(gpio_menu_value_get(model, MenuMode)) {
    case MenuModeOutput:
        max_entries--;
        elements_button_center(canvas, "Enable");
        break;
    case MenuModeOutputToggle:
        max_entries--;
        elements_button_center(canvas, "Toggle");
        break;
    default:
        break;
    }

    /* draw menu entry MenuGpio */
    canvas_set_font(canvas, FontSecondary);
    pos_x = 14;
    for(uint8_t pin_index = 0; pin_index < GPIO_ITEM_COUNT; pin_index++) {
        bool io_high = gpio_item_get_pin(pin_index);
        char* pin_type[] = {"I", "O", "O", "S", "?"};
        char* pin_string = pin_type[model->pin_mode[pin_index]];

        if(io_high) {
            canvas_draw_box(canvas, pos_x - 4, pos_y - 5, 10, 9);
            canvas_invert_color(canvas);
            canvas_draw_str_aligned(canvas, pos_x, pos_y, AlignCenter, AlignCenter, pin_string);
            canvas_invert_color(canvas);
        } else {
            canvas_draw_str_aligned(canvas, pos_x, pos_y, AlignCenter, AlignCenter, pin_string);
        }

        if(gpio_menu_value_get(model, MenuGpio) == pin_index) {
            elements_frame(canvas, pos_x - 5, pos_y - 6, 12, 11);
        }
        pos_x += 13;
    }
    pos_y += 7;

    /*  */
    if(model->menu_entry == MenuGpio) {
        uint8_t pin_index = MIN(gpio_menu_value_get(model, MenuGpio), GPIO_ITEM_COUNT - 1);
        const char* name = gpio_item_get_pin_name(pin_index);

        if(!name) {
            name = "???";
        }
        canvas_draw_str_aligned(canvas, 64, pos_y, AlignCenter, AlignTop, name);
    }
    pos_y += 8;

    /* draw other menu entries */
    int32_t start_entry = 1;
    if(model->menu_entry > 2) {
        start_entry = model->menu_entry - 2;
    }
    int32_t end_entry = MIN(start_entry + max_entries + 1, MENU_ENTRY_COUNT);

    for(int32_t pos = start_entry; pos < MIN(end_entry, MENU_ENTRY_COUNT); pos++) {
        MenuEntry* entry = &model->menu[pos];

        /* if this entry is disabled, we have room for another one. increase end entry */
        if(!model->menu_entry_enabled[pos]) {
            end_entry++;
            continue;
        }

        if(model->menu_entry == pos) {
            canvas_set_font(canvas, FontPrimary);
        } else {
            canvas_set_font(canvas, FontSecondary);
        }
        char line_string[64];
        char arrow_l = ' ';
        char arrow_r = ' ';
        int32_t value = model->menu_entry_value[pos];

        snprintf(line_string, sizeof(line_string), "%s:", entry->name);
        elements_multiline_text_aligned(canvas, 2, pos_y, AlignLeft, AlignTop, line_string);

        if(entry->options[0]) {
            if(value > 0) {
                arrow_l = '<';
            }
            if(entry->options[value + 1] != NULL) {
                arrow_r = '>';
            }

            snprintf(
                line_string,
                sizeof(line_string),
                "%c %s %c",
                arrow_l,
                entry->options[value],
                arrow_r);
        } else {
            if(value > entry->value_min) {
                arrow_l = '<';
            }
            if(value < entry->value_max) {
                arrow_r = '>';
            }

            snprintf(line_string, sizeof(line_string), "%c %ld %c", arrow_l, value, arrow_r);
        }
        elements_multiline_text_aligned(canvas, 85, pos_y, AlignCenter, AlignTop, line_string);
        pos_y += 10;
    }
}

static bool gpio_test_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    GpioTest* gpio_test = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyUp) {
            consumed = gpio_test_process_up(gpio_test);
        } else if(event->key == InputKeyDown) {
            consumed = gpio_test_process_down(gpio_test);
        }
    } else if(event->type == InputTypePress) {
        if(event->key == InputKeyRight) {
            gpio_test->button_held = true;
            gpio_test->button_ticks = 0;
            gpio_test->button = event->key;
            consumed = gpio_test_process_right(gpio_test);
        } else if(event->key == InputKeyLeft) {
            gpio_test->button_held = true;
            gpio_test->button_ticks = 0;
            gpio_test->button = event->key;
            consumed = gpio_test_process_left(gpio_test);
        }
    } else if(event->type == InputTypeRelease) {
        if(event->key == InputKeyRight) {
            gpio_test->button_held = false;
        } else if(event->key == InputKeyLeft) {
            gpio_test->button_held = false;
        }
    } else if(event->key == InputKeyOk) {
        consumed = gpio_test_process_ok(gpio_test, event);
    }

    return consumed;
}

static void gpio_menu_refresh(GpioTestModel* model) {
    int loops = 0;
    do {
        model->menu_entries_changed = false;

        for(int pos = 0; pos < MENU_ENTRY_COUNT; pos++) {
            if(model->menu[pos].update_cbr) {
                bool prev = model->menu_entries_changed;
                model->menu[pos].update_cbr(model, pos);
                if(model->menu_entries_changed != prev) {
                    FURI_LOG_D(
                        "menu", "gpio_menu_refresh %s changed settings", model->menu[pos].name);
                }
            }
        }
        loops++;
    } while(model->menu_entries_changed && loops < 10);
}

static bool gpio_test_process_up(GpioTest* gpio_test) {
    with_view_model(
        gpio_test->view,
        GpioTestModel * model,
        {
            for(int pos = model->menu_entry - 1; pos >= 0; pos--) {
                if(model->menu_entry_enabled[pos]) {
                    model->menu_entry = pos;
                    break;
                }
            }
        },
        true);
    return true;
}

static bool gpio_test_process_down(GpioTest* gpio_test) {
    with_view_model(
        gpio_test->view,
        GpioTestModel * model,
        {
            for(int pos = model->menu_entry + 1; pos < MENU_ENTRY_COUNT; pos++) {
                if(model->menu_entry_enabled[pos]) {
                    model->menu_entry = pos;
                    break;
                }
            }
        },
        true);
    return true;
}

static void gpio_test_value_change(GpioTestModel* model, int32_t step) {
    int32_t pos = model->menu_entry;
    int32_t val = gpio_menu_value_get(model, pos);

    MenuEntry* menu_entry = &model->menu[pos];

    /* if the menu entry has options */
    if(menu_entry->options[0]) {
        if(step > 0 && menu_entry->options[val + 1]) {
            gpio_menu_value_set(model, pos, val + 1);
        } else if(step < 0 && val > 0) {
            gpio_menu_value_set(model, pos, val - 1);
        }
    } else {
        int32_t new_value = MAX(menu_entry->value_min, MIN(menu_entry->value_max, val + step));
        gpio_menu_value_set(model, pos, new_value);
    }
    gpio_menu_refresh(model);
}

static bool gpio_test_process_left(GpioTest* gpio_test) {
    with_view_model(
        gpio_test->view, GpioTestModel * model, { gpio_test_value_change(model, -1); }, true);
    return true;
}

static bool gpio_test_process_right(GpioTest* gpio_test) {
    with_view_model(
        gpio_test->view, GpioTestModel * model, { gpio_test_value_change(model, 1); }, true);
    return true;
}

static bool gpio_test_process_ok(GpioTest* gpio_test, InputEvent* event) {
    bool consumed = false;

    with_view_model(
        gpio_test->view,
        GpioTestModel * model,
        {
            int32_t pin = gpio_menu_value_get(model, MenuGpio);

            if(event->type == InputTypePress) {
                if(model->pin_mode[pin] == MenuModeOutput) {
                    gpio_item_set_pin(pin, true);
                } else if(model->pin_mode[pin] == MenuModeOutputToggle) {
                    gpio_item_set_pin(pin, !gpio_item_get_pin(pin));
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                if(model->pin_mode[pin] == MenuModeOutput) {
                    gpio_item_set_pin(pin, false);
                }
                consumed = true;
            }
            gpio_test->callback(event->type, gpio_test->context);
        },
        true);

    return consumed;
}

void gpio_config_pin(GpioTestModel* model, uint8_t pin_index) {
    model->pin_mode[pin_index] = gpio_menu_value_get(model, MenuMode);
    model->pin_in_config[pin_index] = gpio_menu_value_get(model, MenuInputConfig);
    model->pin_out_config[pin_index] = gpio_menu_value_get(model, MenuOutputConfig);

    switch(model->pin_mode[pin_index]) {
    case MenuModeInput: {
        int32_t mode = model->pin_in_config[pin_index];
        GpioPull pull[] = {GpioPullNo, GpioPullUp, GpioPullDown};
        gpio_item_configure_pin(pin_index, GpioModeInput, pull[mode]);
        break;
    }
    case MenuModeOutput:
    case MenuModeOutputToggle:
    case MenuModeSignal: {
        int32_t mode = model->pin_out_config[pin_index];
        GpioMode modes[] = {GpioModeOutputPushPull, GpioModeOutputOpenDrain};
        gpio_item_configure_pin(pin_index, modes[mode], GpioPullNo);
        break;
    }
    }
}

static void gpio_menu_gpio_cbr(void* _model, uint32_t entry) {
    UNUSED(entry);
    GpioTestModel* model = _model;

    if(model->menu_entry != MenuGpio) {
        return;
    }

    uint8_t pin_index = gpio_menu_value_get(model, MenuGpio);
    gpio_menu_value_set(model, MenuMode, model->pin_mode[pin_index]);
    gpio_menu_value_set(model, MenuInputConfig, model->pin_in_config[pin_index]);
    gpio_menu_value_set(model, MenuOutputConfig, model->pin_out_config[pin_index]);
    gpio_menu_value_set(model, MenuSignal, model->pin_signal[pin_index]);
}

static void gpio_menu_config_cbr(void* _model, uint32_t entry) {
    UNUSED(entry);
    GpioTestModel* model = _model;

    gpio_config_pin(model, gpio_menu_value_get(model, MenuGpio));
}

static void gpio_menu_mode_cbr(void* _model, uint32_t entry) {
    GpioTestModel* model = _model;

    switch(gpio_menu_value_get(model, entry)) {
    case MenuModeInput:
        gpio_menu_enable(model, MenuInputConfig, true);
        gpio_menu_enable(model, MenuOutputConfig, false);
        gpio_menu_enable(model, MenuSignal, false);
        break;
    case MenuModeOutput:
    case MenuModeOutputToggle:
        gpio_menu_enable(model, MenuInputConfig, false);
        gpio_menu_enable(model, MenuOutputConfig, true);
        gpio_menu_enable(model, MenuSignal, false);
        break;
    case MenuModeSignal:
        gpio_menu_enable(model, MenuInputConfig, false);
        gpio_menu_enable(model, MenuOutputConfig, true);
        gpio_menu_enable(model, MenuSignal, true);
        break;
    default:
        gpio_menu_enable(model, MenuInputConfig, false);
        gpio_menu_enable(model, MenuOutputConfig, false);
        gpio_menu_enable(model, MenuSignal, false);
        break;
    }

    gpio_config_pin(model, gpio_menu_value_get(model, MenuGpio));
}

static void gpio_menu_ws2812_cbr(void* _model, uint32_t entry) {
    UNUSED(entry);
    GpioTestModel* model = _model;

    model->ws2812_led = gpio_menu_value_get(model, MenuWs2812Led);
    model->ws2812_mode = gpio_menu_value_get(model, MenuWs2812);

    gpio_test_ws2812_update(model);
}

static void gpio_menu_signal_cbr(void* _model, uint32_t entry) {
    GpioTestModel* model = _model;
    uint8_t pin_index = gpio_menu_value_get(model, MenuGpio);

    /* this menu must be enabled for the other menu to be shown */
    if(model->pin_mode[pin_index] != MenuModeSignal) {
        gpio_menu_enable(model, MenuWs2812, false);
        gpio_menu_enable(model, MenuWs2812Led, false);
        gpio_menu_enable(model, MenuPwmFreq, false);
        gpio_menu_enable(model, MenuPwmDuty, false);
        gpio_menu_enable(model, MenuServoFreq, false);
        gpio_menu_enable(model, MenuServoMinPulse, false);
        gpio_menu_enable(model, MenuServoMaxPulse, false);
        gpio_menu_enable(model, MenuServoAngle, false);
        return;
    }

    if(!gpio_menu_enabled(model, entry)) {
        return;
    }

    model->pin_signal[pin_index] = gpio_menu_value_get(model, entry);

    switch(model->pin_signal[pin_index]) {
    case MenuSignalWs2812:
        gpio_menu_enable(model, MenuWs2812, true);
        gpio_menu_enable(model, MenuWs2812Led, true);
        gpio_menu_enable(model, MenuPwmFreq, false);
        gpio_menu_enable(model, MenuPwmDuty, false);
        gpio_menu_enable(model, MenuServoFreq, false);
        gpio_menu_enable(model, MenuServoMinPulse, false);
        gpio_menu_enable(model, MenuServoMaxPulse, false);
        gpio_menu_enable(model, MenuServoAngle, false);
        break;
    case MenuSignalPwm:
        gpio_menu_enable(model, MenuWs2812, false);
        gpio_menu_enable(model, MenuWs2812Led, false);
        gpio_menu_enable(model, MenuPwmFreq, true);
        gpio_menu_enable(model, MenuPwmDuty, true);
        gpio_menu_enable(model, MenuServoFreq, false);
        gpio_menu_enable(model, MenuServoMinPulse, false);
        gpio_menu_enable(model, MenuServoMaxPulse, false);
        gpio_menu_enable(model, MenuServoAngle, false);
        model->pwm_updated = true;
        break;
    case MenuSignalServo:
        gpio_menu_enable(model, MenuWs2812, false);
        gpio_menu_enable(model, MenuWs2812Led, false);
        gpio_menu_enable(model, MenuPwmFreq, false);
        gpio_menu_enable(model, MenuPwmDuty, false);
        gpio_menu_enable(model, MenuServoFreq, true);
        gpio_menu_enable(model, MenuServoMinPulse, true);
        gpio_menu_enable(model, MenuServoMaxPulse, true);
        gpio_menu_enable(model, MenuServoAngle, true);
        model->pwm_updated = true;
        break;
    default:
        gpio_menu_enable(model, MenuWs2812, false);
        gpio_menu_enable(model, MenuWs2812Led, false);
        gpio_menu_enable(model, MenuPwmFreq, false);
        gpio_menu_enable(model, MenuPwmDuty, false);
        gpio_menu_enable(model, MenuServoFreq, false);
        gpio_menu_enable(model, MenuServoMinPulse, false);
        gpio_menu_enable(model, MenuServoMaxPulse, false);
        gpio_menu_enable(model, MenuServoAngle, false);
        break;
    }

    gpio_test_ws2812_update(model);
    gpio_test_pwm_update(model);
}

static void gpio_menu_pwm_cbr(void* _model, uint32_t entry) {
    GpioTestModel* model = _model;

    if(!gpio_menu_enabled(model, entry)) {
        return;
    }

    if(model->pwm_freq != gpio_menu_value_get(model, MenuPwmFreq)) {
        model->pwm_freq = gpio_menu_value_get(model, MenuPwmFreq);
        model->pwm_updated = true;
    }
    if(model->pwm_duty != gpio_menu_value_get(model, MenuPwmDuty) * 100) {
        model->pwm_duty = gpio_menu_value_get(model, MenuPwmDuty) * 100;
        model->pwm_updated = true;
    }

    gpio_test_pwm_update(model);
}

static void gpio_menu_servo_cbr(void* _model, uint32_t entry) {
    GpioTestModel* model = _model;

    if(!gpio_menu_enabled(model, entry)) {
        return;
    }
    int32_t freqs[] = {50, 100, 200};
    int32_t min_pulse[] = {500, 1000};
    int32_t max_pulse[] = {2000, 2500};

    int32_t freq = freqs[gpio_menu_value_get(model, MenuServoFreq)];
    int32_t angle = gpio_menu_value_get(model, MenuServoAngle);
    int32_t min = min_pulse[gpio_menu_value_get(model, MenuServoMinPulse)];
    int32_t max = max_pulse[gpio_menu_value_get(model, MenuServoMaxPulse)];

    int32_t span = max - min;
    int32_t pulse_length = min + (span * angle) / 18;
    int32_t duty = (pulse_length * freq) / 100;

    if(model->pwm_freq != freq) {
        model->pwm_freq = freq;
        model->pwm_updated = true;
    }
    if(model->pwm_duty != duty) {
        model->pwm_duty = duty;
        model->pwm_updated = true;
    }

    gpio_test_pwm_update(model);
}

static void gpio_test_pwm_update(GpioTestModel* model) {
    if(furi_mutex_acquire(model->mutex, 0) != FuriStatusOk) {
        return;
    }
    bool pwm_used = false;

    for(uint32_t pin_index = 0; pin_index < GPIO_ITEM_COUNT; pin_index++) {
        if(model->pin_signal[pin_index] == MenuSignalPwm ||
           model->pin_signal[pin_index] == MenuSignalServo) {
            pwm_used = true;
            advanced_pwm_add_gpio(model->pwm_ctx, gpio_item_get_gpiopin(pin_index));
        }
    }

    if(!pwm_used) {
        if(model->pwm_started) {
            FURI_LOG_D("PWM", "not used, disabling generator");
            advanced_pwm_stop(model->pwm_ctx);
            advanced_pwm_reset_gpio(model->pwm_ctx);
            model->pwm_started = false;
        }
    } else if(!model->pwm_started) {
        FURI_LOG_D("PWM", "not started yet, starting");

        if(model->pwm_freq == 0 || model->pwm_duty == 0) {
            FURI_LOG_D(
                "PWM", "starting with %ld %ld -> zero, aborting", model->pwm_freq, model->pwm_duty);
        } else {
            advanced_pwm_set_frequency(model->pwm_ctx, model->pwm_freq, model->pwm_duty);
            advanced_pwm_start(model->pwm_ctx);
            model->pwm_started = true;
        }
    } else {
        FURI_LOG_D("PWM", "update with %ld %ld", model->pwm_freq, model->pwm_duty);
        advanced_pwm_set_frequency(model->pwm_ctx, model->pwm_freq, model->pwm_duty);
    }

    model->pwm_updated = false;
    furi_mutex_release(model->mutex);
}

static void modulate_colors(GpioTestModel* model) {
    uint8_t led_mode = model->ws2812_mode;
    uint32_t channels = (led_mode != 1) ? 3 : 4;
    int offset = (channels == 3) ? 120 : 90;

    model->ws2812_time += 0.1f;
    if(model->ws2812_time >= 2 * M_PI) {
        model->ws2812_time -= 2 * M_PI;
    }

    float t = model->ws2812_time;
    int r = (int)(sinf(t) * 128 + 128);
    if(channels == 3) {
        int g = (int)(sinf(t + offset * M_PI / 180) * 127 + 128);
        int b = (int)(sinf(t + 2 * offset * M_PI / 180) * 127 + 128);
        model->ws2812_color = (r << 16) | (g << 8) | b;
    } else {
        int g = (int)(sinf(t + offset * M_PI / 180) * 127 + 128);
        int b = (int)(sinf(t + 2 * offset * M_PI / 180) * 127 + 128);
        int a = (int)(sinf(t + 3 * offset * M_PI / 180) * 127 + 128);
        model->ws2812_color = (r << 24) | (g << 16) | (b << 8) | a;
    }
}

static void gpio_test_ws2812_update(GpioTestModel* model) {
    uint8_t led_mode = model->ws2812_mode;
    uint32_t led = model->ws2812_led;
    uint32_t channels = (led_mode != 1) ? 3 : 4;

    if(furi_mutex_acquire(model->mutex, 0) != FuriStatusOk) {
        return;
    }

    if(model->signal_color->edge_cnt != 2 * channels * 8) {
        model->signal_off->edge_cnt = 0;

        for(uint32_t pos = 0; pos < channels * 8; pos++) {
            digital_signal_append(model->signal_off, model->signal_bit_0);
        }
    }

    model->signal_color->edge_cnt = 0;
    for(uint32_t pos = 0; pos < channels * 8; pos++) {
        uint32_t bit_val = 1 << (channels * 8 - pos - 1);
        DigitalSignal* sig = (model->ws2812_color & bit_val) ? model->signal_bit_1 :
                                                               model->signal_bit_0;

        digital_signal_append(model->signal_color, sig);
    }

    for(uint32_t pin_index = 0; pin_index < GPIO_ITEM_COUNT; pin_index++) {
        if(model->pin_mode[pin_index] != MenuModeSignal) {
            continue;
        }
        if(model->pin_signal[pin_index] != MenuSignalWs2812) {
            continue;
        }
        DigitalSequence* seq =
            digital_sequence_alloc(MAX_WS2812 + 10, gpio_item_get_gpiopin(pin_index));

        digital_sequence_set_signal(seq, 0, model->signal_off);
        digital_sequence_set_signal(seq, 1, model->signal_color);

        for(uint32_t pos = 0; pos < MAX_WS2812 + 1; pos++) {
            digital_sequence_add(seq, ((pos + 1) == led) ? 1 : 0);
        }

        digital_sequence_send(seq);

        digital_sequence_free(seq);
    }
    furi_mutex_release(model->mutex);
}

GpioTest* gpio_test_alloc() {
    GpioTest* gpio_test = malloc(sizeof(GpioTest));

    gpio_test->view = view_alloc();
    view_allocate_model(gpio_test->view, ViewModelTypeLocking, sizeof(GpioTestModel));
    view_set_context(gpio_test->view, gpio_test);
    view_set_draw_callback(gpio_test->view, gpio_test_draw_callback);
    view_set_input_callback(gpio_test->view, gpio_test_input_callback);

    /* set default values */
    GpioTestModel* model = view_get_model(gpio_test->view);

    model->mutex = furi_mutex_alloc(FuriMutexTypeNormal);

    model->pwm_ctx = advanced_pwm_alloc();
    advanced_pwm_set_polarity(model->pwm_ctx, true);

    model->menu_entry = 0;
    model->menu = gpio_menu_entries;
    model->signal_bit_0 = digital_signal_alloc(4);
    model->signal_bit_1 = digital_signal_alloc(4);
    model->signal_reset = digital_signal_alloc(4);
    model->signal_off = digital_signal_alloc(2 * 8 * 8);
    model->signal_color = digital_signal_alloc(2 * 8 * 8);

    model->signal_reset->start_level = true;
    model->signal_reset->edge_cnt = 2;
    model->signal_reset->edge_timings[0] = DIGITAL_SIGNAL_US(1);
    model->signal_reset->edge_timings[1] = DIGITAL_SIGNAL_US(50);

    model->signal_bit_0->start_level = true;
    model->signal_bit_0->edge_cnt = 2;
    model->signal_bit_0->edge_timings[0] = DIGITAL_SIGNAL_NS(350);
    model->signal_bit_0->edge_timings[1] = DIGITAL_SIGNAL_NS(700);

    model->signal_bit_1->start_level = true;
    model->signal_bit_1->edge_cnt = 2;
    model->signal_bit_1->edge_timings[0] = DIGITAL_SIGNAL_NS(700);
    model->signal_bit_1->edge_timings[1] = DIGITAL_SIGNAL_NS(600);

    for(int32_t pos = 0; pos < MENU_ENTRY_COUNT; pos++) {
        model->menu_entry_enabled[pos] = true;

        int32_t value_min = 0;
        if(!model->menu[pos].options[0]) {
            value_min = model->menu[pos].value_min;
        }
        model->menu_entry_value[pos] = value_min;
    }
    gpio_menu_refresh(model);

    view_commit_model(gpio_test->view, false);

    return gpio_test;
}

void gpio_test_free(GpioTest* gpio_test) {
    furi_assert(gpio_test);

    GpioTestModel* model = view_get_model(gpio_test->view);
    furi_mutex_free(model->mutex);
    advanced_pwm_free(model->pwm_ctx);
    digital_signal_free(model->signal_bit_0);
    digital_signal_free(model->signal_bit_1);
    digital_signal_free(model->signal_reset);
    digital_signal_free(model->signal_color);
    digital_signal_free(model->signal_off);

    view_free(gpio_test->view);
    free(gpio_test);
}

View* gpio_test_get_view(GpioTest* gpio_test) {
    furi_assert(gpio_test);
    return gpio_test->view;
}

void gpio_test_set_ok_callback(GpioTest* gpio_test, GpioTestOkCallback callback, void* context) {
    furi_assert(gpio_test);
    furi_assert(callback);
    with_view_model(
        gpio_test->view,
        GpioTestModel * model,
        {
            UNUSED(model);
            gpio_test->callback = callback;
            gpio_test->context = context;
        },
        false);
}

void gpio_test_update_callback(GpioTest* gpio_test) {
    furi_assert(gpio_test);
    with_view_model(
        gpio_test->view,
        GpioTestModel * model,
        {
            bool found_ws2812 = false;

            for(int pin = 0; pin < GPIO_ITEM_COUNT; pin++) {
                if(model->pin_mode[pin] == MenuModeSignal) {
                    found_ws2812 = true;
                }
            }

            if(found_ws2812) {
                modulate_colors(model);
                gpio_test_ws2812_update(model);
            }

            gpio_test_pwm_update(model);

            /* update when button is held */
            if(gpio_test->button_held) {
                gpio_test->button_ticks++;

                /* every 2nd tick, increase value */
                if((gpio_test->button_ticks % 2) == 0) {
                    int32_t exp = gpio_test->button_ticks / 20;
                    int32_t delta = 1;

                    /* a simple integer based pow, increasing step size every 10 counts */
                    for(int cnt = 0; cnt < exp; cnt++) {
                        delta *= 10;
                    }

                    if(gpio_test->button == InputKeyRight) {
                        gpio_test_value_change(model, delta);
                    } else if(gpio_test->button == InputKeyLeft) {
                        gpio_test_value_change(model, -delta);
                    }
                }
            }
        },
        true);
}
