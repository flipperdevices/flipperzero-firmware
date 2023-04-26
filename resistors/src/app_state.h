#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"
#include "resistor_logic.h"

typedef struct AppState {
    uint8_t edit_selection;
    ResistorType resistor_type;
    BandColour resistor_bands[5];
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
void app_quit(App* app);
void app_free(App* app);
void app_init_resistor(App* app, ResistorType rtype);

#endif