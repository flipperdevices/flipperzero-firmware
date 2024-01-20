#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"
#include "scenes.h"

typedef struct AppState {
    bool usbSerialEnabled;
} AppState;

typedef struct App {
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;
    Submenu* submenu;
    DialogEx* dialog;
    Widget* widget;
    AppState* state;
    FuriThread* read_thread;
    NotificationApp* notification;
} App;

App* app_alloc();
AppState* app_state_alloc();
void app_quit(App* app);
void app_free(App* app);

#endif