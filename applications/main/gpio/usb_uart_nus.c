#include "usb_uart_nus.h"
#include <bt/bt_service/bt.h>
#include <furi_hal.h>
#include "app_common.h" // IWYU pragma: keep
#include <ble/ble.h>
#include <furi_ble/gatt.h>
#include <furi_ble/event_dispatcher.h>

#define TAG "UsbUartNus"

// Nordic UART Service UUIDs and characteristic definitions:
// https://nrfconnectdocs.nordicsemi.com/ncs/latest/nrf/libraries/bluetooth/services/nus.html

/* STM32WB represents 128-bit UUIDs least significant byte first. */
#define USB_UART_NUS_UUID(value) \
    {0x9e,                       \
     0xca,                       \
     0xdc,                       \
     0x24,                       \
     0x0e,                       \
     0xe5,                       \
     0xa9,                       \
     0xe0,                       \
     0x93,                       \
     0xf3,                       \
     0xa3,                       \
     0xb5,                       \
     value,                      \
     0x00,                       \
     0x40,                       \
     0x6e}

#define USB_UART_NUS_BUFFER_SIZE 1024

struct UsbUartNus {
    Bt* bt;
    FuriHalBleProfileBase* profile;
    FuriStreamBuffer* incoming;
    UsbUartNusCallback ready;
    UsbUartNusCallback disconnected;
    void* context;
};

typedef struct {
    FuriHalBleProfileBase base;
    UsbUartNus* transport;
    uint16_t service;
    BleGattCharacteristicInstance rx;
    BleGattCharacteristicInstance tx;
    GapSvcEventHandler* handler;
    FuriMutex* mutex;
    bool subscribed;
} UsbUartNusProfile;

_Static_assert(offsetof(UsbUartNusProfile, base) == 0, "Wrong layout");

typedef struct {
    const uint8_t* data;
    uint16_t length;
} UsbUartNusPacket;

static bool usb_uart_nus_packet_data(const void* context, const uint8_t** data, uint16_t* length) {
    if(!data) {
        *length = USB_UART_NUS_PACKET_SIZE;
    } else {
        const UsbUartNusPacket* packet = context;
        *data = packet->data;
        *length = packet->length;
    }
    return false;
}

static const BleGattCharacteristicParams usb_uart_nus_rx_params = {
    .name = "NUS RX",
    .uuid.Char_UUID_128 = USB_UART_NUS_UUID(2),
    .uuid_type = UUID_TYPE_128,
    .data_prop_type = FlipperGattCharacteristicDataFixed,
    .data.fixed.length = 512,
    .char_properties = CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
    .security_permissions = ATTR_PERMISSION_NONE,
    .gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE,
    .is_variable = CHAR_VALUE_LEN_VARIABLE,
};

static const BleGattCharacteristicParams usb_uart_nus_tx_params = {
    .name = "NUS TX",
    .uuid.Char_UUID_128 = USB_UART_NUS_UUID(3),
    .uuid_type = UUID_TYPE_128,
    .data_prop_type = FlipperGattCharacteristicDataCallback,
    .data.callback.fn = usb_uart_nus_packet_data,
    .char_properties = CHAR_PROP_NOTIFY,
    .security_permissions = ATTR_PERMISSION_NONE,
    .gatt_evt_mask = GATT_NOTIFY_ATTRIBUTE_WRITE,
    .is_variable = CHAR_VALUE_LEN_VARIABLE,
};

static BleEventAckStatus usb_uart_nus_on_event(void* event, void* context) {
    UsbUartNusProfile* profile = context;
    hci_event_pckt* packet = (void*)((hci_uart_pckt*)event)->data;
    if(packet->evt == HCI_DISCONNECTION_COMPLETE_EVT_CODE) {
        furi_mutex_acquire(profile->mutex, FuriWaitForever);
        profile->subscribed = false;
        furi_mutex_release(profile->mutex);
        furi_stream_buffer_reset(profile->transport->incoming);
        profile->transport->disconnected(profile->transport->context);
        return BleEventNotAck;
    }
    if(packet->evt != HCI_VENDOR_SPECIFIC_DEBUG_EVT_CODE) return BleEventNotAck;
    evt_blecore_aci* aci = (void*)packet->data;
    if(aci->ecode != ACI_GATT_ATTRIBUTE_MODIFIED_VSEVT_CODE) return BleEventNotAck;
    aci_gatt_attribute_modified_event_rp0* attr = (void*)aci->data;
    if(attr->Attr_Handle == profile->rx.handle + 1) {
        size_t sent = furi_stream_buffer_send(
            profile->transport->incoming, attr->Attr_Data, attr->Attr_Data_Length, 0);
        if(sent != attr->Attr_Data_Length)
            FURI_LOG_W(
                TAG, "RX overflow: %u bytes dropped", (unsigned)(attr->Attr_Data_Length - sent));
        profile->transport->ready(profile->transport->context);
        return BleEventAckFlowEnable;
    }
    if(attr->Attr_Handle == profile->tx.handle + 2) {
        furi_mutex_acquire(profile->mutex, FuriWaitForever);
        profile->subscribed = attr->Attr_Data_Length >= 2 && (attr->Attr_Data[0] & 1);
        furi_mutex_release(profile->mutex);
        return BleEventAckFlowEnable;
    }
    return BleEventNotAck;
}

static const FuriHalBleProfileTemplate usb_uart_nus_template;
static FuriHalBleProfileBase* usb_uart_nus_profile_start(FuriHalBleProfileParams params) {
    UsbUartNusProfile* profile = calloc(1, sizeof(UsbUartNusProfile));
    profile->base.config = &usb_uart_nus_template;
    profile->transport = params;
    profile->mutex = furi_mutex_alloc(FuriMutexTypeNormal);
    const Service_UUID_t uuid = {.Service_UUID_128 = USB_UART_NUS_UUID(1)};
    if(!ble_gatt_service_add(UUID_TYPE_128, &uuid, PRIMARY_SERVICE, 6, &profile->service)) {
        furi_mutex_free(profile->mutex);
        free(profile);
        return NULL;
    }
    ble_gatt_characteristic_init(profile->service, &usb_uart_nus_rx_params, &profile->rx);
    ble_gatt_characteristic_init(profile->service, &usb_uart_nus_tx_params, &profile->tx);
    if(!profile->rx.handle || !profile->tx.handle) {
        if(profile->rx.handle) ble_gatt_characteristic_delete(profile->service, &profile->rx);
        if(profile->tx.handle) ble_gatt_characteristic_delete(profile->service, &profile->tx);
        ble_gatt_service_delete(profile->service);
        furi_mutex_free(profile->mutex);
        free(profile);
        return NULL;
    }
    profile->handler = ble_event_dispatcher_register_svc_handler(usb_uart_nus_on_event, profile);
    return &profile->base;
}

static void usb_uart_nus_profile_stop(FuriHalBleProfileBase* base) {
    UsbUartNusProfile* profile = (void*)base;
    ble_event_dispatcher_unregister_svc_handler(profile->handler);
    ble_gatt_characteristic_delete(profile->service, &profile->rx);
    ble_gatt_characteristic_delete(profile->service, &profile->tx);
    ble_gatt_service_delete(profile->service);
    furi_mutex_free(profile->mutex);
    free(profile);
}

static void usb_uart_nus_address(uint8_t address[6]) {
    memcpy(address, furi_hal_version_get_ble_mac(), 6);
    address[2] += 2;
}

void usb_uart_nus_get_identity(
    char name[USB_UART_NUS_NAME_SIZE],
    char address[USB_UART_NUS_ADDRESS_SIZE]) {
    uint8_t mac[6];
    usb_uart_nus_address(mac);
    const char* configured_name = furi_hal_version_get_name_ptr();
    /* Legacy advertising has five bytes left alongside the NUS UUID. */
    snprintf(
        name,
        USB_UART_NUS_NAME_SIZE,
        "%.5s",
        configured_name && configured_name[0] ? configured_name : "Flipper");
    snprintf(
        address,
        USB_UART_NUS_ADDRESS_SIZE,
        "%02X:%02X:%02X:%02X:%02X:%02X",
        mac[5],
        mac[4],
        mac[3],
        mac[2],
        mac[1],
        mac[0]);
}

static void usb_uart_nus_gap_config(GapConfig* config, FuriHalBleProfileParams params) {
    UNUSED(params);
    memset(config, 0, sizeof(*config));
    config->adv_service.UUID_Type = UUID_TYPE_128;
    const uint8_t uuid[] = USB_UART_NUS_UUID(1);
    memcpy(config->adv_service.Service_UUID_128, uuid, sizeof(uuid));
    usb_uart_nus_address(config->mac_address);
    /* The full NUS UUID leaves five characters for the advertising name. */
    config->adv_name[0] = AD_TYPE_SHORTENED_LOCAL_NAME;
    char name[USB_UART_NUS_NAME_SIZE];
    char address[USB_UART_NUS_ADDRESS_SIZE];
    usb_uart_nus_get_identity(name, address);
    strlcpy(config->adv_name + 1, name, sizeof(config->adv_name) - 1);
    config->pairing_method = GapPairingNone;
    config->bonding_mode = false;
    config->conn_param.conn_int_min = 0x000c;
    config->conn_param.conn_int_max = 0x0024;
    config->conn_param.supervisor_timeout = 400;
}

static const FuriHalBleProfileTemplate usb_uart_nus_template = {
    .start = usb_uart_nus_profile_start,
    .stop = usb_uart_nus_profile_stop,
    .get_gap_config = usb_uart_nus_gap_config,
};

UsbUartNus*
    usb_uart_nus_alloc(UsbUartNusCallback ready, UsbUartNusCallback disconnected, void* context) {
    furi_assert(ready);
    furi_assert(disconnected);

    UsbUartNus* transport = calloc(1, sizeof(UsbUartNus));
    transport->incoming = furi_stream_buffer_alloc(USB_UART_NUS_BUFFER_SIZE, 1);
    transport->ready = ready;
    transport->disconnected = disconnected;
    transport->context = context;
    transport->bt = furi_record_open(RECORD_BT);
    bt_disconnect(transport->bt);
    transport->profile = bt_profile_start(transport->bt, &usb_uart_nus_template, transport);
    if(!transport->profile) {
        bt_profile_restore_default(transport->bt);
        furi_record_close(RECORD_BT);
        furi_stream_buffer_free(transport->incoming);
        free(transport);
        return NULL;
    }
    furi_hal_bt_start_advertising();
    return transport;
}

void usb_uart_nus_free(UsbUartNus* transport) {
    furi_assert(transport);
    bt_disconnect(transport->bt);
    furi_hal_bt_stop_advertising();
    furi_check(bt_profile_restore_default(transport->bt));
    furi_record_close(RECORD_BT);
    furi_stream_buffer_free(transport->incoming);
    free(transport);
}

size_t usb_uart_nus_read(UsbUartNus* transport, uint8_t* data, size_t capacity) {
    furi_assert(transport);
    furi_assert(data);
    return furi_stream_buffer_receive(transport->incoming, data, capacity, 0);
}

bool usb_uart_nus_send(UsbUartNus* transport, const uint8_t* data, size_t length) {
    furi_assert(transport);
    furi_assert(data);
    furi_check(length <= USB_UART_NUS_PACKET_SIZE);
    UsbUartNusProfile* profile = (void*)transport->profile;
    furi_mutex_acquire(profile->mutex, FuriWaitForever);
    const UsbUartNusPacket packet = {.data = data, .length = length};
    bool subscribed = profile->subscribed;
    furi_mutex_release(profile->mutex);
    /* The GATT helper returns true on failure. */
    return subscribed && !ble_gatt_characteristic_update(profile->service, &profile->tx, &packet);
}
