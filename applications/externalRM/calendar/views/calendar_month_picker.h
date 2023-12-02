#pragma once

#include "../helpers/calendar_event.h"
#include "../helpers/variable_shared_context.h"
#include <gui/view.h>

typedef struct MonthPicker MonthPicker;

typedef void (*MonthPickerCallback)(CalendarAppCustomEvent event, void* context);

void calendar_month_picker_set_callback(
    MonthPicker* calendar_month_picker,
    MonthPickerCallback callback,
    void* context);

MonthPicker* calendar_month_picker_alloc(VariableSharedContext* variable_shared_context);

void calendar_month_picker_free(MonthPicker* calendar_month_picker);

View* calendar_month_picker_get_view(MonthPicker* bt_packet_test);