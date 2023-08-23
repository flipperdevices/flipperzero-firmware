#pragma once

#include <furi.h>

typedef struct CanHacker2 CanHacker2;

typedef enum {
    CanHacker2Disconnected,
    CanHacker2Connected,
} CanHacker2Status;

typedef void (*CanHacker2Callback)(void* context);
typedef void (*CanHacker2ConnectCallback)(void* context, CanHacker2Status status);

CanHacker2* can_hacker2_alloc(void);
void can_hacker2_free(CanHacker2* instance);
bool can_hacker2_is_device_connected(CanHacker2* instance);
void can_hacker2_set_callback(
    CanHacker2* instance,
    CanHacker2Callback callback,
    CanHacker2ConnectCallback connect_callback,
    void* context);
size_t can_hacker2_spaces_rx(CanHacker2* instance);
bool can_hacker2_rx(CanHacker2* instance, uint8_t* data, size_t len);
size_t can_hacker2_tx(CanHacker2* instance, uint8_t* data, size_t max_len);
void can_hacker2_exit(CanHacker2* instance);
void can_hacker2_process_cmd(CanHacker2* instance);
void can_hacker2_get_cmd(CanHacker2* instance);
void can_hacker2_process(CanHacker2* instance);
