#include "power_battery_low.h"
#include <furi.h>
#include <gui/elements.h>
#include <assets_icons.h>

struct PowerBatteryLow {
    View* view;
    PowerBatteryLowCallback callback;
    void* context;
};

typedef struct {
    uint8_t charge;
} PowerBatteryLowModel;

static void power_battery_low_draw_callback(Canvas* canvas, void* _model) {
    furi_assert(_model);
    PowerBatteryLowModel* model = _model;
    char buff[32];

    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_str_aligned(canvas, 64, 1, AlignCenter, AlignTop, "Low battery");
    canvas_draw_icon(canvas, 0, 18, &I_BatteryBody_52x28);
    canvas_draw_icon(canvas, 16, 25, &I_FaceNopower_29x14);
    elements_bubble(canvas, 54, 17, 70, 30);

    canvas_set_font(canvas, FontSecondary);
    snprintf(buff, sizeof(buff), "Charge me!\n%u%% left", model->charge);
    elements_multiline_text_aligned(canvas, 70, 23, AlignLeft, AlignTop, buff);

    elements_button_center(canvas, "OK");
}

static bool power_battery_low_input_callback(InputEvent* event, void* context) {
    PowerBatteryLow* power_battery_low = context;

    if(event->type == InputTypeShort && (event->key == InputKeyOk || event->key == InputKeyBack)) {
        if(power_battery_low->callback) {
            power_battery_low->callback(power_battery_low->context);
        }
    }

    // Do not let any input through while the alert is shown
    return true;
}

PowerBatteryLow* power_battery_low_alloc(void) {
    PowerBatteryLow* power_battery_low = malloc(sizeof(PowerBatteryLow));

    power_battery_low->view = view_alloc();
    view_allocate_model(
        power_battery_low->view, ViewModelTypeLocking, sizeof(PowerBatteryLowModel));
    view_set_context(power_battery_low->view, power_battery_low);
    view_set_draw_callback(power_battery_low->view, power_battery_low_draw_callback);
    view_set_input_callback(power_battery_low->view, power_battery_low_input_callback);

    return power_battery_low;
}

void power_battery_low_free(PowerBatteryLow* power_battery_low) {
    furi_assert(power_battery_low);
    view_free(power_battery_low->view);
    free(power_battery_low);
}

View* power_battery_low_get_view(PowerBatteryLow* power_battery_low) {
    furi_assert(power_battery_low);
    return power_battery_low->view;
}

void power_battery_low_set_callback(
    PowerBatteryLow* power_battery_low,
    PowerBatteryLowCallback callback,
    void* context) {
    furi_assert(power_battery_low);
    power_battery_low->callback = callback;
    power_battery_low->context = context;
}

void power_battery_low_set_charge(PowerBatteryLow* power_battery_low, uint8_t charge) {
    furi_assert(power_battery_low);
    with_view_model(
        power_battery_low->view, PowerBatteryLowModel * model, { model->charge = charge; }, true);
}
