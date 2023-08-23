#pragma once

#include <furi.h>
#include <furi_hal.h>

#define CAN_LIN_TOOLS_VERSION_APP "0.1"
#define CAN_LIN_TOOLS_DEVELOPED "SkorP"
#define CAN_LIN_TOOLS_GITHUB "https://github.com/flipperdevices/flipperzero-firmware"

typedef enum {
    CanLinToolsViewVariableItemList,
    CanLinToolsViewSubmenu,
    CanLinToolsViewCanHacker2,
    CanLinToolsViewLinHacker,
    CanLinToolsViewCarBusAnalyzer,
    CanLinToolsViewWidget,
} CanLinToolsView;
