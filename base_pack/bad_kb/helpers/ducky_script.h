#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <furi.h>
#include <furi_hal.h>
#include <bt/bt_service/bt.h>

#include "../bad_kb_app.h"

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
    WorkerEvtStartStop = (1 << 0),
    WorkerEvtPauseResume = (1 << 1),
    WorkerEvtEnd = (1 << 2),
    WorkerEvtConnect = (1 << 3),
    WorkerEvtDisconnect = (1 << 4),
} WorkerEvtFlags;

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

typedef struct {
    BadKbWorkerState state;
    bool is_bt;
    uint32_t pin;
    size_t line_cur;
    size_t line_nb;
    uint32_t delay_remain;
    size_t error_line;
    char error[64];
    uint32_t elapsed;
} BadKbState;

typedef struct BadKbScript BadKbScript;

BadKbScript* bad_kb_script_open(FuriString* file_path, Bt* bt, BadKbApp* app);

void bad_kb_script_close(BadKbScript* bad_kb);

void bad_kb_script_set_keyboard_layout(BadKbScript* bad_kb, FuriString* layout_path);

void bad_kb_script_start(BadKbScript* bad_kb);

void bad_kb_script_stop(BadKbScript* bad_kb);

void bad_kb_script_start_stop(BadKbScript* bad_kb);

void bad_kb_script_pause_resume(BadKbScript* bad_kb);

BadKbState* bad_kb_script_get_state(BadKbScript* bad_kb);

void bad_kb_bt_hid_state_callback(BtStatus status, void* context);

void bad_kb_usb_hid_state_callback(bool state, void* context);

#ifdef __cplusplus
}
#endif
