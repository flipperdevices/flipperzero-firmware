#pragma once

#include <stdint.h>
#include <core/common_defines.h>

#ifdef __cplusplus
extern "C" {
#endif

// Using other types so not to leak all the BLE stack headers (we don't have a wrapper yet)
// Return type is SVCCTL_EvtAckStatus_t
// Event data is hci_uart_pckt*
// Context is user-defined
typedef int32_t (*BleServiceEventHandlerCb)(void* event, void* context);

typedef struct GapSvcEventHandler GapSvcEventHandler;

// To be called once at startup
void ble_service_event_dispatcher_init();

// To be called at stack reset - ensures that all handlers are unregistered
void ble_service_event_dispatcher_reset();

// Add a handler to the list of handlers
FURI_WARN_UNUSED GapSvcEventHandler*
    ble_service_event_dispatcher_register_handler(BleServiceEventHandlerCb handler, void* context);

// Remove a handler from the list of handlers
void ble_service_event_dispatcher_unregister_handler(GapSvcEventHandler* handler);

#ifdef __cplusplus
}
#endif