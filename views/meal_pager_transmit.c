#include "../meal_pager_i.h"
#include <furi.h>
#include <furi_hal.h>
#include <input/input.h>
#include <gui/elements.h>
#include <dolphin/dolphin.h>

const char* const pager_type_text_long[4] = {
    "Retekess T119",
    "Retekess TD157",
    "Retekess TD165",
    "Retekess TD174",
};

struct Meal_PagerTransmit {
    View* view;
    Meal_PagerTransmitCallback callback;
    void* context;
};

typedef struct {
    uint32_t pager_type;
    uint32_t station;
    uint32_t pager;
    uint32_t sending;
} Meal_PagerTransmitModel;

void meal_pager_transmit_set_callback(
    Meal_PagerTransmit* instance,
    Meal_PagerTransmitCallback callback,
    void* context) {
    furi_assert(instance);
    furi_assert(callback);
    instance->callback = callback;
    instance->context = context;
}

void meal_pager_transmit_draw(Canvas* canvas, Meal_PagerTransmitModel* model) {
    canvas_clear(canvas);
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    //char* test = "";
    //snprintf(test, 20, "%lu", model->pager_type);
    char stationText[20] = "";
    char pagerText[20] = "";
    snprintf(stationText, 20, "Station: %lu", model->station);
    snprintf(pagerText, 20, "Pager: %lu", model->pager);
    canvas_draw_str_aligned(
        canvas, 0, 10, AlignLeft, AlignTop, pager_type_text_long[model->pager_type]);
    canvas_set_font(canvas, FontSecondary);
    if(model->sending == 0) {
        canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, "Generating Data");
        canvas_draw_str_aligned(canvas, 0, 32, AlignLeft, AlignTop, stationText);
        canvas_draw_str_aligned(canvas, 0, 42, AlignLeft, AlignTop, pagerText);
    } else {
        canvas_draw_str_aligned(canvas, 0, 22, AlignLeft, AlignTop, "Sending Data");
    }
}

static void meal_pager_transmit_model_init(Meal_PagerTransmitModel* const model) {
    FURI_LOG_D(TAG, "Scene 1 Model Init");
    model->pager_type = 0;
    model->station = 0;
    model->pager = 0;
    model->sending = 0;
}

void meal_pager_transmit_model_set_type(Meal_PagerTransmit* instance, uint32_t type) {
    furi_assert(instance);
    Meal_PagerTransmitModel* model = view_get_model(instance->view);
    model->pager_type = type;
    view_commit_model(instance->view, false);
}

void meal_pager_transmit_model_set_station(Meal_PagerTransmit* instance, uint32_t station) {
    furi_assert(instance);
    Meal_PagerTransmitModel* model = view_get_model(instance->view);
    model->station = station;
    view_commit_model(instance->view, false);
    with_view_model(
        instance->view, Meal_PagerTransmitModel * model, { UNUSED(model); }, true);
}

void meal_pager_transmit_model_set_sending(Meal_PagerTransmit* instance, uint32_t value) {
    furi_assert(instance);
    Meal_PagerTransmitModel* model = view_get_model(instance->view);
    model->sending = value;
    view_commit_model(instance->view, false);
    with_view_model(
        instance->view, Meal_PagerTransmitModel * model, { UNUSED(model); }, true);
}

void meal_pager_transmit_model_set_pager(Meal_PagerTransmit* instance, uint32_t pager) {
    furi_assert(instance);
    Meal_PagerTransmitModel* model = view_get_model(instance->view);
    model->pager = pager;
    view_commit_model(instance->view, false);
    with_view_model(
        instance->view, Meal_PagerTransmitModel * model, { UNUSED(model); }, true);
}

bool meal_pager_transmit_input(InputEvent* event, void* context) {
    furi_assert(context);
    Meal_PagerTransmit* instance = context;
    if(event->type == InputTypeRelease) {
        switch(event->key) {
        case InputKeyBack:
            with_view_model(
                instance->view,
                Meal_PagerTransmitModel * model,
                {
                    UNUSED(model);
                    instance->callback(Meal_PagerCustomEventTransmitBack, instance->context);
                },
                true);
            break;
        case InputKeyLeft:
        case InputKeyRight:
        case InputKeyUp:
        case InputKeyDown:
        case InputKeyOk:
            with_view_model(
                instance->view, Meal_PagerTransmitModel * model, { UNUSED(model); }, true);
            break;
        case InputKeyMAX:
            break;
        }
    }
    return true;
}

void meal_pager_transmit_exit(void* context) {
    furi_assert(context);
    FURI_LOG_D(TAG, "Scene 1 Exit");
}

void meal_pager_transmit_enter(void* context) {
    FURI_LOG_D(TAG, "Scene 1 Enter");
    furi_assert(context);
    Meal_PagerTransmit* instance = (Meal_PagerTransmit*)context;
    with_view_model(
        instance->view, Meal_PagerTransmitModel * model, { UNUSED(model); }, true);
}

Meal_PagerTransmit* meal_pager_transmit_alloc(void* context) {
    FURI_LOG_D(TAG, "Scene 1 Alloc");
    furi_assert(context);
    Meal_PagerTransmit* instance = malloc(sizeof(Meal_PagerTransmit));
    instance->view = view_alloc();
    view_allocate_model(instance->view, ViewModelTypeLocking, sizeof(Meal_PagerTransmitModel));
    view_set_context(instance->view, instance); // furi_assert crashes in events without this
    view_set_draw_callback(instance->view, (ViewDrawCallback)meal_pager_transmit_draw);
    view_set_input_callback(instance->view, meal_pager_transmit_input);
    view_set_enter_callback(instance->view, meal_pager_transmit_enter);
    view_set_exit_callback(instance->view, meal_pager_transmit_exit);

    with_view_model(
        instance->view,
        Meal_PagerTransmitModel * model,
        {
            meal_pager_transmit_model_init(model);
            //meal_pager_transmit_model_set_type(instance, 0);
        },
        true);

    return instance;
}

void meal_pager_transmit_free(Meal_PagerTransmit* instance) {
    FURI_LOG_D(TAG, "Transmit Free");
    furi_assert(instance);

    with_view_model(
        instance->view, Meal_PagerTransmitModel * model, { UNUSED(model); }, true);
    view_free(instance->view);
    free(instance);
}

View* meal_pager_transmit_get_view(Meal_PagerTransmit* instance) {
    furi_assert(instance);
    return instance->view;
}
