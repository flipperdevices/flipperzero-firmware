#include "calendar_month_browser.h"
#include <furi.h>
#include <core/check.h>
#include <core/record.h>
#include <core/log.h>
#include <furi_hal_rtc.h>

#define COLUMN_GAP_PX 17
#define ROW_GAP_PX 11
#define GRID_OFFSET_X 3
#define GRID_OFFSET_Y 8
#define GRID_TEMPLATE_COLUMNS 7
#define GRID_TEMPLATE_ROWS 5

struct MonthBrowser {
    View* view;
    VariableSharedContext* variable_shared_context;
};

typedef struct {
    int16_t year_selected;
    int8_t month_selected;
} MonthBrowserViewModel;

static bool is_leap_year(int16_t year) {
    return (year % 4 == 0 && year % 100 != 0) || year % 400 == 0;
}

static int8_t get_days_in_month(int16_t year, int8_t month) {
    int8_t month_days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if(month - 1 == 1) {
        bool leap_days = is_leap_year(year);
        return 28 + leap_days;
    } else {
        return month_days[month - 1];
    }
}

int8_t get_first_day_of_week(int16_t year, int8_t month) {
    int16_t a = (14 - month) / 12;
    int16_t y = year - a;
    int16_t m = month + 12 * a - 2;
    return (1 + y + y / 4 - y / 100 + y / 100 + 31 * m / 12 - 1) % 7;
}

static void calendar_month_browser_draw_callback(Canvas* canvas, MonthBrowserViewModel* model) {
    furi_assert(canvas);

    int8_t days_in_month = get_days_in_month(model->year_selected, model->month_selected);

    int8_t first_day_of_week = get_first_day_of_week(model->year_selected, model->month_selected);

    canvas_draw_str(canvas, 6, 8, "Su   Mo   Tu   We   Th   Fr   Sa");
    canvas_set_font(canvas, FontKeyboard);

    for(int8_t week = 1; week <= GRID_TEMPLATE_ROWS; week++) {
        for(int8_t day_of_week = 1; day_of_week <= GRID_TEMPLATE_COLUMNS; day_of_week++) {
            int8_t day = (week - 1) * GRID_TEMPLATE_COLUMNS + day_of_week - first_day_of_week;

            if(day > days_in_month) continue;

            if(week == 1 && day_of_week <= first_day_of_week) continue;

            char day_str[5] = {0};
            snprintf(day_str, sizeof(day_str), "%d", day);
            canvas_draw_str_aligned(
                canvas,
                GRID_OFFSET_X + day_of_week * COLUMN_GAP_PX,
                GRID_OFFSET_Y + week * ROW_GAP_PX,
                AlignRight,
                AlignBottom,
                day_str);
        }
    }
}

void calendar_month_browser_alloc_enter_callback(void* context) {
    furi_assert(context);

    MonthBrowser* calendar_month_browser = context;

    with_view_model(
        calendar_month_browser->view,
        MonthBrowserViewModel * model,
        {
            model->year_selected = calendar_month_browser->variable_shared_context->year_selected;
            model->month_selected =
                calendar_month_browser->variable_shared_context->month_selected;
        },
        true);
}

MonthBrowser* calendar_month_browser_alloc(VariableSharedContext* variable_shared_context) {
    furi_assert(variable_shared_context);

    MonthBrowser* calendar_month_browser = malloc(sizeof(MonthBrowser));
    calendar_month_browser->variable_shared_context = variable_shared_context;
    calendar_month_browser->view = view_alloc();

    view_allocate_model(
        calendar_month_browser->view, ViewModelTypeLocking, sizeof(MonthBrowserViewModel));
    view_set_context(calendar_month_browser->view, calendar_month_browser);
    view_set_draw_callback(
        calendar_month_browser->view, (ViewDrawCallback)calendar_month_browser_draw_callback);

    view_set_enter_callback(
        calendar_month_browser->view, calendar_month_browser_alloc_enter_callback);

    return calendar_month_browser;
}

void calendar_month_browser_free(MonthBrowser* calendar_month_browser) {
    furi_assert(calendar_month_browser);
    view_free(calendar_month_browser->view);
    free(calendar_month_browser);
}

View* calendar_month_browser_get_view(MonthBrowser* calendar_month_browser) {
    furi_assert(calendar_month_browser);
    return calendar_month_browser->view;
}