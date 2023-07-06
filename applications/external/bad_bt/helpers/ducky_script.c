#include <furi.h>
#include <furi_hal.h>
#include <gui/gui.h>
#include <input/input.h>
#include <lib/toolbox/args.h>
#include <furi_hal_bt_hid.h>
#include <bt/bt_service/bt.h>
#include <storage/storage.h>
#include "ducky_script.h"
#include "ducky_script_i.h"
#include <dolphin/dolphin.h>
#include <toolbox/hex.h>
#include "../scenes/bad_bt_scene.h"

const uint8_t BAD_BT_EMPTY_MAC[BAD_BT_MAC_LEN] = FURI_HAL_BT_EMPTY_MAC_ADDR;

// Adjusts to serial MAC +2 in app init
uint8_t BAD_BT_BOUND_MAC[BAD_BT_MAC_LEN] = FURI_HAL_BT_EMPTY_MAC_ADDR;

#define TAG "BadBT"
#define WORKER_TAG TAG "Worker"

#define BADBT_ASCII_TO_KEY(script, x) \
    (((uint8_t)x < 128) ? (script->layout[(uint8_t)x]) : HID_KEYBOARD_NONE)

// Delays for waiting between HID key press and key release
const uint8_t bt_hid_delays[LevelRssiNum] = {
    45, // LevelRssi122_100
    38, // LevelRssi99_80
    30, // LevelRssi79_60
    26, // LevelRssi59_40
    21, // LevelRssi39_0
};

uint8_t bt_timeout = 0;

static LevelRssiRange bt_remote_rssi_range(Bt* bt) {
    uint8_t rssi;

    if(!bt_remote_rssi(bt, &rssi)) return LevelRssiError;

    if(rssi <= 39)
        return LevelRssi39_0;
    else if(rssi <= 59)
        return LevelRssi59_40;
    else if(rssi <= 79)
        return LevelRssi79_60;
    else if(rssi <= 99)
        return LevelRssi99_80;
    else if(rssi <= 122)
        return LevelRssi122_100;

    return LevelRssiError;
}

static inline void update_bt_timeout(Bt* bt) {
    LevelRssiRange r = bt_remote_rssi_range(bt);
    if(r < LevelRssiNum) {
        bt_timeout = bt_hid_delays[r];
        FURI_LOG_D(WORKER_TAG, "BLE Key timeout : %u", bt_timeout);
    }
}

typedef enum {
    WorkerEvtStartStop = (1 << 0),
    WorkerEvtPauseResume = (1 << 1),
    WorkerEvtEnd = (1 << 2),
    WorkerEvtConnect = (1 << 3),
    WorkerEvtDisconnect = (1 << 4),
} WorkerEvtFlags;

static const char ducky_cmd_bt_id[] = {"BT_ID"};

static const uint8_t numpad_keys[10] = {
    HID_KEYPAD_0,
    HID_KEYPAD_1,
    HID_KEYPAD_2,
    HID_KEYPAD_3,
    HID_KEYPAD_4,
    HID_KEYPAD_5,
    HID_KEYPAD_6,
    HID_KEYPAD_7,
    HID_KEYPAD_8,
    HID_KEYPAD_9,
};

uint32_t ducky_get_command_len(const char* line) {
    uint32_t len = strlen(line);
    for(uint32_t i = 0; i < len; i++) {
        if(line[i] == ' ') return i;
    }
    return 0;
}

bool ducky_is_line_end(const char chr) {
    return ((chr == ' ') || (chr == '\0') || (chr == '\r') || (chr == '\n'));
}

uint16_t ducky_get_keycode(BadBtScript* bad_bt, const char* param, bool accept_chars) {
    uint16_t keycode = ducky_get_keycode_by_name(param);
    if(keycode != HID_KEYBOARD_NONE) {
        return keycode;
    }

    if((accept_chars) && (strlen(param) > 0)) {
        return (BADBT_ASCII_TO_KEY(bad_bt, param[0]) & 0xFF);
    }
    return 0;
}

bool ducky_get_number(const char* param, uint32_t* val) {
    uint32_t value = 0;
    if(sscanf(param, "%lu", &value) == 1) {
        *val = value;
        return true;
    }
    return false;
}

void ducky_numlock_on(BadBtScript* bad_bt) {
    UNUSED(bad_bt);
    if((furi_hal_bt_hid_get_led_state() & HID_KB_LED_NUM) == 0) {
        furi_hal_bt_hid_kb_press(HID_KEYBOARD_LOCK_NUM_LOCK);
        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(HID_KEYBOARD_LOCK_NUM_LOCK);
    }
}

bool ducky_numpad_press(BadBtScript* bad_bt, const char num) {
    UNUSED(bad_bt);
    if((num < '0') || (num > '9')) return false;

    uint16_t key = numpad_keys[num - '0'];

    furi_hal_bt_hid_kb_press(key);
    furi_delay_ms(bt_timeout);
    furi_hal_bt_hid_kb_release(key);

    return true;
}

bool ducky_altchar(BadBtScript* bad_bt, const char* charcode) {
    uint8_t i = 0;
    bool state = false;

    furi_hal_bt_hid_kb_press(KEY_MOD_LEFT_ALT);

    while(!ducky_is_line_end(charcode[i])) {
        state = ducky_numpad_press(bad_bt, charcode[i]);
        if(state == false) break;
        i++;
    }

    furi_hal_bt_hid_kb_release(KEY_MOD_LEFT_ALT);

    return state;
}

bool ducky_altstring(BadBtScript* bad_bt, const char* param) {
    uint32_t i = 0;
    bool state = false;

    while(param[i] != '\0') {
        if((param[i] < ' ') || (param[i] > '~')) {
            i++;
            continue; // Skip non-printable chars
        }

        char temp_str[4];
        snprintf(temp_str, 4, "%u", param[i]);

        state = ducky_altchar(bad_bt, temp_str);
        if(state == false) break;
        i++;
    }
    return state;
}

int32_t ducky_error(BadBtScript* bad_bt, const char* text, ...) {
    va_list args;
    va_start(args, text);

    vsnprintf(bad_bt->st.error, sizeof(bad_bt->st.error), text, args);

    va_end(args);
    return SCRIPT_STATE_ERROR;
}

bool ducky_string(BadBtScript* bad_bt, const char* param) {
    uint32_t i = 0;

    while(param[i] != '\0') {
        if(param[i] != '\n') {
            uint16_t keycode = BADBT_ASCII_TO_KEY(bad_bt, param[i]);
            if(keycode != HID_KEYBOARD_NONE) {
                furi_hal_bt_hid_kb_press(keycode);
                furi_delay_ms(bt_timeout);
                furi_hal_bt_hid_kb_release(keycode);
            }
        } else {
            furi_hal_bt_hid_kb_press(HID_KEYBOARD_RETURN);
            furi_delay_ms(bt_timeout);
            furi_hal_bt_hid_kb_release(HID_KEYBOARD_RETURN);
        }
        i++;
    }
    bad_bt->stringdelay = 0;
    return true;
}

static bool ducky_string_next(BadBtScript* bad_bt) {
    if(bad_bt->string_print_pos >= furi_string_size(bad_bt->string_print)) {
        return true;
    }

    char print_char = furi_string_get_char(bad_bt->string_print, bad_bt->string_print_pos);

    if(print_char != '\n') {
        uint16_t keycode = BADBT_ASCII_TO_KEY(bad_bt, print_char);
        if(keycode != HID_KEYBOARD_NONE) {
            furi_hal_bt_hid_kb_press(keycode);
            furi_delay_ms(bt_timeout);
            furi_hal_bt_hid_kb_release(keycode);
        }
    } else {
        furi_hal_bt_hid_kb_press(HID_KEYBOARD_RETURN);
        furi_delay_ms(bt_timeout);
        furi_hal_bt_hid_kb_release(HID_KEYBOARD_RETURN);
    }

    bad_bt->string_print_pos++;

    return false;
}

static int32_t ducky_parse_line(BadBtScript* bad_bt, FuriString* line) {
    uint32_t line_len = furi_string_size(line);
    const char* line_tmp = furi_string_get_cstr(line);

    if(line_len == 0) {
        return SCRIPT_STATE_NEXT_LINE; // Skip empty lines
    }
    FURI_LOG_D(WORKER_TAG, "line:%s", line_tmp);

    // Ducky Lang Functions
    int32_t cmd_result = ducky_execute_cmd(bad_bt, line_tmp);
    if(cmd_result != SCRIPT_STATE_CMD_UNKNOWN) {
        return cmd_result;
    }

    // Special keys + modifiers
    uint16_t key = ducky_get_keycode(bad_bt, line_tmp, false);
    if(key == HID_KEYBOARD_NONE) {
        return ducky_error(bad_bt, "No keycode defined for %s", line_tmp);
    }
    if((key & 0xFF00) != 0) {
        // It's a modifier key
        line_tmp = &line_tmp[ducky_get_command_len(line_tmp) + 1];
        key |= ducky_get_keycode(bad_bt, line_tmp, true);
    }
    furi_hal_bt_hid_kb_press(key);
    furi_delay_ms(bt_timeout);
    furi_hal_bt_hid_kb_release(key);

    return 0;
}

static bool ducky_set_bt_id(BadBtScript* bad_bt, const char* line) {
    BadBtConfig* cfg = &bad_bt->app->id_config;

    size_t line_len = strlen(line);
    size_t mac_len = BAD_BT_MAC_LEN * 3; // 2 text chars + separator per byte
    if(line_len < mac_len + 1) return false; // MAC + at least 1 char for name

    for(size_t i = 0; i < BAD_BT_MAC_LEN; i++) {
        char a = line[i * 3];
        char b = line[i * 3 + 1];
        if((a < 'A' && a > 'F') || (a < '0' && a > '9') || (b < 'A' && b > 'F') ||
           (b < '0' && b > '9') || !hex_char_to_uint8(a, b, &cfg->bt_mac[i])) {
            return false;
        }
    }

    strlcpy(cfg->bt_name, line + mac_len, BAD_BT_NAME_LEN);
    FURI_LOG_D(WORKER_TAG, "set bt id: %s", line);
    return true;
}

static void ducky_script_preload(BadBtScript* bad_bt, File* script_file) {
    BadBtApp* app = bad_bt->app;
    uint8_t ret = 0;
    uint32_t line_len = 0;

    furi_string_reset(bad_bt->line);

    do {
        ret = storage_file_read(script_file, bad_bt->file_buf, FILE_BUFFER_LEN);
        for(uint16_t i = 0; i < ret; i++) {
            if(bad_bt->file_buf[i] == '\n' && line_len > 0) {
                bad_bt->st.line_nb++;
                line_len = 0;
            } else {
                if(bad_bt->st.line_nb == 0) { // Save first line
                    furi_string_push_back(bad_bt->line, bad_bt->file_buf[i]);
                }
                line_len++;
            }
        }
        if(storage_file_eof(script_file)) {
            if(line_len > 0) {
                bad_bt->st.line_nb++;
                break;
            }
        }
    } while(ret > 0);

    // Looking for ID or BT_ID command at first line
    const char* line_tmp = furi_string_get_cstr(bad_bt->line);
    app->set_bt_id = false;
    app->has_bt_id = strncmp(line_tmp, ducky_cmd_bt_id, strlen(ducky_cmd_bt_id)) == 0;

    if(app->has_bt_id) {
        app->set_bt_id = ducky_set_bt_id(bad_bt, &line_tmp[strlen(ducky_cmd_bt_id) + 1]);
    }

    storage_file_seek(script_file, 0, true);
    furi_string_reset(bad_bt->line);
}

static int32_t ducky_script_execute_next(BadBtScript* bad_bt, File* script_file) {
    int32_t delay_val = 0;

    if(bad_bt->repeat_cnt > 0) {
        bad_bt->repeat_cnt--;
        delay_val = ducky_parse_line(bad_bt, bad_bt->line_prev);
        if(delay_val == SCRIPT_STATE_NEXT_LINE) { // Empty line
            return 0;
        } else if(delay_val == SCRIPT_STATE_STRING_START) { // Print string with delays
            return delay_val;
        } else if(delay_val == SCRIPT_STATE_WAIT_FOR_BTN) { // wait for button
            return delay_val;
        } else if(delay_val < 0) { // Script error
            bad_bt->st.error_line = bad_bt->st.line_cur - 1;
            FURI_LOG_E(WORKER_TAG, "Unknown command at line %u", bad_bt->st.line_cur - 1U);
            return SCRIPT_STATE_ERROR;
        } else {
            return (delay_val + bad_bt->defdelay);
        }
    }

    furi_string_set(bad_bt->line_prev, bad_bt->line);
    furi_string_reset(bad_bt->line);

    while(1) {
        if(bad_bt->buf_len == 0) {
            bad_bt->buf_len = storage_file_read(script_file, bad_bt->file_buf, FILE_BUFFER_LEN);
            if(storage_file_eof(script_file)) {
                if((bad_bt->buf_len < FILE_BUFFER_LEN) && (bad_bt->file_end == false)) {
                    bad_bt->file_buf[bad_bt->buf_len] = '\n';
                    bad_bt->buf_len++;
                    bad_bt->file_end = true;
                }
            }

            bad_bt->buf_start = 0;
            if(bad_bt->buf_len == 0) return SCRIPT_STATE_END;
        }
        for(uint8_t i = bad_bt->buf_start; i < (bad_bt->buf_start + bad_bt->buf_len); i++) {
            if(bad_bt->file_buf[i] == '\n' && furi_string_size(bad_bt->line) > 0) {
                bad_bt->st.line_cur++;
                bad_bt->buf_len = bad_bt->buf_len + bad_bt->buf_start - (i + 1);
                bad_bt->buf_start = i + 1;
                furi_string_trim(bad_bt->line);
                delay_val = ducky_parse_line(bad_bt, bad_bt->line);
                if(delay_val == SCRIPT_STATE_NEXT_LINE) { // Empty line
                    return 0;
                } else if(delay_val == SCRIPT_STATE_STRING_START) { // Print string with delays
                    return delay_val;
                } else if(delay_val == SCRIPT_STATE_WAIT_FOR_BTN) { // wait for button
                    return delay_val;
                } else if(delay_val < 0) {
                    bad_bt->st.error_line = bad_bt->st.line_cur;
                    FURI_LOG_E(WORKER_TAG, "Unknown command at line %u", bad_bt->st.line_cur);
                    return SCRIPT_STATE_ERROR;
                } else {
                    return (delay_val + bad_bt->defdelay);
                }
            } else {
                furi_string_push_back(bad_bt->line, bad_bt->file_buf[i]);
            }
        }
        bad_bt->buf_len = 0;
        if(bad_bt->file_end) return SCRIPT_STATE_END;
    }

    return 0;
}

static void bad_bt_bt_hid_state_callback(BtStatus status, void* context) {
    furi_assert(context);
    BadBtScript* bad_bt = context;
    bool state = (status == BtStatusConnected);

    if(state == true) {
        LevelRssiRange r = bt_remote_rssi_range(bad_bt->bt);
        if(r != LevelRssiError) {
            bt_timeout = bt_hid_delays[r];
        }
        furi_thread_flags_set(furi_thread_get_id(bad_bt->thread), WorkerEvtConnect);
    } else {
        furi_thread_flags_set(furi_thread_get_id(bad_bt->thread), WorkerEvtDisconnect);
    }
}

static uint32_t bad_bt_flags_get(uint32_t flags_mask, uint32_t timeout) {
    uint32_t flags = furi_thread_flags_get();
    furi_check((flags & FuriFlagError) == 0);
    if(flags == 0) {
        flags = furi_thread_flags_wait(flags_mask, FuriFlagWaitAny, timeout);
        furi_check(((flags & FuriFlagError) == 0) || (flags == (unsigned)FuriFlagErrorTimeout));
    } else {
        uint32_t state = furi_thread_flags_clear(flags);
        furi_check((state & FuriFlagError) == 0);
    }
    return flags;
}

int32_t bad_bt_conn_apply(BadBtApp* app) {
    // Shorthands so this bs is readable
    BadBtConfig* cfg = app->set_bt_id ? &app->id_config : &app->config;
    FuriHalBtProfile kbd = FuriHalBtProfileHidKeyboard;

    // Setup new config
    bt_timeout = bt_hid_delays[LevelRssi39_0];
    bt_disconnect(app->bt);
    furi_delay_ms(200);
    bt_keys_storage_set_storage_path(app->bt, BAD_BT_KEYS_PATH);
    furi_hal_bt_set_profile_adv_name(kbd, cfg->bt_name);
    if(app->bt_remember) {
        furi_hal_bt_set_profile_mac_addr(kbd, BAD_BT_BOUND_MAC);
        furi_hal_bt_set_profile_pairing_method(kbd, GapPairingPinCodeVerifyYesNo);
    } else {
        furi_hal_bt_set_profile_mac_addr(kbd, cfg->bt_mac);
        furi_hal_bt_set_profile_pairing_method(kbd, GapPairingNone);
    }

    // Set profile, restart BT, adjust defaults
    furi_check(bt_set_profile(app->bt, BtProfileHidKeyboard));

    // Advertise even if BT is off in settings
    furi_hal_bt_start_advertising();

    // Toggle key callback after since BT restart resets it
    if(app->bt_remember) {
        bt_enable_peer_key_update(app->bt);
    } else {
        bt_disable_peer_key_update(app->bt);
    }

    app->conn_mode = BadBtConnModeBt;

    return 0;
}

void bad_bt_conn_reset(BadBtApp* app) {
    if(app->conn_mode == BadBtConnModeBt) {
        // TODO: maybe also restore BT profile?
        bt_disconnect(app->bt);
        furi_delay_ms(200);
        bt_keys_storage_set_default_path(app->bt);
        FuriHalBtProfile kbd = FuriHalBtProfileHidKeyboard;
        furi_hal_bt_set_profile_mac_addr(kbd, app->prev_bt_mac);
        furi_hal_bt_set_profile_adv_name(kbd, app->prev_bt_name);
        furi_hal_bt_set_profile_pairing_method(kbd, app->prev_bt_mode);
        furi_check(bt_set_profile(app->bt, BtProfileSerial));
        bt_enable_peer_key_update(app->bt);
    }

    app->conn_mode = BadBtConnModeNone;
}

void bad_bt_config_adjust(BadBtConfig* cfg) {
    // Avoid empty name
    if(strcmp(cfg->bt_name, "") == 0) {
        snprintf(cfg->bt_name, BAD_BT_NAME_LEN, "Control %s", furi_hal_version_get_name_ptr());
    }

    // MAC is adjusted by furi_hal_bt, adjust here too so it matches after applying
    const uint8_t* normal_mac = furi_hal_version_get_ble_mac();
    uint8_t empty_mac[BAD_BT_MAC_LEN] = FURI_HAL_BT_EMPTY_MAC_ADDR;
    uint8_t default_mac[BAD_BT_MAC_LEN] = FURI_HAL_BT_DEFAULT_MAC_ADDR;
    if(memcmp(cfg->bt_mac, empty_mac, BAD_BT_MAC_LEN) == 0 ||
       memcmp(cfg->bt_mac, normal_mac, BAD_BT_MAC_LEN) == 0 ||
       memcmp(cfg->bt_mac, default_mac, BAD_BT_MAC_LEN) == 0) {
        memcpy(cfg->bt_mac, normal_mac, BAD_BT_MAC_LEN);
        cfg->bt_mac[2]++;
    }
}

void bad_bt_config_refresh(BadBtApp* app) {
    bt_set_status_changed_callback(app->bt, NULL, NULL);
    furi_hal_hid_set_state_callback(NULL, NULL);
    if(app->bad_bt_script) {
        furi_thread_flags_set(furi_thread_get_id(app->bad_bt_script->thread), WorkerEvtDisconnect);
    }
    if(app->conn_init_thread) {
        furi_thread_join(app->conn_init_thread);
    }

    bool apply = false;

    BadBtConfig* cfg = app->set_bt_id ? &app->id_config : &app->config;
    bad_bt_config_adjust(cfg);

    if(app->conn_mode != BadBtConnModeBt) {
        apply = true;
        bad_bt_conn_reset(app);
    } else {
        apply = apply || strncmp(
                             cfg->bt_name,
                             furi_hal_bt_get_profile_adv_name(FuriHalBtProfileHidKeyboard),
                             BAD_BT_NAME_LEN);
        apply = apply || memcmp(
                             app->bt_remember ? BAD_BT_BOUND_MAC : cfg->bt_mac,
                             furi_hal_bt_get_profile_mac_addr(FuriHalBtProfileHidKeyboard),
                             BAD_BT_MAC_LEN);
    }

    if(apply) {
        bad_bt_conn_apply(app);
    }

    if(app->bad_bt_script) {
        BadBtScript* script = app->bad_bt_script;
        script->bt = app->bt;
        bool connected;

        bt_set_status_changed_callback(app->bt, bad_bt_bt_hid_state_callback, script);
        connected = furi_hal_bt_is_connected();

        if(connected) {
            furi_thread_flags_set(furi_thread_get_id(script->thread), WorkerEvtConnect);
        }
    }

    // Reload config page
    scene_manager_next_scene(app->scene_manager, BadBtSceneConfig);
    scene_manager_previous_scene(app->scene_manager);
}

static int32_t bad_bt_worker(void* context) {
    BadBtScript* bad_bt = context;

    BadBtWorkerState worker_state = BadBtStateInit;
    BadBtWorkerState pause_state = BadBtStateRunning;
    int32_t delay_val = 0;

    FURI_LOG_I(WORKER_TAG, "Init");
    File* script_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    bad_bt->line = furi_string_alloc();
    bad_bt->line_prev = furi_string_alloc();
    bad_bt->string_print = furi_string_alloc();

    while(1) {
        if(worker_state == BadBtStateInit) { // State: initialization
            FURI_LOG_D(WORKER_TAG, "init start");
            if(storage_file_open(
                   script_file,
                   furi_string_get_cstr(bad_bt->file_path),
                   FSAM_READ,
                   FSOM_OPEN_EXISTING)) {
                ducky_script_preload(bad_bt, script_file);
                if(bad_bt->st.line_nb > 0) {
                    bad_bt_config_refresh(bad_bt->app);
                    worker_state = BadBtStateNotConnected; // Refresh will set connected flag
                } else {
                    worker_state = BadBtStateScriptError; // Script preload error
                }
            } else {
                FURI_LOG_E(WORKER_TAG, "File open error");
                worker_state = BadBtStateFileError; // File open error
            }
            bad_bt->st.state = worker_state;
            FURI_LOG_D(WORKER_TAG, "init done");

        } else if(worker_state == BadBtStateNotConnected) { // State: Not connected
            FURI_LOG_D(WORKER_TAG, "not connected wait");
            uint32_t flags = bad_bt_flags_get(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtDisconnect | WorkerEvtStartStop,
                FuriWaitForever);
            FURI_LOG_D(WORKER_TAG, "not connected flags: %lu", flags);

            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) {
                worker_state = BadBtStateIdle; // Ready to run
            } else if(flags & WorkerEvtStartStop) {
                worker_state = BadBtStateWillRun; // Will run when connected
            }
            bad_bt->st.state = worker_state;

        } else if(worker_state == BadBtStateIdle) { // State: ready to start
            FURI_LOG_D(WORKER_TAG, "idle wait");
            uint32_t flags = bad_bt_flags_get(
                WorkerEvtEnd | WorkerEvtStartStop | WorkerEvtConnect | WorkerEvtDisconnect,
                FuriWaitForever);
            FURI_LOG_D(WORKER_TAG, "idle flags: %lu", flags);

            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtStartStop) { // Start executing script
                delay_val = 0;
                bad_bt->buf_len = 0;
                bad_bt->st.line_cur = 0;
                bad_bt->defdelay = 0;
                bad_bt->stringdelay = 0;
                bad_bt->repeat_cnt = 0;
                bad_bt->key_hold_nb = 0;
                bad_bt->file_end = false;
                storage_file_seek(script_file, 0, true);
                bad_bt_script_set_keyboard_layout(bad_bt, bad_bt->keyboard_layout);
                worker_state = BadBtStateRunning;
            } else if(flags & WorkerEvtDisconnect) {
                worker_state = BadBtStateNotConnected; // Disconnected
            }
            bad_bt->st.state = worker_state;

        } else if(worker_state == BadBtStateWillRun) { // State: start on connection
            FURI_LOG_D(WORKER_TAG, "will run wait");
            uint32_t flags = bad_bt_flags_get(
                WorkerEvtEnd | WorkerEvtConnect | WorkerEvtDisconnect | WorkerEvtStartStop,
                FuriWaitForever);
            FURI_LOG_D(WORKER_TAG, "will run flags: %lu", flags);

            if(flags & WorkerEvtEnd) {
                break;
            } else if(flags & WorkerEvtConnect) { // Start executing script
                delay_val = 0;
                bad_bt->buf_len = 0;
                bad_bt->st.line_cur = 0;
                bad_bt->defdelay = 0;
                bad_bt->stringdelay = 0;
                bad_bt->repeat_cnt = 0;
                bad_bt->file_end = false;
                storage_file_seek(script_file, 0, true);
                // extra time for PC to recognize Flipper as keyboard
                flags = furi_thread_flags_wait(
                    WorkerEvtEnd | WorkerEvtDisconnect | WorkerEvtStartStop,
                    FuriFlagWaitAny | FuriFlagNoClear,
                    bad_bt->bt ? 3000 : 1500);
                if(flags == (unsigned)FuriFlagErrorTimeout) {
                    // If nothing happened - start script execution
                    worker_state = BadBtStateRunning;
                } else if(flags & WorkerEvtStartStop) {
                    worker_state = BadBtStateIdle;
                    furi_thread_flags_clear(WorkerEvtStartStop);
                }
                if(bad_bt->bt) {
                    update_bt_timeout(bad_bt->bt);
                }
                bad_bt_script_set_keyboard_layout(bad_bt, bad_bt->keyboard_layout);
            } else if(flags & WorkerEvtStartStop) { // Cancel scheduled execution
                worker_state = BadBtStateNotConnected;
            }
            bad_bt->st.state = worker_state;

        } else if(worker_state == BadBtStateRunning) { // State: running
            FURI_LOG_D(WORKER_TAG, "running");
            uint16_t delay_cur = (delay_val > 1000) ? (1000) : (delay_val);
            uint32_t flags = furi_thread_flags_wait(
                WorkerEvtEnd | WorkerEvtStartStop | WorkerEvtPauseResume | WorkerEvtConnect |
                    WorkerEvtDisconnect,
                FuriFlagWaitAny,
                delay_cur);
            FURI_LOG_D(WORKER_TAG, "running flags: %lu", flags);

            delay_val -= delay_cur;
            if(!(flags & FuriFlagError)) {
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtStartStop) {
                    worker_state = BadBtStateIdle; // Stop executing script
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtDisconnect) {
                    worker_state = BadBtStateNotConnected; // Disconnected
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtPauseResume) {
                    pause_state = BadBtStateRunning;
                    worker_state = BadBtStatePaused; // Pause
                }
                bad_bt->st.state = worker_state;
                continue;
            } else if(
                (flags == (unsigned)FuriFlagErrorTimeout) ||
                (flags == (unsigned)FuriFlagErrorResource)) {
                if(delay_val > 0) {
                    bad_bt->st.delay_remain--;
                    continue;
                }
                bad_bt->st.state = BadBtStateRunning;
                delay_val = ducky_script_execute_next(bad_bt, script_file);
                if(delay_val == SCRIPT_STATE_ERROR) { // Script error
                    delay_val = 0;
                    worker_state = BadBtStateScriptError;
                    bad_bt->st.state = worker_state;
                    furi_hal_bt_hid_kb_release_all();
                } else if(delay_val == SCRIPT_STATE_END) { // End of script
                    delay_val = 0;
                    worker_state = BadBtStateIdle;
                    bad_bt->st.state = BadBtStateDone;
                    furi_hal_bt_hid_kb_release_all();
                    continue;
                } else if(delay_val == SCRIPT_STATE_STRING_START) { // Start printing string with delays
                    delay_val = bad_bt->defdelay;
                    bad_bt->string_print_pos = 0;
                    worker_state = BadBtStateStringDelay;
                } else if(delay_val == SCRIPT_STATE_WAIT_FOR_BTN) { // set state to wait for user input
                    worker_state = BadBtStateWaitForBtn;
                    bad_bt->st.state = BadBtStateWaitForBtn; // Show long delays
                } else if(delay_val > 1000) {
                    bad_bt->st.state = BadBtStateDelay; // Show long delays
                    bad_bt->st.delay_remain = delay_val / 1000;
                }
            } else {
                furi_check((flags & FuriFlagError) == 0);
            }
        } else if(worker_state == BadBtStateWaitForBtn) { // State: Wait for button Press
            FURI_LOG_D(WORKER_TAG, "button wait");
            uint32_t flags = bad_bt_flags_get(
                WorkerEvtEnd | WorkerEvtStartStop | WorkerEvtPauseResume | WorkerEvtConnect |
                    WorkerEvtDisconnect,
                FuriWaitForever);
            FURI_LOG_D(WORKER_TAG, "button flags: %lu", flags);
            if(!(flags & FuriFlagError)) {
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtStartStop) {
                    delay_val = 0;
                    worker_state = BadBtStateRunning;
                } else if(flags & WorkerEvtDisconnect) {
                    worker_state = BadBtStateNotConnected; // Disconnected
                    furi_hal_bt_hid_kb_release_all();
                }
                bad_bt->st.state = worker_state;
                continue;
            }
        } else if(worker_state == BadBtStatePaused) { // State: Paused
            FURI_LOG_D(WORKER_TAG, "paused wait");
            uint32_t flags = bad_bt_flags_get(
                WorkerEvtEnd | WorkerEvtStartStop | WorkerEvtPauseResume | WorkerEvtConnect |
                    WorkerEvtDisconnect,
                FuriWaitForever);
            FURI_LOG_D(WORKER_TAG, "paused flags: %lu", flags);
            if(!(flags & FuriFlagError)) {
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtStartStop) {
                    worker_state = BadBtStateIdle; // Stop executing script
                    bad_bt->st.state = worker_state;
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtDisconnect) {
                    worker_state = BadBtStateNotConnected; // Disconnected
                    bad_bt->st.state = worker_state;
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtPauseResume) {
                    if(pause_state == BadBtStateRunning) {
                        if(delay_val > 0) {
                            bad_bt->st.state = BadBtStateDelay;
                            bad_bt->st.delay_remain = delay_val / 1000;
                        } else {
                            bad_bt->st.state = BadBtStateRunning;
                            delay_val = 0;
                        }
                        worker_state = BadBtStateRunning; // Resume
                    } else if(pause_state == BadBtStateStringDelay) {
                        bad_bt->st.state = BadBtStateRunning;
                        worker_state = BadBtStateStringDelay; // Resume
                    }
                }
                continue;
            }
        } else if(worker_state == BadBtStateStringDelay) { // State: print string with delays
            FURI_LOG_D(WORKER_TAG, "delay wait");
            uint32_t flags = bad_bt_flags_get(
                WorkerEvtEnd | WorkerEvtStartStop | WorkerEvtPauseResume | WorkerEvtConnect |
                    WorkerEvtDisconnect,
                bad_bt->stringdelay);
            FURI_LOG_D(WORKER_TAG, "delay flags: %lu", flags);

            if(!(flags & FuriFlagError)) {
                if(flags & WorkerEvtEnd) {
                    break;
                } else if(flags & WorkerEvtStartStop) {
                    worker_state = BadBtStateIdle; // Stop executing script
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtDisconnect) {
                    worker_state = BadBtStateNotConnected; // Disconnected
                    furi_hal_bt_hid_kb_release_all();
                } else if(flags & WorkerEvtPauseResume) {
                    pause_state = BadBtStateStringDelay;
                    worker_state = BadBtStatePaused; // Pause
                }
                bad_bt->st.state = worker_state;
                continue;
            } else if(
                (flags == (unsigned)FuriFlagErrorTimeout) ||
                (flags == (unsigned)FuriFlagErrorResource)) {
                bool string_end = ducky_string_next(bad_bt);
                if(string_end) {
                    bad_bt->stringdelay = 0;
                    worker_state = BadBtStateRunning;
                }
            } else {
                furi_check((flags & FuriFlagError) == 0);
            }
        } else if(
            (worker_state == BadBtStateFileError) ||
            (worker_state == BadBtStateScriptError)) { // State: error
            FURI_LOG_D(WORKER_TAG, "error wait");
            uint32_t flags =
                bad_bt_flags_get(WorkerEvtEnd, FuriWaitForever); // Waiting for exit command
            FURI_LOG_D(WORKER_TAG, "error flags: %lu", flags);

            if(flags & WorkerEvtEnd) {
                break;
            }
        }
        update_bt_timeout(bad_bt->bt);
    }

    bt_set_status_changed_callback(bad_bt->app->bt, NULL, NULL);
    furi_hal_hid_set_state_callback(NULL, NULL);

    storage_file_close(script_file);
    storage_file_free(script_file);
    furi_string_free(bad_bt->line);
    furi_string_free(bad_bt->line_prev);
    furi_string_free(bad_bt->string_print);

    FURI_LOG_I(WORKER_TAG, "End");

    return 0;
}

static void bad_bt_script_set_default_keyboard_layout(BadBtScript* bad_bt) {
    furi_assert(bad_bt);
    furi_string_set_str(bad_bt->keyboard_layout, "");
    memset(bad_bt->layout, HID_KEYBOARD_NONE, sizeof(bad_bt->layout));
    memcpy(bad_bt->layout, hid_asciimap, MIN(sizeof(hid_asciimap), sizeof(bad_bt->layout)));
}

BadBtScript* bad_bt_script_open(FuriString* file_path, Bt* bt, BadBtApp* app) {
    furi_assert(file_path);

    BadBtScript* bad_bt = malloc(sizeof(BadBtScript));
    bad_bt->app = app;
    bad_bt->file_path = furi_string_alloc();
    furi_string_set(bad_bt->file_path, file_path);
    bad_bt->keyboard_layout = furi_string_alloc();
    bad_bt_script_set_default_keyboard_layout(bad_bt);

    bad_bt->st.state = BadBtStateInit;
    bad_bt->st.error[0] = '\0';

    bad_bt->bt = bt;

    bad_bt->thread = furi_thread_alloc_ex("BadBtWorker", 2048, bad_bt_worker, bad_bt);
    furi_thread_start(bad_bt->thread);
    return bad_bt;
} //-V773

void bad_bt_script_close(BadBtScript* bad_bt) {
    furi_assert(bad_bt);
    furi_record_close(RECORD_STORAGE);
    furi_thread_flags_set(furi_thread_get_id(bad_bt->thread), WorkerEvtEnd);
    furi_thread_join(bad_bt->thread);
    furi_thread_free(bad_bt->thread);
    furi_string_free(bad_bt->file_path);
    furi_string_free(bad_bt->keyboard_layout);
    free(bad_bt);
}

void bad_bt_script_set_keyboard_layout(BadBtScript* bad_bt, FuriString* layout_path) {
    furi_assert(bad_bt);

    if((bad_bt->st.state == BadBtStateRunning) || (bad_bt->st.state == BadBtStateDelay)) {
        // do not update keyboard layout while a script is running
        return;
    }

    File* layout_file = storage_file_alloc(furi_record_open(RECORD_STORAGE));
    if(!furi_string_empty(layout_path)) { //-V1051
        furi_string_set(bad_bt->keyboard_layout, layout_path);
        if(storage_file_open(
               layout_file, furi_string_get_cstr(layout_path), FSAM_READ, FSOM_OPEN_EXISTING)) {
            uint16_t layout[128];
            if(storage_file_read(layout_file, layout, sizeof(layout)) == sizeof(layout)) {
                memcpy(bad_bt->layout, layout, sizeof(layout));
            }
        }
        storage_file_close(layout_file);
    } else {
        bad_bt_script_set_default_keyboard_layout(bad_bt);
    }
    storage_file_free(layout_file);
}

void bad_bt_script_start_stop(BadBtScript* bad_bt) {
    furi_assert(bad_bt);
    furi_thread_flags_set(furi_thread_get_id(bad_bt->thread), WorkerEvtStartStop);
}

void bad_bt_script_pause_resume(BadBtScript* bad_bt) {
    furi_assert(bad_bt);
    furi_thread_flags_set(furi_thread_get_id(bad_bt->thread), WorkerEvtPauseResume);
}

BadBtState* bad_bt_script_get_state(BadBtScript* bad_bt) {
    furi_assert(bad_bt);
    return &(bad_bt->st);
}
