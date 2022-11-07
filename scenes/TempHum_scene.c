#include "Scenes.h"

static View* view;

static void TempHum_draw_callback(Canvas* canvas, void* _model) {
    UNUSED(_model);

    //Рисование бара
    canvas_draw_box(canvas, 0, 0, 128, 14);
    canvas_set_color(canvas, ColorWhite);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 7, AlignCenter, AlignCenter, "Unitemp");

    canvas_set_color(canvas, ColorBlack);
    if(app->sensors_count > 0) {
        for(uint8_t i = 0; i < app->sensors_count; i++) {
            canvas_set_font(canvas, FontPrimary);
            canvas_draw_str(canvas, 0, 24 + 10 * i, app->sensors[i]->name);

            canvas_set_font(canvas, FontSecondary);
            if(app->sensors[i]->status != UT_OK && app->sensors[i]->status != UT_EARLYPOOL) {
                canvas_draw_str(canvas, 96, 24 + 10 * i, "timeout");
            } else {
                char buff[20];
                snprintf(
                    buff,
                    sizeof(buff),
                    "%2.1f*%c/%d%%",
                    (double)app->sensors[i]->temp,
                    app->settings.unit == CELSIUS ? 'C' : 'F',
                    (int8_t)app->sensors[i]->hum);
                canvas_draw_str(canvas, 64, 24 + 10 * i, buff);
            }
        }
    } else {
        canvas_set_font(canvas, FontSecondary);
        if(app->sensors_count == 0) canvas_draw_str(canvas, 0, 24, "Sensors not found");
    }
}
void TempHum_secene_alloc(void) {
    view = view_alloc();
    view_set_context(view, app);
    view_set_draw_callback(view, TempHum_draw_callback);

    view_dispatcher_add_view(app->view_dispatcher, TEMPHUM_VIEW, view);
}

void TempHum_secene_free(void) {
    view_free(view);
}
