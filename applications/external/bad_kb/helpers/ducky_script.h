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
#include "../views/bad_kb_view.h"
#include "../bad_kb_paths.h"

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
    BadKbStateInit,
    BadKbStateNotConnected,
    BadKbStateIdle,
    BadKbStateWillRun,
    BadKbStateRunning,
    BadKbStateDelay,
    BadKbStateStringDelay,
    BadKbStateWaitForBtn,
    BadKbStatePaused,
    BadKbStateDone,
    BadKbStateScriptError,
    BadKbStateFileError,
} BadKbWorkerState;

struct BadKbState {
    BadKbWorkerState state;
    bool is_bt;
    uint32_t pin;
    uint16_t line_cur;
    uint16_t line_nb;
    uint32_t delay_remain;
    uint16_t error_line;
    char error[64];
};

typedef struct BadKbApp BadKbApp;

typedef struct {
    FuriThread* thread;
    BadKbState st;

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
    BadKbApp* app;
} BadKbScript;

BadKbScript* bad_kb_script_open(FuriString* file_path, Bt* bt, BadKbApp* app);

void bad_kb_script_close(BadKbScript* bad_kb);

void bad_kb_script_set_keyboard_layout(BadKbScript* bad_kb, FuriString* layout_path);

void bad_kb_script_start(BadKbScript* bad_kb);

void bad_kb_script_stop(BadKbScript* bad_kb);

void bad_kb_script_start_stop(BadKbScript* bad_kb);

void bad_kb_script_pause_resume(BadKbScript* bad_kb);

BadKbState* bad_kb_script_get_state(BadKbScript* bad_kb);

#define BAD_KB_NAME_LEN FURI_HAL_BT_ADV_NAME_LENGTH
#define BAD_KB_MAC_LEN GAP_MAC_ADDR_SIZE
#define BAD_KB_USB_LEN HID_MANUF_PRODUCT_NAME_LEN

extern const uint8_t BAD_KB_EMPTY_MAC[BAD_KB_MAC_LEN];
extern uint8_t BAD_KB_BOUND_MAC[BAD_KB_MAC_LEN]; // For remember mode

typedef enum {
    BadKbAppErrorNoFiles,
} BadKbAppError;

typedef struct {
    char bt_name[BAD_KB_NAME_LEN];
    uint8_t bt_mac[BAD_KB_MAC_LEN];
    FuriHalUsbHidConfig usb_cfg;
} BadKbConfig;

typedef enum {
    BadKbConnModeNone,
    BadKbConnModeUsb,
    BadKbConnModeBt,
} BadKbConnMode;

struct BadKbApp {
    Gui* gui;
    ViewDispatcher* view_dispatcher;
    SceneManager* scene_manager;
    NotificationApp* notifications;
    DialogsApp* dialogs;
    Widget* widget;
    VariableItemList* var_item_list;
    TextInput* text_input;
    ByteInput* byte_input;
    char usb_name_buf[BAD_KB_USB_LEN];
    uint16_t usb_vidpid_buf[2];
    char bt_name_buf[BAD_KB_NAME_LEN];
    uint8_t bt_mac_buf[BAD_KB_MAC_LEN];

    BadKbAppError error;
    FuriString* file_path;
    FuriString* keyboard_layout;
    BadKb* bad_kb_view;
    BadKbScript* bad_kb_script;

    Bt* bt;
    bool is_bt;
    bool bt_remember;
    BadKbConfig config; // User options
    BadKbConfig id_config; // ID and BT_ID values

    bool set_usb_id;
    bool set_bt_id;
    bool has_usb_id;
    bool has_bt_id;

    GapPairing prev_bt_mode;
    char prev_bt_name[BAD_KB_NAME_LEN];
    uint8_t prev_bt_mac[BAD_KB_MAC_LEN];
    FuriHalUsbInterface* prev_usb_mode;

    FuriHalUsbHidConfig* hid_cfg;
    BadKbConnMode conn_mode;
    FuriThread* conn_init_thread;
};

int32_t bad_kb_conn_apply(BadKbApp* app);

void bad_kb_conn_reset(BadKbApp* app);

void bad_kb_config_refresh(BadKbApp* app);

void bad_kb_config_adjust(BadKbConfig* cfg);

#ifdef __cplusplus
}
#endif
