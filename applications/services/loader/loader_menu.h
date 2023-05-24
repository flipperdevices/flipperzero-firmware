#pragma once
#include <furi.h>
#include "loader_extmainapp.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct LoaderMenu LoaderMenu;

LoaderMenu* loader_menu_alloc();

void loader_menu_free(LoaderMenu* loader_menu);

void loader_menu_start(LoaderMenu* loader_menu, bool settings, ExtMainAppList_t* ext_main_apps);

void loader_menu_stop(LoaderMenu* loader_menu);

void loader_menu_set_closed_callback(
    LoaderMenu* loader_menu,
    void (*callback)(void*),
    void* context);

void loader_menu_set_click_callback(
    LoaderMenu* loader_menu,
    void (*callback)(const char*, bool, void*),
    void* context);

#ifdef __cplusplus
}
#endif
