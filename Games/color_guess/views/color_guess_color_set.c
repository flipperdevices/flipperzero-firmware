#include "../color_guess.h"
//#include "color_guess_color_set.h"
#include "color_guess_icons.h"
#include "../digits.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

struct ColorGuessColorSet {
    View* view;
    ColorGuessColorSetCallback callback;
    void* context;
};

typedef struct {
    ColorGuessColorSetStatus status;

    int cursorpos;
    int digit[6];
    int red;
    int green;
    int blue;
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
    //UNUSED(model);
    const int cursorOffset = 30;
    const int newCursorPos = (model->cursorpos * 12) + cursorOffset;
    
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 7, "random Text");
    
    canvas_draw_icon(canvas, newCursorPos, 18, &I_ButtonUp_10x5);
    canvas_draw_icon(canvas, newCursorPos, 41, &I_ButtonDown_10x5);
    canvas_draw_icon(canvas, 18, 25, digits[16]);
    canvas_draw_icon(canvas, 30, 25, digits[model->digit[0]]);
    canvas_draw_icon(canvas, 42, 25, digits[model->digit[1]]);
    canvas_draw_icon(canvas, 54, 25, digits[model->digit[2]]);
    canvas_draw_icon(canvas, 66, 25, digits[model->digit[3]]);
    canvas_draw_icon(canvas, 78, 25, digits[model->digit[4]]);
    canvas_draw_icon(canvas, 90, 25, digits[model->digit[5]]);
}

static void color_guess_color_set_model_init(ColorGuessColorSetModel* const model) {
    //model->scene = 0;
    model->cursorpos = 0;
    model->red = 0;
    for (int i = 0;i < 6; i++) {
        model->digit[i] = 0;
    }
    model->green = 0;
    model->blue = 0;
}

bool color_guess_color_set_input(InputEvent* event, void* context) {
    furi_assert(context);
    //UNUSED(context);
    ColorGuessColorSet* instance = context;
    UNUSED(instance);
    //UNUSED(event);
    if (event->type == InputTypeRelease) {
        switch(event->key) {
            case InputKeyBack:
            //if(event->key == InputKeyBack && event->type == InputTypeRelease) {
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
            //} else if (event->key == InputKeyLeft && event->type == InputTypeRelease) {
                with_view_model(
                    instance->view,
                    ColorGuessColorSetModel* model,
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
                    ColorGuessColorSetModel* model,
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
                    ColorGuessColorSetModel* model,
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
                    ColorGuessColorSetModel* model,
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
    //ColorGuessColorSet* instance = context;


    return true;
}

void color_guess_color_set_exit(void* context) {
    furi_assert(context);
    /*ColorGuessColorSet* instance = context;

    with_view_model(instance->view, ColorGuessColorSetModel * model, {
        if (model->status != 1) {
            model->status = 1;
        }
    }, true);*/
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
        {
            //model->status = 1;
            color_guess_color_set_model_init(model);
        },
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

