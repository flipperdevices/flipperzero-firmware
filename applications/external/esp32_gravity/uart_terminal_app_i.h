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
#include <gui/modules/byte_input.h>
#include "uart_text_input.h"

#define GRAVITY_VERSION "0.6.0"

//#define NUM_MENU_ITEMS (23)
#define MAX_MENU_ITEMS (21)
#define NUM_MAIN_ITEMS (6)
#define NUM_TARGET_ITEMS (8)
#define NUM_TARGETS_SCAN_ITEMS (8)
#define NUM_PACKETS_ITEMS (5)
#define NUM_PACKETS_DEAUTH_ITEMS (3)
#define NUM_PACKETS_FUZZ_ITEMS (4)
#define NUM_HELP_ITEMS (5)
#define NUM_HELP_INFO_ITEMS (21)
#define NUM_ATTACK_ITEMS (4)
#define NUM_ATTACK_MANA_ITEMS (5)
#define NUM_SETTINGS_ITEMS (16)
#define NUM_PURGE_ITEMS (7)
#define MAX_MENU_STR_LEN (12)
#define NUM_MAC_BYTES (6)

#define UART_TERMINAL_TEXT_BOX_STORE_SIZE (4096)
#define UART_TERMINAL_TEXT_INPUT_STORE_SIZE (512)
#define UART_CH (FuriHalUartIdUSART1)

/* GRAVITY: Import GravityMode etc. */
#include "esp_flip_struct.h"

typedef enum {
    GRAVITY_MENU_MAIN,
    GRAVITY_MENU_TARGETS,
    GRAVITY_MENU_TARGETS_SCAN,
    GRAVITY_MENU_PACKETS,
    GRAVITY_MENU_PACKETS_FUZZ,
    GRAVITY_MENU_PACKETS_DEAUTH,
    GRAVITY_MENU_ATTACKS,
    GRAVITY_MENU_ATTACKS_MANA,
    GRAVITY_MENU_SETTINGS,
    GRAVITY_MENU_SETTINGS_MAC,
    GRAVITY_MENU_HELP,
    GRAVITY_MENU_HELP_INFO,
    GRAVITY_MENU_PURGE,
} GravityMenu;

char* strToken(char* cmdLine, char sep, int tokenNum);

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
    VariableItemList *attacks_menu_list, *settings_menu_list, *help_menu_list;
    VariableItemList *packets_deauth_menu_list, *packets_fuzz_menu_list;
    VariableItemList *help_info_menu_list, *targets_scan_menu_list;
    VariableItemList *attacks_mana_menu_list, *purge_menu_list;
    ByteInput* settings_mac_bytes;

    uint8_t mac_bytes[NUM_MAC_BYTES];

    UART_TerminalUart* uart;
    int selected_menu_index;
    int selected_option_index[MAX_MENU_ITEMS];
    char* selected_tx_string;
    bool is_command;
    bool is_custom_tx_string;
    bool focus_console_start;
    bool show_stopscan_tip;
    int BAUDRATE;
    int TERMINAL_MODE; //1=AT mode, 0=other mode
    bool free_command; /* Does selected_tx_string need to be free'd after send? */

    GravityCommand gravityCommand; /* Gravity command */
    GravityMenu currentMenu;
    int purgeStrategy;
    int purgeAge;
    int purgeRSSI;
};

typedef enum {
    UART_TerminalAppViewConsoleOutput,
    UART_TerminalAppViewTextInput,
    Gravity_AppViewMainMenu,
    Gravity_AppViewTargetsMenu,
    Gravity_AppViewTargetsScanMenu,
    Gravity_AppViewPacketsMenu,
    Gravity_AppViewPacketsDeauthMenu,
    Gravity_AppViewPacketsFuzzMenu,
    Gravity_AppViewAttacksMenu,
    Gravity_AppViewAttacksManaMenu,
    Gravity_AppViewSettingsMenu,
    Gravity_AppViewSettingsMac,
    Gravity_AppViewHelpMenu,
    Gravity_AppViewHelpInfoMenu,
    Gravity_AppViewPurgeMenu,
} UART_TerminalAppView;

// For each command, define whether additional arguments are needed
// (enabling text input to fill them out), and whether the console
// text box should focus at the start of the output or the end
typedef enum { NO_ARGS = 0, INPUT_ARGS, TOGGLE_ARGS } InputArgs;

typedef enum { FOCUS_CONSOLE_END = 0, FOCUS_CONSOLE_START, FOCUS_CONSOLE_TOGGLE } FocusConsole;

#define SHOW_STOPSCAN_TIP (true)
#define NO_TIP (false)

#define MAX_OPTIONS (24)
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
