#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <furi_hal.h>
#include <bt/bt_service/bt_i.h>

#include <gui/view_dispatcher.h>
#include <gui/modules/widget.h>
#include <gui/modules/variable_item_list.h>
#include <gui/modules/text_input.h>
#include <gui/modules/byte_input.h>
#include "../views/bad_bt_view.h"
#include "../bad_bt_paths.h"

#define FILE_BUFFER_LEN 16

typedef enum {
    LevelRssi122_100,
    LevelRssi99_80,
    LevelRssi79_60,
    LevelRssi59_40,
    LevelRssi39_0,
    LevelRssiNum,
    LevelRssiError = 0xFF,
} LevelRssiRange;

extern const uint8_t bt_hid_delays[LevelRssiNum];

extern uint8_t bt_timeout;

typedef enum {
    BadBtStateInit,
    BadBtStateNotConnected,
    BadBtStateIdle,
    BadBtStateWillRun,
    BadBtStateRunning,
    BadBtStateDelay,
    BadBtStateStringDelay,
    BadBtStateWaitForBtn,
    BadBtStatePaused,
    BadBtStateDone,
    BadBtStateScriptError,
    BadBtStateFileError,
} BadBtWorkerState;

struct BadBtState {
    BadBtWorkerState state;
    uint32_t pin;
    uint16_t line_cur;
    uint16_t line_nb;
    uint32_t delay_remain;
    uint16_t error_line;
    char error[64];
};

typedef struct BadBtApp BadBtApp;

typedef struct {
    FuriThread* thread;
    BadBtState st;

    FuriString* file_path;
    FuriString* keyboard_layout;
    uint8_t file_buf[FILE_BUFFER_LEN + 1];
    uint8_t buf_start;
    uint8_t buf_len;
    bool file_end;

    uint32_t defdelay;
    uint32_t stringdelay;
    uint16_t layout[128];

    FuriString* line;
    FuriString* line_prev;
    uint32_t repeat_cnt;
    uint8_t key_hold_nb;

    FuriString* string_print;
    size_t string_print_pos;

    Bt* bt;
    BadBtApp* app;
} BadBtScript;

BadBtScript* bad_bt_script_open(FuriString* file_path, Bt* bt, BadBtApp* app);

void bad_bt_script_close(BadBtScript* bad_bt);

void bad_bt_script_set_keyboard_layout(BadBtScript* bad_bt, FuriString* layout_path);

void bad_bt_script_start_stop(BadBtScript* bad_bt);

void bad_bt_script_pause_resume(BadBtScript* bad_bt);

BadBtState* bad_bt_script_get_state(BadBtScript* bad_bt);

#define BAD_BT_NAME_LEN FURI_HAL_BT_ADV_NAME_LENGTH
#define BAD_BT_MAC_LEN GAP_MAC_ADDR_SIZE
#define BAD_BT_USB_LEN HID_MANUF_PRODUCT_NAME_LEN

extern const uint8_t BAD_BT_EMPTY_MAC[BAD_BT_MAC_LEN];
extern uint8_t BAD_BT_BOUND_MAC[BAD_BT_MAC_LEN]; // For remember mode

typedef enum {
    BadBtAppErrorNoFiles,
} BadBtAppError;

typedef struct {
    char bt_name[BAD_BT_NAME_LEN];
    uint8_t bt_mac[BAD_BT_MAC_LEN];
} BadBtConfig;

typedef enum {
    BadBtConnModeNone,
    BadBtConnModeBt,
} BadBtConnMode;

struct BadBtApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    VariableItemList* var_item_list;
    TextInput* text_input;
    ByteInput* byte_input;

    char bt_name_buf[BAD_BT_NAME_LEN];
    uint8_t bt_mac_buf[BAD_BT_MAC_LEN];

    BadBtAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadBt* bad_bt_view;
    BadBtScript* bad_bt_script;

    Bt* bt;
    bool bt_remember;
    BadBtConfig config; // User options
    BadBtConfig id_config; // BT_ID values

    bool set_bt_id;
    bool has_bt_id;

    GapPairing prev_bt_mode;
    char prev_bt_name[BAD_BT_NAME_LEN];
    uint8_t prev_bt_mac[BAD_BT_MAC_LEN];

    BadBtConnMode conn_mode;
    FuriThread* conn_init_thread;
};

int32_t bad_bt_conn_apply(BadBtApp* app);

void bad_bt_conn_reset(BadBtApp* app);

void bad_bt_config_refresh(BadBtApp* app);

void bad_bt_config_adjust(BadBtConfig* cfg);

#ifdef __cplusplus
}
#endif
