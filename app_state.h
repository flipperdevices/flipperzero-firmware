#pragma once

#include "flipper.h"
#include "item.h"

#define QUAC_NAME "Quac!"

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    ButtonMenu* btn_menu;

    Storage* storage;
    NotificationApp* notifications;

    int depth;
    ItemsView* items_view;
    int selected_item;
} App;

App* app_alloc();
void app_free(App* app);