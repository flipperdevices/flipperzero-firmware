#pragma once
#include <furi.h>
#include "loader_extapps.h"

#define MANUALLY_ADDED_ITEMS_COUNT 2

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LoaderMenu LoaderMenu;

LoaderMenu* loader_menu_alloc(void (*closed_cb)(void*), void* context);

void loader_menu_free(LoaderMenu* loader_menu);

#ifdef __cplusplus
}
#endif
