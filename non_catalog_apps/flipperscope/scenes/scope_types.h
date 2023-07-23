#pragma once

#include <furi.h>
#include <furi_hal.h>

#define S_DEVELOPED "anfractuosity"
#define S_GITHUB "https://github.com/anfractuosity/flipperscope"

typedef enum {
    ScopeViewVariableItemList,
    ScopeViewSubmenu,
    ScopeViewWidget,
} ScopeView;
