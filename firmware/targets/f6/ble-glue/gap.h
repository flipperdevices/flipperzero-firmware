#pragma once

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef void(*GapOnConnectCallback) (void* context);
typedef void(*GapOnDisconnectCallback) (void* context);

typedef enum {
    GapStateIdle,
    GapStateAdvFast,
    GapStateAdvLowPower,
    GapStateConnected,
} GapState;

bool gap_init(GapOnConnectCallback on_connect_cb, GapOnDisconnectCallback on_disconnect_cb, void* context);

void gap_start_advertising();

void gap_stop_advertising();

GapState gap_get_state();

#ifdef __cplusplus
}
#endif
