#pragma once

#include <gui/view.h>
#include "../helpers/can_lin_tools_types.h"
#include "../helpers/can_lin_tools_event.h"

typedef struct CanLinToolsCarBusAnalyzerView CanLinToolsCarBusAnalyzerView;

typedef void (*CanLinToolsCarBusAnalyzerViewCallback)(CanLinToolsCustomEvent event, void* context);

void can_lin_tools_view_carbus_analyzer_set_callback(
    CanLinToolsCarBusAnalyzerView* instance,
    CanLinToolsCarBusAnalyzerViewCallback callback,
    void* context);

CanLinToolsCarBusAnalyzerView* can_lin_tools_view_carbus_analyzer_alloc();

void can_lin_tools_view_carbus_analyzer_free(CanLinToolsCarBusAnalyzerView* instance);

View* can_lin_tools_view_carbus_analyzer_get_view(CanLinToolsCarBusAnalyzerView* instance);
