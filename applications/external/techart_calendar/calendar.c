#include "calendar_i.h"

typedef struct {
    int selected_month;
    int selected_year;
} CalendarData;

typedef struct {
    FuriMessageQueue* event_queue;
    CalendarData* calendar_data;
} CalendarContext;

int calculateDayOfWeek(int year, int month, int day) {
    // Adjusting month and year for January and February
    if(month < 3) {
        month += 12;
        year -= 1;
    }

    int K = year % 100; // Year of the century
    int J = year / 100; // Zero-based century

    // Zeller's Congruence formula for the Gregorian calendar
    int h = (day + (13 * (month + 1) / 5) + K + (K / 4) + (J / 4) + (5 * J)) % 7;

    // Adjusting Zeller's output to match conventional day numbering (Sunday=0, ..., Saturday=6)
    int dayOfWeek = (h + 6) % 7;

    return dayOfWeek;
}

DateTime createDateTime(uint16_t year, uint8_t month, uint8_t day) {
    DateTime date = {0, 0, 0, day, month, year, 0};

    date.weekday = calculateDayOfWeek(year, month, day);

    return date;
}

static void calendar_draw_callback(Canvas* canvas, void* ctx) {
    CalendarData* calendar_data = ctx;

    canvas_clear(canvas);
    canvas_set_bitmap_mode(canvas, true);
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 4, 8, "S");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 17, 8, "M");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 28, 8, "T");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 41, 8, "W");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 53, 8, "T");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 65, 8, "F");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 77, 8, "S");

    // horizontal lines
    canvas_draw_line(canvas, 2, 8, 84, 8);
    canvas_draw_line(canvas, 2, 17, 84, 17);
    canvas_draw_line(canvas, 2, 26, 84, 26);
    canvas_draw_line(canvas, 2, 35, 84, 35);
    canvas_draw_line(canvas, 2, 44, 84, 44);
    canvas_draw_line(canvas, 2, 53, 84, 53);

    // vertical lines
    canvas_draw_line(canvas, 13, 1, 13, 61);
    canvas_draw_line(canvas, 25, 1, 25, 61);
    canvas_draw_line(canvas, 37, 1, 37, 61);
    canvas_draw_line(canvas, 49, 1, 49, 61);
    canvas_draw_line(canvas, 61, 1, 61, 61);
    canvas_draw_line(canvas, 73, 1, 73, 61);

    DateTime date = createDateTime(calendar_data->selected_year, calendar_data->selected_month, 1);

    int month = date.month;
    int days_in_month = datetime_get_days_per_month(datetime_is_leap_year(date.year), month);

    int first_day_of_month = date.weekday;
    int day_of_month = 1;

    int x = 4;
    int y = 16;
    int i = 0;
    int j = 0;

    for(i = 0; i < 6; i++) {
        for(j = 0; j < 7; j++) {
            if(i == 0 && j < first_day_of_month) {
                x += 12;
                continue;
            }
            if(day_of_month > days_in_month) {
                break;
            }

            canvas_set_font(canvas, FontSecondary);

            FuriString* dayStr;
            dayStr = furi_string_alloc();
            furi_string_printf(dayStr, "%d", day_of_month);

            canvas_draw_str(canvas, x, y, furi_string_get_cstr(dayStr));
            furi_string_free(dayStr);

            day_of_month++;
            x += 12;
        }

        x = 3;
        if(day_of_month > 10) x = 2;

        y += 9;
    }

    FuriString* temp_str;
    temp_str = furi_string_alloc();
    furi_string_printf(
        temp_str, "%d.%d", calendar_data->selected_month, calendar_data->selected_year);

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 89, 61, furi_string_get_cstr(temp_str));

    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 93, 10, "Press");
    canvas_set_font(canvas, FontSecondary);
    canvas_draw_str(canvas, 93, 19, "to Exit");
    canvas_draw_icon(canvas, 116, 2, &I_Pin_back_arrow_10x8);

    furi_string_free(temp_str);
}

static void calendar_input_callback(InputEvent* input_event, void* ctx) {
    furi_assert(ctx);

    CalendarContext* calendar_ctx = ctx;
    FuriMessageQueue* event_queue = calendar_ctx->event_queue;
    CalendarData* calendar_data = calendar_ctx->calendar_data;

    if(input_event->key == InputKeyLeft && input_event->type == InputTypeRelease) {
        calendar_data->selected_month--;
        if(calendar_data->selected_month < 1) {
            if(calendar_data->selected_year > 2000) {
                calendar_data->selected_month = 12;
                calendar_data->selected_year--;
            } else {
                calendar_data->selected_month = 1;
            }
        }
    }

    if(input_event->key == InputKeyRight && input_event->type == InputTypeRelease) {
        calendar_data->selected_month++;
        if(calendar_data->selected_month > 12) {
            if(calendar_data->selected_year < 2099) {
                calendar_data->selected_month = 1;
                calendar_data->selected_year++;
            } else {
                calendar_data->selected_month = 12;
            }
        }
    }

    if(input_event->key == InputKeyUp && input_event->type == InputTypeRelease) {
        if(calendar_data->selected_year < 2099) {
            calendar_data->selected_year++;
        }
    }

    if(input_event->key == InputKeyDown && input_event->type == InputTypeRelease) {
        if(calendar_data->selected_year > 2000) {
            calendar_data->selected_year--;
        }
    }

    furi_message_queue_put(event_queue, input_event, FuriWaitForever);
}

int32_t calendar_app(void* p) {
    UNUSED(p);

    InputEvent event;
    FuriMessageQueue* event_queue = furi_message_queue_alloc(8, sizeof(InputEvent));

    CalendarData* calendar_data = malloc(sizeof(CalendarData));

    CalendarContext* ctx = malloc(sizeof(CalendarContext));
    ctx->event_queue = event_queue;
    ctx->calendar_data = calendar_data;

    DateTime currentDate;
    furi_hal_rtc_get_datetime(&currentDate);

    calendar_data->selected_month = currentDate.month;
    calendar_data->selected_year = currentDate.year;

    // Configure view port
    ViewPort* view_port = view_port_alloc();
    view_port_draw_callback_set(view_port, calendar_draw_callback, calendar_data);
    view_port_input_callback_set(view_port, calendar_input_callback, ctx);

    // Register view port in GUI
    Gui* gui = furi_record_open(RECORD_GUI);
    gui_add_view_port(gui, view_port, GuiLayerFullscreen);

    while(1) {
        furi_check(furi_message_queue_get(event_queue, &event, FuriWaitForever) == FuriStatusOk);

        if(event.key == InputKeyBack) {
            break;
        }
    }

    furi_message_queue_free(event_queue);

    gui_remove_view_port(gui, view_port);
    view_port_free(view_port);
    furi_record_close(RECORD_GUI);
    free(calendar_data);
    free(ctx);

    return 0;
}
