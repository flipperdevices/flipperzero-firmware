#include "../color_guess.h"
#include "color_guess_icons.h"
#include "../helpers/color_guess_colors.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

extern const Icon* digits[17];

struct ColorGuessPlay {
    View* view;
    ColorGuessPlayCallback callback;
    void* context;
};

typedef struct {
    ColorGuessPlayStatus status;
    int cursorpos;
    int digit[6];
    int color;
    int time_spent;
    int timestamp_start;
    int closeness;
    int difficulty;
} ColorGuessPlayModel;

void color_guess_play_set_callback(
    ColorGuessPlay* instance,
    ColorGuessPlayCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void color_guess_play_new_round(void* context, ColorGuessPlayModel* model) {
    furi_assert(context);
    ColorGuess* app = context;
    //Reset timer
    FuriHalRtcDateTime date_time;
    furi_hal_rtc_get_datetime(&date_time);
    model->timestamp_start = furi_hal_rtc_datetime_to_timestamp(&date_time);

    //Set random color
    NotificationMessage notification_led_message_1;
    notification_led_message_1.type = NotificationMessageTypeLedRed;
    NotificationMessage notification_led_message_2;
    notification_led_message_2.type = NotificationMessageTypeLedGreen;
    NotificationMessage notification_led_message_3;
    notification_led_message_3.type = NotificationMessageTypeLedBlue;

    if(model->difficulty == 0) {
        model->color = colorsEasy[rand() % ARR_SIZE(colorsEasy)];
    } else if(model->difficulty == 1) {
        model->color = colorsNormal[rand() % ARR_SIZE(colorsNormal)];
    } else if(model->difficulty == 2) {
        model->color = colorsHard[rand() % ARR_SIZE(colorsHard)];
    }

    notification_led_message_1.data.led.value = ((model->color >> 16) & 0xFF);
    notification_led_message_2.data.led.value = ((model->color >> 8) & 0xFF);
    notification_led_message_3.data.led.value = ((model->color) & 0xFF);

    //model->closeness = ((model->color >> 8) & 0xFF);

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

void color_guess_play_calculate_closeness(void* context, ColorGuessPlayModel* model) {
    UNUSED(context);
    int userRed = (model->digit[0] * 16) + model->digit[1];
    int userGreen = (model->digit[2] * 16) + model->digit[3];
    int userBlue = (model->digit[4] * 16) + model->digit[5];
    int ledRed = ((model->color >> 16) & 0xFF);
    int ledGreen = ((model->color >> 8) & 0xFF);
    int ledBlue = ((model->color) & 0xFF);

    int distanceRed = abs(ledRed - userRed);
    int distanceGreen = abs(ledGreen - userGreen);
    int distanceBlue = abs(ledBlue - userBlue);
    float percentageRed = 100 - ((distanceRed / 255.0) *
                                 100); //make sure one number is float, otherwise C will calc wrong
    float percentageGreen = 100 - ((distanceGreen / 255.0) * 100);
    float percentageBlue = 100 - ((distanceBlue / 255.0) * 100);
    float fullPercentage = (percentageRed + percentageGreen + percentageBlue) / 3;
    model->closeness = round(fullPercentage);
}

void parse_time_str(char* buffer, int32_t sec) {
    //int seconds = sec % 60;
    //int minutes = (sec % (60 * 60)) / 60;
    snprintf(
        buffer,
        TIMER_LENGHT,
        TIMER_FORMAT,
        (sec % (60 * 60)) / 60, // minute
        sec % 60); // second
}

void drawDifficulty(Canvas* canvas, ColorGuessPlayModel* model) {
    UNUSED(model);
    char* strDifficulty = malloc(7);
    if(model->difficulty == 0) {
        strcpy(strDifficulty, "Easy");
    } else if(model->difficulty == 1) {
        strcpy(strDifficulty, "Medium");
    } else if(model->difficulty == 2) {
        strcpy(strDifficulty, "Hard");
    }
    canvas_draw_box(canvas, 0, 52, 47, 12);
    canvas_invert_color(canvas);
    canvas_draw_icon(canvas, 2, 54, &I_ButtonCenter_7x7);
    canvas_draw_str_aligned(canvas, 11, 54, AlignLeft, AlignTop, strDifficulty);
    canvas_invert_color(canvas);
    free(strDifficulty);
    furi_thread_flags_wait(0, FuriFlagWaitAny, 10);
}

void color_guess_play_draw(Canvas* canvas, ColorGuessPlayModel* model) {
    const int cursorOffset = 30;
    const int newCursorPos = (model->cursorpos * 12) + cursorOffset;
    FuriHalRtcDateTime date_time;
    furi_hal_rtc_get_datetime(&date_time);
    uint32_t timestamp = furi_hal_rtc_datetime_to_timestamp(&date_time);
    uint32_t time_elapsed = timestamp - model->timestamp_start;

    char timer_string[TIMER_LENGHT];
    char closeness_string[4];
    UNUSED(time_elapsed);

    //snprintf(timer_string, TIMER_LENGHT, TIMER_FORMAT, date_time.minute, date_time.second);
    parse_time_str(timer_string, time_elapsed);
    snprintf(closeness_string, CLOSENESS_LENGTH, CLOSENESS_FORMAT, model->closeness);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Time spent:");
    canvas_draw_str_aligned(canvas, 55, 0, AlignLeft, AlignTop, timer_string); // DRAW TIMER
    canvas_draw_str_aligned(canvas, 0, 9, AlignLeft, AlignTop, "You are this close:");
    canvas_draw_str_aligned(canvas, 105, 9, AlignLeft, AlignTop, closeness_string);

    canvas_draw_icon(canvas, newCursorPos, 20, &I_ButtonUp_10x5);
    canvas_draw_icon(canvas, newCursorPos, 43, &I_ButtonDown_10x5);
    canvas_draw_icon(canvas, 18, 27, digits[16]);
    canvas_draw_icon(canvas, 30, 27, digits[model->digit[0]]);
    canvas_draw_icon(canvas, 42, 27, digits[model->digit[1]]);
    canvas_draw_icon(canvas, 54, 27, digits[model->digit[2]]);
    canvas_draw_icon(canvas, 66, 27, digits[model->digit[3]]);
    canvas_draw_icon(canvas, 78, 27, digits[model->digit[4]]);
    canvas_draw_icon(canvas, 90, 27, digits[model->digit[5]]);
    elements_button_right(canvas, "Guess this color");
    drawDifficulty(canvas, model);
}

static void color_guess_play_model_init(ColorGuessPlayModel* const model) {
    model->cursorpos = 0;
    for(int i = 0; i < 6; i++) {
        model->digit[i] = 0;
    }
    model->closeness = 0;
    model->difficulty = 1;
}

bool color_guess_play_input(InputEvent* event, void* context) {
    furi_assert(context);
    ColorGuessPlay* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                ColorGuessPlayModel * model,
                {
                    UNUSED(model);
                    instance->callback(ColorGuessCustomEventPlayBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
            with_view_model(
                instance->view,
                ColorGuessPlayModel * model,
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
                ColorGuessPlayModel * model,
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
                ColorGuessPlayModel * model,
                {
                    model->digit[model->cursorpos]++;
                    if(model->digit[model->cursorpos] > 15) {
                        model->digit[model->cursorpos] = 0;
                    }
                    color_guess_play_calculate_closeness(instance, model);
                },
                true);
            break;
        case InputKeyDown:
            with_view_model(
                instance->view,
                ColorGuessPlayModel * model,
                {
                    model->digit[model->cursorpos]--;
                    if(model->digit[model->cursorpos] < 0) {
                        model->digit[model->cursorpos] = 15;
                    }
                    color_guess_play_calculate_closeness(instance, model);
                },
                true);
            break;
        case InputKeyOk:
            with_view_model(
                instance->view,
                ColorGuessPlayModel * model,
                {
                    model->difficulty++;
                    if(model->difficulty > 2) {
                        model->difficulty = 0;
                    }
                    color_guess_play_new_round(instance->context, model);
                },
                true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void color_guess_play_exit(void* context) {
    furi_assert(context);
}

void color_guess_play_enter(void* context) {
    furi_assert(context);
    ColorGuessPlay* instance = (ColorGuessPlay*)context;
    with_view_model(
        instance->view,
        ColorGuessPlayModel * model,
        {
            color_guess_play_model_init(model);
            color_guess_play_new_round(instance->context, model);
        },
        true);
}

ColorGuessPlay* color_guess_play_alloc() {
    ColorGuessPlay* instance = malloc(sizeof(ColorGuessPlay));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(ColorGuessPlayModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)color_guess_play_draw);
    view_set_input_callback(instance->view, color_guess_play_input);
    view_set_enter_callback(instance->view, color_guess_play_enter);
    //view_set_exit_callback(instance->view, color_guess_play_exit);

    with_view_model(
        instance->view, ColorGuessPlayModel * model, { color_guess_play_model_init(model); }, true);

    return instance;
}

void color_guess_play_free(ColorGuessPlay* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view, ColorGuessPlayModel * model, { free(model->digit); }, true);
    view_free(instance->view);
    free(instance);
}

View* color_guess_play_get_view(ColorGuessPlay* instance) {
    furi_assert(instance);
    return instance->view;
}
