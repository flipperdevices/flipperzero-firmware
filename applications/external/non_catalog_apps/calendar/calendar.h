#pragma once

#include <gui/gui.h>
#include <gui/view.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include "views/calendar_year_picker.h"
#include "views/calendar_month_picker.h"
#include "views/calendar_month_browser.h"
#include "scenes/calendar_scene.h"
#include "helpers/variable_shared_context.h"

typedef struct {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    YearPicker* calendar_year_picker;
    MonthPicker* calendar_month_picker;
    MonthBrowser* calendar_month_browser;
    VariableSharedContext* variable_shared_context;
} CalendarApp;

typedef enum {
    CalendarAppViewYearPicker,
    CalendarAppViewMonthPicker,
    CalendarAppViewMonthBrowser,
} CalendarAppView;