#ifndef APP_HEADERS
#define APP_HEADERS

#include "flipper.h"
#include "scenes.h"
#include "dmcomm_link.h"

#define MAX_FILENAME_LEN 64
#define MAX_DIGIROM_LEN 128

typedef struct AppState {
    char current_code[MAX_DIGIROM_LEN]; // What the user loaded
    char result_code[MAX_DIGIROM_LEN]; // What dmcomm sent back
    char file_name_tmp[MAX_FILENAME_LEN]; // Filename to save codes as
    bool waitForCode;
    int codeLen;
    int rpackets;
    int spackets;
    FuriString* s_code; // First code sent
    FuriString* r_code; // Second code sent
    FcomScene save_code_return_scene; // What scene we should go back to when done saving
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

    FuriMutex* text_box_mutex; // lock so UI/Serial callbacks can communicate safely
    FuriString* text_box_store;

    // Interfaces to other things
    AppState* state;

    // dmcomm Thread things
    bool dmcomm_run;
    FuriThread* dcomm_thread;
    FuriStreamBuffer* dmcomm_input_stream;
    FuriStreamBuffer* dmcomm_output_stream;

    // USB serial bridge
    UsbUartBridge* usb_uart_bridge;

    // System handles
    NotificationApp* notification;
    Storage* storage;
} App;

App* app_alloc();
AppState* app_state_alloc();
void app_quit(App* app);
void app_free(App* app);

#endif