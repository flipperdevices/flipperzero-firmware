#pragma once

#include "uart_terminal_app.h"
#include "scenes/uart_terminal_scene.h"
#include "uart_terminal_custom_event.h"
#include "uart_terminal_uart.h"

#include <gui/gui.h>
#include <gui/view_dispatcher.h>
#include <gui/scene_manager.h>
#include <gui/modules/text_box.h>
#include <gui/modules/variable_item_list.h>
#include "uart_text_input.h"

#define GRAVITY_VERSION "0.6.0"

//#define NUM_MENU_ITEMS (23)
#define MAX_MENU_ITEMS (8)
#define NUM_MAIN_ITEMS (6)
#define NUM_TARGET_ITEMS (8)
#define NUM_PACKETS_ITEMS (4)
#define NUM_OTHER_ITEMS (4)
#define NUM_ATTACK_ITEMS (6)
#define NUM_SETTINGS_ITEMS (3)
#define NUM_DEAUTH_ITEMS (3)
#define NUM_FUZZ_ITEMS (4)
#define MAX_MENU_STR_LEN (12)

#define UART_TERMINAL_TEXT_BOX_STORE_SIZE (1024)
#define UART_TERMINAL_TEXT_INPUT_STORE_SIZE (512)
#define UART_CH (FuriHalUartIdUSART1)

/* GRAVITY: Import GravityMode etc. */
#include "esp_flip_struct.h"

typedef enum {
  GRAVITY_MENU_MAIN,
  GRAVITY_MENU_TARGETS,
  GRAVITY_MENU_PACKETS,
  GRAVITY_MENU_ATTACKS,
  GRAVITY_MENU_SETTINGS,
  GRAVITY_MENU_OTHERS,
  GRAVITY_MENU_FUZZ,
  GRAVITY_MENU_DEAUTH
} GravityMenu;

char *strToken(char *cmdLine, char sep, int tokenNum);

struct UART_TerminalApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;

    char text_input_store[UART_TERMINAL_TEXT_INPUT_STORE_SIZE + 1];
    FuriString* text_box_store;
    size_t text_box_store_strlen;
    TextBox* text_box;
    UART_TextInput* text_input;

    VariableItemList *main_menu_list, *targets_menu_list, *packets_menu_list;
    VariableItemList *attacks_menu_list, *settings_menu_list, *others_menu_list;
    VariableItemList *deauth_menu_list, *fuzz_menu_list;

    UART_TerminalUart* uart;
    int selected_menu_index;
    int selected_option_index[MAX_MENU_ITEMS];
    const char* selected_tx_string;
    bool is_command;
    bool is_custom_tx_string;
    bool focus_console_start;
    bool show_stopscan_tip;
    int BAUDRATE;
    int TERMINAL_MODE; //1=AT mode, 0=other mode

    GravityCommand gravityCommand; /* Gravity command */
    GravityMenu currentMenu;
};

typedef enum {
    UART_TerminalAppViewConsoleOutput,
    UART_TerminalAppViewTextInput,
    Gravity_AppViewMainMenu,
    Gravity_AppViewTargetsMenu,
    Gravity_AppViewPacketsMenu,
    Gravity_AppViewAttacksMenu,
    Gravity_AppViewSettingsMenu,
    Gravity_AppViewOthersMenu,
    Gravity_AppViewDeauthMenu,
    Gravity_AppViewFuzzMenu,
} UART_TerminalAppView;

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
typedef enum { NO_ARGS = 0, INPUT_ARGS, TOGGLE_ARGS } InputArgs;

typedef enum { FOCUS_CONSOLE_END = 0, FOCUS_CONSOLE_START, FOCUS_CONSOLE_TOGGLE } FocusConsole;

#define SHOW_STOPSCAN_TIP (true)
#define NO_TIP (false)

#define MAX_OPTIONS (12)
typedef struct {
    const char* item_string;
    const char* options_menu[MAX_OPTIONS];
    int num_options_menu;
    const char* actual_commands[MAX_OPTIONS];
    InputArgs needs_keyboard;
    FocusConsole focus_console;
    bool show_stopscan_tip;
    bool isSubMenu;
} UART_TerminalItem;
