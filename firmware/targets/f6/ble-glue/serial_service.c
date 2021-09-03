#include "serial_service.h"
#include "app_common.h"
#include "ble.h"

#include <furi.h>

#define SERIAL_SERVICE_TAG "serial service"

struct SerialSvc {
    uint16_t svc_handle;
    uint16_t rx_char_handle;
    uint16_t tx_char_handle;
    uint16_t char_value_size;
};

static SVCCTL_EvtAckStatus_t serial_svc_event_handler(void *Event) {
    SVCCTL_EvtAckStatus_t ret = SVCCTL_EvtNotAck;

    return ret; 
}

SerialSvc* serial_svc_init() {
    tBleStatus status;
    const uint8_t service_uuid[] = {SERIAL_SVC_UUID_128};
    const uint8_t char_rx_uuid[] = {SERIAL_CHAR_RX_UUID_128};
    const uint8_t char_tx_uuid[] = {SERIAL_CHAR_TX_UUID_128};

    SerialSvc* serial_svc = furi_alloc(sizeof(SerialSvc));
    serial_svc->char_value_size = 20;
    SVCCTL_RegisterSvcHandler(serial_svc_event_handler);

    // Add service
    status = aci_gatt_add_service(UUID_TYPE_128, (Service_UUID_t *)service_uuid, PRIMARY_SERVICE, 5, &serial_svc->svc_handle);
    if(status) {
        FURI_LOG_E(SERIAL_SERVICE_TAG, "Failed to add Serial service: %d", status);
    }

    // Add TX characteristics
    status = aci_gatt_add_char(serial_svc->svc_handle, UUID_TYPE_128, (const Char_UUID_t*)char_tx_uuid ,
                                serial_svc->char_value_size,                                  
                                CHAR_PROP_WRITE_WITHOUT_RESP | CHAR_PROP_READ,
                                ATTR_PERMISSION_NONE,
                                GATT_NOTIFY_ATTRIBUTE_WRITE,
                                10,
                                1,
                                &serial_svc->rx_char_handle);
    if(status) {
        FURI_LOG_E(SERIAL_SERVICE_TAG, "Failed to add TX characteristic: %d", status);
    }

    // Add RX characteristic
    status = aci_gatt_add_char(serial_svc->svc_handle, UUID_TYPE_128, (const Char_UUID_t*)char_rx_uuid ,
                                serial_svc->char_value_size,                                  
                                CHAR_PROP_READ | CHAR_PROP_NOTIFY,
                                ATTR_PERMISSION_NONE,
                                GATT_NOTIFY_ATTRIBUTE_WRITE,
                                10,
                                1,
                                &serial_svc->tx_char_handle);
    if(status) {
        FURI_LOG_E(SERIAL_SERVICE_TAG, "Failed to add RX characteristic: %d", status);
    }

    return serial_svc;
}

void serial_svc_free(SerialSvc* serial_svc) {
    furi_assert(serial_svc);
    free(serial_svc);
}


