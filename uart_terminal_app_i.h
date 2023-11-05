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
#include "sync.h"

#define GRAVITY_VERSION "1.0.0"
#define SYNC_BUFFER_SIZE 256

#define MAX_MENU_ITEMS (21)
#define NUM_MAIN_ITEMS (6)
#define NUM_TARGET_ITEMS (8)
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

#define STRINGS_GET "Get"

/* GRAVITY: Import GravityMode etc. */
#include "esp_flip_struct.h"

typedef enum {
  GRAVITY_MENU_MAIN = 0,
  GRAVITY_MENU_TARGETS,
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
  GRAVITY_MENU_COUNT
} GravityMenu;

/* Menu items and options for Settings menu */
enum SettingsMenuItems {
    SETTINGS_MENU_HOP_MODE = 0,
    SETTINGS_MENU_HOP_STATUS,
    SETTINGS_MENU_SSID_MIN,
    SETTINGS_MENU_SSID_MAX,
    SETTINGS_MENU_SSID_DEFAULT,
    SETTINGS_MENU_CHANNEL,
    SETTINGS_MENU_MAC,
    SETTINGS_MENU_ATTACK_PKTS,
    SETTINGS_MENU_ATTACK_MILLIS,
    SETTINGS_MENU_MAC_RAND,
    SETTINGS_MENU_PKT_EXPIRY,
    SETTINGS_MENU_DICT_DISABLE,
    SETTINGS_MENU_PURGE_STRAT,
    SETTINGS_MENU_PURGE_RSSI_MAX,
    SETTINGS_MENU_PURGE_AGE_MIN
};

#define OPTIONS_HOP_ON 1
#define OPTIONS_HOP_OFF 2
#define OPTIONS_HOP_DEFAULT 3
#define OPTIONS_HOP_MODE_SEQUENTIAL 1
#define OPTIONS_HOP_MODE_RANDOM 2
#define OPTIONS_DICT_WORDS 1
#define OPTIONS_DICT_CHARS 2
#define OPTIONS_MAC_RAND_ON 1
#define OPTIONS_MAC_RAND_OFF 2
#define OPTIONS_SSID_MIN_GET 0
#define OPTIONS_SSID_MAX_GET 0
#define OPTIONS_SSID_DEFAULT_GET 0
#define OPTIONS_ATTACK_MILLIS_GET 0
#define OPTIONS_PKT_EXPIRY_GET 0


/* Menu items and options for Purge menu */
enum PurgeMenuItems {
    PURGE_MENU_AGE_ON = 0,
    PURGE_MENU_AGE,
    PURGE_MENU_RSSI_ON,
    PURGE_MENU_RSSI,
    PURGE_MENU_UNSELECTED_ON,
    PURGE_MENU_UNNAMED_ON,
    PURGE_MENU_RUN,
};

#define OPTIONS_PURGE_ON 0
#define OPTIONS_PURGE_OFF 1


char *strToken(char *cmdLine, char sep, int tokenNum);
bool mac_string_to_bytes(char *strMac, uint8_t *bMac);
bool syncProcessResponse(UART_TerminalApp *app);

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
    VariableItemList *help_info_menu_list, *attacks_mana_menu_list, *purge_menu_list;
    ByteInput *settings_mac_bytes;

    uint8_t mac_bytes[NUM_MAC_BYTES];

    UART_TerminalUart* uart;
    int selected_menu_items[GRAVITY_MENU_COUNT];
    int selected_menu_options[GRAVITY_MENU_COUNT][MAX_MENU_ITEMS];
    char* selected_tx_string;
    bool is_command;
    bool is_custom_tx_string;
    bool focus_console_start;
    bool show_stopscan_tip;
    int BAUDRATE;
    int TERMINAL_MODE; //1=AT mode, 0=other mode
    bool free_command;              /* Does selected_tx_string need to be free'd after send? */

    GravityCommand gravityCommand; /* Gravity command */
    GravityMenu currentMenu;
    int purgeStrategy;
    int purgeAge;
    int purgeRSSI;

    uint8_t syncBuffer[SYNC_BUFFER_SIZE];
    int syncBufLen;
    bool syncComplete;
};

typedef enum {
    UART_TerminalAppViewConsoleOutput,
    UART_TerminalAppViewTextInput,
    Gravity_AppViewMainMenu,
    Gravity_AppViewTargetsMenu,
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
    char* options_menu[MAX_OPTIONS];
    int num_options_menu;
    char* actual_commands[MAX_OPTIONS];
    InputArgs needs_keyboard;
    FocusConsole focus_console;
    bool show_stopscan_tip;
    bool isSubMenu;
} UART_TerminalItem;

extern UART_TerminalItem purgeMenu[]; /* Provided by uart_terminal_scene_purge.c */
extern UART_TerminalItem settings[];  /* Used by sync.c */
