#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"

typedef enum { Resistor4Band = 4, Resistor5Band = 5 } ResistorType;

typedef struct AppState {
    uint8_t edit_selection;
    ResistorType resistor_type;
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