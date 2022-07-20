#include "gpio_i2c_scanner.h"
#include "../gpio_item.h"

#include <gui/elements.h>

struct GpioI2CScanner {
    View* view;
    GpioI2CScannerOkCallback callback;
    void* context;
};

typedef struct {
    uint8_t pin_idx;
} GpioTestModel;

static bool gpio_i2c_scanner_process_left(GpioI2CScanner* gpio_test);
static bool gpio_i2c_scanner_process_right(GpioI2CScanner* gpio_test);
static bool gpio_i2c_scanner_process_ok(GpioI2CScanner* gpio_test, InputEvent* event);

static void gpio_i2c_scanner_draw_callback(Canvas* canvas, void* _model) {
    GpioTestModel* model = _model;
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "I2C-Scanner");
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 64, 16, AlignCenter, AlignTop, "Press < or > to change pin");
    elements_multiline_text_aligned(
        canvas, 64, 32, AlignCenter, AlignTop, gpio_item_get_pin_name(model->pin_idx));
}

static bool gpio_i2c_scanner_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    GpioI2CScanner* gpio_test = context;
    bool consumed = false;

    if(event->type == InputTypeShort) {
        if(event->key == InputKeyRight) {
            consumed = gpio_i2c_scanner_process_right(gpio_test);
        } else if(event->key == InputKeyLeft) {
            consumed = gpio_i2c_scanner_process_left(gpio_test);
        }
    } else if(event->key == InputKeyOk) {
        consumed = gpio_i2c_scanner_process_ok(gpio_test, event);
    }

    return consumed;
}

static bool gpio_i2c_scanner_process_left(GpioI2CScanner* gpio_test) {
    with_view_model(
        gpio_test->view, (GpioTestModel * model) {
            if(model->pin_idx) {
                model->pin_idx--;
            }
            return true;
        });
    return true;
}

static bool gpio_i2c_scanner_process_right(GpioI2CScanner* gpio_test) {
    with_view_model(
        gpio_test->view, (GpioTestModel * model) {
            if(model->pin_idx < GPIO_ITEM_COUNT) {
                model->pin_idx++;
            }
            return true;
        });
    return true;
}

static bool gpio_i2c_scanner_process_ok(GpioI2CScanner* gpio_test, InputEvent* event) {
    bool consumed = false;

    with_view_model(
        gpio_test->view, (GpioTestModel * model) {
            if(event->type == InputTypePress) {
                if(model->pin_idx < GPIO_ITEM_COUNT) {
                    gpio_item_set_pin(model->pin_idx, true);
                } else {
                    gpio_item_set_all_pins(true);
                }
                consumed = true;
            } else if(event->type == InputTypeRelease) {
                if(model->pin_idx < GPIO_ITEM_COUNT) {
                    gpio_item_set_pin(model->pin_idx, false);
                } else {
                    gpio_item_set_all_pins(false);
                }
                consumed = true;
            }
            gpio_test->callback(event->type, gpio_test->context);
            return true;
        });

    return consumed;
}

GpioI2CScanner* gpio_i2c_scanner_alloc() {
    GpioI2CScanner* gpio_test = malloc(sizeof(GpioI2CScanner));

    gpio_test->view = view_alloc();
    view_allocate_model(gpio_test->view, ViewModelTypeLocking, sizeof(GpioTestModel));
    view_set_context(gpio_test->view, gpio_test);
    view_set_draw_callback(gpio_test->view, gpio_i2c_scanner_draw_callback);
    view_set_input_callback(gpio_test->view, gpio_i2c_scanner_input_callback);

    return gpio_test;
}

void gpio_i2c_scanner_free(GpioI2CScanner* gpio_test) {
    furi_assert(gpio_test);
    view_free(gpio_test->view);
    free(gpio_test);
}

View* gpio_i2c_scanner_get_view(GpioI2CScanner* gpio_test) {
    furi_assert(gpio_test);
    return gpio_test->view;
}

void gpio_i2c_scanner_set_ok_callback(GpioI2CScanner* gpio_test, GpioI2CScannerOkCallback callback, void* context) {
    furi_assert(gpio_test);
    furi_assert(callback);
    with_view_model(
        gpio_test->view, (GpioTestModel * model) {
            UNUSED(model);
            gpio_test->callback = callback;
            gpio_test->context = context;
            return false;
        });
}
