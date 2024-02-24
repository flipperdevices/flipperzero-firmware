/*
Handles the life cycle of the flipper app.

Holds all the app-global memory stuff we need.
Flipper will call app_alloc, then app_state_alloc.
Then it runs the application UI code.
On exit it calls app_quit and then app_free.

We need to take care to deallocate everything we allocate
in these methods, or it will leak memory and through repeated
launches the flipper will run out of mem. (though this isn't
super serious because the flipper is pretty easy to just reboot)
*/

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
    bool waitForCode; // If we're reading a code, only read the first one
    int codeLen; // How many packets the current code has
    int rpackets; // count of how many packets we've got. Should match current_code.
    int spackets; // count of how many packets we've sent. Should match current_code
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
    FuriString* dialog_header;
    FuriString* dialog_text;

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