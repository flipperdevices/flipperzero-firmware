#include "../color_guess.h"
#include "color_guess_icons.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

extern const Icon* digits[17];

struct ColorGuessColorSet {
    View* view;
    ColorGuessColorSetCallback callback;
    void* context;
};

typedef struct {
    ColorGuessColorSetStatus status;
    int cursorpos;
    int digit[6];
} ColorGuessColorSetModel;

void color_guess_color_set_set_callback(
    ColorGuessColorSet* instance,
    ColorGuessColorSetCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void color_guess_color_set_draw(Canvas* canvas, ColorGuessColorSetModel* model) {
    const int cursorOffset = 30;
    const int newCursorPos = (model->cursorpos * 12) + cursorOffset;

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 7, "Set a custom color on LED");

    canvas_draw_icon(canvas, newCursorPos, 18, &I_ButtonUp_10x5);
    canvas_draw_icon(canvas, newCursorPos, 41, &I_ButtonDown_10x5);
    canvas_draw_icon(canvas, 18, 25, digits[16]);
    canvas_draw_icon(canvas, 30, 25, digits[model->digit[0]]);
    canvas_draw_icon(canvas, 42, 25, digits[model->digit[1]]);
    canvas_draw_icon(canvas, 54, 25, digits[model->digit[2]]);
    canvas_draw_icon(canvas, 66, 25, digits[model->digit[3]]);
    canvas_draw_icon(canvas, 78, 25, digits[model->digit[4]]);
    canvas_draw_icon(canvas, 90, 25, digits[model->digit[5]]);
    elements_button_right(canvas, "See your color here");
}

static void color_guess_color_set_model_init(ColorGuessColorSetModel* const model) {
    model->cursorpos = 0;
    for(int i = 0; i < 6; i++) {
        model->digit[i] = 0;
    }
}

void color_guess_color_set_set_led(void* context, ColorGuessColorSetModel* model) {
    furi_assert(context);
    ColorGuess* app = context;
    NotificationMessage notification_led_message_1;
    NotificationMessage notification_led_message_2;
    NotificationMessage notification_led_message_3;
    notification_led_message_1.type = NotificationMessageTypeLedRed;
    notification_led_message_1.data.led.value = (model->digit[0] * 16) + model->digit[1];
    notification_led_message_2.type = NotificationMessageTypeLedGreen;
    notification_led_message_2.data.led.value = (model->digit[2] * 16) + model->digit[3];
    notification_led_message_3.type = NotificationMessageTypeLedBlue;
    notification_led_message_3.data.led.value = (model->digit[4] * 16) + model->digit[5];
    const NotificationSequence notification_sequence = {
        &notification_led_message_1,
        &notification_led_message_2,
        &notification_led_message_3,
        &message_do_not_reset,
        NULL,
    };
    notification_message(app->notification, &notification_sequence);
    furi_thread_flags_wait(
        0, FuriFlagWaitAny, 10); //Delay, prevent removal from RAM before LED value set
}

bool color_guess_color_set_input(InputEvent* event, void* context) {
    furi_assert(context);
    ColorGuessColorSet* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                ColorGuessColorSetModel * model,
                {
                    UNUSED(model);
                    instance->callback(ColorGuessCustomEventColorSetBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                ColorGuessColorSetModel * model,
                {
                    model->cursorpos--;
                    if(model->cursorpos < 0) {
                        model->cursorpos = 5;
                    }
                },
                true);
            break;
        case InputKeyRight:
            with_view_model(
                instance->view,
                ColorGuessColorSetModel * model,
                {
                    model->cursorpos++;
                    if(model->cursorpos > 5) {
                        model->cursorpos = 0;
                    }
                },
                true);
            break;
        case InputKeyUp:
            with_view_model(
                instance->view,
                ColorGuessColorSetModel * model,
                {
                    model->digit[model->cursorpos]++;
                    if(model->digit[model->cursorpos] > 15) {
                        model->digit[model->cursorpos] = 0;
                    }
                    color_guess_color_set_set_led(instance->context, model);
                    //instance->callback(ColorGuessCustomEventColorSetUp, instance->context);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                ColorGuessColorSetModel * model,
                {
                    model->digit[model->cursorpos]--;
                    if(model->digit[model->cursorpos] < 0) {
                        model->digit[model->cursorpos] = 15;
                    }
                    color_guess_color_set_set_led(instance->context, model);
                    //instance->callback(ColorGuessCustomEventColorSetUp, instance->context);
                },
                true);
            break;
        case InputKeyOk:
        case InputKeyMAX:
            break;
        }
    }

    return true;
}

void color_guess_color_set_exit(void* context) {
    furi_assert(context);
}

void color_guess_color_set_enter(void* context) {
    furi_assert(context);
    DOLPHIN_DEED(DolphinDeedPluginStart);
    // ColorGuessColorSet* instance = context;
}

ColorGuessColorSet* color_guess_color_set_alloc() {
    ColorGuessColorSet* instance = malloc(sizeof(ColorGuessColorSet));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ColorGuessColorSetModel));
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, (ViewDrawCallback)color_guess_color_set_draw);
    view_set_input_callback(instance->view, color_guess_color_set_input);
    //view_set_enter_callback(instance->view, color_guess_color_set_enter);
    //view_set_exit_callback(instance->view, color_guess_color_set_exit);

    with_view_model(
        instance->view,
        ColorGuessColorSetModel * model,
        { color_guess_color_set_model_init(model); },
        true);

    return instance;
}

void color_guess_color_set_free(ColorGuessColorSet* instance) {
    furi_assert(instance);

    view_free(instance->view);
    free(instance);
}

View* color_guess_color_set_get_view(ColorGuessColorSet* instance) {
    furi_assert(instance);

    return instance->view;
}
