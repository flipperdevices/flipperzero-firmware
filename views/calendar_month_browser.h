#pragma once

#include "../helpers/calendar_event.h"
#include "../helpers/variable_shared_context.h"
#include <gui/view.h>

typedef struct MonthBrowser MonthBrowser;

MonthBrowser* calendar_month_browser_alloc(VariableSharedContext* variable_shared_context);

void calendar_month_browser_free(MonthBrowser* calendar_year_picker);

View* calendar_month_browser_get_view(MonthBrowser* calendar_month_browser);