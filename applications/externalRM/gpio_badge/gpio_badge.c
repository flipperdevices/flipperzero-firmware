#include <furi.h>
#include <gui/gui.h>

const GpioPin* const pin_leds[] = {
    &gpio_ext_pa7,
    &gpio_ext_pa6,
    &gpio_ext_pa4,
    &gpio_ext_pb3,
    &gpio_ext_pb2,
    &gpio_ext_pc3,
    &gpio_swclk,
    &gpio_swdio,
    &gpio_usart_tx,
    &gpio_usart_rx,
    &gpio_ext_pc1,
    &gpio_ext_pc0,
    &gpio_ibutton,
};

uint32_t speed = 50;

typedef enum {
    EffectIdLRL,
    EffectIdBothLRL,
    EffectIdLR,
    EffectIdCounter,
    EffectIdCount // Make sure this is last
} EffectId;

EffectId effect = EffectIdLR;
bool animating = true;
int lri = 0;

#define MAX_EFFECT 1

const GpioPin* const pin_back = &gpio_button_back;

static void my_draw_callback(Canvas* canvas, void* context) {
    UNUSED(context);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 5, 8, "GPIO BADGE");
    canvas_draw_str(canvas, 5, 28, "Left/Right : Speed");
    canvas_draw_str(canvas, 5, 38, "Up/Down : Effect");
}

static void my_input_callback(InputEvent* event, void* context) {
    UNUSED(context);
    if(event->type == InputTypePress) {
        uint32_t last_speed = speed;
        if(event->key == InputKeyLeft) {
            speed *= 0.9;
            if(speed < 1) {
                speed = 1;
            }
        } else if(event->key == InputKeyRight) {
            speed *= 1.1;
            if(speed < last_speed + 5) {
                speed += 5;
            }
        } else if(event->key == InputKeyUp) {
            if(effect-- == 0) {
                animating = true;
                effect = EffectIdCount - 1;
            }
        } else if(event->key == InputKeyDown) {
            animating = true;
            if(++effect == EffectIdCount) {
                effect = 0;
            }
        } else if(event->key == InputKeyOk) {
            animating = !animating;
        }
    }
}

void do_effect_lrl() {
    int i = 0;
    int d = 1;

    for(size_t c = 0; c < COUNT_OF(pin_leds) * 2; c++) {
        furi_hal_gpio_write(pin_leds[i], true);
        furi_delay_ms(speed);
        furi_hal_gpio_write(pin_leds[i], false);
        i += d;
        if(i == COUNT_OF(pin_leds) || i < 0) {
            if(i < 1)
                i++;
            else
                i--;
            d = d * -1;
        }

        if(furi_hal_gpio_read(pin_back) == false || effect != EffectIdLRL) {
            break;
        }
    }
}

void do_effect_both_lrl() {
    int i = 0;
    int d = 1;

    for(size_t c = 0; c < COUNT_OF(pin_leds) * 2; c++) {
        furi_hal_gpio_write(pin_leds[i], true);
        furi_hal_gpio_write(pin_leds[COUNT_OF(pin_leds) - (i + 1)], true);
        furi_delay_ms(speed);
        furi_hal_gpio_write(pin_leds[i], false);
        furi_hal_gpio_write(pin_leds[COUNT_OF(pin_leds) - (i + 1)], false);
        i += d;
        if(i == COUNT_OF(pin_leds) || i < 0) {
            if(i < 1)
                i++;
            else
                i--;
            d = d * -1;
        }

        if(furi_hal_gpio_read(pin_back) == false || effect != EffectIdBothLRL) {
            break;
        }
    }
}

void do_effect_lr() {
    for(size_t c = 0; c < COUNT_OF(pin_leds) * 2; c++) {
        furi_hal_gpio_write(pin_leds[lri], true);
        furi_delay_ms(speed);
        furi_hal_gpio_write(pin_leds[lri], false);
        if(animating) {
            lri++;
        }
        if(lri == COUNT_OF(pin_leds)) {
            lri = 0;
        }
        if(furi_hal_gpio_read(pin_back) == false || effect != EffectIdLR) {
            break;
        }
    }
}

void do_effect_counter() {
    uint32_t c = 0;

    while(true) {
        for(size_t i = 0; i < COUNT_OF(pin_leds); i++) {
            if((c & (1 << i)) != 0) {
                furi_hal_gpio_write(pin_leds[i], true);
            }
        }
        furi_delay_ms(speed);
        for(size_t i = 0; i < COUNT_OF(pin_leds); i++) {
            furi_hal_gpio_write(pin_leds[i], false);
        }
        c++;

        if(furi_hal_gpio_read(pin_back) == false || effect != EffectIdCounter) {
            break;
        }
    }
}

void do_effects() {
    while(furi_hal_gpio_read(pin_back)) {
        switch(effect) {
        case EffectIdLRL:
            do_effect_lrl();
            break;
        case EffectIdBothLRL:
            do_effect_both_lrl();
            break;
        case EffectIdCounter:
            do_effect_counter();
            break;
        default:
            do_effect_lr();
            break;
        }
    }
}

int gpio_diag_lights_app(void* p) {
    UNUSED(p);

    Gui* gui = furi_record_open(RECORD_GUI);
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, my_draw_callback, NULL);
    view_port_input_callback_set(view_port, my_input_callback, NULL);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    for(size_t i = 0; i < COUNT_OF(pin_leds); i++) {
        furi_hal_gpio_init_simple(pin_leds[i], GpioModeOutputPushPull);
    }

    do_effects();

    // Typically when a pin is no longer in use, it is set to analog mode.
    // but we need to use open drain for the 1Wire pin.
    for(size_t i = 0; i < COUNT_OF(pin_leds); i++) {
        furi_hal_gpio_init_simple(pin_leds[i], GpioModeAnalog);
    }
    furi_hal_gpio_init_simple(&gpio_ibutton, GpioModeOutputOpenDrain);

    // Remove the directions from the screen.
    gui_remove_view_port(gui, view_port);
    return 0;
}