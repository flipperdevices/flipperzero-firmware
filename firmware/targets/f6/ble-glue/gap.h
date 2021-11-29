#pragma once

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    BleEventTypeConnected,
    BleEventTypeDisconnected,
    BleEventTypeStartAdvertising,
    BleEventTypeStopAdvertising,
    BleEventTypePinCodeShow,
    BleEventTypeUpdateMTU,
} BleEventType;

typedef union {
    uint32_t pin_code;
    uint16_t max_packet_size;
} BleEventData;

typedef struct {
    BleEventType type;
    BleEventData data;
} BleEvent;

typedef void(*BleEventCallback) (BleEvent event, void* context);

typedef enum {
    GapStateIdle,
    GapStateAdvFast,
    GapStateAdvLowPower,
    GapStateConnected,
} GapState;

typedef struct {
    uint16_t adv_service_uuid;
    uint16_t appearance_char;
    bool bonding_mode;
    bool mitm_enable;
} GapConfig;

bool gap_init(GapConfig* config, BleEventCallback on_event_cb, void* context);

void gap_start_advertising();

void gap_stop_advertising();

GapState gap_get_state();

void gap_kill_thread();

#ifdef __cplusplus
}
#endif
