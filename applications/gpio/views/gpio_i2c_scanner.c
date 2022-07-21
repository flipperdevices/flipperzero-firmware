#include "gpio_i2c_scanner.h"
#include "../gpio_item.h"

#include <gui/elements.h>

struct GpioI2CScanner {
    View* view;
    GpioI2CScannerOkCallback callback;
    void* context;
};

typedef struct {
    uint8_t items;
    uint8_t responding_address[256];
} GpioI2CScannerModel;

static bool gpio_i2c_scanner_process_left(GpioI2CScanner* gpio_test);
static bool gpio_i2c_scanner_process_right(GpioI2CScanner* gpio_test);
static bool gpio_i2c_scanner_process_ok(GpioI2CScanner* gpio_test, InputEvent* event);

static void gpio_i2c_scanner_draw_callback(Canvas* canvas, void* _model) {
    GpioI2CScannerModel* model = _model;
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 64, 2, AlignCenter, AlignTop, "I2C-Scanner");
    canvas_set_font(canvas, FontSecondary);
    elements_multiline_text_aligned(
        canvas, 64, 16, AlignCenter, AlignTop, "SCL: Pin 16, SDA: Pin 15");
    
    
    char temp_str[18];
    snprintf(temp_str, 18, "Slaves:%u", model->items);
    elements_multiline_text_aligned(
        canvas, 64, 32, AlignCenter, AlignTop, temp_str);
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
        gpio_test->view, (GpioI2CScannerModel * model) {
            UNUSED(model);
            return true;}
            );
    return true;
}

static bool gpio_i2c_scanner_process_right(GpioI2CScanner* gpio_test) {
    with_view_model(
        gpio_test->view, (GpioI2CScannerModel * model) {
            UNUSED(model);
            return true;}
            );
    return true;
}

static bool gpio_i2c_scanner_process_ok(GpioI2CScanner* gpio_test, InputEvent* event) {
    bool consumed = false;
    gpio_test->callback(event->type, gpio_test->context);


    return consumed;
}

GpioI2CScanner* gpio_i2c_scanner_alloc() {
    GpioI2CScanner* gpio_test = malloc(sizeof(GpioI2CScanner));

    gpio_test->view = view_alloc();
    view_allocate_model(gpio_test->view, ViewModelTypeLocking, sizeof(GpioI2CScannerModel));
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
        gpio_test->view, (GpioI2CScannerModel * model) {
            UNUSED(model);
            gpio_test->callback = callback;
            gpio_test->context = context;
            return false;
        });
}

void gpio_i2c_scanner_update_state(GpioI2CScanner* instance, I2CScannerState* st) {
    furi_assert(instance);
    furi_assert(st);

    with_view_model(
        instance->view, (GpioI2CScannerModel * model) {
            model->items = st->items;
            //UNUSED(model);


            for(int i =0; i<model->items; i++)
            {
                model->responding_address[i] = st->responding_address[i];
            }

            return true;
        });
}


