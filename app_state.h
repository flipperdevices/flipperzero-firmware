#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"
#include "scenes.h"

#define MAX_FILENAME_LEN 64

typedef struct AppState {
    bool usbSerialEnabled;
    char file_name_tmp[MAX_FILENAME_LEN];
} AppState;

typedef struct App {
    // Scene stuff
    SceneManager* scene_manager;
    ViewDispatcher* view_dispatcher;

    // Widgets for views
    Submenu* submenu;
    DialogEx* dialog;
    TextInput* text_input;
    FileBrowser* file_browser;
    FuriString* file_path;
    TextBox* text_box;
    FuriString* text_box_store;

    // Interfaces to other things
    AppState* state;
    FuriThread* read_thread;
    NotificationApp* notification;
} App;

App* app_alloc();
AppState* app_state_alloc();
void app_quit(App* app);
void app_free(App* app);

#endif