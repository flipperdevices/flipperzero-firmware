#include "weather_station_show.h"
#include "../weather_station_app_i.h"

#include <input/input.h>
#include <gui/elements.h>

struct WeatherStationShow {
    View* view;
    //WeatherStationShowCallback callback;
    //void* context;
};

typedef struct {
    //FuriString* frequency_str;
    //FuriString* preset_str;
    FuriString* key_str;
    //uint8_t show_button;
} WeatherStationShowModel;

// void weather_station_show_set_callback(
//     WeatherStationShow* weather_station_show,
//     WeatherStationShowCallback callback,
//     void* context) {
//     furi_assert(weather_station_show);

//     weather_station_show->callback = callback;
//     weather_station_show->context = context;
// }

void weather_station_show_add_data_to_show(
    WeatherStationShow* weather_station_show,
    const char* key_str) {
    furi_assert(weather_station_show);

    with_view_model(
        weather_station_show->view,
        WeatherStationShowModel * model,
        {
            furi_string_set_str(model->key_str, key_str);
            //furi_string_set_str(model->frequency_str, frequency_str);
            // furi_string_set_str(model->preset_str, preset_str);
            // model->show_button = show_button;
        },
        true);
}

void weather_station_show_draw(Canvas* canvas, WeatherStationShowModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 20, 8, "Weather Station");

    canvas_set_font(canvas, FontKeyboard);
    elements_multiline_text(canvas, 0, 20, furi_string_get_cstr(model->key_str));
    // canvas_draw_str(canvas, 78, 8, furi_string_get_cstr(model->frequency_str));
    // canvas_draw_str(canvas, 113, 8, furi_string_get_cstr(model->preset_str));
    // if(model->show_button) weather_station_show_button_right(canvas, "Send");
}

bool weather_station_show_input(InputEvent* event, void* context) {
    furi_assert(context);
    //WeatherStationShow* weather_station_show = context;

    if(event->key == InputKeyBack) {
        return false;
    }

    return true;
}

void weather_station_show_enter(void* context) {
    furi_assert(context);
}

void weather_station_show_exit(void* context) {
    furi_assert(context);
    WeatherStationShow* weather_station_show = context;
    with_view_model(
        weather_station_show->view,
        WeatherStationShowModel * model,
        { furi_string_reset(model->key_str); },
        true);
}

WeatherStationShow* weather_station_show_alloc() {
    WeatherStationShow* weather_station_show = malloc(sizeof(WeatherStationShow));

    // View allocation and configuration
    weather_station_show->view = view_alloc();
    view_allocate_model(
        weather_station_show->view, ViewModelTypeLocking, sizeof(WeatherStationShowModel));
    view_set_context(weather_station_show->view, weather_station_show);
    view_set_draw_callback(
        weather_station_show->view, (ViewDrawCallback)weather_station_show_draw);
    view_set_input_callback(weather_station_show->view, weather_station_show_input);
    view_set_enter_callback(weather_station_show->view, weather_station_show_enter);
    view_set_exit_callback(weather_station_show->view, weather_station_show_exit);

    with_view_model(
        weather_station_show->view,
        WeatherStationShowModel * model,
        { model->key_str = furi_string_alloc(); },
        true);
    return weather_station_show;
}

void weather_station_show_free(WeatherStationShow* weather_station_show) {
    furi_assert(weather_station_show);

    with_view_model(
        weather_station_show->view,
        WeatherStationShowModel * model,
        { furi_string_free(model->key_str); },
        true);
    view_free(weather_station_show->view);
    free(weather_station_show);
}

View* weather_station_show_get_view(WeatherStationShow* weather_station_show) {
    furi_assert(weather_station_show);
    return weather_station_show->view;
}
