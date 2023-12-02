#include "calendar_month_picker.h"
#include <furi.h>
#include <core/check.h>
#include <core/record.h>
#include <core/log.h>

#define NUMBER_OF_MONTHS 12
#define COLUMN_GAP_PX 30
#define ROW_GAP_PX 17
#define GRID_TEMPLATE_COLUMNS 4
#define GRID_TEMPLATE_ROWS 3

struct MonthPicker {
    View* view;
    MonthPickerCallback callback;
    void* context;
    VariableSharedContext* variable_shared_context;
};

typedef struct {
    int16_t grid_cursor;
} MonthPickerViewModel;

static const char* months[NUMBER_OF_MONTHS] =
    {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

static void calendar_month_picker_draw_callback(Canvas* canvas, MonthPickerViewModel* model) {
    furi_assert(canvas);

    canvas_draw_rframe(canvas, 0, 0, 128, 64, 3);

    for(int8_t col = 0; col < GRID_TEMPLATE_COLUMNS; col++) {
        for(int8_t row = 0; row < GRID_TEMPLATE_ROWS; row++) {
            int8_t month = col + row * GRID_TEMPLATE_COLUMNS;
            if(month == model->grid_cursor) {
                canvas_set_font(canvas, FontPrimary);
            } else {
                canvas_set_font(canvas, FontSecondary);
            }
            canvas_draw_str_aligned(
                canvas,
                col * COLUMN_GAP_PX + 64 - COLUMN_GAP_PX * (GRID_TEMPLATE_COLUMNS - 1) / 2,
                row * ROW_GAP_PX + 32 - ROW_GAP_PX * (GRID_TEMPLATE_ROWS - 1) / 2,
                AlignCenter,
                AlignCenter,
                months[month]);
        }
    }
}

static bool calendar_month_picker_input_callback(InputEvent* event, void* context) {
    furi_assert(context);

    MonthPicker* calendar_month_picker = context;

    if(event->key == InputKeyBack || event->type != InputTypeShort) {
        return false;
    }

    bool consumed = false;

    with_view_model(
        calendar_month_picker->view,
        MonthPickerViewModel * model,
        {
            switch(event->key) {
            case InputKeyOk:
                calendar_month_picker->variable_shared_context->month_selected =
                    model->grid_cursor + 1;
                calendar_month_picker->callback(
                    CalendarAppCustomEventMontPicked, calendar_month_picker->context);
                consumed = true;
                break;

            case InputKeyRight:
                if(model->grid_cursor + 1 < NUMBER_OF_MONTHS) {
                    model->grid_cursor++;
                }
                consumed = true;
                break;

            case InputKeyLeft:
                if(model->grid_cursor > 0) {
                    model->grid_cursor--;
                }
                consumed = true;
                break;

            case InputKeyUp:
                if(model->grid_cursor > GRID_TEMPLATE_COLUMNS - 1) {
                    model->grid_cursor -= GRID_TEMPLATE_COLUMNS;
                }
                consumed = true;
                break;

            case InputKeyDown:
                if(model->grid_cursor < GRID_TEMPLATE_COLUMNS * (GRID_TEMPLATE_ROWS - 1)) {
                    model->grid_cursor += GRID_TEMPLATE_COLUMNS;
                }
                consumed = true;
                break;

            default:
                break;
            }
        },
        true);

    return consumed;
}

void calendar_month_picker_enter_callback(void* context) {
    MonthPicker* calendar_month_picker = context;

    with_view_model(
        calendar_month_picker->view,
        MonthPickerViewModel * model,
        {
            model->grid_cursor =
                calendar_month_picker->variable_shared_context->month_selected - 1;
        },
        true);
}

void calendar_month_picker_set_callback(
    MonthPicker* calendar_month_picker,
    MonthPickerCallback callback,
    void* context) {
    furi_assert(calendar_month_picker);
    furi_assert(callback);
    calendar_month_picker->callback = callback;
    calendar_month_picker->context = context;
}

MonthPicker* calendar_month_picker_alloc(VariableSharedContext* variable_shared_context) {
    furi_assert(variable_shared_context);

    MonthPicker* calendar_month_picker = malloc(sizeof(MonthPicker));
    calendar_month_picker->variable_shared_context = variable_shared_context;
    calendar_month_picker->view = view_alloc();

    view_allocate_model(
        calendar_month_picker->view, ViewModelTypeLocking, sizeof(MonthPickerViewModel));
    view_set_context(calendar_month_picker->view, calendar_month_picker);
    view_set_draw_callback(
        calendar_month_picker->view, (ViewDrawCallback)calendar_month_picker_draw_callback);
    view_set_input_callback(calendar_month_picker->view, calendar_month_picker_input_callback);
    view_set_enter_callback(calendar_month_picker->view, calendar_month_picker_enter_callback);

    return calendar_month_picker;
}

void calendar_month_picker_free(MonthPicker* calendar_month_picker) {
    furi_assert(calendar_month_picker);
    view_free(calendar_month_picker->view);
    free(calendar_month_picker);
}

View* calendar_month_picker_get_view(MonthPicker* calendar_month_picker) {
    furi_assert(calendar_month_picker);
    return calendar_month_picker->view;
}