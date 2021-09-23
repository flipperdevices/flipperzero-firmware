#include "power_off.h"
#include <furi.h>

struct PowerOff {
    View* view;
};

typedef struct {
    uint32_t time_left_sec;
} PowerOffModel;

static void power_off_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(_model);
    PowerOffModel* model = _model;
    char buff[32];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str(canvas, 2, 15, "!!! Low Battery !!!");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 5, 30, "Connect to charger");
    snprintf(buff, sizeof(buff), "Or poweroff in %lds", model->time_left_sec);
    canvas_draw_str(canvas, 5, 42, buff);
}

PowerOff* power_off_alloc() {
    PowerOff* power_off = furi_alloc(sizeof(PowerOff));
    power_off->view = view_alloc();
    view_allocate_model(power_off->view, ViewModelTypeLocking, sizeof(PowerOffModel));
    view_set_draw_callback(power_off->view, power_off_draw_callback);
    return power_off;
}

void power_off_free(PowerOff* power_off) {
    furi_assert(power_off);
    view_free(power_off->view);
    free(power_off);
}

View* power_off_get_view(PowerOff* power_off) {
    furi_assert(power_off);
    return power_off->view;
}

void power_off_set_time_left(PowerOff* power_off, uint8_t time_left) {
    furi_assert(power_off);
    with_view_model(
        power_off->view, (PowerOffModel * model) {
            model->time_left_sec = time_left;
            return true;
        });
}
