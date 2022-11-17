#include "UnitempViews.h"

static View* view;

static void _draw_callback(Canvas* canvas, void* _model) {
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
            if(app->sensors[i]->status != UT_OK && app->sensors[i]->status != UT_EARLYPOOL &&
               app->sensors[i]->status != UT_POLLING) {
                if(app->sensors[i]->status == UT_BADCRC) {
                    canvas_draw_str(canvas, 96, 24 + 10 * i, "bad CRC");
                } else {
                    canvas_draw_str(canvas, 96, 24 + 10 * i, "timeout");
                }
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

static bool _input_callback(InputEvent* event, void* context) {
    Unitemp* app = context;

    //Выход по короткому нажатию "назад"
    if(event->key == InputKeyBack && event->type == InputTypeShort) {
        app->processing = false;
    }
    //Вход в главное меню по короткому нажатию "Ок"
    if(event->key == InputKeyOk && event->type == InputTypeShort) {
        unitemp_MainMenu_switch();
    }

    return true;
}

void unitemp_Summary_alloc(void) {
    view = view_alloc();
    view_set_context(view, app);
    view_set_draw_callback(view, _draw_callback);
    view_set_input_callback(view, _input_callback);

    view_dispatcher_add_view(app->view_dispatcher, SUMMARY_VIEW, view);
}

void unitemp_Summary_switch(void) {
    view_dispatcher_switch_to_view(app->view_dispatcher, SUMMARY_VIEW);
}

void unitemp_Summary_free(void) {
    view_free(view);
}
