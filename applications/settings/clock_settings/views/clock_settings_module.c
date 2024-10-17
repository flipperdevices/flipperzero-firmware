#include "clock_settings_module.h"

#include <gui/elements.h>
#include <assets_icons.h>
#include <locale/locale.h>

struct ClockSettingsModule {
    View* view;
};

typedef struct {
    DateTime datetime;
    bool enabled;
    bool editing;

    uint8_t row;
    uint8_t column;
} ClockSettingsModuleViewModel;

#define CLOCK_X             (64u - 33u)
#define CLOCK_Y             (10u)
#define CLOCK_WIDTH         (30u)
#define CLOCK_HEIGHT        (22u)
#define CLOCK_HOUR_X        (CLOCK_X)
#define CLOCK_HOUR_Y        (CLOCK_Y)
#define CLOCK_SPACING       (6u)
#define CLOCK_MINUTE_X      (CLOCK_HOUR_X + CLOCK_WIDTH + CLOCK_SPACING)
#define CLOCK_MINUTE_Y      (CLOCK_Y)
#define CLOCK_MINUTE_WIDTH  (CLOCK_WIDTH)
#define CLOCK_MINUTE_HEIGHT (CLOCK_HEIGHT)

#define ONOFF_X      (64u - ONOFF_WIDTH / 2u)
#define ONOFF_Y      (40u)
#define ONOFF_WIDTH  (24u)
#define ONOFF_HEIGHT (14u)

static void clock_settings_module_draw_callback(Canvas* canvas, void* _model) {
    ClockSettingsModuleViewModel* model = _model;
    char buffer[64];

    // Clock
    canvas_set_font(canvas, FontBigNumbers);

    if(model->row == 0 && model->column == 0) {
        if(model->editing) {
            canvas_draw_icon(
                canvas, CLOCK_HOUR_X + CLOCK_WIDTH / 2 - 3, CLOCK_HOUR_Y - 2 - 3, &I_ButtonUp_7x4);
            canvas_draw_icon(
                canvas,
                CLOCK_HOUR_X + CLOCK_WIDTH / 2 - 3,
                CLOCK_HOUR_Y + CLOCK_HEIGHT + 2,
                &I_ButtonDown_7x4);
        }
        canvas_draw_rbox(canvas, CLOCK_HOUR_X, CLOCK_HOUR_Y, CLOCK_WIDTH, CLOCK_HEIGHT, 2);
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_draw_rframe(canvas, CLOCK_HOUR_X, CLOCK_HOUR_Y, CLOCK_WIDTH, CLOCK_HEIGHT, 2);
    }
    snprintf(buffer, sizeof(buffer), "%02d", model->datetime.hour);
    canvas_draw_str_aligned(
        canvas,
        CLOCK_HOUR_X + CLOCK_WIDTH / 2,
        CLOCK_HOUR_Y + CLOCK_HEIGHT / 2,
        AlignCenter,
        AlignCenter,
        buffer);

    canvas_set_color(canvas, ColorBlack);
    if(model->row == 0 && model->column == 1) {
        if(model->editing) {
            canvas_draw_icon(
                canvas,
                CLOCK_MINUTE_X + CLOCK_WIDTH / 2 - 3,
                CLOCK_MINUTE_Y - 2 - 3,
                &I_SmallArrowUp_3x5);
            canvas_draw_icon(
                canvas,
                CLOCK_MINUTE_X + CLOCK_WIDTH / 2 - 3,
                CLOCK_MINUTE_Y + CLOCK_HEIGHT + 2,
                &I_SmallArrowDown_3x5);
        }
        canvas_draw_rbox(
            canvas, CLOCK_MINUTE_X, CLOCK_MINUTE_Y, CLOCK_MINUTE_WIDTH, CLOCK_MINUTE_HEIGHT, 2);
        canvas_set_color(canvas, ColorWhite);
    } else {
        canvas_draw_rframe(
            canvas, CLOCK_MINUTE_X, CLOCK_MINUTE_Y, CLOCK_MINUTE_WIDTH, CLOCK_MINUTE_HEIGHT, 2);
    }
    snprintf(buffer, sizeof(buffer), "%02d", model->datetime.minute);
    canvas_draw_str_aligned(
        canvas,
        CLOCK_MINUTE_X + CLOCK_MINUTE_WIDTH / 2,
        CLOCK_MINUTE_Y + CLOCK_MINUTE_HEIGHT / 2,
        AlignCenter,
        AlignCenter,
        buffer);

    // On/Off
    canvas_set_color(canvas, ColorBlack);
    canvas_set_font(canvas, FontPrimary);
    canvas_draw_rframe(canvas, ONOFF_X, ONOFF_Y, ONOFF_WIDTH, ONOFF_HEIGHT, 2);
    if(model->row == 1) {
        canvas_draw_rbox(canvas, ONOFF_X, ONOFF_Y, ONOFF_WIDTH, ONOFF_HEIGHT, 2);
        canvas_set_color(canvas, ColorWhite);
    }
    canvas_draw_str_aligned(
        canvas,
        ONOFF_X + ONOFF_WIDTH / 2,
        ONOFF_Y + ONOFF_HEIGHT / 2,
        AlignCenter,
        AlignCenter,
        model->enabled ? "On" : "Off");
}

static bool clock_settings_module_input_clock_callback(
    InputEvent* event,
    ClockSettingsModuleViewModel* model) {
    if(model->editing) {
        if(event->key == InputKeyUp) {
            if(model->column == 0) {
                model->datetime.hour++;
                model->datetime.hour = model->datetime.hour % 24;
            } else {
                model->datetime.minute++;
                model->datetime.minute = model->datetime.minute % 60;
            }
        } else if(event->key == InputKeyDown) {
            if(model->column == 0) {
                if(model->datetime.hour > 0) {
                    model->datetime.hour--;
                } else {
                    model->datetime.hour = 23;
                }
                model->datetime.hour = model->datetime.hour % 24;
            } else {
                if(model->datetime.minute > 0) {
                    model->datetime.minute--;
                } else {
                    model->datetime.minute = 59;
                }
                model->datetime.minute = model->datetime.minute % 60;
            }
        } else if(event->key == InputKeyOk) {
            model->editing = false;
        } else {
            return false;
        }
    } else {
        if(event->key == InputKeyDown) {
            model->row++;
        } else if(event->key == InputKeyLeft) {
            if(model->column > 0) model->column--;
        } else if(event->key == InputKeyRight) {
            if(model->column < 1) model->column++;
        } else if(event->key == InputKeyOk) {
            model->editing = true;
        } else {
            return false;
        }
    }

    return true;
}

static bool clock_settings_module_input_onoff_callback(
    InputEvent* event,
    ClockSettingsModuleViewModel* model) {
    if(event->key == InputKeyUp) {
        model->row--;
    } else if(event->key == InputKeyOk) {
        model->enabled = !model->enabled;
    } else {
        return false;
    }

    return true;
}

static bool clock_settings_module_input_callback(InputEvent* event, void* context) {
    furi_assert(context);
    ClockSettingsModule* instance = context;
    bool consumed = false;

    with_view_model(
        instance->view,
        ClockSettingsModuleViewModel * model,
        {
            if(event->type == InputTypeShort || event->type == InputTypeRepeat) {
                if(model->row == 0) {
                    consumed = clock_settings_module_input_clock_callback(event, model);
                } else if(model->row == 1) {
                    consumed = clock_settings_module_input_onoff_callback(event, model);
                } else {
                    furi_crash();
                }
            }
        },
        true);

    return consumed;
}

ClockSettingsModule* clock_settings_module_alloc(void) {
    ClockSettingsModule* instance = malloc(sizeof(ClockSettingsModule));

    instance->view = view_alloc();
    view_allocate_model(
        instance->view, ViewModelTypeLocking, sizeof(ClockSettingsModuleViewModel));
    with_view_model(
        instance->view, ClockSettingsModuleViewModel * model, { model->row = 1; }, false);
    view_set_context(instance->view, instance);
    view_set_draw_callback(instance->view, clock_settings_module_draw_callback);
    view_set_input_callback(instance->view, clock_settings_module_input_callback);

    return instance;
}

void clock_settings_module_free(ClockSettingsModule* instance) {
    furi_assert(instance);
    view_free(instance->view);
    free(instance);
}

View* clock_settings_module_get_view(ClockSettingsModule* instance) {
    furi_assert(instance);
    return instance->view;
}

void clock_settings_module_set(
    ClockSettingsModule* instance,
    const DateTime* datetime,
    bool enabled) {
    furi_assert(instance);
    furi_assert(datetime);
    with_view_model(
        instance->view,
        ClockSettingsModuleViewModel * model,
        {
            model->datetime = *datetime;
            model->enabled = enabled;
        },
        true);
}

bool clock_settings_module_get(ClockSettingsModule* instance, DateTime* datetime) {
    furi_assert(instance);
    furi_assert(datetime);

    bool enabled = false;
    with_view_model(
        instance->view,
        ClockSettingsModuleViewModel * model,
        {
            *datetime = model->datetime;
            enabled = model->enabled;
        },
        false);

    return enabled;
}
