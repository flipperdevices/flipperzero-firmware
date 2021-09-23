#include <furi.h>
#include <gui/gui.h>

#include "../flipper_world_i.h"
#include "flipper_world_passport.h"

static const char* mood_strings[MOODS_TOTAL] = {[0] = "Happy", [1] = "Ok", [2] = "Bad"};

static const Icon* portrait_happy[BUTTHURT_MAX] = {
    &I_passport_happy1_43x45,
    &I_passport_happy2_43x45,
    &I_passport_happy3_43x45};
static const Icon* portrait_ok[BUTTHURT_MAX] = {
    &I_passport_okay1_43x45,
    &I_passport_okay2_43x45,
    &I_passport_okay3_43x45};
static const Icon* portrait_bad[BUTTHURT_MAX] = {
    &I_passport_bad1_43x45,
    &I_passport_bad2_43x45,
    &I_passport_bad3_43x45};

static const Icon** portraits[MOODS_TOTAL] = {portrait_happy, portrait_ok, portrait_bad};

void flipper_passport_render(Canvas* canvas, FlipperPassportViewModel* model) {
    furi_assert(model);
    furi_assert(canvas);
    char level[20];
    char mood[32];

    canvas_clear(canvas);
    // multipass
    canvas_draw_icon(canvas, 0, 0, &I_PassportLeft_6x47);
    canvas_draw_icon(canvas, 0, 47, &I_PassportBottom_128x17);
    canvas_draw_line(canvas, 6, 0, 125, 0);
    canvas_draw_line(canvas, 127, 2, 127, 47);
    canvas_draw_dot(canvas, 126, 1);

    //portrait frame
    canvas_draw_line(canvas, 9, 6, 9, 53);
    canvas_draw_line(canvas, 10, 5, 52, 5);
    canvas_draw_line(canvas, 55, 8, 55, 53);
    canvas_draw_line(canvas, 10, 54, 54, 54);
    canvas_draw_line(canvas, 53, 5, 55, 7);

    // portrait
    canvas_draw_icon(canvas, 10, 9, portraits[model->butthurt][model->portrait_level]);
    canvas_draw_line(canvas, 59, 18, 124, 18);
    canvas_draw_line(canvas, 59, 31, 124, 31);
    canvas_draw_line(canvas, 59, 44, 124, 44);

    canvas_draw_str(canvas, 59, 15, model->name ? model->name : "Unknown");

    snprintf(level, 20, "Level: %ld", model->current_level);
    snprintf(mood, 20, "Mood: %s", mood_strings[model->butthurt]);

    canvas_draw_str(canvas, 59, 28, mood);

    canvas_draw_str(canvas, 59, 41, level);
    canvas_set_color(canvas, ColorWhite);
    canvas_draw_box(canvas, 123 - model->exp, 48, model->exp + 1, 6);
    canvas_set_color(canvas, ColorBlack);
    canvas_draw_line(canvas, 123 - model->exp, 48, 123 - model->exp, 54);
}

void flipper_world_passport_set_callback(
    FlipperPassportView* passport,
    FlipperPassportViewCallback callback,
    void* context) {
    furi_assert(passport);
    furi_assert(callback);
    passport->callback = callback;
    passport->context = context;
}

View* flipper_world_passport_get_view(FlipperPassportView* passport) {
    furi_assert(passport);
    return passport->view;
}

bool flipper_passport_input(InputEvent* event, void* context) {
    furi_assert(event);
    furi_assert(context);

    FlipperPassportView* passport = context;

    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        passport->callback(FlipperPassportEventExit, passport->context);
    }

    return true;
}

FlipperPassportView* flipper_world_passport_alloc() {
    FlipperPassportView* passport = furi_alloc(sizeof(FlipperPassportView));
    passport->view = view_alloc();

    view_allocate_model(passport->view, ViewModelTypeLocking, sizeof(FlipperPassportViewModel));
    view_set_context(passport->view, passport);
    view_set_draw_callback(passport->view, (ViewDrawCallback)flipper_passport_render);
    view_set_input_callback(passport->view, flipper_passport_input);

    return passport;
}

void flipper_world_passport_free(FlipperPassportView* passport) {
    furi_assert(passport);
    view_free(passport->view);
    free(passport);
}
