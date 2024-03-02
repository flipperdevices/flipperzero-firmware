#include "ble_serial.h"

#include <gap.h>
#include <furi_ble/profile_interface.h>
#include <services/serial_service.h>
#include <furi.h>

typedef struct {
    FuriHalBleProfileBase base;

    BleServiceSerial* serial_svc;
} BleProfileSerial;
_Static_assert(offsetof(BleProfileSerial, base) == 0, "Wrong layout");

static FuriHalBleProfileBase* ble_profile_serial_start(FuriHalBleProfileParams profile_params) {
    UNUSED(profile_params);

    BleProfileSerial* profile = malloc(sizeof(BleProfileSerial));

    profile->base.config = ble_profile_serial;

    profile->serial_svc = ble_svc_serial_start();

    return &profile->base;
}

static void ble_profile_serial_stop(FuriHalBleProfileBase* profile) {
    furi_check(profile);
    furi_check(profile->config == ble_profile_serial);

    BleProfileSerial* serial_profile = (BleProfileSerial*)profile;
    ble_svc_serial_stop(serial_profile->serial_svc);
}

static GapConfig serial_template_config = {
    .adv_service_uuid = 0x3080,
    .appearance_char = 0x8600,
    .bonding_mode = true,
    .pairing_method = GapPairingPinCodeShow,
    .conn_param = {
        .conn_int_min = 0x18, // 30 ms
        .conn_int_max = 0x24, // 45 ms
        .slave_latency = 0,
        .supervisor_timeout = 0,
    }};

static void
    ble_profile_serial_get_config(GapConfig* config, FuriHalBleProfileParams profile_params) {
    BleProfileSerialParams* serial_profile_params = profile_params;

    furi_check(config);
    memcpy(config, &serial_template_config, sizeof(GapConfig));
    // Set mac address
    memcpy(config->mac_address, furi_hal_version_get_ble_mac(), sizeof(config->mac_address));

    // Change MAC address for HID profile
    config->mac_address[2]++;
    if(serial_profile_params) {
        config->mac_address[0] ^= serial_profile_params->mac_xor;
        config->mac_address[1] ^= serial_profile_params->mac_xor >> 8;
    }

    // Set advertise name
    memset(config->adv_name, 0, sizeof(config->adv_name));

    const char* clicker_str = "Serial";
    if(serial_profile_params && serial_profile_params->device_name_prefix) {
        clicker_str = serial_profile_params->device_name_prefix;
    }
    // We don't have Flipper in BLE name, use printf instead of replace
    FuriString* name = furi_string_alloc_printf(
        "%c%s %s",
        furi_hal_version_get_ble_local_device_name_ptr()[0],
        clicker_str,
        furi_hal_version_get_ble_local_device_name_ptr() + 1);
    if(furi_string_size(name) >= sizeof(config->adv_name)) {
        furi_string_left(name, sizeof(config->adv_name) - 1);
    }
    memcpy(config->adv_name, furi_string_get_cstr(name), furi_string_size(name));
    furi_string_free(name);

    config->adv_service_uuid |= furi_hal_version_get_hw_color();
}

static const FuriHalBleProfileTemplate profile_callbacks = {
    .start = ble_profile_serial_start,
    .stop = ble_profile_serial_stop,
    .get_gap_config = ble_profile_serial_get_config,
};

const FuriHalBleProfileTemplate* ble_profile_serial = &profile_callbacks;

void ble_profile_serial_set_event_callback(
    FuriHalBleProfileBase* profile,
    uint16_t buff_size,
    FuriHalBtSerialCallback callback,
    void* context) {
    furi_check(profile && (profile->config == ble_profile_serial));

    BleProfileSerial* serial_profile = (BleProfileSerial*)profile;
    ble_svc_serial_set_callbacks(serial_profile->serial_svc, buff_size, callback, context);
}

bool ble_profile_serial_tx(FuriHalBleProfileBase* profile, uint8_t* data, uint16_t size) {
    furi_check(profile && (profile->config == ble_profile_serial));

    BleProfileSerial* serial_profile = (BleProfileSerial*)profile;

    if(size > BLE_PROFILE_SERIAL_PACKET_SIZE_MAX) {
        return false;
    }

    return ble_svc_serial_update_tx(serial_profile->serial_svc, data, size);
}
