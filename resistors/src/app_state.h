#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"

typedef struct AppState {
    uint8_t edit_selection;
} AppState;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    Widget* widget;
    AppState* state;
} App;

App* app_alloc();
AppState* app_state_alloc();
void app_free(App* app);

#endif