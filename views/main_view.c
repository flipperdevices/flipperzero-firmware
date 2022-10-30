#include "main_view.h"
#include <furi.h>
#include <furi_hal.h>
#include <gui/elements.h>
// #include <notification/notification.h>
// #include <notification/notification_messages.h>
// #include <furi_hal_uart.h>

// #define MODULE_CONTROL_COMMAND_SEND 's'
// #define FLIPPERZERO_SERIAL_BAUD 115200

// typedef enum ESerialCommand { ESerialCommand_Send } ESerialCommand;

struct MainView {
    View* view;
};

typedef struct {
    bool right_pressed;
    bool connected;
    uint8_t recv[2];
} MainViewModel;

int get_lx() {
    uint8_t value = 0x20;
    uint8_t recv[2];
    furi_hal_i2c_acquire(I2C_BUS);
    uint8_t address = 0x23 << 1;
    furi_hal_i2c_trx(
        I2C_BUS,
        address,
        &value,
        sizeof(value),
        recv,
        sizeof(recv),
        I2C_TIMEOUT);
    furi_hal_i2c_release(I2C_BUS);
    return ((int)recv[0] << 8) | ((int)recv[1]);
}

static void main_view_draw_callback(Canvas* canvas, void* context) {
    furi_assert(context);
    MainViewModel* model = context;

    canvas_clear(canvas);

    // top row
    // draw line
    canvas_draw_line(canvas, 0, 10, 128, 10);

    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "F");

    canvas_draw_str_aligned(canvas, 20, 0, AlignLeft, AlignTop, "ISO: 400");

    char str[10];

    snprintf(str, sizeof(str), "lx: %d", get_lx());

    canvas_draw_str_aligned(canvas, 80, 0, AlignLeft, AlignTop, str);


    // add f, T values
    canvas_set_font(canvas, FontBigNumbers);
    canvas_draw_icon(canvas, 15, 17, &I_f_10x14);
    canvas_draw_str_aligned(canvas, 27, 15, AlignLeft, AlignTop, "/3.2");
    canvas_draw_icon(canvas, 15, 34, &I_T_10x14);
    canvas_draw_str_aligned(canvas, 27, 34, AlignLeft, AlignTop, ":1/50");
    
    // create buttons (for the future)
    canvas_set_font(canvas, FontSecondary);
    elements_button_left(canvas, "Config");
    // elements_button_right(canvas, "Config");

    // draw EV number
    canvas_draw_line(canvas, 95, 15, 95, 33);
    canvas_draw_line(canvas, 96, 15, 96, 33);
    canvas_set_font(canvas, FontPrimary);
    elements_multiline_text_aligned(canvas, 100, 15, AlignLeft, AlignTop, "EV:\n+7");

    // switch(main_view->current_mode) {
    // case FIXED_TIME:
    //     canvas_set_font(canvas, FontBigNumbers);
    //     canvas_draw_str_aligned(canvas, 3, 36, AlignLeft, AlignTop, "*");
    //     break;

    // case FIXED_APERTURE:
    //     canvas_set_font(canvas, FontBigNumbers);
    //     canvas_draw_str_aligned(canvas, 3, 17, AlignLeft, AlignTop, "*");
    //     break;

    // default:
    //     break;
    // }
    

    // Right
    if(model->right_pressed) {
    }
}

static void main_view_process(MainView* main_view, InputEvent* event) {
    with_view_model(
        main_view->view,
        MainViewModel * model,
        {
            if(event->type == InputTypePress) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = true;
                    // Shake();
                    // send_serial_command_send(ESerialCommand_Send);
                } else if(event->key == InputKeyOk) {
                } else if(event->key == InputKeyBack) {
                }
            } else if(event->type == InputTypeRelease) {
                if(event->key == InputKeyUp) {
                } else if(event->key == InputKeyDown) {
                } else if(event->key == InputKeyLeft) {
                } else if(event->key == InputKeyRight) {
                    model->right_pressed = false;
                } else if(event->key == InputKeyOk) {
                } else if(event->key == InputKeyBack) {
                }
            } else if(event->type == InputTypeShort) {
                if(event->key == InputKeyBack) {
                }
            }
        },
        true);
}

static bool main_view_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    MainView* main_view = context;
    bool consumed = false;

    if(event->type == InputTypeShort && event->key == InputKeyBack) {
    } else {
        main_view_process(main_view, event);
        consumed = true;
    }

    return consumed;
}

MainView* main_view_alloc() {
    MainView* main_view = malloc(sizeof(MainView));
    main_view->view = view_alloc();
    view_set_context(main_view->view, main_view);
    view_allocate_model(main_view->view, ViewModelTypeLocking, sizeof(MainViewModel));
    view_set_draw_callback(main_view->view, main_view_draw_callback);
    view_set_input_callback(main_view->view, main_view_input_callback);
    // furi_hal_uart_set_br(FuriHalUartIdUSART1, FLIPPERZERO_SERIAL_BAUD);

    return main_view;
}

void main_view_free(MainView* main_view) {
    furi_assert(main_view);
    view_free(main_view->view);
    free(main_view);
}

View* main_view_get_view(MainView* main_view) {
    furi_assert(main_view);
    return main_view->view;
}

void main_view_set_data(MainView* main_view, bool connected) {
    furi_assert(main_view);
    with_view_model(
        main_view->view, MainViewModel * model, { model->connected = connected; }, true);
}
