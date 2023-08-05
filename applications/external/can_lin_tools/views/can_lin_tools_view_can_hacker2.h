#pragma once

#include <gui/view.h>
#include "../helpers/can_lin_tools_types.h"
#include "../helpers/can_lin_tools_event.h"

typedef struct CanLinToolsCanHacker2 CanLinToolsCanHacker2;

typedef void (*CanLinToolsCanHacker2ViewCallback)(CanLinToolsCustomEvent event, void* context);

typedef enum {
    CanLinToolsCanHacker2ViewStatusNoUSBConnect,
    CanLinToolsCanHacker2ViewStatusUSBConnect,
} CanLinToolsCanHacker2ViewStatus;

// void can_lin_tools_view_can_hacker2_update(
//     CanLinToolsCanHacker2* instance,
//     bool nfc_field,
//     bool rfid_field,
//     uint32_t rfid_frequency);

CanLinToolsCanHacker2* can_lin_tools_view_can_hacker2_alloc();

void can_lin_tools_view_can_hacker2_free(CanLinToolsCanHacker2* instance);

View* can_lin_tools_view_can_hacker2_get_view(CanLinToolsCanHacker2* instance);
