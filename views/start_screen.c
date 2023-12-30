#include "start_screen.h"
#include <gui/elements.h>
#include <gui/icon_animation.h>
#include <furi.h>

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
    uint8_t x, y;
    Align horizontal, vertical;
} TextElement;

typedef struct {
    const Icon* icon;
    uint8_t x, y;
} IconElement;

typedef struct {
    TextElement text1;
    TextElement text2;
    TextElement text3;
    IconAnimation* icon;
} StartScreenModel;

static void start_screen_draw_callback(void* context) {

}

static void start_screen_timer_callback(void* context) {

}

static bool start_screen_input_callback(InputEvent* event, void* context) {
    StartScreen* app = context;
    bool consumed = false;

    // If custom input callback is set use, otherwise default
    if (app->input_callback) {
        app->input_callback(event, context); 
        consumed = true;
    }  else {
        // Do something
    }

}

StartScreen* start_screen_alloc() {
    return NULL;
}

void start_screen_free(StartScreen* instance) {
    UNUSED(instance);
}

void start_screen_reset(StartScreen* instance) {
    UNUSED(instance);

}

View* start_screen_get_view(StartScreen* instance) {
    UNUSED(instance);

    return NULL;
}

void start_screen_set_timer_callback(StartScreen* instance, StartScreenTimerCallback callback) {
    UNUSED(instance);
    UNUSED(callback);

}

void start_screen_set_input_callback(StartScreen* instance, StartScreenInputCallback callback) {
    UNUSED(instance);
    UNUSED(callback);

}

void start_screen_set_secondary_draw_callback(StartScreen* instance, StartScreenDrawCallback callback) {
    UNUSED(instance);
    UNUSED(callback);

}

void start_screen_set_context(StartScreen* instance, void* context) {
    UNUSED(instance);
    UNUSED(context);

}

void start_screen_set_text1(
        StartScreen* instance,
        uint8_t x,
        uint8_t y,
        Align horizontal,
        Align vertical,
        Font font,
        const char* text) {
    UNUSED(instance);
    UNUSED(x);
    UNUSED(y);
    UNUSED(horizontal);
    UNUSED(vertical);
    UNUSED(font);
    UNUSED(text);
}

void start_screen_set_text2(
        StartScreen* instance,
        uint8_t x,
        uint8_t y,
        Align horizontal,
        Align vertical,
        Font font,
        const char* text) {
    UNUSED(instance);
    UNUSED(x);
    UNUSED(y);
    UNUSED(horizontal);
    UNUSED(vertical);
    UNUSED(font);
    UNUSED(text);
}

void start_screen_set_text3(
        StartScreen* instance,
        uint8_t x,
        uint8_t y,
        Align horizontal,
        Align vertical,
        Font font,
        const char* text) {
    UNUSED(instance);
    UNUSED(x);
    UNUSED(y);
    UNUSED(horizontal);
    UNUSED(vertical);
    UNUSED(font);
    UNUSED(text);
}

void start_screen_set_icon_element(
        StartScreen* instance,
        uint8_t x,
        uint8_t y,
        const Icon* icon) {
    UNUSED(instance);
    UNUSED(x);
    UNUSED(y);
    UNUSED(icon);
}
