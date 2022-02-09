#include "pin_code.h"
#include "input/input.h"

#define PIN_CELL_WIDTH      13

void desktop_views_helpers_pin_code_draw_cells(Canvas* canvas, uint8_t x_center, uint8_t y_center, const PinCode* pin_code, bool hidden, bool finished) {
    furi_assert(canvas);

    uint8_t draw_pin_size = MAX(4, pin_code->length + 1);
    if (finished) {
        draw_pin_size = pin_code->length;
    }

    uint8_t x = x_center - (draw_pin_size * PIN_CELL_WIDTH) / 2;
    uint8_t y = y_center - (PIN_CELL_WIDTH / 2);

    for (int i = 0; i < draw_pin_size; ++i) {
        canvas_draw_box(canvas, x, y, PIN_CELL_WIDTH, PIN_CELL_WIDTH);
        if (i < pin_code->length) {
            if (hidden) {
                canvas_draw_icon(canvas, x + 3, y + 3, &I_Pin_star_7x7);
            } else {
                switch(pin_code->data[i]) {
                case InputKeyDown:
                    canvas_draw_icon(canvas, x + 3, y + 2, &I_Pin_arrow_down_7x9);
                    break;
                case InputKeyUp:
                    canvas_draw_icon(canvas, x + 3, y + 2, &I_Pin_arrow_up7x9);
                    break;
                case InputKeyLeft:
                    canvas_draw_icon(canvas, x + 2, y + 3, &I_Pin_arrow_left_9x7);
                    break;
                case InputKeyRight:
                    canvas_draw_icon(canvas, x + 2, y + 3, &I_Pin_arrow_right_9x7);
                    break;
                default:
                    furi_assert(0);
                    break;
                }
            }
        } else if (i == pin_code->length) {
            canvas_draw_icon(canvas, x + 3, y + PIN_CELL_WIDTH + 2, &I_Pin_pointer_5x3);
        }
        x += PIN_CELL_WIDTH;
    }
}

bool desktop_views_helpers_pin_code_are_equal(PinCode* pin_code1, PinCode* pin_code2) {
    furi_assert(pin_code1);
    furi_assert(pin_code2);
    bool result = false;

    if (pin_code1->length == pin_code2->length) {
        result = !memcmp(pin_code1->data, pin_code2->data, pin_code1->length);
    }

    return result;
}

bool desktop_views_helpers_pin_code_input(InputEvent* event, PinCode* pin_code) {

    bool consumed = false;

    switch(event->key) {
    case InputKeyRight:
    case InputKeyLeft:
    case InputKeyDown:
    case InputKeyUp:
        if (pin_code->length < MAX_PIN_SIZE) {
            pin_code->data[pin_code->length++] = event->key;
        }
        consumed = true;
        break;
    case InputKeyOk:
        if (pin_code->length < MIN_PIN_SIZE) {
            consumed = true;
        }
        break;
    case InputKeyBack:
        if (pin_code->length > 0) {
            pin_code->length = 0;
            consumed = true;
        }
        break;
    default:
        furi_assert(0);
        break;
    }

    return consumed;
}


