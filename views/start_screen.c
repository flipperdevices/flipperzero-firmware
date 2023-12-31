#include "start_screen.h"
#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <input/input.h>

#include <furi.h>
#include "minesweeper_icons.h"

struct StartScreen {
    View* view;
    void* context;
    StartScreenTimerCallback timer_callback;
    StartScreenInputCallback input_callback;
    StartScreenDrawCallback secondary_draw_callback;

    FuriTimer* timer;
    uint32_t timer_period_in_ms;
    bool timer_enabled;
};

typedef struct {
    const char* text;
    Font font;
    uint8_t x, y;
    Align horizontal, vertical;
} TextElement;

typedef struct {
    IconAnimation* animation;
    uint8_t x, y;
} IconElement;

typedef struct {
    TextElement text1;
    TextElement text2;
    TextElement text3;
    IconElement icon;
} StartScreenModel;

void start_screen_view_enter(void* context) {
    furi_assert(context);
    StartScreen* start_screen = context;

    with_view_model(
            start_screen->view, StartScreenModel * model, {icon_animation_start(model->icon.animation);}, true);
}

void start_screen_view_exit(void* context) {
    furi_assert(context);
    StartScreen* start_screen = context;

    with_view_model(
            start_screen->view, StartScreenModel * model, {icon_animation_start(model->icon.animation);}, false);
}

void start_screen_view_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(canvas);
    furi_assert(_model);
    StartScreenModel* model = _model;

    // Prepare canvas
    canvas_clear(canvas);

    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 0, 0, canvas_width(canvas), canvas_height(canvas));

    if (model->icon.animation != NULL) {
        //canvas_draw_icon(canvas, model->icon.x, model->icon.y, &A_StartScreen_128x64); 
        canvas_draw_icon_animation(canvas, model->icon.x, model->icon.y, model->icon.animation);
    }

    if (model->text1.text != NULL) {
        canvas_set_font(canvas, model->text1.font);
        elements_multiline_text_aligned(
            canvas,
            model->text1.x,
            model->text1.y,
            model->text1.horizontal,
            model->text1.vertical,
            model->text1.text);
    }

    if (model->text2.text != NULL) {
        canvas_set_font(canvas, model->text2.font);
        elements_multiline_text_aligned(
            canvas,
            model->text2.x,
            model->text2.y,
            model->text2.horizontal,
            model->text2.vertical,
            model->text2.text);

    }

    if (model->text3.text != NULL) {
        canvas_set_font(canvas, model->text3.font);
        elements_multiline_text_aligned(
            canvas,
            model->text3.x,
            model->text3.y,
            model->text3.horizontal,
            model->text3.vertical,
            model->text3.text);

    }
}

bool start_screen_view_input_callback(InputEvent* event, void* context) {
    StartScreen* start_screen = context;
    bool consumed = false;

    // If custom input callback is set pass event to it, otherwise return false 
    if (start_screen->input_callback) {
        start_screen->input_callback(event, start_screen->context); 
        consumed = true;
    }

    return consumed;
}

void start_screen_timer_callback(void* context) {
    furi_assert(context);
    StartScreen* start_screen = context;

    if (start_screen->timer_callback) {
        start_screen->timer_callback(context);
    }
}


//static void start_screen_start_timer(void* context) {
//    furi_assert(context);
//    StartScreen* start_screen = context; 
//
//    if (start_screen->timer_enabled) {
//        uint32_t timer_period = 
//            start_screen->timer_period_in_ms / (1000.0f / furi_kernel_get_tick_frequency());
//        if (timer_period == 0) timer_period = 1;
//
//        if (furi_timer_start(start_screen->timer, timer_period) != FuriStatusOk) {
//            furi_crash();
//        }
//    }
//}

StartScreen* start_screen_alloc() {
    StartScreen* start_screen = (StartScreen*)malloc(sizeof(StartScreen));
    
    start_screen->view = view_alloc();

    start_screen->timer = furi_timer_alloc(start_screen_timer_callback, FuriTimerTypeOnce, start_screen);
    furi_assert(start_screen->timer);

    // This is if we want to enable timer functionality 
    start_screen->timer_period_in_ms = 1000;
    start_screen->timer_enabled = false;

    view_set_context(start_screen->view, start_screen);
    view_allocate_model(start_screen->view, ViewModelTypeLocking, sizeof(StartScreenModel));
    view_set_draw_callback(start_screen->view, start_screen_view_draw_callback);
    view_set_input_callback(start_screen->view, start_screen_view_input_callback);

    // Right now these enter/exit callbacks are being used to start/stop animations
    // We can also set/stop a timer in here
    view_set_enter_callback(start_screen->view, start_screen_view_enter);
    view_set_enter_callback(start_screen->view, start_screen_view_exit);
    
    with_view_model(
        start_screen->view,
        StartScreenModel * model,
        {
            model->text1.x = 0;
            model->text1.y = 0;
            model->text1.text = NULL;
            model->text1.font = FontSecondary;
            model->text1.horizontal = AlignLeft;
            model->text1.vertical = AlignBottom;

            model->text2.x = 0;
            model->text2.y = 0;
            model->text2.text = NULL;
            model->text2.font = FontSecondary;
            model->text2.horizontal = AlignLeft;
            model->text2.vertical = AlignBottom;

            model->text3.x = 0;
            model->text3.y = 0;
            model->text3.text = NULL;
            model->text3.font = FontSecondary;
            model->text3.horizontal = AlignLeft;
            model->text3.vertical = AlignBottom;

            model->icon.x = 10;
            model->icon.y = 0;
            model->icon.animation = icon_animation_alloc(&A_StartScreen_128x64);

            view_tie_icon_animation(start_screen->view, model->icon.animation);
        },
        true);

    return start_screen;
}

void start_screen_free(StartScreen* instance) {
    furi_assert(instance);
    furi_timer_free(instance->timer);
    view_free(instance->view);
    free(instance);
}

void start_screen_reset(StartScreen* instance) {
    furi_assert(instance);

    with_view_model(
        instance->view,
        StartScreenModel * model,
        {
            memset(&model->text1, 0, sizeof(model->text1));
            memset(&model->text1, 0, sizeof(model->text2));
            memset(&model->text1, 0, sizeof(model->text3));
        },
        false);
    instance->timer_callback = NULL;
    instance->input_callback = NULL;
    instance->secondary_draw_callback = NULL;
    instance->timer_enabled = false;
    instance->timer_period_in_ms = 0;
}

View* start_screen_get_view(StartScreen* instance) {
    furi_assert(instance);
    return instance->view;
}

void start_screen_set_timer_callback(StartScreen* instance, StartScreenTimerCallback callback) {
    furi_assert(instance);
    instance->timer_callback = callback;
}

void start_screen_set_input_callback(StartScreen* instance, StartScreenInputCallback callback) {
    furi_assert(instance);
    instance->input_callback = callback;
}

void start_screen_set_secondary_draw_callback(StartScreen* instance, StartScreenDrawCallback callback) {
    furi_assert(instance);
    instance->secondary_draw_callback = callback;
}

void start_screen_set_context(StartScreen* instance, void* context) {
    furi_assert(instance);
    instance->context = context;
}

void start_screen_set_text1(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text) {

    furi_assert(instance);
    with_view_model(
        instance->view,
        StartScreenModel * model,
        {
            model->text1.x = x;
            model->text1.y = y;
            model->text1.horizontal = horizontal;
            model->text1.vertical = vertical;
            model->text1.font = font;
            model->text1.text = text;
        },
        true);
}

void start_screen_set_text2(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text) {

    furi_assert(instance);
    with_view_model(
        instance->view,
        StartScreenModel * model,
        {
            model->text1.x = x;
            model->text1.y = y;
            model->text1.horizontal = horizontal;
            model->text1.vertical = vertical;
            model->text1.font = font;
            model->text1.text = text;
        },
        true);
}

void start_screen_set_text3(
    StartScreen* instance,
    uint8_t x,
    uint8_t y,
    Align horizontal,
    Align vertical,
    Font font,
    const char* text) {

    furi_assert(instance);
    with_view_model(
        instance->view,
        StartScreenModel * model,
        {
            model->text1.x = x;
            model->text1.y = y;
            model->text1.horizontal = horizontal;
            model->text1.vertical = vertical;
            model->text1.font = font;
            model->text1.text = text;
        },
        true);
}

// Icon is hard coded right now, need to change
void start_screen_set_icon_animation(
    StartScreen* instance,
    uint8_t x,
    uint8_t y) {

    furi_assert(instance);
    with_view_model(
        instance->view,
        StartScreenModel * model,
        {
            model->icon.x = x;
            model->icon.y = y;
            model->icon.animation = icon_animation_alloc(&A_StartScreen_128x64);
            view_tie_icon_animation(instance->view, model->icon.animation);
        },
        true);
}

