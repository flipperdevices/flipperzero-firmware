#include "../color_guess.h"
#include "color_guess_icons.h"
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
    int time_spent;
    int timestamp_start;
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

void color_guess_play_draw(Canvas* canvas, ColorGuessPlayModel* model) {
    const int cursorOffset = 30;
    const int newCursorPos = (model->cursorpos * 12) + cursorOffset;
    const int closeness = 0;
    FuriHalRtcDateTime date_time;
    furi_hal_rtc_get_datetime(&date_time);
    uint32_t timestamp = furi_hal_rtc_datetime_to_timestamp(&date_time);
    uint32_t time_elapsed = timestamp - model->timestamp_start;
    
    char timer_string[TIMER_LENGHT];
    char closeness_string[4];
    UNUSED(time_elapsed);

    //snprintf(timer_string, TIMER_LENGHT, TIMER_FORMAT, date_time.minute, date_time.second);
    parse_time_str(timer_string, time_elapsed);
    snprintf(closeness_string, CLOSENESS_LENGTH, CLOSENESS_FORMAT, closeness);

    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str_aligned(canvas, 0, 0, AlignLeft, AlignTop, "Time spent:"); 
    canvas_draw_str_aligned(canvas, 55, 0, AlignLeft, AlignTop, timer_string); // DRAW TIMER
    canvas_draw_str_aligned(canvas, 0, 9, AlignLeft, AlignTop, "You are this close:"); 
    canvas_draw_str_aligned(canvas, 110, 9, AlignLeft, AlignTop, closeness_string); 
    
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
}

static void color_guess_play_model_init(ColorGuessPlayModel* const model) {
    model->cursorpos = 0;
    for (int i = 0;i < 6; i++) {
        model->digit[i] = 0;
    }
    FuriHalRtcDateTime date_time;
    furi_hal_rtc_get_datetime(&date_time);
    model->timestamp_start = furi_hal_rtc_datetime_to_timestamp(&date_time);
}

bool color_guess_play_input(InputEvent* event, void* context) {
    furi_assert(context); 
    ColorGuessPlay* instance = context;
    if (event->type == InputTypeRelease) {
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
                    ColorGuessPlayModel* model,
                    {
                        model->cursorpos--;
                        if (model->cursorpos < 0)
                        {
                            model->cursorpos = 5;
                        }
                    },
                    true);
                break;
            case InputKeyRight:
                with_view_model(
                    instance->view,
                    ColorGuessPlayModel* model,
                    {
                        model->cursorpos++;
                        if (model->cursorpos > 5)
                        {
                            model->cursorpos = 0;
                        }
                    },
                    true);
                break;
            case InputKeyUp:
                with_view_model(
                    instance->view,
                    ColorGuessPlayModel* model,
                    {
                        model->digit[model->cursorpos]++;
                        if (model->digit[model->cursorpos] > 15) {
                            model->digit[model->cursorpos] = 0;
                        }
                    },
                    true);
                break;
            case InputKeyDown:
                with_view_model(
                    instance->view,
                    ColorGuessPlayModel* model,
                    {
                        model->digit[model->cursorpos]--;
                        if (model->digit[model->cursorpos] < 0) {
                            model->digit[model->cursorpos] = 15;
                        }
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
        },
        true
    );
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
        instance->view,
        ColorGuessPlayModel * model,
        {
            color_guess_play_model_init(model);
        },
        true
    );
    
    return instance;
}

void color_guess_play_free(ColorGuessPlay* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        ColorGuessPlayModel * model,
        {
            free(model->digit);
        },
        true);
    view_free(instance->view);
    free(instance);
}

View* color_guess_play_get_view(ColorGuessPlay* instance) {
    furi_assert(instance);
    return instance->view;
}

