#include "findmy_main.h"
#include "../findmy_i.h"

struct FindMyMain {
    View* view;
    FindMyMainCallback callback;
    void* context;
};

typedef struct {
    bool active;
    uint8_t interval;
    FindMyType type;
} FindMyMainModel;

static void findmy_main_draw_callback(Canvas* canvas, void* _model) {
    FindMyMainModel* model = _model;
    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, true);
    canvas_set_font(canvas, FontPrimary);

    canvas_draw_str(canvas, 4, 11, "FindMy Flipper");
    canvas_set_font(canvas, FontSecondary);
    if(model->active) {
        canvas_draw_str(canvas, 4, 49, "Broadcast Active");
        canvas_draw_icon(canvas, 78, 42, &I_Ok_btn_9x9);
    } else {
        canvas_draw_str(canvas, 4, 49, "Broadcast Inactive");
    }
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 21, "Press <- to run in background");
    canvas_set_font(canvas, FontSecondary);
    char interval_str[20];
    snprintf(interval_str, sizeof(interval_str), "Ping Interval: %ds", model->interval);
    canvas_draw_str(canvas, 4, 62, interval_str);
    canvas_set_font(canvas, FontPrimary);
    const char* network_text = "";
    switch(model->type) {
    case FindMyTypeApple:
        network_text = "Apple Network";
        break;
    case FindMyTypeSamsung:
        network_text = "Samsung Network";
        break;
    case FindMyTypeTile:
        network_text = "Tile Network";
        break;
    default:
        break;
    }
    canvas_draw_str(canvas, 4, 32, network_text);
    canvas_draw_icon(canvas, 6 + canvas_string_width(canvas, network_text), 24, &I_Lock_7x8);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 100, 61, "Config");
    canvas_draw_line(canvas, 100, 51, 127, 51);
    canvas_draw_line(canvas, 97, 53, 97, 63);
    canvas_draw_line(canvas, 97, 53, 99, 51);
    canvas_draw_line(canvas, 3, 52, 87, 52);
}

static bool findmy_main_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    FindMyMain* findmy_main = context;
    bool consumed = false;

    if(event->type == InputTypePress) {
        consumed = true;
        FindMyMainEvent cb_event;

        switch(event->key) {
        case InputKeyBack:
            cb_event = FindMyMainEventQuit;
            break;
        case InputKeyOk:
            cb_event = FindMyMainEventToggle;
            break;
        case InputKeyLeft:
            cb_event = FindMyMainEventBackground;
            break;
        case InputKeyRight:
            cb_event = FindMyMainEventConfig;
            break;
        case InputKeyUp:
            cb_event = FindMyMainEventIntervalUp;
            break;
        case InputKeyDown:
            cb_event = FindMyMainEventIntervalDown;
            break;
        default:
            return consumed;
        }

        findmy_main->callback(cb_event, findmy_main->context);
    }

    return consumed;
}

FindMyMain* findmy_main_alloc(FindMy* app) {
    FindMyMain* findmy_main = malloc(sizeof(FindMyMain));

    findmy_main->view = view_alloc();
    view_allocate_model(findmy_main->view, ViewModelTypeLocking, sizeof(FindMyMainModel));
    with_view_model(
        findmy_main->view,
        FindMyMainModel * model,
        {
            model->active = app->state.beacon_active;
            model->interval = app->state.broadcast_interval;
            model->type = app->state.tag_type;
        },
        false);
    view_set_context(findmy_main->view, findmy_main);
    view_set_draw_callback(findmy_main->view, findmy_main_draw_callback);
    view_set_input_callback(findmy_main->view, findmy_main_input_callback);

    return findmy_main;
}

void findmy_main_free(FindMyMain* findmy_main) {
    furi_assert(findmy_main);
    view_free(findmy_main->view);
    free(findmy_main);
}

View* findmy_main_get_view(FindMyMain* findmy_main) {
    furi_assert(findmy_main);
    return findmy_main->view;
}

void findmy_main_set_callback(FindMyMain* findmy_main, FindMyMainCallback callback, void* context) {
    furi_assert(findmy_main);
    furi_assert(callback);
    findmy_main->callback = callback;
    findmy_main->context = context;
}

void findmy_main_update_active(FindMyMain* findmy_main, bool active) {
    furi_assert(findmy_main);
    with_view_model(
        findmy_main->view, FindMyMainModel * model, { model->active = active; }, true);
}

void findmy_main_update_interval(FindMyMain* findmy_main, uint8_t interval) {
    furi_assert(findmy_main);
    with_view_model(
        findmy_main->view, FindMyMainModel * model, { model->interval = interval; }, true);
}

void findmy_main_update_type(FindMyMain* findmy_main, FindMyType type) {
    furi_assert(findmy_main);
    with_view_model(
        findmy_main->view, FindMyMainModel * model, { model->type = type; }, true);
}