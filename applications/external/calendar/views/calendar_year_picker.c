#include "calendar_year_picker.h"
#include <furi.h>
#include <core/check.h>
#include <core/record.h>
#include <core/log.h>

#define COLUMN_GAP_PX 28
#define ROW_GAP_PX 13
#define GRID_TEMPLATE_COLUMNS 4
#define GRID_TEMPLATE_ROWS 4

struct YearPicker {
    View* view;
    YearPickerCallback callback;
    void* context;
    VariableSharedContext* variable_shared_context;
};

typedef struct {
    int16_t grid_cursor;
    int16_t first_display_year;
} YearPickerViewModel;

static int16_t get_first_display_year(int32_t year) {
    return year - year % (GRID_TEMPLATE_COLUMNS * GRID_TEMPLATE_ROWS);
}

static void calendar_year_picker_draw_callback(Canvas* canvas, YearPickerViewModel* model) {
    furi_assert(canvas);

    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);

    char year_str[7] = {0};
    for(int8_t col = 1; col <= GRID_TEMPLATE_COLUMNS; col++) {
        for(int8_t row = 1; row <= GRID_TEMPLATE_ROWS; row++) {
            int16_t year =
                model->first_display_year + (col - 1) + (GRID_TEMPLATE_COLUMNS * (row - 1));

            if(year == model->grid_cursor) {
                canvas_set_font(canvas, FontPrimary);
            } else {
                canvas_set_font(canvas, FontSecondary);
            }

            snprintf(year_str, sizeof(year_str), "%d", year);
            canvas_draw_str_aligned(
                canvas,
                (col - 1) * COLUMN_GAP_PX + 64 - COLUMN_GAP_PX * (GRID_TEMPLATE_COLUMNS - 1) / 2,
                (row - 1) * ROW_GAP_PX + 32 - ROW_GAP_PX * (GRID_TEMPLATE_ROWS - 1) / 2,
                AlignCenter,
                AlignCenter,
                year_str);
        }
    }
}

static bool calendar_year_picker_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    YearPicker* calendar_year_picker = context;

    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    }

    bool consumed = false;

    with_view_model(
        calendar_year_picker->view,
        YearPickerViewModel * model,
        {
            switch(event->key) {
            case InputKeyOk:
                calendar_year_picker->variable_shared_context->year_selected = model->grid_cursor;
                calendar_year_picker->callback(
                    CalendarAppCustomEventYearPicked, calendar_year_picker->context);
                consumed = true;
                break;

            case InputKeyRight:
                if(model->grid_cursor ==
                   model->first_display_year + GRID_TEMPLATE_COLUMNS * GRID_TEMPLATE_ROWS) {
                    model->first_display_year += GRID_TEMPLATE_COLUMNS * GRID_TEMPLATE_ROWS;
                }
                model->grid_cursor++;
                consumed = true;
                break;

            case InputKeyLeft:
                if(model->grid_cursor == model->first_display_year) {
                    model->first_display_year -= GRID_TEMPLATE_COLUMNS * GRID_TEMPLATE_ROWS;
                }
                model->grid_cursor--;
                consumed = true;
                break;

            case InputKeyUp:
                if(model->grid_cursor - model->first_display_year <= GRID_TEMPLATE_COLUMNS) {
                    model->first_display_year -= GRID_TEMPLATE_COLUMNS * GRID_TEMPLATE_ROWS;
                }
                model->grid_cursor -= 4;
                consumed = true;
                break;

            case InputKeyDown:
                if(model->grid_cursor - model->first_display_year >=
                   (GRID_TEMPLATE_ROWS - 1) * GRID_TEMPLATE_COLUMNS) {
                    model->first_display_year += GRID_TEMPLATE_COLUMNS * GRID_TEMPLATE_ROWS;
                }
                model->grid_cursor += 4;
                consumed = true;
                break;

            default:
                break;
            }
        },
        true);

    return consumed;
}

void calendar_year_picker_set_callback(
    YearPicker* calendar_year_picker,
    YearPickerCallback callback,
    void* context) {
    furi_assert(calendar_year_picker);
    furi_assert(callback);
    calendar_year_picker->callback = callback;
    calendar_year_picker->context = context;
}

void calendar_year_picker_enter_callback(void* context) {
    YearPicker* calendar_year_picker = context;

    with_view_model(
        calendar_year_picker->view,
        YearPickerViewModel * model,
        {
            model->grid_cursor = calendar_year_picker->variable_shared_context->year_selected;
            model->first_display_year = get_first_display_year(
                calendar_year_picker->variable_shared_context->year_selected);
        },
        true);
}

YearPicker* calendar_year_picker_alloc(VariableSharedContext* variable_shared_context) {
    furi_assert(variable_shared_context);

    YearPicker* calendar_year_picker = malloc(sizeof(YearPicker));
    calendar_year_picker->variable_shared_context = variable_shared_context;
    calendar_year_picker->view = view_alloc();

    view_allocate_model(
        calendar_year_picker->view, ViewModelTypeLocking, sizeof(YearPickerViewModel));
    view_set_context(calendar_year_picker->view, calendar_year_picker);
    view_set_draw_callback(
        calendar_year_picker->view, (ViewDrawCallback)calendar_year_picker_draw_callback);
    view_set_input_callback(calendar_year_picker->view, calendar_year_picker_input_callback);
    view_set_enter_callback(calendar_year_picker->view, calendar_year_picker_enter_callback);

    return calendar_year_picker;
}

void calendar_year_picker_free(YearPicker* calendar_year_picker) {
    furi_assert(calendar_year_picker);
    view_free(calendar_year_picker->view);
    free(calendar_year_picker);
}

View* calendar_year_picker_get_view(YearPicker* calendar_year_picker) {
    furi_assert(calendar_year_picker);
    return calendar_year_picker->view;
}