#pragma once

#include "../helpers/calendar_event.h"
#include "../helpers/variable_shared_context.h"
#include <gui/view.h>

typedef struct YearPicker YearPicker;

typedef void (*YearPickerCallback)(CalendarAppCustomEvent event, void* context);

void calendar_year_picker_set_callback(
    YearPicker* calendar_year_picker,
    YearPickerCallback callback,
    void* context);

YearPicker* calendar_year_picker_alloc(VariableSharedContext* variable_shared_context);

void calendar_year_picker_free(YearPicker* calendar_year_picker);

View* calendar_year_picker_get_view(YearPicker* bt_packet_test);